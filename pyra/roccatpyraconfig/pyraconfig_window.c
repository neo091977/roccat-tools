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

#include "pyraconfig_window.h"
#include "pyraconfig_configuration_dialog.h"
#include "pyraconfig_profile_page.h"
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

#define PYRACONFIG_WINDOW_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), PYRACONFIG_WINDOW_TYPE, PyraconfigWindowClass))
#define IS_PYRACONFIG_WINDOW_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), PYRACONFIG_WINDOW_TYPE))
#define PYRACONFIG_WINDOW_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), PYRACONFIG_WINDOW_TYPE, PyraconfigWindowPrivate))

typedef struct _PyraconfigWindowClass PyraconfigWindowClass;
typedef struct _PyraconfigWindowPrivate PyraconfigWindowPrivate;

struct _PyraconfigWindow {
	RoccatConfigWindowPages parent;
	PyraconfigWindowPrivate *priv;
};

struct _PyraconfigWindowClass {
	RoccatConfigWindowPagesClass parent_class;
};

struct _PyraconfigWindowPrivate {
	RoccatKeyFile *config;
	DBusGProxy *dbus_proxy;
};

G_DEFINE_TYPE(PyraconfigWindow, pyraconfig_window, ROCCAT_CONFIG_WINDOW_PAGES_TYPE);

static gboolean change_profile_handler(gpointer user_data) {
	RoccatConfigWindow *roccat_window = ROCCAT_CONFIG_WINDOW(user_data);
	guint profile_index = roccat_config_window_pages_get_active_page(ROCCAT_CONFIG_WINDOW_PAGES(roccat_window));
	guint profile_number = profile_index + 1;
	GError *local_error = NULL;

	if (!roccat_config_window_warn_if_no_device(roccat_window))
		return FALSE;

	pyra_actual_profile_write(roccat_config_window_get_device(roccat_window), profile_index, &local_error);
	if (!roccat_handle_error_dialog(GTK_WINDOW(roccat_window), _("Could not activate profile"), &local_error))
		return FALSE;
	pyra_dbus_emit_profile_changed_outside(PYRACONFIG_WINDOW(roccat_window)->priv->dbus_proxy, profile_number);

	return FALSE;
}

static void window_active_page_changed_cb(RoccatConfigWindowPages *roccat_window, gpointer user_data) {
	g_idle_add(change_profile_handler, roccat_window);
}

static void actual_profile_changed_from_device_cb(DBusGProxy *proxy, guchar profile_number, gpointer user_data) {
	roccat_config_window_pages_set_active_page_blocked(ROCCAT_CONFIG_WINDOW_PAGES(user_data), profile_number - 1);
}

static void device_add_cb(RoccatConfigWindow *roccat_window, gpointer user_data) {
	PyraconfigWindow *window = PYRACONFIG_WINDOW(roccat_window);
	PyraconfigWindowPrivate *priv = window->priv;
	GError *local_error = NULL;
	guint i;
	PyraRMP *rmp;
	RoccatDevice *device;
	PyraconfigProfilePage *profile_page;
	guint actual_profile_index;

	device = roccat_config_window_get_device(roccat_window);

	for (i = 0; i < PYRA_PROFILE_NUM; ++i) {
		profile_page = PYRACONFIG_PROFILE_PAGE(roccat_config_window_pages_get_page(ROCCAT_CONFIG_WINDOW_PAGES(roccat_window), i));
		rmp = pyraconfig_profile_page_get_rmp(profile_page);
		pyra_rmp_update_from_device(rmp, device, i);
		pyraconfig_profile_page_set_rmp(profile_page, rmp);
		pyra_rmp_free(rmp);
	}

	actual_profile_index = pyra_actual_profile_read(device, &local_error);
	if (!roccat_handle_error_dialog(GTK_WINDOW(window), _("Could not read actual profile"), &local_error))
		return;
	roccat_config_window_pages_set_active_page_blocked(ROCCAT_CONFIG_WINDOW_PAGES(roccat_window), actual_profile_index);

	dbus_g_proxy_connect_signal(priv->dbus_proxy, "ProfileChanged", G_CALLBACK(actual_profile_changed_from_device_cb), window, NULL);
}

static void device_remove_cb(RoccatConfigWindow *roccat_window, gpointer user_data) {
	PyraconfigWindow *window = PYRACONFIG_WINDOW(roccat_window);
	PyraconfigWindowPrivate *priv = window->priv;

	dbus_g_proxy_disconnect_signal(priv->dbus_proxy, "ProfileChanged", G_CALLBACK(actual_profile_changed_from_device_cb), window);
}

static PyraRMP *get_rmp(PyraconfigWindow *window, PyraconfigProfilePage *profile_page) {
	PyraRMP *rmp;
	gboolean moved;

	rmp = pyraconfig_profile_page_get_rmp(profile_page);
	moved = roccat_config_window_pages_get_page_moved(ROCCAT_CONFIG_WINDOW_PAGES(window), ROCCAT_PROFILE_PAGE(profile_page));

	if (moved)
		pyra_rmp_set_modified(rmp);

	return rmp;
}

static void set_rmp(PyraconfigWindow *window, guint profile_index, PyraRMP *rmp) {
	PyraconfigProfilePage *profile_page = PYRACONFIG_PROFILE_PAGE(roccat_config_window_pages_get_page(ROCCAT_CONFIG_WINDOW_PAGES(window), profile_index));
	guint profile_number = profile_index + 1;
	pyraconfig_profile_page_set_rmp(profile_page, rmp);
	roccat_config_window_pages_set_page_unmoved(ROCCAT_CONFIG_WINDOW_PAGES(window), ROCCAT_PROFILE_PAGE(profile_page));
	pyra_dbus_emit_profile_data_changed_outside(window->priv->dbus_proxy, profile_number);
}

static gboolean save_single(PyraconfigWindow *window, PyraRMP *rmp, guint profile_index, GError **error) {
	RoccatDevice *device = roccat_config_window_get_device(ROCCAT_CONFIG_WINDOW(window));

	if (device)
		pyra_rmp_save(device, rmp, profile_index, error);
	else
		pyra_rmp_save_actual(rmp, profile_index, error);

	if (*error)
		return FALSE;
	else {
		set_rmp(window, profile_index, rmp);
		return TRUE;
	}
}

static gboolean save_all(PyraconfigWindow *window, gboolean ask) {
	PyraRMP *rmps[PYRA_PROFILE_NUM];
	PyraconfigProfilePage *profile_page;
	GError *local_error = NULL;
	gboolean modified;
	gboolean retval;
	guint i;

	modified = FALSE;
	for (i = 0; i < PYRA_PROFILE_NUM; ++i) {
		profile_page = PYRACONFIG_PROFILE_PAGE(roccat_config_window_pages_get_page(ROCCAT_CONFIG_WINDOW_PAGES(window), i));
		rmps[i] = get_rmp(window, profile_page);
		if (pyra_rmp_get_modified(rmps[i]))
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
	for (i = 0; i < PYRA_PROFILE_NUM; ++i) {
		if (!save_single(window, rmps[i], i, &local_error)) {
			retval = FALSE;
			break;
		}
	}
	roccat_handle_error_dialog(GTK_WINDOW(window), _("Could not save unsaved data"), &local_error);

exit:
	for (i = 0; i < PYRA_PROFILE_NUM; ++i)
		pyra_rmp_free(rmps[i]);

	return retval;
}

static gboolean delete_event_cb(GtkWidget *window, GdkEvent *event, gpointer user_data) {
	return !save_all(PYRACONFIG_WINDOW(window), TRUE);
}

static void add_file_filter_to_file_chooser(GtkFileChooser *chooser) {
	GtkFileFilter *filter;
	filter = gtk_file_filter_new();
	gtk_file_filter_set_name(filter, _("Pyra Windows profiles"));
	gtk_file_filter_add_pattern(filter, "*." ROCCAT_MOUSE_PROFILE_EXTENSION);
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(chooser), filter);

	filter = gtk_file_filter_new();
	gtk_file_filter_set_name(filter, _("All files"));
	gtk_file_filter_add_pattern(filter, "*");
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(chooser), filter);
}

static gchar *pyra_create_filename_proposition(PyraRMP *rmp, guint profile_index) {
	gchar *profile_name;
	gchar *filename;

	profile_name = pyra_rmp_get_profile_name(rmp);
	filename = roccat_profile_filename_proposition(profile_name, profile_index, ROCCAT_MOUSE_PROFILE_EXTENSION);
	g_free(profile_name);

	return filename;
}

static void save_profile_to_file_cb(RoccatProfilePage *profile_page, gpointer user_data) {
	PyraconfigWindow *window = PYRACONFIG_WINDOW(user_data);
	PyraconfigWindowPrivate *priv = window->priv;
	GtkWidget *dialog;
	gchar *filename, *path;
	gchar *temp_filename;
	GError *error = NULL;
	guint profile_index = roccat_config_window_pages_get_page_index(ROCCAT_CONFIG_WINDOW_PAGES(window), profile_page);
	PyraRMP *rmp;

	rmp = pyraconfig_profile_page_get_rmp(PYRACONFIG_PROFILE_PAGE(profile_page));

	dialog = gtk_file_chooser_dialog_new(_("Save profile"),
			GTK_WINDOW(window),
			GTK_FILE_CHOOSER_ACTION_SAVE,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
			NULL);

	add_file_filter_to_file_chooser(GTK_FILE_CHOOSER(dialog));

	path = pyra_configuration_get_rmp_save_path(priv->config);
	gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), path);
	g_free(path);

	filename = pyra_create_filename_proposition(rmp, profile_index);
	gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog), filename);
	g_free(filename);

	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
		filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
		temp_filename = roccat_create_filename_with_extension(filename, ROCCAT_MOUSE_PROFILE_EXTENSION);
		g_free(filename);

		path = g_path_get_dirname(temp_filename);
		pyra_configuration_set_rmp_save_path(priv->config, path);
		g_free(path);

		pyra_rmp_write_with_path(temp_filename, rmp, &error);
		g_free(temp_filename);
		if (error) {
			roccat_warning_dialog(GTK_WINDOW(dialog), _("Could not save profile"), error->message);
			g_error_free(error);
		}
	}
	gtk_widget_destroy(dialog);
	pyra_rmp_free(rmp);
}

static void load_profile_from_file_cb(RoccatProfilePage *profile_page, gpointer user_data) {
	PyraconfigWindow *window = PYRACONFIG_WINDOW(user_data);
	PyraconfigWindowPrivate *priv = window->priv;
	GtkWidget *dialog;
	gchar *filename, *path;
	GError *error = NULL;
	PyraRMP *rmp;

	dialog = gtk_file_chooser_dialog_new(_("Load profile"),
			GTK_WINDOW(window),
			GTK_FILE_CHOOSER_ACTION_OPEN,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
			NULL);

	add_file_filter_to_file_chooser(GTK_FILE_CHOOSER(dialog));
	path = pyra_configuration_get_rmp_save_path(priv->config);
	gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), path);
	g_free(path);

	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
		filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
		path = g_path_get_dirname(filename);
		pyra_configuration_set_rmp_save_path(priv->config, path);
		g_free(path);

		rmp = pyra_rmp_read_with_path(filename, &error);
		if (error) {
			roccat_warning_dialog(GTK_WINDOW(dialog), _("Could not load profile"), error->message);
			g_error_free(error);
		} else {
			pyra_rmp_set_modified(rmp);
			pyraconfig_profile_page_set_rmp(PYRACONFIG_PROFILE_PAGE(profile_page), rmp);
			pyra_rmp_free(rmp);
		}

		g_free(filename);
	}

	gtk_widget_destroy(dialog);
}

static void save_all_cb(RoccatConfigWindow *roccat_window, gpointer user_data) {
	save_all(PYRACONFIG_WINDOW(roccat_window), FALSE);
}

static void menu_edit_preferences_cb(GtkMenuItem *item, gpointer user_data) {
	PyraconfigWindow *window = PYRACONFIG_WINDOW(user_data);
	PyraconfigWindowPrivate *priv = window->priv;
	pyraconfig_configuration_dialog(GTK_WINDOW(window), priv->config);
	pyra_dbus_emit_configuration_changed_outside(priv->dbus_proxy);
}

static void menu_device_info_cb(GtkMenuItem *item, gpointer user_data) {
	PyraconfigWindow *window = PYRACONFIG_WINDOW(user_data);
	RoccatInfoDialog *dialog;
	GError *local_error = NULL;
	PyraInfo *info;
	gchar *firmware;

	if (!roccat_config_window_warn_if_no_device(ROCCAT_CONFIG_WINDOW(window)))
		return;

	info = pyra_info_read(roccat_config_window_get_device(ROCCAT_CONFIG_WINDOW(window)), &local_error);
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
	PyraconfigWindow *window = PYRACONFIG_WINDOW(user_data);
	PyraconfigWindowPrivate *priv = window->priv;
	PyraRMP *rmp;
	guint i;
	GError *error = NULL;
	RoccatDevice *device;
	PyraconfigProfilePage *profile_page;

	if (!roccat_config_window_warn_if_no_device(ROCCAT_CONFIG_WINDOW(window)))
		return;

	if (!roccat_continue_dialog(GTK_WINDOW(window), _("This resets the device to factory defaults.")))
		return;

	device = roccat_config_window_get_device(ROCCAT_CONFIG_WINDOW(window));
	pyra_reset(device, &error);
	if (!roccat_handle_error_dialog(GTK_WINDOW(window), _("Could not reset device"), &error))
		return;

	for (i = 0; i < PYRA_PROFILE_NUM; ++i) {
		profile_page = PYRACONFIG_PROFILE_PAGE(roccat_config_window_pages_get_page(ROCCAT_CONFIG_WINDOW_PAGES(window), i));
		rmp = pyra_rmp_load_save_after_reset(device, i, &error);
		pyraconfig_profile_page_set_rmp(profile_page, rmp);
		pyra_rmp_free(rmp);
		pyra_dbus_emit_profile_data_changed_outside(priv->dbus_proxy, i + 1);
	}
}

GtkWidget *pyraconfig_window_new(void) {
	return GTK_WIDGET(g_object_new(PYRACONFIG_WINDOW_TYPE,
			"device-name", PYRA_DEVICE_NAME_COMBINED,
			NULL));
}

static GObject *constructor(GType gtype, guint n_properties, GObjectConstructParam *properties) {
	GObject *obj;
	PyraconfigWindow *window;
	PyraconfigWindowPrivate *priv;
	RoccatConfigWindow *roccat_window;
	PyraRMP *rmp;
	guint i;
	GtkMenuItem *menu_item;
	GtkWidget *profile_page;

	obj = G_OBJECT_CLASS(pyraconfig_window_parent_class)->constructor(gtype, n_properties, properties);
	window = PYRACONFIG_WINDOW(obj);
	priv = window->priv;
	roccat_window = ROCCAT_CONFIG_WINDOW(obj);

	priv->config = pyra_configuration_load();

	for (i = 0; i < PYRA_PROFILE_NUM; ++i) {
		profile_page = pyraconfig_profile_page_new();
		roccat_config_window_pages_append_page(ROCCAT_CONFIG_WINDOW_PAGES(roccat_window), ROCCAT_PROFILE_PAGE(profile_page));

		rmp = pyra_rmp_load_actual(i);
		pyraconfig_profile_page_set_rmp(PYRACONFIG_PROFILE_PAGE(profile_page), rmp);
		pyra_rmp_free(rmp);

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
	priv->dbus_proxy = pyra_dbus_proxy_new();

	roccat_config_window_set_device_scanner(roccat_window, ROCCAT_DEVICE_SCANNER_INTERFACE(pyra_device_scanner_new()));

	gtk_widget_show(GTK_WIDGET(window));

	/* if a device is already plugged in, add_cb has already been executed */
	roccat_config_window_warn_if_no_device(roccat_window);

	return obj;
}

static void pyraconfig_window_init(PyraconfigWindow *window) {
	window->priv = PYRACONFIG_WINDOW_GET_PRIVATE(window);
}

static void finalize(GObject *object) {
	PyraconfigWindowPrivate *priv = PYRACONFIG_WINDOW(object)->priv;

	g_clear_pointer(&priv->dbus_proxy, dbus_roccat_proxy_free);
	pyraconfig_configuration_save(GTK_WINDOW(object), priv->config);
	g_clear_pointer(&priv->config, pyra_configuration_free);

	G_OBJECT_CLASS(pyraconfig_window_parent_class)->finalize(object);
}

static void pyraconfig_window_class_init(PyraconfigWindowClass *klass) {
	GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

	gobject_class->constructor = constructor;
	gobject_class->finalize = finalize;

	g_type_class_add_private(klass, sizeof(PyraconfigWindowClass));
}
