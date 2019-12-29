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

#include "sovaconfig_window.h"
#include "sova.h"
#include "sova_config.h"
#include "sova_dbus_services.h"
#include "sova_device.h"
#include "sova_profile.h"
#include "sova_profile_data.h"
#include "sova_profile_page.h"
#include "sova_rkp.h"
#include "sova_configuration_dialog.h"
#include "sova_info.h"
#include "sova_reset.h"
#include "sova_firmware.h"
#include "roccat.h"
#include "roccat_firmware.h"
#include "roccat_helper.h"
#include "roccat_continue_dialog.h"
#include "roccat_info_dialog.h"
#include "roccat_save_dialog.h"
#include "roccat_warning_dialog.h"
#include "roccat_swarm_rmp.h"
#include "roccat_update_assistant.h"
#include "g_roccat_helper.h"
#include "g_dbus_roccat_helper.h"
#include "i18n.h"

#define SOVACONFIG_WINDOW_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), SOVACONFIG_WINDOW_TYPE, SovaconfigWindowClass))
#define IS_SOVACONFIG_WINDOW_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), SOVACONFIG_WINDOW_TYPE))
#define SOVACONFIG_WINDOW_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), SOVACONFIG_WINDOW_TYPE, SovaconfigWindowPrivate))

typedef struct _SovaconfigWindowClass SovaconfigWindowClass;
typedef struct _SovaconfigWindowPrivate SovaconfigWindowPrivate;

struct _SovaconfigWindow {
	RoccatConfigWindowPages parent;
	SovaconfigWindowPrivate *priv;
};

struct _SovaconfigWindowClass {
	RoccatConfigWindowPagesClass parent_class;
};

struct _SovaconfigWindowPrivate {
	RoccatKeyFile *config;
	DBusGProxy *dbus_proxy;
	guint actual_profile_count;
};

G_DEFINE_TYPE(SovaconfigWindow, sovaconfig_window, ROCCAT_CONFIG_WINDOW_PAGES_TYPE);

static gboolean change_profile_handler(gpointer user_data) {
	RoccatConfigWindow *roccat_window = ROCCAT_CONFIG_WINDOW(user_data);
	SovaconfigWindowPrivate *priv = SOVACONFIG_WINDOW(roccat_window)->priv;
	guint profile_index = roccat_config_window_pages_get_active_page(ROCCAT_CONFIG_WINDOW_PAGES(roccat_window));
	guint profile_number = profile_index + 1;
	GError *local_error = NULL;

	if (!roccat_config_window_warn_if_no_device(roccat_window))
		return FALSE;

	sova_profile_write_index(roccat_config_window_get_device(roccat_window), profile_index, priv->actual_profile_count, &local_error);
	if (!roccat_handle_error_dialog(GTK_WINDOW(roccat_window), _("Could not activate profile"), &local_error))
		return FALSE;
	sova_dbus_emit_profile_changed_outside(priv->dbus_proxy, profile_number);

	return FALSE;
}

static void window_active_page_changed_cb(RoccatConfigWindowPages *roccat_window, gpointer user_data) {
	g_idle_add(change_profile_handler, roccat_window);
}

static void actual_profile_changed_from_device_cb(DBusGProxy *proxy, guchar profile_number, gpointer user_data) {
	roccat_config_window_pages_set_active_page_blocked(ROCCAT_CONFIG_WINDOW_PAGES(user_data), profile_number - 1);
}

static void brightness_changed_from_device_cb(DBusGProxy *proxy, guchar profile_number, guchar brightness_number, gpointer user_data) {
	sova_profile_page_update_brightness(SOVA_PROFILE_PAGE(roccat_config_window_pages_get_page(ROCCAT_CONFIG_WINDOW_PAGES(user_data), profile_number - 1)), brightness_number - 1);
}

static void macro_changed_from_device_cb(DBusGProxy *proxy, guchar profile_number, guchar button_number, gchar const *data, gpointer user_data) {
	SovaMacro *macro;
	gsize length;

	macro = (SovaMacro *)g_base64_decode(data, &length);
	if (length == sizeof(SovaMacro))
		sova_profile_page_update_key(SOVA_PROFILE_PAGE(roccat_config_window_pages_get_page(ROCCAT_CONFIG_WINDOW_PAGES(user_data), profile_number - 1)), button_number - 1, macro);
	g_free(macro);
}

static void add_file_filter_to_file_chooser(GtkFileChooser *chooser) {
	GtkFileFilter *filter;
	filter = gtk_file_filter_new();
	gtk_file_filter_set_name(filter, _("Sova Windows profiles"));
	gtk_file_filter_add_pattern(filter, "*." ROCCAT_KEYBOARD_PROFILE_EXTENSION);
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(chooser), filter);

	filter = gtk_file_filter_new();
	gtk_file_filter_set_name(filter, _("All files"));
	gtk_file_filter_add_pattern(filter, "*");
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(chooser), filter);
}

static gchar *sova_create_filename_proposition(SovaProfileData const *profile_data, guint profile_index) {
	return roccat_profile_filename_proposition(gaminggear_profile_data_get_name(GAMINGGEAR_PROFILE_DATA(profile_data)), profile_index, ROCCAT_KEYBOARD_PROFILE_EXTENSION);
}

static void load_profile_from_file_cb(RoccatProfilePage *profile_page, gpointer user_data) {
	SovaconfigWindow *window = SOVACONFIG_WINDOW(user_data);
	SovaconfigWindowPrivate *priv = window->priv;
	GtkWidget *dialog;
	gchar *filename, *path;
	GError *error = NULL;
	gpointer rkp;
	gsize length;
	SovaProfileData *profile_data;

	dialog = gtk_file_chooser_dialog_new(_("Load profile"),
			GTK_WINDOW(window),
			GTK_FILE_CHOOSER_ACTION_OPEN,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
			NULL);

	add_file_filter_to_file_chooser(GTK_FILE_CHOOSER(dialog));
	path = sova_configuration_get_rkp_path(priv->config);
	gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), path);
	g_free(path);

	if (gtk_dialog_run(GTK_DIALOG(dialog)) != GTK_RESPONSE_ACCEPT)
		goto out;

	filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
	path = g_path_get_dirname(filename);
	sova_configuration_set_rkp_path(priv->config, path);
	g_free(path);

	rkp = roccat_swarm_rmp_read_with_path(filename, &length, &error);
	g_free(filename);
	if (error) {
		roccat_warning_dialog(GTK_WINDOW(dialog), _("Could not load profile"), error->message);
		g_error_free(error);
		goto out;
	}

	profile_data = sova_profile_data_new();
	sova_rkp_to_profile_data(profile_data, rkp, length);
	g_free(rkp);
	if (!profile_data) {
		roccat_warning_dialog(GTK_WINDOW(dialog), _("Could not convert profile"), NULL);
		goto out;
	}

	sova_profile_page_set_profile_data(SOVA_PROFILE_PAGE(profile_page), profile_data, priv->config);
	g_object_unref(profile_data);

out:
	gtk_widget_destroy(dialog);
}

static void save_profile_to_file(SovaconfigWindow *window, SovaProfilePage *profile_page) {
	SovaconfigWindowPrivate *priv = window->priv;
	GtkWidget *dialog;
	gchar *filename, *path;
	gchar *temp_filename;
	GError *error = NULL;
	guint profile_index = roccat_config_window_pages_get_page_index(ROCCAT_CONFIG_WINDOW_PAGES(window), ROCCAT_PROFILE_PAGE(profile_page));
	gpointer rkp;
	gsize length;
	SovaProfileData *profile_data;

	profile_data = sova_profile_page_get_profile_data(profile_page, priv->config);

	dialog = gtk_file_chooser_dialog_new(_("Save profile"),
			GTK_WINDOW(window),
			GTK_FILE_CHOOSER_ACTION_SAVE,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
			NULL);

	add_file_filter_to_file_chooser(GTK_FILE_CHOOSER(dialog));

	path = sova_configuration_get_rkp_path(priv->config);
	gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), path);
	g_free(path);

	filename = sova_create_filename_proposition(profile_data, profile_index);
	gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog), filename);
	g_free(filename);

	if (gtk_dialog_run(GTK_DIALOG(dialog)) != GTK_RESPONSE_ACCEPT)
		goto out;

	filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
	temp_filename = roccat_create_filename_with_extension(filename, ROCCAT_KEYBOARD_PROFILE_EXTENSION);
	g_free(filename);

	path = g_path_get_dirname(temp_filename);
	sova_configuration_set_rkp_path(priv->config, path);
	g_free(path);

	rkp = sova_profile_data_to_rkp(profile_data, &length);
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
	gtk_widget_destroy(dialog);
	g_object_unref(profile_data);
}

static void save_profile_to_file_cb(RoccatProfilePage *profile_page, gpointer user_data) {
	save_profile_to_file(SOVACONFIG_WINDOW(user_data), SOVA_PROFILE_PAGE(profile_page));
}

static gboolean remove_page_cb(RoccatConfigWindowPages *roccat_window, guint index, gpointer user_data) {
	SovaconfigWindow *sova_config_window = SOVACONFIG_WINDOW(roccat_window);
	SovaconfigWindowPrivate *priv = sova_config_window->priv;
	SovaProfilePage *profile_page;
	SovaProfileData *profile_data;
	GtkWidget *dialog;
	gint dialog_result;
	gboolean result = TRUE;

	profile_page = SOVA_PROFILE_PAGE(roccat_config_window_pages_get_page(roccat_window, index));
	profile_data = sova_profile_page_get_profile_data(profile_page, priv->config);

	if (gaminggear_profile_data_get_modified(GAMINGGEAR_PROFILE_DATA(profile_data))) {
		dialog = gtk_message_dialog_new(GTK_WINDOW(roccat_window), GTK_DIALOG_MODAL, GTK_MESSAGE_QUESTION, GTK_BUTTONS_NONE, _("There is unsaved data."));
		gtk_dialog_add_buttons(GTK_DIALOG(dialog), GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_DISCARD, GTK_RESPONSE_REJECT, GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT, NULL);
		gtk_window_set_icon_name(GTK_WINDOW(dialog), "roccat");
		gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(dialog), _("Do you really want to remove this profile?"));
		gtk_window_set_title(GTK_WINDOW(dialog), _("Remove profile?"));
		dialog_result = gtk_dialog_run(GTK_DIALOG(dialog));
		gtk_widget_destroy(dialog);
		if (dialog_result == GTK_RESPONSE_ACCEPT)
			save_profile_to_file(sova_config_window, profile_page);
		else if (dialog_result != GTK_RESPONSE_REJECT)
			result = FALSE;
	}
	g_object_unref(profile_data);
	return result;
}

static void add_page(SovaconfigWindow *config_window, SovaProfileData *profile_data) {
	SovaconfigWindowPrivate *priv = config_window->priv;
	SovaProfilePage *profile_page;

	profile_page = SOVA_PROFILE_PAGE(sova_profile_page_new());
	roccat_config_window_pages_append_page(ROCCAT_CONFIG_WINDOW_PAGES(config_window), ROCCAT_PROFILE_PAGE(profile_page));
	g_signal_connect(G_OBJECT(profile_page), "load-from-file", G_CALLBACK(load_profile_from_file_cb), config_window);
	g_signal_connect(G_OBJECT(profile_page), "save-to-file", G_CALLBACK(save_profile_to_file_cb), config_window);
	sova_profile_page_set_keyboard_layout(profile_page, sova_configuration_get_layout(priv->config));
	sova_profile_page_set_profile_data(profile_page, profile_data, priv->config);
}

static void add_page_cb(RoccatConfigWindowPages *roccat_window, gpointer user_data) {
	SovaProfileData *profile_data = sova_profile_data_new();
	add_page(SOVACONFIG_WINDOW(roccat_window), profile_data);
	g_object_unref(profile_data);
}

static void add_pages(SovaconfigWindow *window) {
	SovaconfigWindowPrivate *priv = window->priv;
	GError *local_error = NULL;
	guint i;
	SovaProfileData *profile_data;
	RoccatDevice *device;
	SovaProfile *profile;

	device = roccat_config_window_get_device(ROCCAT_CONFIG_WINDOW(window));

	profile = sova_profile_read(device, &local_error);
	if (!roccat_handle_error_dialog(GTK_WINDOW(window), _("Could not read actual profile"), &local_error))
		return;

	priv->actual_profile_count = profile->count;
	if (priv->actual_profile_count != 0) {
		for (i = 0; i < priv->actual_profile_count; ++i) {
			profile_data = sova_profile_data_new();

			sova_profile_data_load(profile_data, i, &local_error);
			if (!roccat_handle_error_dialog(GTK_WINDOW(window), _("Could not load filesystem data for profile"), &local_error)) {
				g_object_unref(profile_data);
				break;
			}

			gaminggear_profile_data_read(GAMINGGEAR_PROFILE_DATA(profile_data), GAMINGGEAR_DEVICE(device), &local_error);
			if (!roccat_handle_error_dialog(GTK_WINDOW(window), _("Could not load hardware data for profile"), &local_error)) {
				g_object_unref(profile_data);
				break;
			}

			add_page(window, profile_data);
			g_object_unref(profile_data);
		}
		roccat_config_window_pages_set_active_page_blocked(ROCCAT_CONFIG_WINDOW_PAGES(window), profile->index);
	} else {
		profile_data = sova_profile_data_new();
		add_page(window, profile_data);
		g_object_unref(profile_data);
		roccat_config_window_pages_set_active_page_blocked(ROCCAT_CONFIG_WINDOW_PAGES(window), 0);
	}

	dbus_g_proxy_connect_signal(priv->dbus_proxy, "ProfileChanged", G_CALLBACK(actual_profile_changed_from_device_cb), window, NULL);
	dbus_g_proxy_connect_signal(priv->dbus_proxy, "BrightnessChanged", G_CALLBACK(brightness_changed_from_device_cb), window, NULL);
	dbus_g_proxy_connect_signal(priv->dbus_proxy, "MacroChanged", G_CALLBACK(macro_changed_from_device_cb), window, NULL);
}

static void device_add_cb(RoccatConfigWindow *roccat_window, gpointer user_data) {
	add_pages(SOVACONFIG_WINDOW(roccat_window));
}

static void device_remove_cb(RoccatConfigWindow *roccat_window, gpointer user_data) {
	SovaconfigWindow *window = SOVACONFIG_WINDOW(roccat_window);
	SovaconfigWindowPrivate *priv = window->priv;

	roccat_config_window_pages_remove_pages(ROCCAT_CONFIG_WINDOW_PAGES(roccat_window));
	dbus_g_proxy_disconnect_signal(priv->dbus_proxy, "ProfileChanged", G_CALLBACK(actual_profile_changed_from_device_cb), window);
	dbus_g_proxy_disconnect_signal(priv->dbus_proxy, "BrightnessChanged", G_CALLBACK(brightness_changed_from_device_cb), window);
	dbus_g_proxy_disconnect_signal(priv->dbus_proxy, "MacroChanged", G_CALLBACK(macro_changed_from_device_cb), window);
}

static SovaProfileData *get_profile_data(SovaconfigWindow *window, SovaProfilePage *profile_page) {
	SovaProfileData *profile_data;

	profile_data = sova_profile_page_get_profile_data(profile_page, window->priv->config);

	// Always set index. Profile data decides if changed or not.
	gaminggear_profile_data_set_hardware_index(GAMINGGEAR_PROFILE_DATA(profile_data), roccat_config_window_pages_get_page_index(ROCCAT_CONFIG_WINDOW_PAGES(window), ROCCAT_PROFILE_PAGE(profile_page)));

	return profile_data;
}

static void set_profile_data(SovaconfigWindow *window, guint profile_index, SovaProfileData *profile_data) {
	SovaconfigWindowPrivate *priv = window->priv;
	SovaProfilePage *profile_page = SOVA_PROFILE_PAGE(roccat_config_window_pages_get_page(ROCCAT_CONFIG_WINDOW_PAGES(window), profile_index));
	guint profile_number = profile_index + 1;
	sova_profile_page_set_profile_data(profile_page, profile_data, priv->config);
	roccat_config_window_pages_set_page_unmoved(ROCCAT_CONFIG_WINDOW_PAGES(window), ROCCAT_PROFILE_PAGE(profile_page));
	sova_dbus_emit_profile_data_changed_outside(priv->dbus_proxy, profile_number);
}

static gboolean save_single(SovaconfigWindow *window, SovaProfileData *profile_data, guint profile_index, GError **error) {
	RoccatDevice *device = roccat_config_window_get_device(ROCCAT_CONFIG_WINDOW(window));

	gaminggear_profile_data_store(GAMINGGEAR_PROFILE_DATA(profile_data), GAMINGGEAR_DEVICE(device), error);

	if (*error)
		return FALSE;
	else {
		set_profile_data(window, profile_index, profile_data);
		return TRUE;
	}
}

/* returns successfully_saved */
static gboolean save_all(SovaconfigWindow *window, gboolean ask) {
	SovaconfigWindowPrivate *priv = window->priv;
	SovaProfileData *profile_datas[SOVA_PROFILE_NUM];
	SovaProfilePage *profile_page;
	GError *local_error = NULL;
	gboolean modified;
	gboolean retval;
	guint profiles;
	guint i;

	profiles = roccat_config_window_pages_get_num_pages(ROCCAT_CONFIG_WINDOW_PAGES(window));

	modified = FALSE;
	for (i = 0; i < profiles; ++i) {
		profile_page = SOVA_PROFILE_PAGE(roccat_config_window_pages_get_page(ROCCAT_CONFIG_WINDOW_PAGES(window), i));
		profile_datas[i] = get_profile_data(window, profile_page);
		if (gaminggear_profile_data_get_modified(GAMINGGEAR_PROFILE_DATA(profile_datas[i])))
			modified = TRUE;
	}
	if (profiles != priv->actual_profile_count)
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

	if (profiles != priv->actual_profile_count && roccat_config_window_get_device(ROCCAT_CONFIG_WINDOW(window))) {
		sova_profile_write_index(roccat_config_window_get_device(ROCCAT_CONFIG_WINDOW(window)),
				roccat_config_window_pages_get_active_page(ROCCAT_CONFIG_WINDOW_PAGES(window)), profiles, &local_error);
		if (roccat_handle_error_dialog(GTK_WINDOW(window), _("Could not save profile information"), &local_error))
			priv->actual_profile_count = profiles;
	}

	retval = TRUE;
	for (i = 0; i < profiles; ++i) {
		if (!save_single(window, profile_datas[i], i, &local_error)) {
			retval = FALSE;
			break;
		}
	}
	roccat_handle_error_dialog(GTK_WINDOW(window), _("Could not save unsaved data"), &local_error);

exit:
	for (i = 0; i < profiles; ++i)
		g_object_unref(profile_datas[i]);

	return retval;
}

static gboolean delete_event_cb(GtkWidget *window, GdkEvent *event, gpointer user_data) {
	return !save_all(SOVACONFIG_WINDOW(window), TRUE);
}

static void save_all_cb(RoccatConfigWindow *roccat_window, gpointer user_data) {
	save_all(SOVACONFIG_WINDOW(roccat_window), FALSE);
}

static void set_keyboard_layouts(SovaconfigWindow *window, gchar const *layout) {
	SovaProfilePage *profile_page;
	guint i;

	for (i = 0; i < roccat_config_window_pages_get_num_pages(ROCCAT_CONFIG_WINDOW_PAGES(window)); ++i) {
		profile_page = SOVA_PROFILE_PAGE(roccat_config_window_pages_get_page(ROCCAT_CONFIG_WINDOW_PAGES(window), i));
		sova_profile_page_set_keyboard_layout(profile_page, layout);
	}
}

static void menu_edit_preferences_cb(GtkMenuItem *item, gpointer user_data) {
	SovaconfigWindow *window = SOVACONFIG_WINDOW(user_data);
	SovaconfigWindowPrivate *priv = window->priv;
	sova_configuration_dialog(GTK_WINDOW(window), priv->config);
	if (TRUE) {
		set_keyboard_layouts(window, sova_configuration_get_layout(priv->config));
		sova_dbus_emit_configuration_changed_outside(priv->dbus_proxy);
	}
}

static void menu_device_info_cb(GtkMenuItem *item, gpointer user_data) {
	SovaconfigWindow *window = SOVACONFIG_WINDOW(user_data);
	RoccatInfoDialog *dialog;
	GError *local_error = NULL;
	SovaInfo *info;
	gchar *string;

	if (!roccat_config_window_warn_if_no_device(ROCCAT_CONFIG_WINDOW(window)))
		return;

	info = sova_info_read(roccat_config_window_get_device(ROCCAT_CONFIG_WINDOW(window)), &local_error);
	if (!roccat_handle_error_dialog(GTK_WINDOW(window), _("Could not read informations"), &local_error))
		return;

	dialog = ROCCAT_INFO_DIALOG(roccat_info_dialog_new(GTK_WINDOW(window)));

	string = roccat_firmware_version_to_string(info->firmware_version);
	roccat_info_dialog_add_line(dialog, _("Firmware version"), gtk_label_new(string));
	g_free(string);

	string = roccat_firmware_version_to_string(info->dfu_version);
	roccat_info_dialog_add_line(dialog, _("DFU version"), gtk_label_new(string));
	g_free(string);

	g_free(info);

	(void)gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(GTK_WIDGET(dialog));
}

static void emit_profile_data_changed_outside_for_all(SovaconfigWindow *window) {
	guint i;
	for (i = 0; i < roccat_config_window_pages_get_num_pages(ROCCAT_CONFIG_WINDOW_PAGES(window)); ++i)
		sova_dbus_emit_profile_data_changed_outside(window->priv->dbus_proxy, i + 1);
}

static void pre_firmware_update_cb(RoccatUpdateAssistant *assistant, gpointer user_data) {
	roccat_config_window_device_scanner_suspend(ROCCAT_CONFIG_WINDOW(user_data));
}

static void post_firmware_update_cb(RoccatUpdateAssistant *assistant, gpointer user_data) {
	roccat_config_window_device_scanner_continue(ROCCAT_CONFIG_WINDOW(user_data));
}

static void menu_device_firmware_update_cb(GtkMenuItem *item, gpointer user_data) {
	SovaconfigWindow *window = SOVACONFIG_WINDOW(user_data);
	GError *local_error = NULL;
	RoccatDevice *device;
	GtkWidget *assistant;
	guint version;
	gchar *version_string;

	if (!roccat_config_window_warn_if_no_device(ROCCAT_CONFIG_WINDOW(window)))
		return;

	device = roccat_config_window_get_device(ROCCAT_CONFIG_WINDOW(window));
	version = sova_firmware_version_read(device, &local_error);
	if (local_error) {
		roccat_warning_dialog(GTK_WINDOW(window), _("Could not read version info"), local_error->message);
		g_error_free(local_error);
		return;
	}

	version_string = roccat_firmware_version_to_string(version);
	assistant = roccat_update_assistant_new(GTK_WINDOW(window), device, SOVA_INTERFACE_KEYBOARD);
	g_object_set(G_OBJECT(assistant),
			"wait-prepare", SOVA_FIRMWARE_UPDATE_WAIT_PREPARE,
			"wait-init-data", SOVA_FIRMWARE_UPDATE_WAIT_INIT_DATA,
			"wait-data", SOVA_FIRMWARE_UPDATE_WAIT_DATA,
			"final-write-check", SOVA_FIRMWARE_UPDATE_FINAL_WRITE_CHECK,
			NULL);
	roccat_update_assistant_add_firmware(ROCCAT_UPDATE_ASSISTANT(assistant), _("Firmware"), version_string, SOVA_FIRMWARE_SIZE, ROCCAT_FIRMWARE_NUMBER_DEFAULT);
	g_free(version_string);

	g_signal_connect(G_OBJECT(assistant), "pre-action", G_CALLBACK(pre_firmware_update_cb), window);
	g_signal_connect(G_OBJECT(assistant), "post-action", G_CALLBACK(post_firmware_update_cb), window);

	gtk_widget_show_all(assistant);
}

static void menu_device_reset_cb(GtkMenuItem *item, gpointer user_data) {
	SovaconfigWindow *window = SOVACONFIG_WINDOW(user_data);
	GError *error = NULL;
	RoccatDevice *device;

	if (!roccat_config_window_warn_if_no_device(ROCCAT_CONFIG_WINDOW(window)))
		return;

	if (!roccat_continue_dialog(GTK_WINDOW(window), _("This resets the device to factory defaults.")))
		return;

	device = roccat_config_window_get_device(ROCCAT_CONFIG_WINDOW(window));
	sova_reset(device, SOVA_RESET_FUNCTION_RESET, &error);
	if (!roccat_handle_error_dialog(GTK_WINDOW(window), _("Could not reset device"), &error))
		return;

	roccat_config_window_pages_remove_pages(ROCCAT_CONFIG_WINDOW_PAGES(window));
	add_pages(window);
	emit_profile_data_changed_outside_for_all(window);
}

GtkWidget *sovaconfig_window_new(void) {
	return GTK_WIDGET(g_object_new(SOVACONFIG_WINDOW_TYPE,
			"device-name", SOVA_DEVICE_NAME,
			"variable-pages", SOVA_PROFILE_NUM,
			NULL));
}

static GObject *constructor(GType gtype, guint n_properties, GObjectConstructParam *properties) {
	GObject *obj;
	SovaconfigWindow *window;
	RoccatConfigWindow *roccat_window;
	SovaconfigWindowPrivate *priv;
	GtkMenuItem *menu_item;

	obj = G_OBJECT_CLASS(sovaconfig_window_parent_class)->constructor(gtype, n_properties, properties);
	window = SOVACONFIG_WINDOW(obj);
	roccat_window = ROCCAT_CONFIG_WINDOW(obj);
	priv = window->priv;

	priv->config = sova_configuration_load();

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
	g_signal_connect(G_OBJECT(roccat_window), "add-page", G_CALLBACK(add_page_cb), NULL);
	g_signal_connect(G_OBJECT(roccat_window), "remove-page", G_CALLBACK(remove_page_cb), NULL);

	/* keep this order */
	priv->dbus_proxy = sova_dbus_proxy_new();

	roccat_config_window_set_device_scanner(roccat_window, ROCCAT_DEVICE_SCANNER_INTERFACE(sova_device_scanner_new()));

	gtk_widget_show(GTK_WIDGET(window));

	/* if a device is already plugged in, add_cb has already been executed */
	roccat_config_window_warn_if_no_device(roccat_window);

	return obj;
}

static void sovaconfig_window_init(SovaconfigWindow *window) {
	window->priv = SOVACONFIG_WINDOW_GET_PRIVATE(window);
}

static void finalize(GObject *object) {
	SovaconfigWindowPrivate *priv = SOVACONFIG_WINDOW(object)->priv;

	sova_configuration_dialog_save(GTK_WINDOW(object), priv->config);
	sova_dbus_emit_configuration_changed_outside(priv->dbus_proxy);
	g_clear_pointer(&priv->config, roccat_configuration_free);
	g_clear_pointer(&priv->dbus_proxy, dbus_roccat_proxy_free);

	G_OBJECT_CLASS(sovaconfig_window_parent_class)->finalize(object);
}

static void sovaconfig_window_class_init(SovaconfigWindowClass *klass) {
	GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

	gobject_class->constructor = constructor;
	gobject_class->finalize = finalize;

	g_type_class_add_private(klass, sizeof(SovaconfigWindowClass));
}
