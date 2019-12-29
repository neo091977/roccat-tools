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

#include "suoraconfig_window.h"
#include "suora.h"
#include "suora_config.h"
#include "suora_dbus_services.h"
#include "suora_device.h"
#include "suora_profile_data.h"
#include "suora_profile_page.h"
#include "suora_rkp.h"
#include "suora_info.h"
#include "suora_reset.h"
#include "roccat.h"
#include "roccat_firmware.h"
#include "roccat_helper.h"
#include "roccat_continue_dialog.h"
#include "roccat_info_dialog.h"
#include "roccat_swarm_rmp.h"
#include "roccat_warning_dialog.h"
#include "g_roccat_helper.h"
#include "g_dbus_roccat_helper.h"
#include "i18n.h"

#define SUORACONFIG_WINDOW_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), SUORACONFIG_WINDOW_TYPE, SuoraconfigWindowClass))
#define IS_SUORACONFIG_WINDOW_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), SUORACONFIG_WINDOW_TYPE))
#define SUORACONFIG_WINDOW_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), SUORACONFIG_WINDOW_TYPE, SuoraconfigWindowPrivate))

typedef struct _SuoraconfigWindowClass SuoraconfigWindowClass;
typedef struct _SuoraconfigWindowPrivate SuoraconfigWindowPrivate;

struct _SuoraconfigWindow {
	RoccatConfigWindowList parent;
	SuoraconfigWindowPrivate *priv;
};

struct _SuoraconfigWindowClass {
	RoccatConfigWindowListClass parent_class;
};

struct _SuoraconfigWindowPrivate {
	RoccatKeyFile *config;
	DBusGProxy *dbus_proxy;
};

G_DEFINE_TYPE(SuoraconfigWindow, suoraconfig_window, ROCCAT_CONFIG_WINDOW_LIST_TYPE);

static void add_file_filter_to_file_chooser(GtkFileChooser *chooser) {
	GtkFileFilter *filter;
	filter = gtk_file_filter_new();
	gtk_file_filter_set_name(filter, _("Suora Windows profiles"));
	gtk_file_filter_add_pattern(filter, "*." ROCCAT_KEYBOARD_PROFILE_EXTENSION);
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(chooser), filter);

	filter = gtk_file_filter_new();
	gtk_file_filter_set_name(filter, _("All files"));
	gtk_file_filter_add_pattern(filter, "*");
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(chooser), filter);
}

static gchar *suora_create_filename_proposition(SuoraProfileData const *profile_data) {
	return roccat_profile_filename_proposition(gaminggear_profile_data_get_name(GAMINGGEAR_PROFILE_DATA(profile_data)), 0, ROCCAT_KEYBOARD_PROFILE_EXTENSION);
}

static void import_cb(GaminggearProfilePage *profile_page, gpointer user_data) {
	SuoraconfigWindow *window = SUORACONFIG_WINDOW(user_data);
	SuoraconfigWindowPrivate *priv = window->priv;
	GtkWidget *dialog;
	gchar *filename, *path;
	GError *error = NULL;
	gpointer rkp;
	gsize length;
	GaminggearProfileData *profile_data;
	gboolean retval;

	dialog = gtk_file_chooser_dialog_new(_("Load profile"),
			GTK_WINDOW(window),
			GTK_FILE_CHOOSER_ACTION_OPEN,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
			NULL);

	add_file_filter_to_file_chooser(GTK_FILE_CHOOSER(dialog));
	path = suora_configuration_get_rkp_path(priv->config);
	gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), path);
	g_free(path);

	if (gtk_dialog_run(GTK_DIALOG(dialog)) != GTK_RESPONSE_ACCEPT)
		goto out1;

	filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
	path = g_path_get_dirname(filename);
	suora_configuration_set_rkp_path(priv->config, path);
	g_free(path);

	rkp = roccat_swarm_rmp_read_with_path(filename, &length, &error);
	g_free(filename);
	if (error) {
		roccat_warning_dialog(GTK_WINDOW(dialog), _("Could not load profile"), error->message);
		g_error_free(error);
		goto out1;
	}

	gaminggear_profile_page_update_data(profile_page);
	profile_data = gaminggear_profile_page_get_profile_data(profile_page);
	retval = suora_rkp_to_profile_data(SUORA_PROFILE_DATA(profile_data), rkp, length);
	g_free(rkp);
	if (!retval) {
		roccat_warning_dialog(GTK_WINDOW(dialog), _("Could not convert profile"), NULL);
		goto out2;
	}

	gaminggear_profile_page_set_profile_data(profile_page, profile_data);

out2:
	g_object_unref(profile_data);
out1:
	gtk_widget_destroy(dialog);
}

static void export_cb(GaminggearProfilePage *profile_page, gpointer user_data) {
	SuoraconfigWindow *window = SUORACONFIG_WINDOW(user_data);
	SuoraconfigWindowPrivate *priv = window->priv;
	GtkWidget *dialog;
	gchar *filename, *path;
	gchar *temp_filename;
	GError *error = NULL;
	gpointer rkp;
	gsize length;
	GaminggearProfileData *profile_data;

	gaminggear_profile_page_update_data(profile_page);
	profile_data = gaminggear_profile_page_get_profile_data(profile_page);

	dialog = gtk_file_chooser_dialog_new(_("Save profile"),
			GTK_WINDOW(window),
			GTK_FILE_CHOOSER_ACTION_SAVE,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
			NULL);

	add_file_filter_to_file_chooser(GTK_FILE_CHOOSER(dialog));

	path = suora_configuration_get_rkp_path(priv->config);
	gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), path);
	g_free(path);

	filename = suora_create_filename_proposition(SUORA_PROFILE_DATA(profile_data));
	gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog), filename);
	g_free(filename);

	if (gtk_dialog_run(GTK_DIALOG(dialog)) != GTK_RESPONSE_ACCEPT)
		goto out;

	filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
	temp_filename = roccat_create_filename_with_extension(filename, ROCCAT_KEYBOARD_PROFILE_EXTENSION);
	g_free(filename);

	path = g_path_get_dirname(temp_filename);
	suora_configuration_set_rkp_path(priv->config, path);
	g_free(path);

	rkp = suora_profile_data_to_rkp(SUORA_PROFILE_DATA(profile_data), &length);
	if (!rkp) {
		roccat_warning_dialog(GTK_WINDOW(dialog), _("Could not convert profile"), NULL);
		goto free_filename;
	}

	roccat_swarm_rmp_write_with_path(temp_filename, rkp, length, &error);
	g_free(rkp);
	if (error) {
		roccat_warning_dialog(GTK_WINDOW(dialog), _("Could not save profile"), error->message);
		g_error_free(error);
	}

free_filename:
	g_free(temp_filename);
out:
	g_object_unref(profile_data);
	gtk_widget_destroy(dialog);
}

static void profile_data_changed_cb(RoccatConfigWindowList *roccat_window_list, gpointer user_data) {
	suora_dbus_emit_profile_data_changed_outside(SUORACONFIG_WINDOW(roccat_window_list)->priv->dbus_proxy);
}

// TODO move to RoccatConfigWindowList? (mind variable profile_count!)
static void device_add_cb(RoccatConfigWindow *roccat_window, gpointer user_data) {
	RoccatConfigWindowList *list_window = ROCCAT_CONFIG_WINDOW_LIST(roccat_window);
	GError *local_error = NULL;
	RoccatDevice *device;

	device = roccat_config_window_get_device(roccat_window);
	roccat_config_window_list_read(list_window, device, 1, &local_error);
	roccat_handle_error_dialog(GTK_WINDOW(roccat_window), _("Could not load hardware data for profile"), &local_error);
}

static void menu_device_info_cb(GtkMenuItem *item, gpointer user_data) {
	SuoraconfigWindow *window = SUORACONFIG_WINDOW(user_data);
	RoccatInfoDialog *dialog;
	GError *local_error = NULL;
	SuoraInfo *info;
	gchar *string;

	if (!roccat_config_window_warn_if_no_device(ROCCAT_CONFIG_WINDOW(window)))
		return;

	info = suora_info_read(roccat_config_window_get_device(ROCCAT_CONFIG_WINDOW(window)), &local_error);
	if (!roccat_handle_error_dialog(GTK_WINDOW(window), _("Could not read informations"), &local_error))
		return;

	dialog = ROCCAT_INFO_DIALOG(roccat_info_dialog_new(GTK_WINDOW(window)));

	string = suora_firmware_version_to_string(info);
	roccat_info_dialog_add_line(dialog, _("Firmware version"), gtk_label_new(string));
	g_free(string);

	g_free(info);

	(void)gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(GTK_WIDGET(dialog));
}

static void menu_device_reset_cb(GtkMenuItem *item, gpointer user_data) {
	SuoraconfigWindow *window = SUORACONFIG_WINDOW(user_data);
	GError *error = NULL;
	RoccatDevice *device;

	if (!roccat_config_window_warn_if_no_device(ROCCAT_CONFIG_WINDOW(window)))
		return;

	if (!roccat_continue_dialog(GTK_WINDOW(window), _("This resets the device to factory defaults.")))
		return;

	device = roccat_config_window_get_device(ROCCAT_CONFIG_WINDOW(window));
	suora_reset_write(device, &error);
	if (!roccat_handle_error_dialog(GTK_WINDOW(window), _("Could not reset device"), &error))
		return;

	suora_dbus_emit_profile_data_changed_outside(window->priv->dbus_proxy);
}

GtkWidget *suoraconfig_window_new(void) {
	gchar *path;
	GtkWidget *widget;

	path = suora_profile_data_path();
	widget =  GTK_WIDGET(g_object_new(SUORACONFIG_WINDOW_TYPE,
			"device-name", SUORA_DEVICE_NAME_COMBINED,
			"profile-type", SUORA_PROFILE_DATA_TYPE,
			"profile_page", GAMINGGEAR_PROFILE_PAGE(suora_profile_page_new()),
			"num-profiles", 1,
			"path", path,
			NULL));

	g_free(path);
	return widget;
}

static GObject *constructor(GType gtype, guint n_properties, GObjectConstructParam *properties) {
	GObject *obj;
	SuoraconfigWindow *window;
	RoccatConfigWindow *roccat_window;
	SuoraconfigWindowPrivate *priv;
	GtkMenuItem *menu_item;
	GError *local_error = NULL;
	GaminggearProfilePage *profile_page;
	gchar *profile_name;

	obj = G_OBJECT_CLASS(suoraconfig_window_parent_class)->constructor(gtype, n_properties, properties);
	window = SUORACONFIG_WINDOW(obj);
	roccat_window = ROCCAT_CONFIG_WINDOW(obj);
	priv = window->priv;

	priv->config = suora_configuration_load();

	menu_item = GTK_MENU_ITEM(gtk_image_menu_item_new_with_label(_("Reset")));
	g_signal_connect(G_OBJECT(menu_item), "activate", G_CALLBACK(menu_device_reset_cb), window);
	roccat_config_window_device_menu_append(roccat_window, menu_item);

	menu_item = GTK_MENU_ITEM(gtk_image_menu_item_new_with_label(_("Info")));
	g_signal_connect(G_OBJECT(menu_item), "activate", G_CALLBACK(menu_device_info_cb), window);
	roccat_config_window_device_menu_append(roccat_window, menu_item);

	profile_page = roccat_config_window_list_get_profile_page(ROCCAT_CONFIG_WINDOW_LIST(window));
	g_signal_connect(G_OBJECT(profile_page), "import", G_CALLBACK(import_cb), window);
	g_signal_connect(G_OBJECT(profile_page), "export", G_CALLBACK(export_cb), window);
	suora_profile_page_set_keyboard_layout(SUORA_PROFILE_PAGE(profile_page), suora_configuration_get_layout(priv->config));
	g_object_unref(profile_page);

	/* load profile before initializing device scanner */
	roccat_config_window_list_load(ROCCAT_CONFIG_WINDOW_LIST(window), &local_error);
	roccat_handle_error_dialog(GTK_WINDOW(window), _("Could not load filesystem data for profile"), &local_error);

	profile_name = suora_configuration_get_default_profile_name(priv->config);
	roccat_config_window_list_set_default_profile_name(ROCCAT_CONFIG_WINDOW_LIST(window), profile_name);
	g_free(profile_name);
	// FIXME get and save config desktop_rpfoile

	g_signal_connect(G_OBJECT(window), "device-added", G_CALLBACK(device_add_cb), NULL);
	g_signal_connect(G_OBJECT(window), "profile-data-changed", G_CALLBACK(profile_data_changed_cb), NULL);

	/* keep this order */
	priv->dbus_proxy = suora_dbus_proxy_new();

	roccat_config_window_set_device_scanner(roccat_window, ROCCAT_DEVICE_SCANNER_INTERFACE(suora_device_scanner_new()));

	gtk_widget_show(GTK_WIDGET(window));

	/* if a device is already plugged in, add_cb has already been executed */
	roccat_config_window_warn_if_no_device(roccat_window);

	return obj;
}

static void suoraconfig_window_init(SuoraconfigWindow *window) {
	window->priv = SUORACONFIG_WINDOW_GET_PRIVATE(window);
}

gboolean configuration_save(SuoraconfigWindow *window) {
	SuoraconfigWindowPrivate *priv = window->priv;
	GError *local_error = NULL;
	gchar const *profile_name;

	profile_name = roccat_config_window_list_get_default_profile_name(ROCCAT_CONFIG_WINDOW_LIST(window));
	suora_configuration_set_default_profile_name(priv->config, profile_name);

	suora_configuration_save(priv->config, &local_error);
	return roccat_handle_error_dialog(GTK_WINDOW(window), _("Could not save configuration"), &local_error);
}

static void finalize(GObject *object) {
	SuoraconfigWindow *config_window = SUORACONFIG_WINDOW(object);
	SuoraconfigWindowPrivate *priv = config_window->priv;

	configuration_save(config_window);
	suora_dbus_emit_configuration_changed_outside(priv->dbus_proxy);
	g_clear_pointer(&priv->config, roccat_configuration_free);
	g_clear_pointer(&priv->dbus_proxy, dbus_roccat_proxy_free);

	G_OBJECT_CLASS(suoraconfig_window_parent_class)->finalize(object);
}

static void suoraconfig_window_class_init(SuoraconfigWindowClass *klass) {
	GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

	gobject_class->constructor = constructor;
	gobject_class->finalize = finalize;

	g_type_class_add_private(klass, sizeof(SuoraconfigWindowClass));
}
