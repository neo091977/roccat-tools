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

#include "ryos_dbus_services.h"
#include "ryos_reset.h"
#include "ryos_profile.h"
#include "ryos_info.h"
#include "ryos_rkp.h"
#include "ryos_profile_data.h"
#include "ryosconfig_window.h"
#include "ryos_configuration_dialog.h"
#include "ryosconfig_blink.h"
#include "ryosconfig_profile_page.h"
#include "ryosconfig_layer_illumination_dialog.h"
#include "ryos_config.h"
#include "ryos_device.h"
#include "roccat_helper.h"
#include "roccat_warning_dialog.h"
#include "roccat_save_dialog.h"
#include "roccat_continue_dialog.h"
#include "roccat_info_dialog.h"
#include "roccat_update_assistant.h"
#include "roccat_firmware.h"
#include "g_roccat_helper.h"
#include "g_dbus_roccat_helper.h"
#include "roccat.h"
#include "i18n.h"

#define RYOSCONFIG_WINDOW_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), RYOSCONFIG_WINDOW_TYPE, RyosconfigWindowClass))
#define IS_RYOSCONFIG_WINDOW_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), RYOSCONFIG_WINDOW_TYPE))
#define RYOSCONFIG_WINDOW_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), RYOSCONFIG_WINDOW_TYPE, RyosconfigWindowPrivate))

typedef struct _RyosconfigWindowClass RyosconfigWindowClass;
typedef struct _RyosconfigWindowPrivate RyosconfigWindowPrivate;

struct _RyosconfigWindow {
	RoccatConfigWindowPages parent;
	RyosconfigWindowPrivate *priv;
};

struct _RyosconfigWindowClass {
	RoccatConfigWindowPagesClass parent_class;
};

struct _RyosconfigWindowPrivate {
	RoccatKeyFile *config;
	DBusGProxy *dbus_proxy;
	RyosconfigBlink *blink;
};

G_DEFINE_TYPE(RyosconfigWindow, ryosconfig_window, ROCCAT_CONFIG_WINDOW_PAGES_TYPE);

static gboolean change_profile_handler(gpointer user_data) {
	RoccatConfigWindow *roccat_window = ROCCAT_CONFIG_WINDOW(user_data);
	guint profile_index = roccat_config_window_pages_get_active_page(ROCCAT_CONFIG_WINDOW_PAGES(roccat_window));
	guint profile_number = profile_index + 1;
	GError *local_error = NULL;

	if (!roccat_config_window_warn_if_no_device(roccat_window))
		return FALSE;

	ryos_profile_write(roccat_config_window_get_device(roccat_window), profile_index, &local_error);
	if (!roccat_handle_error_dialog(GTK_WINDOW(roccat_window), _("Could not activate profile"), &local_error))
		return FALSE;
	ryos_dbus_emit_profile_changed_outside(RYOSCONFIG_WINDOW(roccat_window)->priv->dbus_proxy, profile_number);

	return FALSE;
}

static void window_active_page_changed_cb(RoccatConfigWindowPages *roccat_window, gpointer user_data) {
	g_idle_add(change_profile_handler, roccat_window);
}

static void actual_profile_changed_from_device_cb(DBusGProxy *proxy, guchar profile_number, gpointer user_data) {
	roccat_config_window_pages_set_active_page_blocked(ROCCAT_CONFIG_WINDOW_PAGES(user_data), profile_number - 1);
}

static void brightness_changed_from_device_cb(DBusGProxy *proxy, guchar profile_number, guchar brightness_number, gpointer user_data) {
	ryosconfig_profile_page_update_brightness(RYOSCONFIG_PROFILE_PAGE(roccat_config_window_pages_get_page(ROCCAT_CONFIG_WINDOW_PAGES(user_data), profile_number - 1)), brightness_number - 1);
}

static void macro_changed_from_device_cb(DBusGProxy *proxy, guchar profile_number, guchar button_number, gchar const *data, gpointer user_data) {
	RyosMacro *macro;
	gsize length;

	macro = (RyosMacro *)g_base64_decode(data, &length);
	if (length == sizeof(RyosMacro))
		ryosconfig_profile_page_update_key(RYOSCONFIG_PROFILE_PAGE(roccat_config_window_pages_get_page(ROCCAT_CONFIG_WINDOW_PAGES(user_data), profile_number - 1)), button_number - 1, macro);
	g_free(macro);
}

static void device_add_cb(RoccatConfigWindow *roccat_window, gpointer user_data) {
	RyosconfigWindow *window = RYOSCONFIG_WINDOW(roccat_window);
	RyosconfigWindowPrivate *priv = window->priv;
	GError *local_error = NULL;
	guint i;
	RyosProfileData *profile_data;
	RoccatDevice *device;
	RyosconfigProfilePage *profile_page;
	guint actual_profile_index;

	device = roccat_config_window_get_device(roccat_window);

	for (i = 0; i < RYOS_PROFILE_NUM; ++i) {
		profile_page = RYOSCONFIG_PROFILE_PAGE(roccat_config_window_pages_get_page(ROCCAT_CONFIG_WINDOW_PAGES(roccat_window), i));
		profile_data = ryosconfig_profile_page_get_profile_data(profile_page, priv->config);

		ryos_profile_data_update_filesystem(profile_data, i, &local_error);
		if (!roccat_handle_error_dialog(GTK_WINDOW(window), _("Could not load filesystem data for profile"), &local_error)) {
			g_free(profile_data);
			break;
		}

		ryos_profile_data_update_hardware(profile_data, device, i, &local_error);
		if (!roccat_handle_error_dialog(GTK_WINDOW(window), _("Could not load hardware data for profile"), &local_error)) {
			g_free(profile_data);
			break;
		}

		ryosconfig_profile_page_set_profile_data(profile_page, profile_data, priv->config);
		ryosconfig_profile_page_set_device(profile_page, device);
		switch (gaminggear_device_get_product_id(GAMINGGEAR_DEVICE(device))) {
		case USB_DEVICE_ID_ROCCAT_RYOS_MK:
		case USB_DEVICE_ID_ROCCAT_RYOS_MK_GLOW:
			ryosconfig_blink_stop(priv->blink);
			break;
		case USB_DEVICE_ID_ROCCAT_RYOS_MK_PRO:
			ryosconfig_blink_start(priv->blink);
			break;
		default:
			break;
		}

		g_free(profile_data);
	}

	actual_profile_index = ryos_profile_read(device, &local_error);
	if (!roccat_handle_error_dialog(GTK_WINDOW(window), _("Could not read actual profile"), &local_error))
		return;
	roccat_config_window_pages_set_active_page_blocked(ROCCAT_CONFIG_WINDOW_PAGES(roccat_window), actual_profile_index);

	dbus_g_proxy_connect_signal(priv->dbus_proxy, "ProfileChanged", G_CALLBACK(actual_profile_changed_from_device_cb), window, NULL);
	dbus_g_proxy_connect_signal(priv->dbus_proxy, "BrightnessChanged", G_CALLBACK(brightness_changed_from_device_cb), window, NULL);
	dbus_g_proxy_connect_signal(priv->dbus_proxy, "MacroChanged", G_CALLBACK(macro_changed_from_device_cb), window, NULL);
}

static void device_remove_cb(RoccatConfigWindow *roccat_window, gpointer user_data) {
	RyosconfigWindow *window = RYOSCONFIG_WINDOW(roccat_window);
	RyosconfigWindowPrivate *priv = window->priv;

	ryosconfig_blink_stop(priv->blink);

	dbus_g_proxy_disconnect_signal(priv->dbus_proxy, "ProfileChanged", G_CALLBACK(actual_profile_changed_from_device_cb), window);
	dbus_g_proxy_disconnect_signal(priv->dbus_proxy, "BrightnessChanged", G_CALLBACK(brightness_changed_from_device_cb), window);
	dbus_g_proxy_disconnect_signal(priv->dbus_proxy, "MacroChanged", G_CALLBACK(macro_changed_from_device_cb), window);
}

static RyosProfileData *get_profile_data(RyosconfigWindow *window, RyosconfigProfilePage *profile_page) {
	RyosProfileData *profile_data;
	gboolean moved;

	profile_data = ryosconfig_profile_page_get_profile_data(profile_page, window->priv->config);
	moved = roccat_config_window_pages_get_page_moved(ROCCAT_CONFIG_WINDOW_PAGES(window), ROCCAT_PROFILE_PAGE(profile_page));

	if (moved)
		ryos_profile_data_set_modified(profile_data);

	return profile_data;
}

static void set_profile_data(RyosconfigWindow *window, guint profile_index, RyosProfileData *profile_data) {
	RyosconfigProfilePage *profile_page = RYOSCONFIG_PROFILE_PAGE(roccat_config_window_pages_get_page(ROCCAT_CONFIG_WINDOW_PAGES(window), profile_index));
	guint profile_number = profile_index + 1;
	ryosconfig_profile_page_set_profile_data(profile_page, profile_data, window->priv->config);
	roccat_config_window_pages_set_page_unmoved(ROCCAT_CONFIG_WINDOW_PAGES(window), ROCCAT_PROFILE_PAGE(profile_page));
	ryos_dbus_emit_profile_data_changed_outside(window->priv->dbus_proxy, profile_number);
}

static gboolean save_single(RyosconfigWindow *window, RyosProfileData *profile_data, guint profile_index, GError **error) {
	RoccatDevice *device = roccat_config_window_get_device(ROCCAT_CONFIG_WINDOW(window));

	if (device)
		ryos_profile_data_save(device, profile_data, profile_index, error);
	else
		ryos_profile_data_eventhandler_save(&profile_data->eventhandler, profile_index, error);

	if (*error)
		return FALSE;
	else {
		set_profile_data(window, profile_index, profile_data);
		return TRUE;
	}
}

static gboolean save_all(RyosconfigWindow *window, gboolean ask) {
	RyosProfileData *profile_datas[RYOS_PROFILE_NUM];
	RyosconfigProfilePage *profile_page;
	GError *local_error = NULL;
	gboolean modified;
	gboolean retval;
	guint i;

	modified = FALSE;
	for (i = 0; i < RYOS_PROFILE_NUM; ++i) {
		profile_page = RYOSCONFIG_PROFILE_PAGE(roccat_config_window_pages_get_page(ROCCAT_CONFIG_WINDOW_PAGES(window), i));
		profile_datas[i] = get_profile_data(window, profile_page);
		if (ryos_profile_data_get_modified(profile_datas[i]))
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
	for (i = 0; i < RYOS_PROFILE_NUM; ++i) {
		if (!save_single(window, profile_datas[i], i, &local_error)) {
			retval = FALSE;
			break;
		}
	}
	roccat_handle_error_dialog(GTK_WINDOW(window), _("Could not save unsaved data"), &local_error);

exit:
	for (i = 0; i < RYOS_PROFILE_NUM; ++i)
		g_free(profile_datas[i]);

	return retval;
}

static gboolean delete_event_cb(GtkWidget *window, GdkEvent *event, gpointer user_data) {
	return !save_all(RYOSCONFIG_WINDOW(window), TRUE);
}

static void add_file_filter_to_file_chooser(GtkFileChooser *chooser) {
	GtkFileFilter *filter;
	filter = gtk_file_filter_new();
	gtk_file_filter_set_name(filter, _("Ryos MK Windows profiles"));
	gtk_file_filter_add_pattern(filter, "*." ROCCAT_KEYBOARD_PROFILE_EXTENSION);
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(chooser), filter);

	filter = gtk_file_filter_new();
	gtk_file_filter_set_name(filter, _("All files"));
	gtk_file_filter_add_pattern(filter, "*");
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(chooser), filter);
}

static gchar *ryos_create_filename_proposition(RyosProfileData const *profile_data, guint profile_index) {
	return roccat_profile_filename_proposition(profile_data->eventhandler.profile_name, profile_index, ROCCAT_KEYBOARD_PROFILE_EXTENSION);
}

static void save_profile_to_file_cb(RoccatProfilePage *profile_page, gpointer user_data) {
	RyosconfigWindow *window = RYOSCONFIG_WINDOW(user_data);
	RyosconfigWindowPrivate *priv = window->priv;
	GtkWidget *dialog;
	gchar *filename, *path;
	gchar *temp_filename;
	GError *error = NULL;
	guint profile_index = roccat_config_window_pages_get_page_index(ROCCAT_CONFIG_WINDOW_PAGES(window), profile_page);
	RyosRkp *rkp;
	RyosProfileData *profile_data;

	profile_data = ryosconfig_profile_page_get_profile_data(RYOSCONFIG_PROFILE_PAGE(profile_page), priv->config);

	dialog = gtk_file_chooser_dialog_new(_("Save profile"),
			GTK_WINDOW(window),
			GTK_FILE_CHOOSER_ACTION_SAVE,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
			NULL);

	add_file_filter_to_file_chooser(GTK_FILE_CHOOSER(dialog));

	path = ryos_configuration_get_rkp_path(priv->config);
	gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), path);
	g_free(path);

	filename = ryos_create_filename_proposition(profile_data, profile_index);
	gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog), filename);
	g_free(filename);

	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
		filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
		temp_filename = roccat_create_filename_with_extension(filename, ROCCAT_KEYBOARD_PROFILE_EXTENSION);
		g_free(filename);

		path = g_path_get_dirname(temp_filename);
		ryos_configuration_set_rkp_path(priv->config, path);
		g_free(path);

		rkp = ryos_profile_data_to_rkp(profile_data);
		ryos_rkp_write_with_path(temp_filename, rkp, &error);
		g_free(rkp);
		g_free(temp_filename);
		if (error) {
			roccat_warning_dialog(GTK_WINDOW(dialog), _("Could not save profile"), error->message);
			g_error_free(error);
		}
	}

	gtk_widget_destroy(dialog);
	g_free(profile_data);
}

static void load_profile_from_file_cb(RoccatProfilePage *profile_page, gpointer user_data) {
	RyosconfigWindow *window = RYOSCONFIG_WINDOW(user_data);
	RyosconfigWindowPrivate *priv = window->priv;
	GtkWidget *dialog;
	gchar *filename, *path;
	GError *error = NULL;
	RyosRkp *rkp;
	RyosProfileData *profile_data;

	dialog = gtk_file_chooser_dialog_new(_("Load profile"),
			GTK_WINDOW(window),
			GTK_FILE_CHOOSER_ACTION_OPEN,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
			NULL);

	add_file_filter_to_file_chooser(GTK_FILE_CHOOSER(dialog));
	path = ryos_configuration_get_rkp_path(priv->config);
	gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), path);
	g_free(path);

	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
		filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
		path = g_path_get_dirname(filename);
		ryos_configuration_set_rkp_path(priv->config, path);
		g_free(path);

		rkp = ryos_rkp_read_with_path(filename, &error);
		g_free(filename);
		if (error) {
			roccat_warning_dialog(GTK_WINDOW(dialog), _("Could not load profile"), error->message);
			g_error_free(error);
		} else {
			profile_data = ryos_rkp_to_profile_data(rkp);
			g_free(rkp);
			ryosconfig_profile_page_set_profile_data(RYOSCONFIG_PROFILE_PAGE(profile_page), profile_data, priv->config);
			g_free(profile_data);
		}
	}

	gtk_widget_destroy(dialog);
}

static void save_all_cb(RoccatConfigWindow *roccat_window, gpointer user_data) {
	save_all(RYOSCONFIG_WINDOW(roccat_window), FALSE);
}

static void set_keyboard_layout(RyosconfigWindow *window, gchar const *layout) {
	RyosconfigProfilePage *profile_page;
	guint i;

	for (i = 0; i < RYOS_PROFILE_NUM; ++i) {
		profile_page = RYOSCONFIG_PROFILE_PAGE(roccat_config_window_pages_get_page(ROCCAT_CONFIG_WINDOW_PAGES(window), i));
		ryosconfig_profile_page_set_keyboard_layout(profile_page, layout);
	}
}

static void menu_edit_preferences_cb(GtkMenuItem *item, gpointer user_data) {
	RyosconfigWindow *window = RYOSCONFIG_WINDOW(user_data);
	RyosconfigWindowPrivate *priv = window->priv;
	ryos_configuration_dialog(GTK_WINDOW(window), priv->config);
	if (TRUE) { // FIXME only if changed!
		set_keyboard_layout(window, ryos_configuration_get_layout(priv->config));
		ryos_dbus_emit_configuration_changed_outside(priv->dbus_proxy);
	}
}

static void pre_firmware_update_cb(RoccatUpdateAssistant *assistant, gpointer user_data) {
	roccat_config_window_device_scanner_suspend(ROCCAT_CONFIG_WINDOW(user_data));
}

static void post_firmware_update_cb(RoccatUpdateAssistant *assistant, gpointer user_data) {
	roccat_config_window_device_scanner_continue(ROCCAT_CONFIG_WINDOW(user_data));
}

static void menu_device_firmware_update_cb(GtkMenuItem *item, gpointer user_data) {
	RyosconfigWindow *window = RYOSCONFIG_WINDOW(user_data);
	GError *local_error = NULL;
	RoccatDevice *ryos;
	GtkWidget *assistant;
	guint version;
	gchar *version_string;

	if (!roccat_config_window_warn_if_no_device(ROCCAT_CONFIG_WINDOW(window)))
		return;

	ryos = roccat_config_window_get_device(ROCCAT_CONFIG_WINDOW(window));
	version = ryos_firmware_version_read(ryos, &local_error);
	if (local_error) {
		roccat_warning_dialog(GTK_WINDOW(window), _("Could not read version info"), local_error->message);
		g_error_free(local_error);
		return;
	}

	version_string = roccat_firmware_version_to_string(version);
	assistant = roccat_update_assistant_new(GTK_WINDOW(window), ryos, RYOS_INTERFACE_KEYBOARD);
	roccat_update_assistant_add_firmware(ROCCAT_UPDATE_ASSISTANT(assistant), _("Firmware"), version_string, RYOS_FIRMWARE_SIZE, ROCCAT_FIRMWARE_NUMBER_DEFAULT);
	g_free(version_string);

	g_signal_connect(G_OBJECT(assistant), "pre-action", G_CALLBACK(pre_firmware_update_cb), window);
	g_signal_connect(G_OBJECT(assistant), "post-action", G_CALLBACK(post_firmware_update_cb), window);

	gtk_widget_show_all(assistant);
}

static void menu_device_info_cb(GtkMenuItem *item, gpointer user_data) {
	RyosconfigWindow *window = RYOSCONFIG_WINDOW(user_data);
	RoccatInfoDialog *dialog;
	GError *local_error = NULL;
	RyosInfo *info;
	gchar *firmware;
	gchar *dfu;

	if (!roccat_config_window_warn_if_no_device(ROCCAT_CONFIG_WINDOW(window)))
		return;

	info = ryos_info_read(roccat_config_window_get_device(ROCCAT_CONFIG_WINDOW(window)), &local_error);
	if (!roccat_handle_error_dialog(GTK_WINDOW(window), _("Could not read informations"), &local_error))
		return;

	dialog = ROCCAT_INFO_DIALOG(roccat_info_dialog_new(GTK_WINDOW(window)));

	firmware = roccat_firmware_version_to_string(info->firmware_version);
	roccat_info_dialog_add_line(dialog, _("Firmware version"), gtk_label_new(firmware));
	g_free(firmware);

	dfu = roccat_firmware_version_to_string(info->dfu_version);
	roccat_info_dialog_add_line(dialog, _("DFU version"), gtk_label_new(dfu));
	g_free(dfu);

	g_free(info);

	(void)gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(GTK_WIDGET(dialog));
}

static void menu_device_reset_cb(GtkMenuItem *item, gpointer user_data) {
	RyosconfigWindow *window = RYOSCONFIG_WINDOW(user_data);
	RyosconfigWindowPrivate *priv = window->priv;
	RyosProfileData *profile_data;
	guint i;
	GError *error = NULL;
	RoccatDevice *device;
	RyosconfigProfilePage *profile_page;

	if (!roccat_config_window_warn_if_no_device(ROCCAT_CONFIG_WINDOW(window)))
		return;

	if (!roccat_continue_dialog(GTK_WINDOW(window), _("This resets the device to factory defaults.")))
		return;

	device = roccat_config_window_get_device(ROCCAT_CONFIG_WINDOW(window));
	ryos_reset(device, RYOS_RESET_FUNCTION_RESET, &error);
	if (!roccat_handle_error_dialog(GTK_WINDOW(window), _("Could not reset device"), &error))
		return;

	for (i = 0; i < RYOS_PROFILE_NUM; ++i) {
		profile_page = RYOSCONFIG_PROFILE_PAGE(roccat_config_window_pages_get_page(ROCCAT_CONFIG_WINDOW_PAGES(window), i));
		profile_data = ryosconfig_profile_page_get_profile_data(profile_page, priv->config);
		ryos_profile_data_update_hardware(profile_data, device, i, &error);
		if (roccat_handle_error_dialog(GTK_WINDOW(window), _("Could not read hardware profile data"), &error))
			ryosconfig_profile_page_set_profile_data(profile_page, profile_data, priv->config);
		g_free(profile_data);
		ryos_dbus_emit_profile_data_changed_outside(priv->dbus_proxy, i + 1);
	}
}

static gboolean layer_editor_cb(RyosconfigProfilePage *profile_page, gpointer light_layer, gpointer user_data) {
	RyosconfigWindow *window = RYOSCONFIG_WINDOW(user_data);
	return ryosconfig_layer_illumination_dialog_run(GTK_WINDOW(window),
			roccat_config_window_get_device(ROCCAT_CONFIG_WINDOW(window)), light_layer);
}

GtkWidget *ryosconfig_window_new(void) {
	return GTK_WIDGET(g_object_new(RYOSCONFIG_WINDOW_TYPE,
			"device-name", RYOS_DEVICE_NAME_COMBINED,
			NULL));
}

static void blink_timeout_cb(RyosconfigBlink *blink, gboolean blink_state, gpointer user_data) {
	RoccatConfigWindowPages *pages_window = ROCCAT_CONFIG_WINDOW_PAGES(user_data);
	RyosconfigProfilePage *profile_page;
	guint i;

	for (i = 0; i < RYOS_PROFILE_NUM; ++i) {
		profile_page = RYOSCONFIG_PROFILE_PAGE(roccat_config_window_pages_get_page(pages_window, i));
		ryosconfig_profile_page_update_keyboard_illumination(profile_page, blink_state);
	}
}

static GObject *constructor(GType gtype, guint n_properties, GObjectConstructParam *properties) {
	GObject *obj;
	RyosconfigWindow *window;
	RoccatConfigWindow *roccat_window;
	RyosconfigWindowPrivate *priv;
	RyosProfileData *profile_data;
	guint i;
	GtkMenuItem *menu_item;
	GtkWidget *profile_page;

	obj = G_OBJECT_CLASS(ryosconfig_window_parent_class)->constructor(gtype, n_properties, properties);
	window = RYOSCONFIG_WINDOW(obj);
	roccat_window = ROCCAT_CONFIG_WINDOW(obj);
	priv = window->priv;

	priv->config = ryos_configuration_load();

	for (i = 0; i < RYOS_PROFILE_NUM; ++i) {
		profile_page = ryosconfig_profile_page_new();
		roccat_config_window_pages_append_page(ROCCAT_CONFIG_WINDOW_PAGES(roccat_window), ROCCAT_PROFILE_PAGE(profile_page));

		profile_data = ryos_profile_data_new();
		ryosconfig_profile_page_set_profile_data(RYOSCONFIG_PROFILE_PAGE(profile_page), profile_data, priv->config);
		g_free(profile_data);

		g_signal_connect(G_OBJECT(profile_page), "load-from-file", G_CALLBACK(load_profile_from_file_cb), window);
		g_signal_connect(G_OBJECT(profile_page), "save-to-file", G_CALLBACK(save_profile_to_file_cb), window);
		g_signal_connect(G_OBJECT(profile_page), "layer-editor", G_CALLBACK(layer_editor_cb), window);
	}

	set_keyboard_layout(window, ryos_configuration_get_layout(priv->config));

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

	g_signal_connect(G_OBJECT(roccat_window), "active-changed", G_CALLBACK(window_active_page_changed_cb), NULL);
	g_signal_connect(G_OBJECT(roccat_window), "delete-event", G_CALLBACK(delete_event_cb), NULL);
	g_signal_connect(G_OBJECT(roccat_window), "save-all", G_CALLBACK(save_all_cb), NULL);
	g_signal_connect(G_OBJECT(roccat_window), "device-removed", G_CALLBACK(device_remove_cb), NULL);
	g_signal_connect(G_OBJECT(roccat_window), "device-added", G_CALLBACK(device_add_cb), NULL);

	priv->blink = ryosconfig_blink_new();
	g_object_ref(G_OBJECT(priv->blink));
	g_signal_connect(G_OBJECT(priv->blink), "timeout", G_CALLBACK(blink_timeout_cb), roccat_window);

	/* keep this order */
	priv->dbus_proxy = ryos_dbus_proxy_new();

	roccat_config_window_set_device_scanner(roccat_window, ROCCAT_DEVICE_SCANNER_INTERFACE(ryos_device_scanner_new()));

	gtk_widget_show(GTK_WIDGET(window));

	/* if a device is already plugged in, add_cb has already been executed */
	roccat_config_window_warn_if_no_device(roccat_window);

	return obj;
}

static void ryosconfig_window_init(RyosconfigWindow *window) {
	window->priv = RYOSCONFIG_WINDOW_GET_PRIVATE(window);
}

static void finalize(GObject *object) {
	RyosconfigWindowPrivate *priv = RYOSCONFIG_WINDOW(object)->priv;

	ryos_configuration_dialog_save(GTK_WINDOW(object), priv->config);
	ryos_dbus_emit_configuration_changed_outside(priv->dbus_proxy);
	g_clear_pointer(&priv->config, roccat_configuration_free);
	g_clear_pointer(&priv->dbus_proxy, dbus_roccat_proxy_free);
	g_object_unref(G_OBJECT(priv->blink));

	G_OBJECT_CLASS(ryosconfig_window_parent_class)->finalize(object);
}

static void ryosconfig_window_class_init(RyosconfigWindowClass *klass) {
	GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

	gobject_class->constructor = constructor;
	gobject_class->finalize = finalize;

	g_type_class_add_private(klass, sizeof(RyosconfigWindowClass));
}
