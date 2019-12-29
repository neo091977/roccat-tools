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

#include "kova2016config_window.h"
#include "kova2016_configuration_dialog.h"
#include "kova2016_profile_page.h"
#include "kova2016_config.h"
#include "kova2016_device.h"
#include "kova2016_firmware.h"
#include "kova2016_profile_data.h"
#include "kova2016_profile.h"
#include "kova2016_dbus_services.h"
#include "kova2016_profile.h"
#include "kova2016_rmp.h"
#include "kova2016_sensor.h"
#include "kova2016_info.h"
#include "roccat_helper.h"
#include "roccat_warning_dialog.h"
#include "roccat_save_dialog.h"
#include "roccat_sensor_register_dialog.h"
#include "roccat_continue_dialog.h"
#include "roccat_info_dialog.h"
#include "roccat_update_assistant.h"
#include "g_roccat_helper.h"
#include "g_dbus_roccat_helper.h"
#include "roccat.h"
#include "i18n.h"

#define KOVA2016CONFIG_WINDOW_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), KOVA2016CONFIG_WINDOW_TYPE, Kova2016configWindowClass))
#define IS_KOVA2016CONFIG_WINDOW_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), KOVA2016CONFIG_WINDOW_TYPE))
#define KOVA2016CONFIG_WINDOW_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), KOVA2016CONFIG_WINDOW_TYPE, Kova2016configWindowPrivate))

typedef struct _Kova2016configWindowClass Kova2016configWindowClass;
typedef struct _Kova2016configWindowPrivate Kova2016configWindowPrivate;

struct _Kova2016configWindow {
	RoccatConfigWindowPages parent;
	Kova2016configWindowPrivate *priv;
};

struct _Kova2016configWindowClass {
	RoccatConfigWindowPagesClass parent_class;
};

struct _Kova2016configWindowPrivate {
	RoccatKeyFile *config;
	DBusGProxy *dbus_proxy;
	guint actual_profile_count;
};

G_DEFINE_TYPE(Kova2016configWindow, kova2016config_window, ROCCAT_CONFIG_WINDOW_PAGES_TYPE);

static gboolean change_profile_handler(gpointer user_data) {
	RoccatConfigWindow *roccat_window = ROCCAT_CONFIG_WINDOW(user_data);
	Kova2016configWindowPrivate *priv = KOVA2016CONFIG_WINDOW(roccat_window)->priv;
	guint profile_index = roccat_config_window_pages_get_active_page(ROCCAT_CONFIG_WINDOW_PAGES(roccat_window));
	guint profile_number = profile_index + 1;
	GError *local_error = NULL;

	if (!roccat_config_window_warn_if_no_device(roccat_window))
		return FALSE;

	kova2016_profile_write_index(roccat_config_window_get_device(roccat_window), profile_index, priv->actual_profile_count, &local_error);
	if (!roccat_handle_error_dialog(GTK_WINDOW(roccat_window), _("Could not activate profile"), &local_error))
		return FALSE;
	kova2016_dbus_emit_profile_changed_outside(priv->dbus_proxy, profile_number);

	return FALSE;
}

static void window_active_page_changed_cb(RoccatConfigWindowPages *roccat_window, gpointer user_data) {
	g_idle_add(change_profile_handler, roccat_window);
}

static void actual_profile_changed_from_device_cb(DBusGProxy *proxy, guchar profile_number, gpointer user_data) {
	roccat_config_window_pages_set_active_page_blocked(ROCCAT_CONFIG_WINDOW_PAGES(user_data), profile_number - 1);
}

static void add_file_filter_to_file_chooser(GtkFileChooser *chooser) {
	GtkFileFilter *filter;
	filter = gtk_file_filter_new();
	gtk_file_filter_set_name(filter, _("Kova2016 Windows profiles"));
	gtk_file_filter_add_pattern(filter, "*." ROCCAT_MOUSE_PROFILE_EXTENSION);
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(chooser), filter);

	filter = gtk_file_filter_new();
	gtk_file_filter_set_name(filter, _("All files"));
	gtk_file_filter_add_pattern(filter, "*");
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(chooser), filter);
}

static gchar *kova2016_create_filename_proposition(Kova2016ProfileData const *profile_data, guint profile_index) {
	return roccat_profile_filename_proposition(profile_data->eventhandler.profile_name, profile_index, ROCCAT_MOUSE_PROFILE_EXTENSION);
}

static void load_profile_from_file_cb(RoccatProfilePage *profile_page, gpointer user_data) {
	Kova2016configWindow *window = KOVA2016CONFIG_WINDOW(user_data);
	Kova2016configWindowPrivate *priv = window->priv;
	GtkWidget *dialog;
	gchar *filename, *path;
	GError *error = NULL;
	gpointer rmp;
	gsize length;
	Kova2016ProfileData *profile_data;

	dialog = gtk_file_chooser_dialog_new(_("Load profile"),
			GTK_WINDOW(window),
			GTK_FILE_CHOOSER_ACTION_OPEN,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
			NULL);

	add_file_filter_to_file_chooser(GTK_FILE_CHOOSER(dialog));
	path = kova2016_configuration_get_rmp_path(priv->config);
	gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), path);
	g_free(path);

	if (gtk_dialog_run(GTK_DIALOG(dialog)) != GTK_RESPONSE_ACCEPT)
		goto out;

	filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
	path = g_path_get_dirname(filename);
	kova2016_configuration_set_rmp_path(priv->config, path);
	g_free(path);

	rmp = roccat_swarm_rmp_read_with_path(filename, &length, &error);
	g_free(filename);
	if (error) {
		roccat_warning_dialog(GTK_WINDOW(dialog), _("Could not load profile"), error->message);
		g_error_free(error);
		goto out;
	}

	profile_data = kova2016_rmp_to_profile_data(rmp, length);
	g_free(rmp);
	if (!profile_data) {
		roccat_warning_dialog(GTK_WINDOW(dialog), _("Could not convert profile"), NULL);
		goto out;
	}

	kova2016_profile_page_set_profile_data(KOVA2016_PROFILE_PAGE(profile_page), profile_data);
	g_free(profile_data);

out:
	gtk_widget_destroy(dialog);
}

static void save_profile_to_file(Kova2016configWindow *window, Kova2016ProfilePage *profile_page) {
	Kova2016configWindowPrivate *priv = window->priv;
	GtkWidget *dialog;
	gchar *filename, *path;
	gchar *temp_filename;
	GError *error = NULL;
	guint profile_index = roccat_config_window_pages_get_page_index(ROCCAT_CONFIG_WINDOW_PAGES(window), ROCCAT_PROFILE_PAGE(profile_page));
	gpointer rmp;
	gsize length;
	Kova2016ProfileData *profile_data;

	profile_data = kova2016_profile_page_get_profile_data(profile_page);

	dialog = gtk_file_chooser_dialog_new(_("Save profile"),
			GTK_WINDOW(window),
			GTK_FILE_CHOOSER_ACTION_SAVE,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
			NULL);

	add_file_filter_to_file_chooser(GTK_FILE_CHOOSER(dialog));

	path = kova2016_configuration_get_rmp_path(priv->config);
	gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), path);
	g_free(path);

	filename = kova2016_create_filename_proposition(profile_data, profile_index);
	gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog), filename);
	g_free(filename);

	if (gtk_dialog_run(GTK_DIALOG(dialog)) != GTK_RESPONSE_ACCEPT)
		goto out;

	filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
	temp_filename = roccat_create_filename_with_extension(filename, ROCCAT_MOUSE_PROFILE_EXTENSION);
	g_free(filename);

	path = g_path_get_dirname(temp_filename);
	kova2016_configuration_set_rmp_path(priv->config, path);
	g_free(path);

	rmp = kova2016_profile_data_to_rmp(profile_data, &length);
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
	save_profile_to_file(KOVA2016CONFIG_WINDOW(user_data), KOVA2016_PROFILE_PAGE(profile_page));
}

static gboolean remove_page_cb(RoccatConfigWindowPages *roccat_window, guint index, gpointer user_data) {
	Kova2016ProfilePage *profile_page;
	Kova2016ProfileData *profile_data;
	GtkWidget *dialog;
	gint dialog_result;
	gboolean result = TRUE;

	profile_page = KOVA2016_PROFILE_PAGE(roccat_config_window_pages_get_page(roccat_window, index));
	profile_data = kova2016_profile_page_get_profile_data(profile_page);

	if (kova2016_profile_data_get_modified(profile_data)) {
		dialog = gtk_message_dialog_new(GTK_WINDOW(roccat_window), GTK_DIALOG_MODAL, GTK_MESSAGE_QUESTION, GTK_BUTTONS_NONE, _("There is unsaved data."));
		gtk_dialog_add_buttons(GTK_DIALOG(dialog), GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_DISCARD, GTK_RESPONSE_REJECT, GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT, NULL);
		gtk_window_set_icon_name(GTK_WINDOW(dialog), "roccat");
		gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(dialog), _("Do you really want to remove this profile?"));
		gtk_window_set_title(GTK_WINDOW(dialog), _("Remove profile?"));
		dialog_result = gtk_dialog_run(GTK_DIALOG(dialog));
		gtk_widget_destroy(dialog);
		if (dialog_result == GTK_RESPONSE_ACCEPT)
			save_profile_to_file(KOVA2016CONFIG_WINDOW(roccat_window), profile_page);
		else if (dialog_result != GTK_RESPONSE_REJECT)
			result = FALSE;
	}
	g_free(profile_data);
	return result;
}

static void add_page(Kova2016configWindow *config_window, Kova2016ProfileData *profile_data) {
	Kova2016ProfilePage *profile_page;

	profile_page = KOVA2016_PROFILE_PAGE(kova2016_profile_page_new());
	roccat_config_window_pages_append_page(ROCCAT_CONFIG_WINDOW_PAGES(config_window), ROCCAT_PROFILE_PAGE(profile_page));
	g_signal_connect(G_OBJECT(profile_page), "load-from-file", G_CALLBACK(load_profile_from_file_cb), config_window);
	g_signal_connect(G_OBJECT(profile_page), "save-to-file", G_CALLBACK(save_profile_to_file_cb), config_window);
	kova2016_profile_page_set_profile_data(profile_page, profile_data);
}

static void add_page_cb(RoccatConfigWindowPages *roccat_window, gpointer user_data) {
	Kova2016ProfileData *profile_data = kova2016_profile_data_new();
	add_page(KOVA2016CONFIG_WINDOW(roccat_window), profile_data);
	g_free(profile_data);
}

static void add_pages(Kova2016configWindow *window) {
	Kova2016configWindowPrivate *priv = window->priv;
	GError *local_error = NULL;
	guint i;
	Kova2016ProfileData *profile_data;
	RoccatDevice *device;
	Kova2016Profile *profile;

	device = roccat_config_window_get_device(ROCCAT_CONFIG_WINDOW(window));

	profile = kova2016_profile_read(device, &local_error);
	if (!roccat_handle_error_dialog(GTK_WINDOW(window), _("Could not read actual profile"), &local_error))
		return;

	priv->actual_profile_count = profile->count;
	if (priv->actual_profile_count != 0) {
		for (i = 0; i < priv->actual_profile_count; ++i) {
			profile_data = kova2016_profile_data_new();

			kova2016_profile_data_update_filesystem(profile_data, i, &local_error);
			if (!roccat_handle_error_dialog(GTK_WINDOW(window), _("Could not load filesystem data for profile"), &local_error)) {
				g_free(profile_data);
				break;
			}

			kova2016_profile_data_update_hardware(profile_data, device, i, &local_error);
			if (!roccat_handle_error_dialog(GTK_WINDOW(window), _("Could not load hardware data for profile"), &local_error)) {
				g_free(profile_data);
				break;
			}

			add_page(window, profile_data);
			g_free(profile_data);
		}
		roccat_config_window_pages_set_active_page_blocked(ROCCAT_CONFIG_WINDOW_PAGES(window), profile->index);
	} else {
		profile_data = kova2016_profile_data_new();
		add_page(window, profile_data);
		g_free(profile_data);
		roccat_config_window_pages_set_active_page_blocked(ROCCAT_CONFIG_WINDOW_PAGES(window), 0);
	}

	g_free(profile);

	dbus_g_proxy_connect_signal(priv->dbus_proxy, "ProfileChanged", G_CALLBACK(actual_profile_changed_from_device_cb), window, NULL);
}

static void device_add_cb(RoccatConfigWindow *roccat_window, gpointer user_data) {
	add_pages(KOVA2016CONFIG_WINDOW(roccat_window));
}

static void device_remove_cb(RoccatConfigWindow *roccat_window, gpointer user_data) {
	Kova2016configWindow *window = KOVA2016CONFIG_WINDOW(roccat_window);
	Kova2016configWindowPrivate *priv = window->priv;

	roccat_config_window_pages_remove_pages(ROCCAT_CONFIG_WINDOW_PAGES(roccat_window));
	dbus_g_proxy_disconnect_signal(priv->dbus_proxy, "ProfileChanged", G_CALLBACK(actual_profile_changed_from_device_cb), window);
}

static Kova2016ProfileData *get_profile_data(Kova2016configWindow *window, Kova2016ProfilePage *profile_page) {
	Kova2016ProfileData *profile_data;
	gboolean moved;

	profile_data = kova2016_profile_page_get_profile_data(profile_page);
	moved = roccat_config_window_pages_get_page_moved(ROCCAT_CONFIG_WINDOW_PAGES(window), ROCCAT_PROFILE_PAGE(profile_page));

	if (moved)
		kova2016_profile_data_set_modified(profile_data);

	return profile_data;
}

static void set_profile_data(Kova2016configWindow *window, guint profile_index, Kova2016ProfileData *profile_data) {
	Kova2016ProfilePage *profile_page = KOVA2016_PROFILE_PAGE(roccat_config_window_pages_get_page(ROCCAT_CONFIG_WINDOW_PAGES(window), profile_index));
	guint profile_number = profile_index + 1;
	kova2016_profile_page_set_profile_data(profile_page, profile_data);
	roccat_config_window_pages_set_page_unmoved(ROCCAT_CONFIG_WINDOW_PAGES(window), ROCCAT_PROFILE_PAGE(profile_page));
	kova2016_dbus_emit_profile_data_changed_outside(window->priv->dbus_proxy, profile_number);
}

static gboolean save_single(Kova2016configWindow *window, Kova2016ProfileData *profile_data, guint profile_index, GError **error) {
	RoccatDevice *device = roccat_config_window_get_device(ROCCAT_CONFIG_WINDOW(window));

	if (device)
		kova2016_profile_data_save(device, profile_data, profile_index, error);
	else
		kova2016_profile_data_eventhandler_save(&profile_data->eventhandler, profile_index, error);

	if (*error)
		return FALSE;
	else {
		set_profile_data(window, profile_index, profile_data);
		return TRUE;
	}
}

/* returns successfully_saved */
static gboolean save_all(Kova2016configWindow *window, gboolean ask) {
	Kova2016configWindowPrivate *priv = window->priv;
	Kova2016ProfileData *profile_datas[KOVA2016_PROFILE_NUM];
	Kova2016ProfilePage *profile_page;
	GError *local_error = NULL;
	gboolean modified;
	gboolean retval;
	guint profiles;
	guint i;

	profiles = roccat_config_window_pages_get_num_pages(ROCCAT_CONFIG_WINDOW_PAGES(window));

	modified = FALSE;
	for (i = 0; i < profiles; ++i) {
		profile_page = KOVA2016_PROFILE_PAGE(roccat_config_window_pages_get_page(ROCCAT_CONFIG_WINDOW_PAGES(window), i));
		profile_datas[i] = get_profile_data(window, profile_page);
		if (kova2016_profile_data_get_modified(profile_datas[i]))
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
		kova2016_profile_write_index(roccat_config_window_get_device(ROCCAT_CONFIG_WINDOW(window)),
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
		g_free(profile_datas[i]);

	return retval;
}

static gboolean delete_event_cb(GtkWidget *window, GdkEvent *event, gpointer user_data) {
	return !save_all(KOVA2016CONFIG_WINDOW(window), TRUE);
}

static void save_all_cb(RoccatConfigWindow *roccat_window, gpointer user_data) {
	save_all(KOVA2016CONFIG_WINDOW(roccat_window), FALSE);
}

static void sensor_read_cb(RoccatSensorRegisterDialog *dialog, gpointer user_data) {
	Kova2016configWindow *window = KOVA2016CONFIG_WINDOW(user_data);
	guint reg = roccat_sensor_register_dialog_get_register(dialog);
	guint value;
	GError *error = NULL;

	value = kova2016_sensor_read_register(roccat_config_window_get_device(ROCCAT_CONFIG_WINDOW(window)), reg, &error);
	if (roccat_handle_error_dialog(GTK_WINDOW(window), _("Could not read sensor register"), &error))
		roccat_sensor_register_dialog_set_value(dialog, value);
}

static void sensor_write_cb(RoccatSensorRegisterDialog *dialog, gpointer user_data) {
	Kova2016configWindow *window = KOVA2016CONFIG_WINDOW(user_data);
	guint reg = roccat_sensor_register_dialog_get_register(dialog);
	guint value = roccat_sensor_register_dialog_get_value(dialog);
	GError *error = NULL;
	kova2016_sensor_write_register(roccat_config_window_get_device(ROCCAT_CONFIG_WINDOW(window)), reg, value, &error);
	roccat_handle_error_dialog(GTK_WINDOW(window), _("Could not write sensor register"), &error);
}

static void menu_edit_sensor_cb(GtkMenuItem *item, gpointer user_data) {
	Kova2016configWindow *window = KOVA2016CONFIG_WINDOW(user_data);
	RoccatSensorRegisterDialog *dialog;

	if (!roccat_config_window_warn_if_no_device(ROCCAT_CONFIG_WINDOW(window)))
		return;

	dialog = ROCCAT_SENSOR_REGISTER_DIALOG(roccat_sensor_register_dialog_new(GTK_WINDOW(window)));
	g_signal_connect(G_OBJECT(dialog), "read", G_CALLBACK(sensor_read_cb), window);
	g_signal_connect(G_OBJECT(dialog), "write", G_CALLBACK(sensor_write_cb), window);
	(void)gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(GTK_WIDGET(dialog));
}

static void menu_edit_preferences_cb(GtkMenuItem *item, gpointer user_data) {
	Kova2016configWindow *window = KOVA2016CONFIG_WINDOW(user_data);
	Kova2016configWindowPrivate *priv = window->priv;
	kova2016_configuration_dialog(GTK_WINDOW(window), priv->config);
	if (TRUE)
		kova2016_dbus_emit_configuration_changed_outside(priv->dbus_proxy);
}

static void pre_firmware_update_cb(RoccatUpdateAssistant *assistant, gpointer user_data) {
	roccat_config_window_device_scanner_suspend(ROCCAT_CONFIG_WINDOW(user_data));
}

static void post_firmware_update_cb(RoccatUpdateAssistant *assistant, gpointer user_data) {
	roccat_config_window_device_scanner_continue(ROCCAT_CONFIG_WINDOW(user_data));
}

static void menu_device_firmware_update_cb(GtkMenuItem *item, gpointer user_data) {
	Kova2016configWindow *window = KOVA2016CONFIG_WINDOW(user_data);
	GError *local_error = NULL;
	RoccatDevice *kova2016;
	GtkWidget *assistant;
	guint firmware_version;
	gchar *version_string;

	if (!roccat_config_window_warn_if_no_device(ROCCAT_CONFIG_WINDOW(window)))
		return;

	kova2016 = roccat_config_window_get_device(ROCCAT_CONFIG_WINDOW(window));
	firmware_version = kova2016_firmware_version_read(kova2016, &local_error);
	if (local_error) {
		roccat_warning_dialog(GTK_WINDOW(window), _("Could not read version info"), local_error->message);
		g_error_free(local_error);
		return;
	}

	assistant = roccat_update_assistant_new(GTK_WINDOW(window), kova2016, KOVA2016_INTERFACE_MOUSE);
	g_object_set(G_OBJECT(assistant),
			"wait-prepare", KOVA2016_FIRMWARE_UPDATE_WAIT_PREPARE,
			"wait-init-data", KOVA2016_FIRMWARE_UPDATE_WAIT_INIT_DATA,
			"wait-data", KOVA2016_FIRMWARE_UPDATE_WAIT_DATA,
			"final-write-check", KOVA2016_FIRMWARE_UPDATE_FINAL_WRITE_CHECK,
			NULL);

	version_string = roccat_firmware_version_to_string(firmware_version);
	roccat_update_assistant_add_firmware(ROCCAT_UPDATE_ASSISTANT(assistant), _("Main firmware"), version_string,
			KOVA2016_FIRMWARE_SIZE, ROCCAT_FIRMWARE_NUMBER_DEFAULT);
	g_free(version_string);

	g_signal_connect(G_OBJECT(assistant), "pre-action", G_CALLBACK(pre_firmware_update_cb), window);
	g_signal_connect(G_OBJECT(assistant), "post-action", G_CALLBACK(post_firmware_update_cb), window);

	gtk_widget_show_all(assistant);
}

static void menu_device_info_cb(GtkMenuItem *item, gpointer user_data) {
	Kova2016configWindow *window = KOVA2016CONFIG_WINDOW(user_data);
	RoccatInfoDialog *dialog;
	GError *local_error = NULL;
	Kova2016Info *info;
	gchar *string;

	if (!roccat_config_window_warn_if_no_device(ROCCAT_CONFIG_WINDOW(window)))
		return;

	info = kova2016_info_read(roccat_config_window_get_device(ROCCAT_CONFIG_WINDOW(window)), &local_error);
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

static void emit_profile_data_changed_outside_for_all(Kova2016configWindow *window) {
	guint i;
	for (i = 0; i < roccat_config_window_pages_get_num_pages(ROCCAT_CONFIG_WINDOW_PAGES(window)); ++i)
		kova2016_dbus_emit_profile_data_changed_outside(window->priv->dbus_proxy, i + 1);
}

static void menu_device_reset_cb(GtkMenuItem *item, gpointer user_data) {
	Kova2016configWindow *window = KOVA2016CONFIG_WINDOW(user_data);
	GError *error = NULL;
	RoccatDevice *device;

	if (!roccat_config_window_warn_if_no_device(ROCCAT_CONFIG_WINDOW(window)))
		return;

	if (!roccat_continue_dialog(GTK_WINDOW(window), _("This resets the device to factory defaults.")))
		return;

	device = roccat_config_window_get_device(ROCCAT_CONFIG_WINDOW(window));
	kova2016_reset(device, &error);
	if (!roccat_handle_error_dialog(GTK_WINDOW(window), _("Could not reset device"), &error))
		return;

	roccat_config_window_pages_remove_pages(ROCCAT_CONFIG_WINDOW_PAGES(window));
	add_pages(window);
	emit_profile_data_changed_outside_for_all(window);
}

GtkWidget *kova2016config_window_new(void) {
	return GTK_WIDGET(g_object_new(KOVA2016CONFIG_WINDOW_TYPE,
			"device-name", KOVA2016_DEVICE_NAME_COMBINED,
			"variable-pages", KOVA2016_PROFILE_NUM,
			NULL));
}

static GObject *constructor(GType gtype, guint n_properties, GObjectConstructParam *properties) {
	GObject *obj;
	Kova2016configWindow *window;
	RoccatConfigWindow *roccat_window;
	Kova2016configWindowPrivate *priv;
	GtkMenuItem *menu_item;

	obj = G_OBJECT_CLASS(kova2016config_window_parent_class)->constructor(gtype, n_properties, properties);
	window = KOVA2016CONFIG_WINDOW(obj);
	roccat_window = ROCCAT_CONFIG_WINDOW(obj);
	priv = window->priv;

	priv->config = kova2016_configuration_load();

	menu_item = GTK_MENU_ITEM(gtk_image_menu_item_new_from_stock(GTK_STOCK_PREFERENCES, NULL));
	g_signal_connect(G_OBJECT(menu_item), "activate", G_CALLBACK(menu_edit_preferences_cb), window);
	roccat_config_window_edit_menu_append(roccat_window, menu_item);

	menu_item = GTK_MENU_ITEM(gtk_image_menu_item_new_with_label(_("Sensor register")));
	g_signal_connect(G_OBJECT(menu_item), "activate", G_CALLBACK(menu_edit_sensor_cb), window);
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
	priv->dbus_proxy = kova2016_dbus_proxy_new();

	roccat_config_window_set_device_scanner(roccat_window, ROCCAT_DEVICE_SCANNER_INTERFACE(kova2016_device_scanner_new()));

	gtk_widget_show(GTK_WIDGET(window));

	/* if a device is already plugged in, add_cb has already been executed */
	roccat_config_window_warn_if_no_device(roccat_window);

	return obj;
}

static void kova2016config_window_init(Kova2016configWindow *window) {
	window->priv = KOVA2016CONFIG_WINDOW_GET_PRIVATE(window);
}

static void finalize(GObject *object) {
	Kova2016configWindowPrivate *priv = KOVA2016CONFIG_WINDOW(object)->priv;

	kova2016_configuration_dialog_save(GTK_WINDOW(object), priv->config);
	kova2016_dbus_emit_configuration_changed_outside(priv->dbus_proxy);
	g_clear_pointer(&priv->config, roccat_configuration_free);
	g_clear_pointer(&priv->dbus_proxy, dbus_roccat_proxy_free);

	G_OBJECT_CLASS(kova2016config_window_parent_class)->finalize(object);
}

static void kova2016config_window_class_init(Kova2016configWindowClass *klass) {
	GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

	gobject_class->constructor = constructor;
	gobject_class->finalize = finalize;

	g_type_class_add_private(klass, sizeof(Kova2016configWindowClass));
}
