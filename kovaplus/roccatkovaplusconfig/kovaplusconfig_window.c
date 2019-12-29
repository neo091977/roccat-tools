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

#include "kovaplusconfig_window.h"
#include "kovaplusconfig_configuration_dialog.h"
#include "kovaplusconfig_profile_page.h"
#include "roccat_warning_dialog.h"
#include "roccat_save_dialog.h"
#include "roccat_continue_dialog.h"
#include "roccat_info_dialog.h"
#include "g_roccat_helper.h"
#include "g_dbus_roccat_helper.h"
#include "roccat_helper.h"
#include "roccat.h"
#include "roccat_firmware.h"
#include "i18n.h"

#define KOVAPLUSCONFIG_WINDOW_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), KOVAPLUSCONFIG_WINDOW_TYPE, KovaplusconfigWindowClass))
#define IS_KOVAPLUSCONFIG_WINDOW_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), KOVAPLUSCONFIG_WINDOW_TYPE))
#define KOVAPLUSCONFIG_WINDOW_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), KOVAPLUSCONFIG_WINDOW_TYPE, KovaplusconfigWindowPrivate))

typedef struct _KovaplusconfigWindowClass KovaplusconfigWindowClass;
typedef struct _KovaplusconfigWindowPrivate KovaplusconfigWindowPrivate;

struct _KovaplusconfigWindow {
	RoccatConfigWindowPages parent;
	KovaplusconfigWindowPrivate *priv;
};

struct _KovaplusconfigWindowClass {
	RoccatConfigWindowPagesClass parent_class;
};

struct _KovaplusconfigWindowPrivate {
	RoccatKeyFile *config;
	DBusGProxy *dbus_proxy;
};

G_DEFINE_TYPE(KovaplusconfigWindow, kovaplusconfig_window, ROCCAT_CONFIG_WINDOW_PAGES_TYPE);

static gboolean change_profile_handler(gpointer user_data) {
	RoccatConfigWindow *roccat_window = ROCCAT_CONFIG_WINDOW(user_data);
	guint profile_index = roccat_config_window_pages_get_active_page(ROCCAT_CONFIG_WINDOW_PAGES(roccat_window));
	guint profile_number = profile_index + 1;
	GError *local_error = NULL;

	if (!roccat_config_window_warn_if_no_device(roccat_window))
		return FALSE;

	kovaplus_actual_profile_write(roccat_config_window_get_device(roccat_window), profile_index, &local_error);
	if (!roccat_handle_error_dialog(GTK_WINDOW(roccat_window), _("Could not activate profile"), &local_error))
		return FALSE;
	kovaplus_dbus_emit_profile_changed_outside(KOVAPLUSCONFIG_WINDOW(roccat_window)->priv->dbus_proxy, profile_number);

	return FALSE;
}

static void window_active_page_changed_cb(RoccatConfigWindowPages *roccat_window, gpointer user_data) {
	g_idle_add(change_profile_handler, roccat_window);
}

static void actual_profile_changed_from_device_cb(DBusGProxy *proxy, guchar profile_number, gpointer user_data) {
	roccat_config_window_pages_set_active_page_blocked(ROCCAT_CONFIG_WINDOW_PAGES(user_data), profile_number - 1);
}

static void device_add_cb(RoccatConfigWindow *roccat_window, gpointer user_data) {
	KovaplusconfigWindow *window = KOVAPLUSCONFIG_WINDOW(roccat_window);
	KovaplusconfigWindowPrivate *priv = window->priv;
	GError *local_error = NULL;
	guint i;
	KovaplusRmp *rmp;
	RoccatDevice *device;
	KovaplusconfigProfilePage *profile_page;
	guint actual_profile_index;

	device = roccat_config_window_get_device(roccat_window);

	for (i = 0; i < KOVAPLUS_PROFILE_NUM; ++i) {
		profile_page = KOVAPLUSCONFIG_PROFILE_PAGE(roccat_config_window_pages_get_page(ROCCAT_CONFIG_WINDOW_PAGES(window), i));
		rmp = kovaplusconfig_profile_page_get_rmp(profile_page);
		kovaplus_rmp_update_from_device(rmp, device, i);
		kovaplusconfig_profile_page_set_rmp(profile_page, rmp);
		kovaplus_rmp_free(rmp);
	}

	actual_profile_index = kovaplus_actual_profile_read(device, &local_error);
	if (!roccat_handle_error_dialog(GTK_WINDOW(window), _("Could not read actual profile"), &local_error))
		return;
	roccat_config_window_pages_set_active_page_blocked(ROCCAT_CONFIG_WINDOW_PAGES(window), actual_profile_index);

	dbus_g_proxy_connect_signal(priv->dbus_proxy, "ProfileChanged", G_CALLBACK(actual_profile_changed_from_device_cb), window, NULL);
}

static void device_remove_cb(RoccatConfigWindow *roccat_window, gpointer user_data) {
	KovaplusconfigWindow *window = KOVAPLUSCONFIG_WINDOW(roccat_window);
	KovaplusconfigWindowPrivate *priv = window->priv;

	dbus_g_proxy_disconnect_signal(priv->dbus_proxy, "ProfileChanged", G_CALLBACK(actual_profile_changed_from_device_cb), window);
}

static KovaplusRmp *get_rmp(KovaplusconfigWindow *window, KovaplusconfigProfilePage *profile_page) {
	KovaplusRmp *rmp;
	gboolean moved;

	rmp = kovaplusconfig_profile_page_get_rmp(profile_page);
	moved = roccat_config_window_pages_get_page_moved(ROCCAT_CONFIG_WINDOW_PAGES(window), ROCCAT_PROFILE_PAGE(profile_page));

	if (moved)
		kovaplus_rmp_set_modified(rmp);

	return rmp;
}

static void set_rmp(KovaplusconfigWindow *window, guint profile_index, KovaplusRmp *rmp) {
	KovaplusconfigProfilePage *profile_page = KOVAPLUSCONFIG_PROFILE_PAGE(roccat_config_window_pages_get_page(ROCCAT_CONFIG_WINDOW_PAGES(window), profile_index));
	guint profile_number = profile_index + 1;
	kovaplusconfig_profile_page_set_rmp(profile_page, rmp);
	roccat_config_window_pages_set_page_unmoved(ROCCAT_CONFIG_WINDOW_PAGES(window), ROCCAT_PROFILE_PAGE(profile_page));
	kovaplus_dbus_emit_profile_data_changed_outside(window->priv->dbus_proxy, profile_number);
}

static gboolean save_single(KovaplusconfigWindow *window, KovaplusRmp *rmp, guint profile_index, GError **error) {
	RoccatDevice *device = roccat_config_window_get_device(ROCCAT_CONFIG_WINDOW(window));

	if (device)
		kovaplus_rmp_save(device, rmp, profile_index, error);
	else
		kovaplus_rmp_save_actual(rmp, profile_index, error);

	if (*error)
		return FALSE;
	else {
		set_rmp(window, profile_index, rmp);
		return TRUE;
	}
}

static gboolean save_all(KovaplusconfigWindow *window, gboolean ask) {
	KovaplusRmp *rmps[KOVAPLUS_PROFILE_NUM];
	KovaplusconfigProfilePage *profile_page;
	GError *local_error = NULL;
	gboolean modified;
	gboolean retval;
	guint i;

	modified = FALSE;
	for (i = 0; i < KOVAPLUS_PROFILE_NUM; ++i) {
		profile_page = KOVAPLUSCONFIG_PROFILE_PAGE(roccat_config_window_pages_get_page(ROCCAT_CONFIG_WINDOW_PAGES(window), i));
		rmps[i] = get_rmp(window, profile_page);
		if (kovaplus_rmp_get_modified(rmps[i]))
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
	for (i = 0; i < KOVAPLUS_PROFILE_NUM; ++i) {
		if (!save_single(window, rmps[i], i, &local_error)) {
			retval = FALSE;
			break;
		}
	}
	roccat_handle_error_dialog(GTK_WINDOW(window), _("Could not save unsaved data"), &local_error);

exit:
	for (i = 0; i < KOVAPLUS_PROFILE_NUM; ++i)
		kovaplus_rmp_free(rmps[i]);

	return retval;
}

static gboolean delete_event_cb(GtkWidget *window, GdkEvent *event, gpointer user_data) {
	return !save_all(KOVAPLUSCONFIG_WINDOW(window), TRUE);
}

static void add_file_filter_to_file_chooser(GtkFileChooser *chooser) {
	GtkFileFilter *filter;
	filter = gtk_file_filter_new();
	gtk_file_filter_set_name(filter, _("Kova[+] Windows profiles"));
	gtk_file_filter_add_pattern(filter, "*." ROCCAT_MOUSE_PROFILE_EXTENSION);
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(chooser), filter);

	filter = gtk_file_filter_new();
	gtk_file_filter_set_name(filter, _("All files"));
	gtk_file_filter_add_pattern(filter, "*");
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(chooser), filter);
}

static gchar *kovaplus_create_filename_proposition(KovaplusRmp *rmp, guint profile_index) {
	gchar *profile_name;
	gchar *filename;

	profile_name = kovaplus_rmp_get_profile_name(rmp);
	filename = roccat_profile_filename_proposition(profile_name, profile_index, ROCCAT_MOUSE_PROFILE_EXTENSION);
	g_free(profile_name);

	return filename;
}

static void save_profile_to_file_cb(RoccatProfilePage *profile_page, gpointer user_data) {
	KovaplusconfigWindow *window = KOVAPLUSCONFIG_WINDOW(user_data);
	KovaplusconfigWindowPrivate *priv = window->priv;
	GtkWidget *dialog;
	gchar *filename, *path;
	gchar *temp_filename;
	GError *error = NULL;
	guint profile_index = roccat_config_window_pages_get_page_index(ROCCAT_CONFIG_WINDOW_PAGES(window), profile_page);
	KovaplusRmp *rmp;

	rmp = kovaplusconfig_profile_page_get_rmp(KOVAPLUSCONFIG_PROFILE_PAGE(profile_page));

	dialog = gtk_file_chooser_dialog_new(_("Save profile"),
			GTK_WINDOW(window),
			GTK_FILE_CHOOSER_ACTION_SAVE,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
			NULL);

	add_file_filter_to_file_chooser(GTK_FILE_CHOOSER(dialog));

	path = kovaplus_configuration_get_rmp_save_path(priv->config);
	gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), path);
	g_free(path);

	filename = kovaplus_create_filename_proposition(rmp, profile_index);
	gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog), filename);
	g_free(filename);

	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
		filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
		temp_filename = roccat_create_filename_with_extension(filename, ROCCAT_MOUSE_PROFILE_EXTENSION);
		g_free(filename);

		path = g_path_get_dirname(temp_filename);
		kovaplus_configuration_set_rmp_save_path(priv->config, path);
		g_free(path);

		kovaplus_rmp_write_with_path(temp_filename, rmp, &error);
		g_free(temp_filename);
		if (error) {
			roccat_warning_dialog(GTK_WINDOW(dialog), _("Could not save profile"), error->message);
			g_error_free(error);
		}
	}
	gtk_widget_destroy(dialog);
	kovaplus_rmp_free(rmp);
}

static void load_profile_from_file_cb(RoccatProfilePage *profile_page, gpointer user_data) {
	KovaplusconfigWindow *window = KOVAPLUSCONFIG_WINDOW(user_data);
	KovaplusconfigWindowPrivate *priv = window->priv;
	GtkWidget *dialog;
	gchar *filename, *path;
	GError *error = NULL;
	KovaplusRmp *rmp;

	dialog = gtk_file_chooser_dialog_new(_("Load profile"),
			GTK_WINDOW(window),
			GTK_FILE_CHOOSER_ACTION_OPEN,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
			NULL);

	add_file_filter_to_file_chooser(GTK_FILE_CHOOSER(dialog));
	path = kovaplus_configuration_get_rmp_save_path(priv->config);
	gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), path);
	g_free(path);

	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
		filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
		path = g_path_get_dirname(filename);
		kovaplus_configuration_set_rmp_save_path(priv->config, path);
		g_free(path);

		rmp = kovaplus_rmp_read_with_path(filename, &error);
		if (error) {
			roccat_warning_dialog(GTK_WINDOW(dialog), _("Could not load profile"), error->message);
			g_error_free(error);
		} else {
			kovaplus_rmp_set_modified(rmp);
			kovaplusconfig_profile_page_set_rmp(KOVAPLUSCONFIG_PROFILE_PAGE(profile_page), rmp);
			kovaplus_rmp_free(rmp);
		}

		g_free(filename);
	}

	gtk_widget_destroy(dialog);
}

static void save_all_cb(RoccatConfigWindow *roccat_window, gpointer user_data) {
	save_all(KOVAPLUSCONFIG_WINDOW(roccat_window), FALSE);
}

static void menu_edit_preferences_cb(GtkMenuItem *item, gpointer user_data) {
	KovaplusconfigWindow *window = KOVAPLUSCONFIG_WINDOW(user_data);
	KovaplusconfigWindowPrivate *priv = window->priv;
	kovaplusconfig_configuration_dialog(GTK_WINDOW(window), priv->config);
	kovaplus_dbus_emit_configuration_changed_outside(priv->dbus_proxy);
}

static void menu_device_info_cb(GtkMenuItem *item, gpointer user_data) {
	KovaplusconfigWindow *window = KOVAPLUSCONFIG_WINDOW(user_data);
	RoccatInfoDialog *dialog;
	GError *local_error = NULL;
	KovaplusInfo *info;
	gchar *firmware;

	if (!roccat_config_window_warn_if_no_device(ROCCAT_CONFIG_WINDOW(window)))
		return;

	info = kovaplus_info_read(roccat_config_window_get_device(ROCCAT_CONFIG_WINDOW(window)), &local_error);
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

static void menu_device_reset_cb(GtkMenuItem *item, gpointer user_data) {
	KovaplusconfigWindow *window = KOVAPLUSCONFIG_WINDOW(user_data);
	KovaplusconfigWindowPrivate *priv = window->priv;
	KovaplusRmp *rmp;
	guint i;
	GError *error = NULL;
	RoccatDevice *device;
	KovaplusconfigProfilePage *profile_page;

	if (!roccat_config_window_warn_if_no_device(ROCCAT_CONFIG_WINDOW(window)))
		return;

	if (!roccat_continue_dialog(GTK_WINDOW(window), _("This resets the device to factory defaults.")))
		return;

	device = roccat_config_window_get_device(ROCCAT_CONFIG_WINDOW(window));
	kovaplus_reset(device, &error);
	if (!roccat_handle_error_dialog(GTK_WINDOW(window), _("Could not reset device"), &error))
		return;

	for (i = 0; i < KOVAPLUS_PROFILE_NUM; ++i) {
		profile_page = KOVAPLUSCONFIG_PROFILE_PAGE(roccat_config_window_pages_get_page(ROCCAT_CONFIG_WINDOW_PAGES(window), i));
		rmp = kovaplus_rmp_load_save_after_reset(device, i, &error);
		kovaplusconfig_profile_page_set_rmp(profile_page, rmp);
		kovaplus_rmp_free(rmp);
		kovaplus_dbus_emit_profile_data_changed_outside(priv->dbus_proxy, i + 1);
	}
}

GtkWidget *kovaplusconfig_window_new(void) {
	return GTK_WIDGET(g_object_new(KOVAPLUSCONFIG_WINDOW_TYPE,
			"device-name", KOVAPLUS_DEVICE_NAME,
			NULL));
}

static GObject *constructor(GType gtype, guint n_properties, GObjectConstructParam *properties) {
	GObject *obj;
	KovaplusconfigWindow *window;
	KovaplusconfigWindowPrivate *priv;
	RoccatConfigWindow *roccat_window;
	KovaplusRmp *rmp;
	guint i;
	GtkMenuItem *menu_item;
	GtkWidget *profile_page;

	obj = G_OBJECT_CLASS(kovaplusconfig_window_parent_class)->constructor(gtype, n_properties, properties);
	window = KOVAPLUSCONFIG_WINDOW(obj);
	priv = window->priv;
	roccat_window = ROCCAT_CONFIG_WINDOW(obj);

	priv->config = kovaplus_configuration_load();

	for (i = 0; i < KOVAPLUS_PROFILE_NUM; ++i) {
		profile_page = kovaplusconfig_profile_page_new();
		roccat_config_window_pages_append_page(ROCCAT_CONFIG_WINDOW_PAGES(roccat_window), ROCCAT_PROFILE_PAGE(profile_page));

		rmp = kovaplus_rmp_load_actual(i);
		kovaplusconfig_profile_page_set_rmp(KOVAPLUSCONFIG_PROFILE_PAGE(profile_page), rmp);
		kovaplus_rmp_free(rmp);

		g_signal_connect(G_OBJECT(profile_page), "load-from-file", G_CALLBACK(load_profile_from_file_cb), window);
		g_signal_connect(G_OBJECT(profile_page), "save-to-file", G_CALLBACK(save_profile_to_file_cb), window);
	}

	menu_item = GTK_MENU_ITEM(gtk_image_menu_item_new_from_stock(GTK_STOCK_PREFERENCES, NULL));
	g_signal_connect(G_OBJECT(menu_item), "activate", G_CALLBACK(menu_edit_preferences_cb), window);
	roccat_config_window_edit_menu_append(roccat_window, menu_item);

	menu_item = GTK_MENU_ITEM(gtk_image_menu_item_new_with_label(_("Reset")));
	g_signal_connect(G_OBJECT(menu_item), "activate", G_CALLBACK(menu_device_reset_cb), window);
	roccat_config_window_device_menu_append(roccat_window, menu_item);

	menu_item = GTK_MENU_ITEM(gtk_image_menu_item_new_with_label(_("Info")));
	g_signal_connect(G_OBJECT(menu_item), "activate", G_CALLBACK(menu_device_info_cb), window);
	roccat_config_window_device_menu_append(roccat_window, menu_item);

	g_signal_connect(G_OBJECT(roccat_window), "active-changed", G_CALLBACK(window_active_page_changed_cb), NULL);
	g_signal_connect(G_OBJECT(roccat_window), "delete-event", G_CALLBACK(delete_event_cb), NULL);
	g_signal_connect(G_OBJECT(roccat_window), "save-all", G_CALLBACK(save_all_cb), NULL);
	g_signal_connect(G_OBJECT(roccat_window), "device-added", G_CALLBACK(device_add_cb), NULL);
	g_signal_connect(G_OBJECT(roccat_window), "device-removed", G_CALLBACK(device_remove_cb), NULL);

	/* keep this order */
	priv->dbus_proxy = kovaplus_dbus_proxy_new();

	roccat_config_window_set_device_scanner(roccat_window, ROCCAT_DEVICE_SCANNER_INTERFACE(kovaplus_device_scanner_new()));

	gtk_widget_show(GTK_WIDGET(window));

	/* if a device is already plugged in, add_cb has already been executed */
	roccat_config_window_warn_if_no_device(roccat_window);

	return obj;
}

static void kovaplusconfig_window_init(KovaplusconfigWindow *window) {
	window->priv = KOVAPLUSCONFIG_WINDOW_GET_PRIVATE(window);
}

static void finalize(GObject *object) {
	KovaplusconfigWindowPrivate *priv = KOVAPLUSCONFIG_WINDOW(object)->priv;

	g_clear_pointer(&priv->dbus_proxy, dbus_roccat_proxy_free);
	kovaplusconfig_configuration_save(GTK_WINDOW(object), priv->config);
	g_clear_pointer(&priv->config, kovaplus_configuration_free);

	G_OBJECT_CLASS(kovaplusconfig_window_parent_class)->finalize(object);
}

static void kovaplusconfig_window_class_init(KovaplusconfigWindowClass *klass) {
	GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

	gobject_class->constructor = constructor;
	gobject_class->finalize = finalize;

	g_type_class_add_private(klass, sizeof(KovaplusconfigWindowClass));
}
