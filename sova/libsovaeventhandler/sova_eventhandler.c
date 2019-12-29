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

#include <gaminggear/hid_uid.h>
#include <gaminggear/macro.h>
#include "sova_eventhandler_channel_special.h"
#include "sova_eventhandler_channel_typing.h"
#include "sova_dbus_server.h"
#include "sova_config.h"
#include "sova_profile_data.h"
#include "sova_profile.h"
#include "sova_device.h"
#include "sova_event.h"
#include "sova.h"
#include "sova_key_relations.h"
#include "sova_info.h"
#include "sova_notification_powersave.h"
#include "sova_notification_typing.h"
#include "sova_talk.h"
#include "sova_macro.h"
#include "roccat_eventhandler_plugin.h"
#include "roccat_firmware.h"
#include "roccat_strings.h"
#include "roccat_process_helper.h"
#include "roccat_notification_profile.h"
#include "roccat_notification_live_recording.h"
#include "roccat_notification_timer.h"
#include "roccat_talk.h"
#include "config.h"
#include "i18n-lib.h"
#include "g_roccat_helper.h"
#include <glib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

GType sova_eventhandler_get_type(void);

#define SOVA_EVENTHANDLER_TYPE (sova_eventhandler_get_type())
#define SOVA_EVENTHANDLER(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), SOVA_EVENTHANDLER_TYPE, SovaEventhandler))
#define IS_SOVA_EVENTHANDLER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), SOVA_EVENTHANDLER_TYPE))
#define SOVA_EVENTHANDLER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), SOVA_EVENTHANDLER_TYPE, SovaEventhandlerClass))
#define IS_SOVA_EVENTHANDLER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), SOVA_EVENTHANDLER_TYPE))
#define SOVA_EVENTHANDLER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), SOVA_EVENTHANDLER_TYPE, SovaEventhandlerPrivate))

typedef struct _SovaEventhandler SovaEventhandler;
typedef struct _SovaEventhandlerClass SovaEventhandlerClass;
typedef struct _SovaEventhandlerPrivate SovaEventhandlerPrivate;

struct _SovaEventhandler {
	GObject parent;
	SovaEventhandlerPrivate *priv;
};

struct _SovaEventhandlerClass {
	GObjectClass parent_class;
};

struct _SovaEventhandlerPrivate {
	RoccatEventhandlerHost *host;
	SovaDBusServer *dbus_server;
	RoccatDeviceScanner *device_scanner;
	RoccatDevice *device;
	gboolean device_set_up;
	RoccatKeyFile *config;
	guint actual_profile_index;
	guint actual_profile_count;
	SovaProfileData *profile_data[SOVA_PROFILE_NUM];
	SovaEventhandlerChannelSpecial *channel_special;
	SovaEventhandlerChannelTyping *channel_typing;
	gulong active_window_changed_handler;
	RoccatNotificationProfile *profile_note;
	RoccatNotificationTimer *timer_note;
	RoccatNotificationLiveRecording *live_recording_note;
	SovaNotificationPowersave *powersave_note;
	SovaNotificationTyping *typing_note;

	guint16 active_talk_target;
};

static void profile_changed(SovaEventhandler *eventhandler, guint profile_number) {
	SovaEventhandlerPrivate *priv = eventhandler->priv;
	guint profile_index = profile_number - 1;
	SovaProfileData const *profile_data = priv->profile_data[profile_index];

	priv->actual_profile_index = profile_index;

	roccat_notification_profile_update(priv->profile_note,
			sova_profile_data_get_profile_switch_notification_type(profile_data),
			sova_profile_data_get_sound_feedback_volume(profile_data),
			profile_number, gaminggear_profile_data_get_name(GAMINGGEAR_PROFILE_DATA(profile_data)));

	sova_dbus_server_emit_profile_changed(priv->dbus_server, profile_number);
}

static gboolean set_profile(SovaEventhandler *eventhandler, guint profile_number) {
	SovaEventhandlerPrivate *priv = eventhandler->priv;
	guint profile_index;
	GError *local_error = NULL;

	if (profile_number == 0)
		return TRUE;

	profile_index = profile_number - 1;

	if (priv->actual_profile_index != profile_index) {
		if (!sova_profile_write_index(priv->device, profile_index, priv->actual_profile_count, &local_error)) {
			g_warning(_("Could not activate profile %i: %s"), profile_number, local_error->message);
			g_clear_error(&local_error);
			return FALSE;
		}
		profile_changed(eventhandler, profile_number);
	}
	return TRUE;
}

static gboolean talk_easyshift(SovaEventhandler *eventhandler, gboolean state) {
	SovaEventhandlerPrivate *priv = eventhandler->priv;
	GError *error = NULL;
	gboolean retval = FALSE;

	if (priv->device_set_up) {
		retval = sova_talk_write_easyshift(priv->device,
				(state != FALSE) ? SOVA_TALK_EASYSHIFT_ON : SOVA_TALK_EASYSHIFT_OFF,
				&error);
		if (error) {
			g_warning(_("Could not activate easyshift: %s"), error->message);
			g_clear_error(&error);
		}
	}

	return retval;
}

static guint sova_eventhandler_get_talk_device(RoccatEventhandlerPlugin *self) {
	SovaEventhandlerPrivate *priv = SOVA_EVENTHANDLER(self)->priv;

	if (!priv->device_set_up)
		return 0;

	return gaminggear_device_get_product_id(GAMINGGEAR_DEVICE(priv->device));
}

static void sova_eventhandler_talk_easyshift(RoccatEventhandlerPlugin *self, gboolean state) {
	SovaEventhandler *eventhandler = SOVA_EVENTHANDLER(self);
	talk_easyshift(eventhandler, state);
}

static void talk_easyshift_cb(SovaDBusServer *server, guchar state, gpointer user_data) {
	SovaEventhandler *eventhandler = SOVA_EVENTHANDLER(user_data);
	talk_easyshift(eventhandler, state);
}

static void open_gui_cb(gpointer source, gpointer user_data) {
	gchar *path;
	path = g_build_path("/", INSTALL_PREFIX, "bin", "roccatsovaconfig", (gchar const *)NULL);
	roccat_double_fork(path);
	g_free(path);
}

static SovaProfileData *single_profile_data_load(RoccatDevice *device, guint profile_index) {
	SovaProfileData *profile_data;
	GError *local_error = NULL;

	profile_data = sova_profile_data_new();

	if (!sova_profile_data_load(profile_data, profile_index, &local_error)) {
		g_warning(_("Could not load filesystem data for profile %i: %s"), profile_index, local_error->message);
		g_clear_error(&local_error);
	}

	if (!gaminggear_profile_data_read(GAMINGGEAR_PROFILE_DATA(profile_data), GAMINGGEAR_DEVICE(device), &local_error)) {
		g_warning(_("Could not load hardware data for profile %i: %s"), profile_index, local_error->message);
		g_clear_error(&local_error);
	}

	return profile_data;
}

static void profile_data_changed_outside_cb(SovaDBusServer *server, guchar profile_number, gpointer user_data) {
	SovaEventhandler *eventhandler = SOVA_EVENTHANDLER(user_data);
	SovaEventhandlerPrivate *priv = eventhandler->priv;
	guint profile_index = profile_number - 1;

	g_object_unref(priv->profile_data[profile_index]);
	priv->profile_data[profile_index] = single_profile_data_load(priv->device, profile_index);
}

static void configuration_reload(SovaEventhandler *eventhandler) {
	SovaEventhandlerPrivate *priv = eventhandler->priv;
	if (priv->config)
		roccat_configuration_free(priv->config);
	priv->config = sova_configuration_load();
}

static void configuration_changed_outside_cb(SovaDBusServer *server, gpointer user_data) {
	SovaEventhandler *eventhandler = SOVA_EVENTHANDLER(user_data);
	configuration_reload(eventhandler);
}

static void profile_changed_cb(gpointer source, guchar profile_number, gpointer user_data) {
	SovaEventhandler *eventhandler = SOVA_EVENTHANDLER(user_data);
	profile_changed(eventhandler, profile_number);
}

static void open_application_cb(SovaEventhandlerChannelSpecial *channel, guchar key_index, gpointer user_data) {
	SovaEventhandler *eventhandler = SOVA_EVENTHANDLER(user_data);
	SovaEventhandlerPrivate *priv = eventhandler->priv;

	roccat_double_fork(sova_profile_data_get_opener(priv->profile_data[priv->actual_profile_index], key_index));
}

static void talk_cb(SovaEventhandlerChannelSpecial *channel, guchar hid, guchar event, gpointer user_data) {
	SovaEventhandler *eventhandler = SOVA_EVENTHANDLER(user_data);
	SovaEventhandlerPrivate *priv = eventhandler->priv;

	if (event == SOVA_SPECIAL_ACTION_PRESS)
		priv->active_talk_target = sova_profile_data_get_capslock_talk_target(priv->profile_data[priv->actual_profile_index]);
g_debug("talk target 0x%04x", priv->active_talk_target);
	roccat_eventhandler_plugin_emit_talk_easyshift(ROCCAT_EVENTHANDLER_PLUGIN(eventhandler),
			priv->active_talk_target,
			(event == SOVA_SPECIAL_ACTION_PRESS) ? TRUE : FALSE);

	if (event == SOVA_SPECIAL_ACTION_RELEASE)
		priv->active_talk_target = 0;
}

static void brightness_changed_cb(SovaEventhandlerChannelSpecial *channel, guchar brightness_index, gpointer user_data) {
	SovaEventhandler *eventhandler = SOVA_EVENTHANDLER(user_data);
	SovaEventhandlerPrivate *priv = eventhandler->priv;
	guint profile_index = priv->actual_profile_index;

	sova_profile_data_set_brightness_from_hardware(priv->profile_data[profile_index], brightness_index);
	sova_dbus_server_emit_brightness_changed(priv->dbus_server, profile_index + 1, brightness_index + 1);
}

static gboolean recording_event_store_roccat_macro(guint key_index, SovaMacro const *macro, GError **error) {
	GaminggearMacro *gaminggear_macro;
	gboolean retval;

	gaminggear_macro = sova_macro_to_gaminggear_macro(macro);
	retval = gaminggear_macros_store_macro(gaminggear_macro, error);
	gaminggear_macro_free(gaminggear_macro);

	return retval;
}

static gboolean handle_recording_event(SovaEventhandler *eventhandler, guint profile_index, guint key_index) {
	SovaEventhandlerPrivate *priv = eventhandler->priv;
	SovaMacro *macro;
	GError *error = NULL;

	macro = sova_macro_read(priv->device, profile_index, key_index, &error);
	if (error) {
		g_clear_error(&error);
		return FALSE;
	}

	sova_profile_data_set_macro_from_hardware(priv->profile_data[profile_index], key_index, macro);

	recording_event_store_roccat_macro(key_index, macro, &error);
	if (error) {
		g_debug(_("Could not store gaminggear macro: %s"), error->message);
		g_clear_error(&error);
		g_free(macro);
		return FALSE;
	}

	/* rkp has to be stored before this */
	sova_dbus_server_emit_macro_changed(priv->dbus_server, profile_index + 1, key_index + 1, macro);

	g_free(macro);
	return TRUE;
}

static void live_recording_cb(SovaEventhandlerChannelSpecial *channel, guchar key_index, guchar event, gpointer user_data) {
	SovaEventhandler *eventhandler = SOVA_EVENTHANDLER(user_data);
	SovaEventhandlerPrivate *priv = eventhandler->priv;
	SovaProfileData const *profile_data = priv->profile_data[priv->actual_profile_index];

	switch(event) {
	case SOVA_SPECIAL_LIVE_RECORDING_ACTION_START:
		roccat_notification_live_recording_select_macro_key(priv->live_recording_note,
				sova_profile_data_get_macro_record_notification_type(profile_data),
				sova_profile_data_get_sound_feedback_volume(profile_data));
		break;
	case SOVA_SPECIAL_LIVE_RECORDING_ACTION_MACRO_KEY_SELECTED:
		roccat_notification_live_recording_record_macro(priv->live_recording_note,
				sova_profile_data_get_macro_record_notification_type(profile_data),
				sova_profile_data_get_sound_feedback_volume(profile_data));
		break;
	case SOVA_SPECIAL_LIVE_RECORDING_ACTION_END_SUCCESS:
		roccat_notification_live_recording_end_success(priv->live_recording_note,
				sova_profile_data_get_macro_record_notification_type(profile_data),
				sova_profile_data_get_sound_feedback_volume(profile_data));
		handle_recording_event(eventhandler, priv->actual_profile_index, key_index);
		break;
	case SOVA_SPECIAL_LIVE_RECORDING_ACTION_END_ABORT:
		roccat_notification_live_recording_end_abort(priv->live_recording_note,
				sova_profile_data_get_macro_record_notification_type(profile_data),
				sova_profile_data_get_sound_feedback_volume(profile_data));
		break;
	case SOVA_SPECIAL_LIVE_RECORDING_ACTION_INVALID_KEY:
		roccat_notification_live_recording_invalid_key(priv->live_recording_note,
				sova_profile_data_get_macro_record_notification_type(profile_data),
				sova_profile_data_get_sound_feedback_volume(profile_data));
		break;
	default:
		g_debug(_("Got unknown live recording event 0x%02x"), event);
	}
}

static void open_cb(SovaEventhandlerChannelSpecial *channel, guchar subtype, guchar key_index, gpointer user_data) {
	SovaEventhandlerPrivate *priv = SOVA_EVENTHANDLER(user_data)->priv;
	SovaProfileData const *profile_data = priv->profile_data[priv->actual_profile_index];

	switch (subtype) {
	case SOVA_SPECIAL_SUBTYPE_OPEN_FOLDER_DESKTOP:
		roccat_open_default_folder("DESKTOP");
		break;
	case SOVA_SPECIAL_SUBTYPE_OPEN_FOLDER_MY_DOCUMENTS:
		roccat_open_default_folder("DOCUMENTS");
		break;
	case SOVA_SPECIAL_SUBTYPE_OPEN_FOLDER_MY_DOWNLOADS:
		roccat_open_default_folder("DOWNLOAD");
		break;
	case SOVA_SPECIAL_SUBTYPE_OPEN_FOLDER_MY_MUSIC:
		roccat_open_default_folder("MUSIC");
		break;
	case SOVA_SPECIAL_SUBTYPE_OPEN_FOLDER_MY_PICTURES:
		roccat_open_default_folder("PICTURES");
		break;
	case SOVA_SPECIAL_SUBTYPE_OPEN_FOLDER_FAVORITES:
	case SOVA_SPECIAL_SUBTYPE_OPEN_FOLDER_FONTS:
	case SOVA_SPECIAL_SUBTYPE_OPEN_FOLDER_NETWORK:
	case SOVA_SPECIAL_SUBTYPE_OPEN_FOLDER_PRINTERS:
	case SOVA_SPECIAL_SUBTYPE_OPEN_FOLDER_SYSTEM:
	case SOVA_SPECIAL_SUBTYPE_OPEN_WINDOWS_CONTROL_PANEL:
	case SOVA_SPECIAL_SUBTYPE_OPEN_WINDOWS_SYSTEM_PANEL:
	case SOVA_SPECIAL_SUBTYPE_OPEN_WINDOWS_TASK_MANAGER:
	case SOVA_SPECIAL_SUBTYPE_OPEN_WINDOWS_SCREEN_SETTINGS:
	case SOVA_SPECIAL_SUBTYPE_OPEN_WINDOWS_SCREENSAVER:
	case SOVA_SPECIAL_SUBTYPE_OPEN_WINDOWS_THEMES:
	case SOVA_SPECIAL_SUBTYPE_OPEN_WINDOWS_DATE_TIME:
	case SOVA_SPECIAL_SUBTYPE_OPEN_WINDOWS_NETWORK_SETTINGS:
	case SOVA_SPECIAL_SUBTYPE_OPEN_WINDOWS_ADMIN_PANEL:
	case SOVA_SPECIAL_SUBTYPE_OPEN_WINDOWS_FIREWALL:
	case SOVA_SPECIAL_SUBTYPE_OPEN_WINDOWS_REGEDIT:
	case SOVA_SPECIAL_SUBTYPE_OPEN_WINDOWS_EVENT_MONITOR:
	case SOVA_SPECIAL_SUBTYPE_OPEN_WINDOWS_PERFORMANCE_MONITOR:
	case SOVA_SPECIAL_SUBTYPE_OPEN_WINDOWS_SOUND_SETTINGS:
	case SOVA_SPECIAL_SUBTYPE_OPEN_WINDOWS_INTERNET_SETTINGS:
	case SOVA_SPECIAL_SUBTYPE_OPEN_WINDOWS_DIRECTX_DIAGNOSTICS:
	case SOVA_SPECIAL_SUBTYPE_OPEN_COMMAND_LINE:
		g_roccat_info(_("Unsupported open subtype %i"), subtype);
		break;
	case SOVA_SPECIAL_SUBTYPE_OPEN_DOCUMENT:
	case SOVA_SPECIAL_SUBTYPE_OPEN_FOLDER:
	case SOVA_SPECIAL_SUBTYPE_OPEN_WEBSITE:
		roccat_open_url(sova_profile_data_get_opener(profile_data, key_index));
		break;
	default:
		g_warning(_("Unknown open subtype %i"), subtype);
		break;
	}
}

static void powersave_cb(SovaEventhandlerChannelSpecial *channel, guchar subtype, gpointer user_data) {
	SovaEventhandler *eventhandler = SOVA_EVENTHANDLER(user_data);
	SovaEventhandlerPrivate *priv = eventhandler->priv;
	SovaProfileData const *profile_data = priv->profile_data[priv->actual_profile_index];

	switch(subtype) {
	case SOVA_SPECIAL_SUBTYPE_POWERSAVE_WAKEUP:
		sova_notification_powersave_wakeup(priv->powersave_note,
				sova_profile_data_get_wakeup_notification_type(profile_data),
				sova_profile_data_get_sound_feedback_volume(profile_data));
		break;
	case SOVA_SPECIAL_SUBTYPE_POWERSAVE_SLEEP:
		sova_notification_powersave_sleep(priv->powersave_note,
				sova_profile_data_get_sleep_notification_type(profile_data),
				sova_profile_data_get_sound_feedback_volume(profile_data));
		break;
	}
}

static void timer_start_cb(SovaEventhandlerChannelSpecial *channel, guchar key_index, gpointer user_data) {
	SovaEventhandler *eventhandler = SOVA_EVENTHANDLER(user_data);
	SovaEventhandlerPrivate *priv = eventhandler->priv;
	SovaProfileData const *profile_data = priv->profile_data[priv->actual_profile_index];
	RoccatTimer const *timer = sova_profile_data_get_timer(profile_data, key_index);

	roccat_notification_timer_start(priv->timer_note,
			sova_profile_data_get_timer_notification_type(profile_data),
			sova_profile_data_get_sound_feedback_volume(profile_data),
			(gchar const *)timer->name, timer->seconds);
}

static void timer_stop_cb(SovaEventhandlerChannelSpecial *channel, gpointer user_data) {
	SovaEventhandler *eventhandler = SOVA_EVENTHANDLER(user_data);
	SovaEventhandlerPrivate *priv = eventhandler->priv;

	roccat_notification_timer_stop(priv->timer_note);
}

static void profile_data_load(SovaEventhandler *eventhandler, guint count) {
	SovaEventhandlerPrivate *priv = eventhandler->priv;
	guint i;

	for (i = 0; i < count; ++i)
		priv->profile_data[i] = single_profile_data_load(priv->device, i);
}

static void window_changed_cb(RoccatEventhandlerHost *host, gchar const *title, gpointer user_data) {
	SovaEventhandler *eventhandler = SOVA_EVENTHANDLER(user_data);
	SovaEventhandlerPrivate *priv = eventhandler->priv;
	guint profile_index;
	guint gamefile_index;
	gchar const *pattern;

	for (profile_index = 0; profile_index < SOVA_PROFILE_NUM; ++profile_index) {

		if (priv->profile_data[profile_index] == NULL)
			continue;

		for (gamefile_index = 0; gamefile_index < GAMINGGEAR_PROFILE_DATA_GAMEFILE_NUM; ++gamefile_index) {
			pattern = gaminggear_profile_data_get_gamefile(GAMINGGEAR_PROFILE_DATA(priv->profile_data[profile_index]), gamefile_index);
			if (strcmp(pattern, "") == 0)
				continue;

			if (g_regex_match_simple(pattern, title, 0, 0)) {
				set_profile(eventhandler, profile_index + 1);
				return;
			}
		}
	}

	set_profile(eventhandler, sova_configuration_get_default_profile_number(priv->config));
	return;
}

static void device_add_cb(RoccatDeviceScannerInterface *interface, RoccatDevice *device, gpointer user_data) {
	SovaEventhandler *eventhandler = SOVA_EVENTHANDLER(user_data);
	SovaEventhandlerPrivate *priv = eventhandler->priv;
	GError *local_error = NULL;
	SovaProfile *profile;

	// FIXME test equality for no error
	if (priv->device) {
		g_critical(_("Multiple devices present."));
		return;
	}

	priv->device = device;
	g_object_ref(G_OBJECT(device));
	roccat_device_debug(device);

	configuration_reload(eventhandler);

	/* Notificators gets initialized only once if a device is found.
	 * Get deinited once in finalizer.
	 */
	if (!priv->profile_note)
		priv->profile_note = roccat_notification_profile_new(roccat_eventhandler_host_get_notificator(priv->host), SOVA_DEVICE_NAME);
	if (!priv->timer_note)
		priv->timer_note = roccat_notification_timer_new(roccat_eventhandler_host_get_notificator(priv->host), SOVA_DEVICE_NAME);
	if (!priv->live_recording_note)
		priv->live_recording_note = roccat_notification_live_recording_new(roccat_eventhandler_host_get_notificator(priv->host), SOVA_DEVICE_NAME);
	if (!priv->powersave_note)
		priv->powersave_note = sova_notification_powersave_new(roccat_eventhandler_host_get_notificator(priv->host), SOVA_DEVICE_NAME);
	if (!priv->typing_note)
		priv->typing_note = sova_notification_typing_new(roccat_eventhandler_host_get_notificator(priv->host));

	profile = sova_profile_read(priv->device, &local_error);
	if (local_error) {
		g_warning(_("Could not read actual profile: %s"), local_error->message);
		g_clear_error(&local_error);
		priv->actual_profile_index = 0;
		priv->actual_profile_count = 5;
	} else {
		priv->actual_profile_index = profile->index;
		priv->actual_profile_count = MAX(1, profile->count);
	}
	g_free(profile);
	profile_data_load(eventhandler, priv->actual_profile_count);

	priv->dbus_server = sova_dbus_server_new();
	g_signal_connect(G_OBJECT(priv->dbus_server), "talk-easyshift", G_CALLBACK(talk_easyshift_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->dbus_server), "talk-easyshift-lock", G_CALLBACK(talk_easyshift_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->dbus_server), "open-gui", G_CALLBACK(open_gui_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->dbus_server), "profile-changed-outside", G_CALLBACK(profile_changed_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->dbus_server), "profile-data-changed-outside", G_CALLBACK(profile_data_changed_outside_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->dbus_server), "configuration-changed-outside", G_CALLBACK(configuration_changed_outside_cb), eventhandler);
	sova_dbus_server_connect(priv->dbus_server);

	priv->active_window_changed_handler = g_signal_connect(G_OBJECT(priv->host), "active-window-changed", G_CALLBACK(window_changed_cb), eventhandler);

	sova_eventhandler_channel_special_start(priv->channel_special, priv->device, &local_error);
	if (local_error) {
		g_warning(_("Could not start special eventhandler: %s"), local_error->message);
		g_clear_error(&local_error);
	}

	sova_eventhandler_channel_typing_start(priv->channel_typing, priv->device, &local_error);
	if (local_error) {
		g_warning(_("Could not start typing eventhandler: %s"), local_error->message);
		g_clear_error(&local_error);
	}

	priv->active_talk_target = 0;
	priv->device_set_up = TRUE;

	roccat_eventhandler_plugin_emit_device_added(ROCCAT_EVENTHANDLER_PLUGIN(eventhandler), gaminggear_device_get_product_id(GAMINGGEAR_DEVICE(priv->device)));
}

static void stop(SovaEventhandler *eventhandler) {
	SovaEventhandlerPrivate *priv = eventhandler->priv;
	guint i;

	roccat_eventhandler_plugin_emit_device_removed(ROCCAT_EVENTHANDLER_PLUGIN(eventhandler), gaminggear_device_get_product_id(GAMINGGEAR_DEVICE(priv->device)));

	priv->device_set_up = FALSE;

	g_clear_object(&priv->dbus_server);
	sova_eventhandler_channel_special_stop(priv->channel_special);
	sova_eventhandler_channel_typing_stop(priv->channel_typing);
	g_signal_handler_disconnect(G_OBJECT(priv->host), priv->active_window_changed_handler);
	g_clear_object(&priv->device);
	g_clear_pointer(&priv->config, roccat_configuration_free);

	for (i = 0; i < SOVA_PROFILE_NUM; ++i)
		g_clear_object(&priv->profile_data[i]);
}

static void device_remove_cb(RoccatDeviceScanner *scanner, RoccatDevice *device, gpointer user_data) {
	SovaEventhandler *eventhandler = SOVA_EVENTHANDLER(user_data);
	SovaEventhandlerPrivate *priv = eventhandler->priv;
	if (priv->device && gaminggear_device_equal(GAMINGGEAR_DEVICE(device), GAMINGGEAR_DEVICE(priv->device)))
		stop(eventhandler);
}

static void key_press_cb(SovaEventhandlerChannelTyping *channel, gpointer data) {
	SovaEventhandler *eventhandler = SOVA_EVENTHANDLER(data);
	SovaEventhandlerPrivate *priv = eventhandler->priv;
	SovaProfileData *profile_data = priv->profile_data[priv->actual_profile_index];

	sova_notification_typing_update(priv->typing_note,
			sova_profile_data_get_sound_feedback_typing(profile_data),
			sova_profile_data_get_sound_feedback_volume(profile_data));
}

static void sova_eventhandler_start(RoccatEventhandlerPlugin *self, RoccatEventhandlerHost *host) {
	SovaEventhandler *eventhandler = SOVA_EVENTHANDLER(self);
	SovaEventhandlerPrivate *priv = eventhandler->priv;

	priv->host = host;
	g_object_ref(priv->host);

	priv->channel_special = sova_eventhandler_channel_special_new();
	g_signal_connect(G_OBJECT(priv->channel_special), "open-driver", G_CALLBACK(open_gui_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->channel_special), "profile-changed", G_CALLBACK(profile_changed_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->channel_special), "open-app", G_CALLBACK(open_application_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->channel_special), "talk", G_CALLBACK(talk_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->channel_special), "timer-start", G_CALLBACK(timer_start_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->channel_special), "timer-stop", G_CALLBACK(timer_stop_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->channel_special), "open", G_CALLBACK(open_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->channel_special), "powersave", G_CALLBACK(powersave_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->channel_special), "brightness-changed", G_CALLBACK(brightness_changed_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->channel_special), "live-recording", G_CALLBACK(live_recording_cb), eventhandler);

	priv->channel_typing = sova_eventhandler_channel_typing_new();
	g_signal_connect(G_OBJECT(priv->channel_typing), "key-press", G_CALLBACK(key_press_cb), eventhandler);

	priv->device_scanner = sova_device_scanner_new();
	g_signal_connect(G_OBJECT(priv->device_scanner), "device-added", G_CALLBACK(device_add_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->device_scanner), "device-removed", G_CALLBACK(device_remove_cb), eventhandler);
	roccat_device_scanner_interface_start(ROCCAT_DEVICE_SCANNER_INTERFACE(priv->device_scanner));
}

static void sova_eventhandler_stop(RoccatEventhandlerPlugin *self) {
	SovaEventhandler *eventhandler = SOVA_EVENTHANDLER(self);
	SovaEventhandlerPrivate *priv = eventhandler->priv;

	stop(eventhandler);

	g_clear_object(&priv->device_scanner);
	g_clear_object(&priv->channel_special);
	g_clear_object(&priv->channel_typing);
	g_clear_object(&priv->host);
}

static void sova_roccat_eventhandler_interface_init(RoccatEventhandlerPluginInterface *iface) {
	iface->start = sova_eventhandler_start;
	iface->stop = sova_eventhandler_stop;
	iface->get_talk_device = sova_eventhandler_get_talk_device;
	iface->talk_easyshift = sova_eventhandler_talk_easyshift;
	iface->talk_easyshift_lock = sova_eventhandler_talk_easyshift;
	iface->talk_easyaim = NULL;
	iface->talkfx_set_led_rgb = NULL;
	iface->talkfx_restore_led_rgb = NULL;
	iface->talkfx_ryos_set_sdk_mode = NULL;
	iface->talkfx_ryos_set_all_leds = NULL;
	iface->talkfx_ryos_turn_on_all_leds = NULL;
	iface->talkfx_ryos_turn_off_all_leds = NULL;
	iface->talkfx_ryos_set_led_on = NULL;
	iface->talkfx_ryos_set_led_off = NULL;
	iface->talkfx_ryos_all_key_blinking = NULL;
}

G_DEFINE_TYPE_WITH_CODE(SovaEventhandler, sova_eventhandler, G_TYPE_OBJECT,
		G_IMPLEMENT_INTERFACE(ROCCAT_EVENTHANDLER_PLUGIN_TYPE, sova_roccat_eventhandler_interface_init));

static void sova_eventhandler_init(SovaEventhandler *eventhandler) {
	eventhandler->priv = SOVA_EVENTHANDLER_GET_PRIVATE(eventhandler);
}

static void dispose(GObject *object) {
	sova_eventhandler_stop(ROCCAT_EVENTHANDLER_PLUGIN(object));

	G_OBJECT_CLASS(sova_eventhandler_parent_class)->dispose(object);
}

static void finalize(GObject *object) {
	SovaEventhandlerPrivate *priv = SOVA_EVENTHANDLER(object)->priv;

	g_clear_pointer(&priv->live_recording_note, roccat_notification_live_recording_free);
	g_clear_pointer(&priv->timer_note, roccat_notification_timer_free);
	g_clear_pointer(&priv->profile_note, roccat_notification_profile_free);
	g_clear_pointer(&priv->powersave_note, sova_notification_powersave_free);
	g_clear_pointer(&priv->typing_note, sova_notification_typing_free);

	G_OBJECT_CLASS(sova_eventhandler_parent_class)->finalize(object);
}

static void sova_eventhandler_class_init(SovaEventhandlerClass *klass) {
	GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
	gobject_class->dispose = dispose;
	gobject_class->finalize = finalize;

	g_type_class_add_private(klass, sizeof(SovaEventhandlerPrivate));
}

ROCCAT_EVENTHANDLER_PLUGIN_API RoccatEventhandlerPlugin *roccat_eventhandler_plugin_new(void) {
	return ROCCAT_EVENTHANDLER_PLUGIN(g_object_new(SOVA_EVENTHANDLER_TYPE, NULL));
}
