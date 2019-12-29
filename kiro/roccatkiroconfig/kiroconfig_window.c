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

#include "kiroconfig_window.h"
#include "kiro_configuration_dialog.h"
#include "kiro_profile_page.h"
#include "kiro_config.h"
#include "kiro_device.h"
#include "kiro_firmware.h"
#include "kiro_profile_data.h"
#include "kiro_profile.h"
#include "kiro_dbus_services.h"
#include "kiro_profile.h"
#include "kiro_rmp.h"
#include "kiro_info.h"
#include "roccat_helper.h"
#include "roccat_warning_dialog.h"
#include "roccat_save_dialog.h"
#include "roccat_continue_dialog.h"
#include "roccat_info_dialog.h"
#include "roccat_update_assistant.h"
#include "roccat_swarm_rmp.h"
#include "g_roccat_helper.h"
#include "g_dbus_roccat_helper.h"
#include "roccat.h"
#include "i18n.h"

#define KIROCONFIG_WINDOW_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), KIROCONFIG_WINDOW_TYPE, KiroconfigWindowClass))
#define IS_KIROCONFIG_WINDOW_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), KIROCONFIG_WINDOW_TYPE))
#define KIROCONFIG_WINDOW_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), KIROCONFIG_WINDOW_TYPE, KiroconfigWindowPrivate))

typedef struct _KiroconfigWindowClass KiroconfigWindowClass;
typedef struct _KiroconfigWindowPrivate KiroconfigWindowPrivate;

struct _KiroconfigWindow {
	RoccatConfigWindowPages parent;
	KiroconfigWindowPrivate *priv;
};

struct _KiroconfigWindowClass {
	RoccatConfigWindowPagesClass parent_class;
};

struct _KiroconfigWindowPrivate {
	RoccatKeyFile *config;
	DBusGProxy *dbus_proxy;
};

G_DEFINE_TYPE(KiroconfigWindow, kiroconfig_window, ROCCAT_CONFIG_WINDOW_PAGES_TYPE);

static void add_file_filter_to_file_chooser(GtkFileChooser *chooser) {
	GtkFileFilter *filter;
	filter = gtk_file_filter_new();
	gtk_file_filter_set_name(filter, _("Kiro Windows profiles"));
	gtk_file_filter_add_pattern(filter, "*." ROCCAT_MOUSE_PROFILE_EXTENSION);
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(chooser), filter);

	filter = gtk_file_filter_new();
	gtk_file_filter_set_name(filter, _("All files"));
	gtk_file_filter_add_pattern(filter, "*");
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(chooser), filter);
}

static gchar *kiro_create_filename_proposition(KiroProfileData const *profile_data) {
	return roccat_profile_filename_proposition(profile_data->eventhandler.profile_name, 0, ROCCAT_MOUSE_PROFILE_EXTENSION);
}

static void load_profile_from_file_cb(RoccatProfilePage *profile_page, gpointer user_data) {
	KiroconfigWindow *window = KIROCONFIG_WINDOW(user_data);
	KiroconfigWindowPrivate *priv = window->priv;
	GtkWidget *dialog;
	gchar *filename, *path;
	GError *error = NULL;
	gpointer rmp;
	gsize length;
	KiroProfileData *profile_data;

	dialog = gtk_file_chooser_dialog_new(_("Load profile"),
			GTK_WINDOW(window),
			GTK_FILE_CHOOSER_ACTION_OPEN,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
			NULL);

	add_file_filter_to_file_chooser(GTK_FILE_CHOOSER(dialog));
	path = kiro_configuration_get_rmp_path(priv->config);
	gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), path);
	g_free(path);

	if (gtk_dialog_run(GTK_DIALOG(dialog)) != GTK_RESPONSE_ACCEPT)
		goto out;

	filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
	path = g_path_get_dirname(filename);
	kiro_configuration_set_rmp_path(priv->config, path);
	g_free(path);

	rmp = roccat_swarm_rmp_read_with_path(filename, &length, &error);
	g_free(filename);
	if (error) {
		roccat_warning_dialog(GTK_WINDOW(dialog), _("Could not load profile"), error->message);
		g_error_free(error);
		goto out;
	}

	profile_data = kiro_rmp_to_profile_data(rmp, length);
	g_free(rmp);
	if (!profile_data) {
		roccat_warning_dialog(GTK_WINDOW(dialog), _("Could not convert profile"), NULL);
		goto out;
	}

	kiro_profile_page_set_profile_data(KIRO_PROFILE_PAGE(profile_page), profile_data);
	g_free(profile_data);

out:
	gtk_widget_destroy(dialog);
}

static void save_profile_to_file(KiroconfigWindow *window, KiroProfilePage *profile_page) {
	KiroconfigWindowPrivate *priv = window->priv;
	GtkWidget *dialog;
	gchar *filename, *path;
	gchar *temp_filename;
	GError *error = NULL;
	gpointer rmp;
	gsize length;
	KiroProfileData *profile_data;

	profile_data = kiro_profile_page_get_profile_data(profile_page);

	dialog = gtk_file_chooser_dialog_new(_("Save profile"),
			GTK_WINDOW(window),
			GTK_FILE_CHOOSER_ACTION_SAVE,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
			NULL);

	add_file_filter_to_file_chooser(GTK_FILE_CHOOSER(dialog));

	path = kiro_configuration_get_rmp_path(priv->config);
	gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), path);
	g_free(path);

	filename = kiro_create_filename_proposition(profile_data);
	gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog), filename);
	g_free(filename);

	if (gtk_dialog_run(GTK_DIALOG(dialog)) != GTK_RESPONSE_ACCEPT)
		goto out;

	filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
	temp_filename = roccat_create_filename_with_extension(filename, ROCCAT_MOUSE_PROFILE_EXTENSION);
	g_free(filename);

	path = g_path_get_dirname(temp_filename);
	kiro_configuration_set_rmp_path(priv->config, path);
	g_free(path);

	rmp = kiro_profile_data_to_rmp(profile_data, &length);
	if (!rmp) {
		roccat_warning_dialog(GTK_WINDOW(dialog), _("Could not convert profile"), NULL);
		goto free_filename;
	}

	roccat_swarm_rmp_write_with_path(temp_filename, rmp, length, &error);
	g_free(rmp);
	if (error) {
		roccat_warning_dialog(GTK_WINDOW(dialog), _("Could not save profile"), error->message);
		g_error_free(error);
	}

free_filename:
	g_free(temp_filename);
out:
	gtk_widget_destroy(dialog);
	g_free(profile_data);
}

static void save_profile_to_file_cb(RoccatProfilePage *profile_page, gpointer user_data) {
	save_profile_to_file(KIROCONFIG_WINDOW(user_data), KIRO_PROFILE_PAGE(profile_page));
}

static void add_page(KiroconfigWindow *config_window, KiroProfileData *profile_data) {
	KiroProfilePage *profile_page;

	profile_page = KIRO_PROFILE_PAGE(kiro_profile_page_new());
	roccat_config_window_pages_append_page(ROCCAT_CONFIG_WINDOW_PAGES(config_window), ROCCAT_PROFILE_PAGE(profile_page));
	g_signal_connect(G_OBJECT(profile_page), "load-from-file", G_CALLBACK(load_profile_from_file_cb), config_window);
	g_signal_connect(G_OBJECT(profile_page), "save-to-file", G_CALLBACK(save_profile_to_file_cb), config_window);
	kiro_profile_page_set_profile_data(profile_page, profile_data);
}

static void add_pages(KiroconfigWindow *window) {
	GError *local_error = NULL;
	KiroProfileData *profile_data;
	RoccatDevice *device;

	device = roccat_config_window_get_device(ROCCAT_CONFIG_WINDOW(window));

	profile_data = kiro_profile_data_new();

	kiro_profile_data_update_filesystem(profile_data, &local_error);
	if (!roccat_handle_error_dialog(GTK_WINDOW(window), _("Could not load filesystem data for profile"), &local_error)) {
		g_free(profile_data);
		return;
	}

	kiro_profile_data_update_hardware(profile_data, device, &local_error);
	if (!roccat_handle_error_dialog(GTK_WINDOW(window), _("Could not load hardware data for profile"), &local_error)) {
		g_free(profile_data);
		return;
	}

	add_page(window, profile_data);
	g_free(profile_data);

	roccat_config_window_pages_set_active_page_blocked(ROCCAT_CONFIG_WINDOW_PAGES(window), 0);
}

static void device_add_cb(RoccatConfigWindow *roccat_window, gpointer user_data) {
	add_pages(KIROCONFIG_WINDOW(roccat_window));
}

static void device_remove_cb(RoccatConfigWindow *roccat_window, gpointer user_data) {
	roccat_config_window_pages_remove_pages(ROCCAT_CONFIG_WINDOW_PAGES(roccat_window));
}

static KiroProfileData *get_profile_data(KiroconfigWindow *window, KiroProfilePage *profile_page) {
	KiroProfileData *profile_data;
	gboolean moved;

	profile_data = kiro_profile_page_get_profile_data(profile_page);
	moved = roccat_config_window_pages_get_page_moved(ROCCAT_CONFIG_WINDOW_PAGES(window), ROCCAT_PROFILE_PAGE(profile_page));

	if (moved)
		kiro_profile_data_set_modified(profile_data);

	return profile_data;
}

static void set_profile_data(KiroconfigWindow *window, KiroProfileData *profile_data) {
	KiroProfilePage *profile_page = KIRO_PROFILE_PAGE(roccat_config_window_pages_get_page(ROCCAT_CONFIG_WINDOW_PAGES(window), 0));
	kiro_profile_page_set_profile_data(profile_page, profile_data);
	roccat_config_window_pages_set_page_unmoved(ROCCAT_CONFIG_WINDOW_PAGES(window), ROCCAT_PROFILE_PAGE(profile_page));
	kiro_dbus_emit_profile_data_changed_outside(window->priv->dbus_proxy, 1);
}

static gboolean save_single(KiroconfigWindow *window, KiroProfileData *profile_data, GError **error) {
	RoccatDevice *device = roccat_config_window_get_device(ROCCAT_CONFIG_WINDOW(window));

	if (device)
		kiro_profile_data_save(device, profile_data, error);
	else
		kiro_profile_data_eventhandler_save(&profile_data->eventhandler, error);

	if (*error)
		return FALSE;
	else {
		set_profile_data(window, profile_data);
		return TRUE;
	}
}

/* returns successfully_saved */
static gboolean save_all(KiroconfigWindow *window, gboolean ask) {
	KiroProfileData *profile_data;
	KiroProfilePage *profile_page;
	GError *local_error = NULL;
	gboolean modified;
	gboolean retval;

	modified = FALSE;
	profile_page = KIRO_PROFILE_PAGE(roccat_config_window_pages_get_page(ROCCAT_CONFIG_WINDOW_PAGES(window), 0));
	profile_data = get_profile_data(window, profile_page);
	if (kiro_profile_data_get_modified(profile_data))
		modified = TRUE;

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
	if (!save_single(window, profile_data, &local_error))
		retval = FALSE;

	roccat_handle_error_dialog(GTK_WINDOW(window), _("Could not save unsaved data"), &local_error);

exit:
	g_free(profile_data);
	return retval;
}

static gboolean delete_event_cb(GtkWidget *window, GdkEvent *event, gpointer user_data) {
	return !save_all(KIROCONFIG_WINDOW(window), TRUE);
}

static void save_all_cb(RoccatConfigWindow *roccat_window, gpointer user_data) {
	save_all(KIROCONFIG_WINDOW(roccat_window), FALSE);
}

static void menu_edit_preferences_cb(GtkMenuItem *item, gpointer user_data) {
	KiroconfigWindow *window = KIROCONFIG_WINDOW(user_data);
	KiroconfigWindowPrivate *priv = window->priv;
	kiro_configuration_dialog(GTK_WINDOW(window), priv->config);
	if (TRUE)
		kiro_dbus_emit_configuration_changed_outside(priv->dbus_proxy);
}

static void pre_firmware_update_cb(RoccatUpdateAssistant *assistant, gpointer user_data) {
	roccat_config_window_device_scanner_suspend(ROCCAT_CONFIG_WINDOW(user_data));
}

static void post_firmware_update_cb(RoccatUpdateAssistant *assistant, gpointer user_data) {
	roccat_config_window_device_scanner_continue(ROCCAT_CONFIG_WINDOW(user_data));
}

static void menu_device_firmware_update_cb(GtkMenuItem *item, gpointer user_data) {
	KiroconfigWindow *window = KIROCONFIG_WINDOW(user_data);
	GError *local_error = NULL;
	RoccatDevice *kiro;
	GtkWidget *assistant;
	guint firmware_version;
	gchar *version_string;

	if (!roccat_config_window_warn_if_no_device(ROCCAT_CONFIG_WINDOW(window)))
		return;

	kiro = roccat_config_window_get_device(ROCCAT_CONFIG_WINDOW(window));
	firmware_version = kiro_firmware_version_read(kiro, &local_error);
	if (local_error) {
		roccat_warning_dialog(GTK_WINDOW(window), _("Could not read version info"), local_error->message);
		g_error_free(local_error);
		return;
	}

	assistant = roccat_update_assistant_new(GTK_WINDOW(window), kiro, KIRO_INTERFACE_MOUSE);
	g_object_set(G_OBJECT(assistant),
			"wait-prepare", KIRO_FIRMWARE_UPDATE_WAIT_PREPARE,
			"wait-init-data", KIRO_FIRMWARE_UPDATE_WAIT_INIT_DATA,
			"wait-data", KIRO_FIRMWARE_UPDATE_WAIT_DATA,
			"final-write-check", KIRO_FIRMWARE_UPDATE_FINAL_WRITE_CHECK,
			NULL);

	version_string = roccat_firmware_version_to_string(firmware_version);
	roccat_update_assistant_add_firmware(ROCCAT_UPDATE_ASSISTANT(assistant), _("Main firmware"), version_string,
			KIRO_FIRMWARE_SIZE, ROCCAT_FIRMWARE_NUMBER_DEFAULT);
	g_free(version_string);

	g_signal_connect(G_OBJECT(assistant), "pre-action", G_CALLBACK(pre_firmware_update_cb), window);
	g_signal_connect(G_OBJECT(assistant), "post-action", G_CALLBACK(post_firmware_update_cb), window);

	gtk_widget_show_all(assistant);
}

static void menu_device_info_cb(GtkMenuItem *item, gpointer user_data) {
	KiroconfigWindow *window = KIROCONFIG_WINDOW(user_data);
	RoccatInfoDialog *dialog;
	GError *local_error = NULL;
	KiroInfo *info;
	gchar *string;

	if (!roccat_config_window_warn_if_no_device(ROCCAT_CONFIG_WINDOW(window)))
		return;

	info = kiro_info_read(roccat_config_window_get_device(ROCCAT_CONFIG_WINDOW(window)), &local_error);
	if (!roccat_handle_error_dialog(GTK_WINDOW(window), _("Could not read informations"), &local_error))
		return;

	dialog = ROCCAT_INFO_DIALOG(roccat_info_dialog_new(GTK_WINDOW(window)));

	string = roccat_firmware_version_to_string(info->firmware_version);
	roccat_info_dialog_add_line(dialog, _("Firmware version"), gtk_label_new(string));
	g_free(string);

	g_free(info);

	(void)gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(GTK_WIDGET(dialog));
}

static void emit_profile_data_changed_outside_for_all(KiroconfigWindow *window) {
	guint i;
	for (i = 0; i < roccat_config_window_pages_get_num_pages(ROCCAT_CONFIG_WINDOW_PAGES(window)); ++i)
		kiro_dbus_emit_profile_data_changed_outside(window->priv->dbus_proxy, i + 1);
}

static void menu_device_reset_cb(GtkMenuItem *item, gpointer user_data) {
	KiroconfigWindow *window = KIROCONFIG_WINDOW(user_data);
	GError *error = NULL;
	RoccatDevice *device;

	if (!roccat_config_window_warn_if_no_device(ROCCAT_CONFIG_WINDOW(window)))
		return;

	if (!roccat_continue_dialog(GTK_WINDOW(window), _("This resets the device to factory defaults.")))
		return;

	device = roccat_config_window_get_device(ROCCAT_CONFIG_WINDOW(window));
	kiro_reset(device, &error);
	if (!roccat_handle_error_dialog(GTK_WINDOW(window), _("Could not reset device"), &error))
		return;

	roccat_config_window_pages_remove_pages(ROCCAT_CONFIG_WINDOW_PAGES(window));
	add_pages(window);
	emit_profile_data_changed_outside_for_all(window);
}

GtkWidget *kiroconfig_window_new(void) {
	return GTK_WIDGET(g_object_new(KIROCONFIG_WINDOW_TYPE,
			"device-name", KIRO_DEVICE_NAME,
			NULL));
}

static GObject *constructor(GType gtype, guint n_properties, GObjectConstructParam *properties) {
	GObject *obj;
	KiroconfigWindow *window;
	RoccatConfigWindow *roccat_window;
	KiroconfigWindowPrivate *priv;
	GtkMenuItem *menu_item;

	obj = G_OBJECT_CLASS(kiroconfig_window_parent_class)->constructor(gtype, n_properties, properties);
	window = KIROCONFIG_WINDOW(obj);
	roccat_window = ROCCAT_CONFIG_WINDOW(obj);
	priv = window->priv;

	priv->config = kiro_configuration_load();

	menu_item = GTK_MENU_ITEM(gtk_image_menu_item_new_from_stock(GTK_STOCK_PREFERENCES, NULL));
	g_signal_connect(G_OBJECT(menu_item), "activate", G_CALLBACK(menu_edit_preferences_cb), window);
	roccat_config_window_edit_menu_append(roccat_window, menu_item);

	menu_item = GTK_MENU_ITEM(gtk_image_menu_item_new_with_label(_("Firmware update")));
	g_signal_connect(G_OBJECT(menu_item), "activate", G_CALLBACK(menu_device_firmware_update_cb), window);
	roccat_config_window_device_menu_append(roccat_window, menu_item);

	menu_item = GTK_MENU_ITEM(gtk_image_menu_item_new_with_label(_("Reset")));
	g_signal_connect(G_OBJECT(menu_item), "activate", G_CALLBACK(menu_device_reset_cb), window);
	roccat_config_window_device_menu_append(roccat_window, menu_item);

	menu_item = GTK_MENU_ITEM(gtk_image_menu_item_new_with_label(_("Info")));
	g_signal_connect(G_OBJECT(menu_item), "activate", G_CALLBACK(menu_device_info_cb), window);
	roccat_config_window_device_menu_append(roccat_window, menu_item);

	g_signal_connect(G_OBJECT(roccat_window), "delete-event", G_CALLBACK(delete_event_cb), NULL);
	g_signal_connect(G_OBJECT(roccat_window), "save-all", G_CALLBACK(save_all_cb), NULL);
	g_signal_connect(G_OBJECT(roccat_window), "device-removed", G_CALLBACK(device_remove_cb), NULL);
	g_signal_connect(G_OBJECT(roccat_window), "device-added", G_CALLBACK(device_add_cb), NULL);

	/* keep this order */
	priv->dbus_proxy = kiro_dbus_proxy_new();

	roccat_config_window_set_device_scanner(roccat_window, ROCCAT_DEVICE_SCANNER_INTERFACE(kiro_device_scanner_new()));

	gtk_widget_show(GTK_WIDGET(window));

	/* if a device is already plugged in, add_cb has already been executed */
	roccat_config_window_warn_if_no_device(roccat_window);

	return obj;
}

static void kiroconfig_window_init(KiroconfigWindow *window) {
	window->priv = KIROCONFIG_WINDOW_GET_PRIVATE(window);
}

static void finalize(GObject *object) {
	KiroconfigWindowPrivate *priv = KIROCONFIG_WINDOW(object)->priv;

	kiro_configuration_dialog_save(GTK_WINDOW(object), priv->config);
	kiro_dbus_emit_configuration_changed_outside(priv->dbus_proxy);
	g_clear_pointer(&priv->config, roccat_configuration_free);
	g_clear_pointer(&priv->dbus_proxy, dbus_roccat_proxy_free);

	G_OBJECT_CLASS(kiroconfig_window_parent_class)->finalize(object);
}

static void kiroconfig_window_class_init(KiroconfigWindowClass *klass) {
	GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

	gobject_class->constructor = constructor;
	gobject_class->finalize = finalize;

	g_type_class_add_private(klass, sizeof(KiroconfigWindowClass));
}
