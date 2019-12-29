/*
 * This file is part of roccat-tools.
 *
 * roccat-tools is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * roccat-tools is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with roccat-tools. If not, see <http://www.gnu.org/licenses/>.
 */

#include "arvoconfig_window.h"
#include "arvoconfig_configuration_dialog.h"
#include "arvoconfig_profile_page.h"
#include "roccat_warning_dialog.h"
#include "roccat_save_dialog.h"
#include "roccat_info_dialog.h"
#include "g_roccat_helper.h"
#include "g_dbus_roccat_helper.h"
#include "roccat_helper.h"
#include "roccat.h"
#include "roccat_firmware.h"
#include "i18n.h"

#define ARVOCONFIG_WINDOW_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), ARVOCONFIG_WINDOW_TYPE, ArvoconfigWindowClass))
#define IS_ARVOCONFIG_WINDOW_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), ARVOCONFIG_WINDOW_TYPE))
#define ARVOCONFIG_WINDOW_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), ARVOCONFIG_WINDOW_TYPE, ArvoconfigWindowPrivate))

typedef struct _ArvoconfigWindowClass ArvoconfigWindowClass;
typedef struct _ArvoconfigWindowPrivate ArvoconfigWindowPrivate;

struct _ArvoconfigWindow {
	RoccatConfigWindowPages parent;
	ArvoconfigWindowPrivate *priv;
};

struct _ArvoconfigWindowClass {
	RoccatConfigWindowPagesClass parent_class;
};

struct _ArvoconfigWindowPrivate {
	RoccatKeyFile *config;
	DBusGProxy *dbus_proxy;
};

G_DEFINE_TYPE(ArvoconfigWindow, arvoconfig_window, ROCCAT_CONFIG_WINDOW_PAGES_TYPE);

static gboolean change_profile_handler(gpointer user_data) {
	RoccatConfigWindow *roccat_window = ROCCAT_CONFIG_WINDOW(user_data);
	ArvoconfigProfilePage *profile_page;
	guint profile_index = roccat_config_window_pages_get_active_page(ROCCAT_CONFIG_WINDOW_PAGES(roccat_window));
	guint profile_number = profile_index + 1;
	GError *local_error = NULL;
	ArvoRkp *rkp;

	if (!roccat_config_window_warn_if_no_device(roccat_window))
		return FALSE;

	profile_page = ARVOCONFIG_PROFILE_PAGE(roccat_config_window_pages_get_page(ROCCAT_CONFIG_WINDOW_PAGES(roccat_window), profile_index));
	/* using unmodified data to change profile */
	rkp = arvoconfig_profile_page_get_rkp_orig(profile_page);
	arvo_profile_activate(roccat_config_window_get_device(roccat_window), profile_number, rkp, &local_error);
	arvo_rkp_free(rkp);
	if (!roccat_handle_error_dialog(GTK_WINDOW(roccat_window), _("Could not activate profile"), &local_error))
		return FALSE;
	arvo_dbus_emit_profile_changed_outside(ARVOCONFIG_WINDOW(roccat_window)->priv->dbus_proxy, profile_number);

	return FALSE;
}

static void window_active_page_changed_cb(RoccatConfigWindowPages *roccat_window, gpointer user_data) {
	g_idle_add(change_profile_handler, roccat_window);
}

static void actual_profile_changed_from_device_cb(DBusGProxy *proxy, guchar profile_number, gpointer user_data) {
	roccat_config_window_pages_set_active_page_blocked(ROCCAT_CONFIG_WINDOW_PAGES(user_data), profile_number - 1);
}

static void device_add_cb(RoccatConfigWindow *roccat_window, gpointer user_data) {
	ArvoconfigWindow *window = ARVOCONFIG_WINDOW(roccat_window);
	ArvoconfigWindowPrivate *priv = window->priv;
	GError *local_error = NULL;
	guint profile_number;
	RoccatDevice *device;

	device = roccat_config_window_get_device(roccat_window);

	profile_number = arvo_actual_profile_read(device, &local_error);
	if (!roccat_handle_error_dialog(GTK_WINDOW(roccat_window), _("Could not read actual profile"), &local_error))
		return;
	roccat_config_window_pages_set_active_page_blocked(ROCCAT_CONFIG_WINDOW_PAGES(roccat_window), profile_number - 1);

	dbus_g_proxy_connect_signal(priv->dbus_proxy, "ProfileChanged", G_CALLBACK(actual_profile_changed_from_device_cb), window, NULL);
}

static void device_remove_cb(RoccatConfigWindow *roccat_window, gpointer user_data) {
	ArvoconfigWindow *window = ARVOCONFIG_WINDOW(roccat_window);
	ArvoconfigWindowPrivate *priv = window->priv;

	dbus_g_proxy_disconnect_signal(priv->dbus_proxy, "ProfileChanged", G_CALLBACK(actual_profile_changed_from_device_cb), window);
}

static ArvoRkp *get_rkp(ArvoconfigWindow *window, ArvoconfigProfilePage *profile_page) {
	ArvoRkp *rkp;
	gboolean moved;

	rkp = arvoconfig_profile_page_get_rkp(profile_page);
	moved = roccat_config_window_pages_get_page_moved(ROCCAT_CONFIG_WINDOW_PAGES(window), ROCCAT_PROFILE_PAGE(profile_page));

	if (moved)
		arvo_rkp_set_modified(rkp);

	return rkp;
}

static void set_rkp(ArvoconfigWindow *window, guint profile_index, ArvoRkp *rkp) {
	ArvoconfigProfilePage *profile_page = ARVOCONFIG_PROFILE_PAGE(roccat_config_window_pages_get_page(ROCCAT_CONFIG_WINDOW_PAGES(window), profile_index));
	guint profile_number = profile_index + 1;
	arvoconfig_profile_page_set_rkp(profile_page, rkp);
	roccat_config_window_pages_set_page_unmoved(ROCCAT_CONFIG_WINDOW_PAGES(window), ROCCAT_PROFILE_PAGE(profile_page));
	arvo_dbus_emit_profile_data_changed_outside(window->priv->dbus_proxy, profile_number);
}

static gboolean save_single(ArvoconfigWindow *window, ArvoRkp *rkp, guint profile_index, GError **error) {
	RoccatDevice *device = roccat_config_window_get_device(ROCCAT_CONFIG_WINDOW(window));
	guint profile_number = profile_index + 1;

	if (device)
		arvo_rkp_save(device, rkp, profile_number, error);
	else
		arvo_rkp_save_actual(rkp, profile_number, error);

	if (*error)
		return FALSE;
	else {
		set_rkp(window, profile_index, rkp);
		return TRUE;
	}
}

static gboolean save_all(ArvoconfigWindow *window, gboolean ask) {
	ArvoRkp *rkps[ARVO_PROFILE_NUM];
	ArvoconfigProfilePage *profile_page;
	GError *local_error = NULL;
	gboolean modified;
	gboolean retval;
	guint i;

	modified = FALSE;
	for (i = 0; i < ARVO_PROFILE_NUM; ++i) {
		profile_page = ARVOCONFIG_PROFILE_PAGE(roccat_config_window_pages_get_page(ROCCAT_CONFIG_WINDOW_PAGES(window), i));
		rkps[i] = get_rkp(window, profile_page);
		if (arvo_rkp_get_modified(rkps[i]))
			modified = TRUE;
	}
	if (!modified) {
		retval = TRUE;
		goto exit;
	}

	if (ask) {
		switch (roccat_save_unsaved_dialog(GTK_WINDOW(window), TRUE)) {
		case GTK_RESPONSE_CANCEL:
			/* not saving by user request */
			retval = FALSE;
			goto exit;
			break;
		case GTK_RESPONSE_ACCEPT:
			break;
		case GTK_RESPONSE_REJECT:
			/* discarding by user request */
			retval = TRUE;
			goto exit;
			break;
		default:
			break;
		}
	}

	roccat_config_window_warn_if_no_device(ROCCAT_CONFIG_WINDOW(window));

	retval = TRUE;
	for (i = 0; i < ARVO_PROFILE_NUM; ++i) {
		if (!save_single(window, rkps[i], i, &local_error)) {
			retval = FALSE;
			break;
		}
	}
	roccat_handle_error_dialog(GTK_WINDOW(window), _("Could not save unsaved data"), &local_error);

exit:
	for (i = 0; i < ARVO_PROFILE_NUM; ++i)
		arvo_rkp_free(rkps[i]);

	return retval;
}

static gboolean delete_event_cb(GtkWidget *window, GdkEvent *event, gpointer user_data) {
	return !save_all(ARVOCONFIG_WINDOW(window), TRUE);
}

static GtkFileFilter *windows_file_filter_new(void) {
	GtkFileFilter *filter;
	filter = gtk_file_filter_new();
	gtk_file_filter_set_name(filter, _("Arvo Windows profiles"));
	gtk_file_filter_add_pattern(filter, "*." ROCCAT_KEYBOARD_PROFILE_EXTENSION);
	return filter;
}

static GtkFileFilter *linux_file_filter_new(void) {
	GtkFileFilter *filter;
	filter = gtk_file_filter_new();
	gtk_file_filter_set_name(filter, _("Arvo Linux profiles"));
	gtk_file_filter_add_pattern(filter, "*." ROCCAT_KEYBOARD_PROFILE_EXTENSION);
	return filter;
}

static GtkFileFilter *all_file_filter_new(void) {
	GtkFileFilter *filter;
	filter = gtk_file_filter_new();
	gtk_file_filter_set_name(filter, _("All files"));
	gtk_file_filter_add_pattern(filter, "*");
	return filter;
}

static gchar *arvo_create_filename_proposition(ArvoRkp *rkp, guint profile_index) {
	gchar *profile_name;
	gchar *filename;

	profile_name = arvo_rkp_get_profile_name(rkp);
	filename = roccat_profile_filename_proposition(profile_name, profile_index, ROCCAT_KEYBOARD_PROFILE_EXTENSION);
	g_free(profile_name);

	return filename;
}

static void save_profile_to_file_cb(RoccatProfilePage *profile_page, gpointer user_data) {
	ArvoconfigWindow *window = ARVOCONFIG_WINDOW(user_data);
	ArvoconfigWindowPrivate *priv = window->priv;
	GtkWidget *dialog;
	gchar *filename, *path;
	gchar *temp_filename;
	GError *error = NULL;
	GtkFileFilter *windows_filter;
	GtkFileFilter *linux_filter;
	GtkFileFilter *all_filter;
	GtkFileFilter *filter;
	guint profile_index = roccat_config_window_pages_get_page_index(ROCCAT_CONFIG_WINDOW_PAGES(window), profile_page);
	ArvoRkp *rkp;

	rkp = arvoconfig_profile_page_get_rkp(ARVOCONFIG_PROFILE_PAGE(profile_page));

	dialog = gtk_file_chooser_dialog_new(_("Save profile"),
			GTK_WINDOW(window),
			GTK_FILE_CHOOSER_ACTION_SAVE,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
			NULL);

	windows_filter = windows_file_filter_new();
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), windows_filter);
	linux_filter = linux_file_filter_new();
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), linux_filter);
	all_filter = all_file_filter_new();
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), all_filter);

	path = arvo_configuration_get_rkp_save_path(priv->config);
	gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), path);
	g_free(path);

	filename = arvo_create_filename_proposition(rkp, profile_index);
	gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog), filename);
	g_free(filename);

	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
		filter = gtk_file_chooser_get_filter(GTK_FILE_CHOOSER(dialog));
		filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
		temp_filename = roccat_create_filename_with_extension(filename, ROCCAT_KEYBOARD_PROFILE_EXTENSION);
		g_free(filename);

		path = g_path_get_dirname(temp_filename);
		arvo_configuration_set_rkp_save_path(priv->config, path);
		g_free(path);

		if (filter != linux_filter)
			arvo_linux_rkp_to_windows(rkp);

		arvo_rkp_write_with_path(temp_filename, rkp, &error);
		g_free(temp_filename);
		if (error) {
			roccat_warning_dialog(GTK_WINDOW(dialog), _("Could not save profile"), error->message);
			g_error_free(error);
		}
	}
	gtk_widget_destroy(dialog);
	arvo_rkp_free(rkp);
}

static void load_profile_from_file_cb(RoccatProfilePage *profile_page, gpointer user_data) {
	ArvoconfigWindow *window = ARVOCONFIG_WINDOW(user_data);
	ArvoconfigWindowPrivate *priv = window->priv;
	GtkWidget *dialog;
	gchar *filename, *path;
	GError *error = NULL;
	GtkFileFilter *windows_filter;
	GtkFileFilter *linux_filter;
	GtkFileFilter *all_filter;
	GtkFileFilter *filter;
	ArvoRkp *rkp;

	dialog = gtk_file_chooser_dialog_new(_("Load profile"),
			GTK_WINDOW(window),
			GTK_FILE_CHOOSER_ACTION_OPEN,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
			NULL);

	windows_filter = windows_file_filter_new();
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), windows_filter);
	linux_filter = linux_file_filter_new();
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), linux_filter);
	all_filter = all_file_filter_new();
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), all_filter);

	path = arvo_configuration_get_rkp_save_path(priv->config);
	gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), path);
	g_free(path);

	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
		filter = gtk_file_chooser_get_filter(GTK_FILE_CHOOSER(dialog));
		filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
		path = g_path_get_dirname(filename);
		arvo_configuration_set_rkp_save_path(priv->config, path);
		g_free(path);

		rkp = arvo_rkp_read_with_path(filename, &error);

		if (filter != linux_filter)
			arvo_windows_rkp_to_linux(rkp);

		if (error) {
			roccat_warning_dialog(GTK_WINDOW(dialog), _("Could not load profile"), error->message);
			g_error_free(error);
		} else {
			arvo_rkp_set_modified(rkp);
			arvoconfig_profile_page_set_rkp(ARVOCONFIG_PROFILE_PAGE(profile_page), rkp);
			arvo_rkp_free(rkp);
		}

		g_free(filename);
	}

	gtk_widget_destroy(dialog);
}

static void save_all_cb(RoccatConfigWindow *roccat_window, gpointer user_data) {
	save_all(ARVOCONFIG_WINDOW(roccat_window), FALSE);
}

static void menu_edit_preferences_cb(GtkMenuItem *item, gpointer user_data) {
	ArvoconfigWindow *window = ARVOCONFIG_WINDOW(user_data);
	ArvoconfigWindowPrivate *priv = window->priv;
	arvoconfig_configuration_dialog(GTK_WINDOW(window), priv->config);
	arvo_dbus_emit_configuration_changed_outside(priv->dbus_proxy);
}

static void menu_device_info_cb(GtkMenuItem *item, gpointer user_data) {
	ArvoconfigWindow *window = ARVOCONFIG_WINDOW(user_data);
	RoccatInfoDialog *dialog;
	GError *local_error = NULL;
	ArvoInfo *info;
	gchar *firmware;

	if (!roccat_config_window_warn_if_no_device(ROCCAT_CONFIG_WINDOW(window)))
		return;

	info = arvo_info_read(roccat_config_window_get_device(ROCCAT_CONFIG_WINDOW(window)), &local_error);
	if (!roccat_handle_error_dialog(GTK_WINDOW(window), _("Could not read informations"), &local_error))
		return;

	dialog = ROCCAT_INFO_DIALOG(roccat_info_dialog_new(GTK_WINDOW(window)));

	firmware = roccat_firmware_version_to_string(info->firmware_version);
	roccat_info_dialog_add_line(dialog, _("Firmware"), gtk_label_new(firmware));
	g_free(firmware);

	g_free(info);

	(void)gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(GTK_WIDGET(dialog));
}

GtkWidget *arvoconfig_window_new(void) {
	return GTK_WIDGET(g_object_new(ARVOCONFIG_WINDOW_TYPE,
			"device-name", ARVO_DEVICE_NAME,
			NULL));
}

static GObject *constructor(GType gtype, guint n_properties, GObjectConstructParam *properties) {
	GObject *obj;
	ArvoconfigWindow *window;
	ArvoconfigWindowPrivate *priv;
	ArvoRkp *rkp;
	RoccatConfigWindow *roccat_window;
	guint i;
	GtkMenuItem *menu_item;
	GtkWidget *profile_page;

	obj = G_OBJECT_CLASS(arvoconfig_window_parent_class)->constructor(gtype, n_properties, properties);
	window = ARVOCONFIG_WINDOW(obj);
	priv = window->priv;
	roccat_window = ROCCAT_CONFIG_WINDOW(obj);

	priv->config = arvo_configuration_load();

	for (i = 0; i < ARVO_PROFILE_NUM; ++i) {
		profile_page = arvoconfig_profile_page_new();
		roccat_config_window_pages_append_page(ROCCAT_CONFIG_WINDOW_PAGES(roccat_window), ROCCAT_PROFILE_PAGE(profile_page));

		rkp = arvo_rkp_load_actual(i + 1);
		arvoconfig_profile_page_set_rkp(ARVOCONFIG_PROFILE_PAGE(profile_page), rkp);
		arvo_rkp_free(rkp);

		g_signal_connect(G_OBJECT(profile_page), "load-from-file", G_CALLBACK(load_profile_from_file_cb), window);
		g_signal_connect(G_OBJECT(profile_page), "save-to-file", G_CALLBACK(save_profile_to_file_cb), window);
	}

	menu_item = GTK_MENU_ITEM(gtk_image_menu_item_new_from_stock(GTK_STOCK_PREFERENCES, NULL));
	g_signal_connect(G_OBJECT(menu_item), "activate", G_CALLBACK(menu_edit_preferences_cb), window);
	roccat_config_window_edit_menu_append(roccat_window, menu_item);

	menu_item = GTK_MENU_ITEM(gtk_image_menu_item_new_with_label(_("Info")));
	g_signal_connect(G_OBJECT(menu_item), "activate", G_CALLBACK(menu_device_info_cb), window);
	roccat_config_window_device_menu_append(roccat_window, menu_item);

	g_signal_connect(G_OBJECT(roccat_window), "active-changed", G_CALLBACK(window_active_page_changed_cb), NULL);
	g_signal_connect(G_OBJECT(roccat_window), "delete-event", G_CALLBACK(delete_event_cb), NULL);
	g_signal_connect(G_OBJECT(roccat_window), "save-all", G_CALLBACK(save_all_cb), NULL);
	g_signal_connect(G_OBJECT(roccat_window), "device-added", G_CALLBACK(device_add_cb), NULL);
	g_signal_connect(G_OBJECT(roccat_window), "device-removed", G_CALLBACK(device_remove_cb), NULL);

	/* keep this order */
	priv->dbus_proxy = arvo_dbus_proxy_new();

	roccat_config_window_set_device_scanner(roccat_window, ROCCAT_DEVICE_SCANNER_INTERFACE(arvo_device_scanner_new()));

	gtk_widget_show(GTK_WIDGET(window));

	/* if a device is already plugged in, add_cb has already been executed */
	roccat_config_window_warn_if_no_device(roccat_window);

	return obj;
}

static void arvoconfig_window_init(ArvoconfigWindow *window) {
	window->priv = ARVOCONFIG_WINDOW_GET_PRIVATE(window);
}

static void finalize(GObject *object) {
	ArvoconfigWindowPrivate *priv = ARVOCONFIG_WINDOW(object)->priv;

	g_clear_pointer(&priv->dbus_proxy, dbus_roccat_proxy_free);
	arvoconfig_configuration_save(GTK_WINDOW(object), priv->config);
	g_clear_pointer(&priv->config, arvo_configuration_free);

	G_OBJECT_CLASS(arvoconfig_window_parent_class)->finalize(object);
}

static void arvoconfig_window_class_init(ArvoconfigWindowClass *klass) {
	GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

	gobject_class->constructor = constructor;
	gobject_class->finalize = finalize;

	g_type_class_add_private(klass, sizeof(ArvoconfigWindowClass));
}
