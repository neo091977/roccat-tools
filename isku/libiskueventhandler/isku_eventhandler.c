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
#include "isku_eventhandler_channel.h"
#include "isku_dbus_server.h"
#include "roccat_eventhandler_plugin.h"
#include "roccat_notification_profile.h"
#include "roccat_notification_timer.h"
#include "roccat_notification_live_recording.h"
#include "roccat_process_helper.h"
#include "roccat_talk.h"
#include "isku.h"
#include "config.h"
#include "i18n-lib.h"
#include "g_roccat_helper.h"
#include <glib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

GType isku_eventhandler_get_type(void);

#define ISKU_EVENTHANDLER_TYPE (isku_eventhandler_get_type())
#define ISKU_EVENTHANDLER(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), ISKU_EVENTHANDLER_TYPE, IskuEventhandler))
#define IS_ISKU_EVENTHANDLER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), ISKU_EVENTHANDLER_TYPE))
#define ISKU_EVENTHANDLER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), ISKU_EVENTHANDLER_TYPE, IskuEventhandlerClass))
#define IS_ISKU_EVENTHANDLER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), ISKU_EVENTHANDLER_TYPE))
#define ISKU_EVENTHANDLER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), ISKU_EVENTHANDLER_TYPE, IskuEventhandlerPrivate))

typedef struct _IskuEventhandler IskuEventhandler;
typedef struct _IskuEventhandlerClass IskuEventhandlerClass;
typedef struct _IskuEventhandlerPrivate IskuEventhandlerPrivate;

struct _IskuEventhandler {
	GObject parent;
	IskuEventhandlerPrivate *priv;
};

struct _IskuEventhandlerClass {
	GObjectClass parent_class;
};

struct _IskuEventhandlerPrivate {
	RoccatEventhandlerHost *host;
	IskuDBusServer *dbus_server;
	RoccatDeviceScanner *device_scanner;
	RoccatDevice *device;
	gboolean device_set_up;
	RoccatKeyFile *config;
	guint actual_profile_index;
	IskuRkp *rkp[ISKU_PROFILE_NUM];
	IskuEventhandlerChannel *channel;
	gulong active_window_changed_handler;

	RoccatNotificationProfile *profile_note;
	RoccatNotificationTimer *timer_note;
	RoccatNotificationLiveRecording *live_recording_note;

	guint8 active_talk_type;
	guint16 active_talk_device;
};

static void profile_changed(IskuEventhandler *eventhandler, guint profile_number) {
	IskuEventhandlerPrivate *priv = eventhandler->priv;
	guint profile_index = profile_number - 1;

	priv->actual_profile_index = profile_index;

	roccat_notification_profile_update(priv->profile_note, isku_configuration_get_profile_notification_type(priv->config),
			isku_configuration_get_notification_volume(priv->config),
			profile_number, isku_rkp_get_profile_name(priv->rkp[profile_index]));

	isku_dbus_server_emit_profile_changed(priv->dbus_server, profile_number);
}

static gboolean set_profile(IskuEventhandler *eventhandler, guint profile_number) {
	IskuEventhandlerPrivate *priv = eventhandler->priv;
	guint profile_index;
	GError *local_error = NULL;

	if (profile_number == 0)
		return TRUE;

	profile_index = profile_number - 1;

	if (priv->actual_profile_index != profile_index) {
		if (!isku_actual_profile_write(priv->device, profile_index, &local_error)) {
			g_warning(_("Could not activate profile %i: %s"), profile_number, local_error->message);
			g_clear_error(&local_error);
			return FALSE;
		}
		profile_changed(eventhandler, profile_number);
	}
	return TRUE;
}

static void window_changed_cb(RoccatEventhandlerHost *host, gchar const *title, gpointer user_data) {
	IskuEventhandler *eventhandler = ISKU_EVENTHANDLER(user_data);
	IskuEventhandlerPrivate *priv = eventhandler->priv;
	guint profile_index;
	guint game_file_index;
	gchar *pattern;

	for (profile_index = 0; profile_index < ISKU_PROFILE_NUM; ++profile_index) {

		if (priv->rkp[profile_index] == NULL)
			continue;

		for (game_file_index = 0; game_file_index < ISKU_GAMEFILE_NUM; ++game_file_index) {
			pattern = isku_rkp_get_game_file_name(priv->rkp[profile_index], game_file_index);
			if (strcmp(pattern, "") == 0) {
				g_free(pattern);
				continue;
			}

			if (g_regex_match_simple(pattern, title, 0, 0)) {
				set_profile(eventhandler, profile_index + 1);
				g_free(pattern);
				return;
			}
			g_free(pattern);
		}
	}

	set_profile(eventhandler, isku_configuration_get_default_profile_number(priv->config));
	return;
}

static gboolean talk_easyshift(IskuEventhandler *eventhandler, gboolean state) {
	IskuEventhandlerPrivate *priv = eventhandler->priv;
	GError *error = NULL;
	gboolean retval = FALSE;

	if (priv->device_set_up) {
		retval = isku_talk_easyshift(priv->device,
				(state != FALSE) ? ISKU_TALK_EASYSHIFT_ON : ISKU_TALK_EASYSHIFT_OFF,
				&error);
		if (error) {
			g_warning(_("Could not activate easyshift: %s"), error->message);
			g_clear_error(&error);
		}
	}

	return retval;
}

static gboolean talk_easyshift_lock(IskuEventhandler *eventhandler, gboolean state) {
	IskuEventhandlerPrivate *priv = eventhandler->priv;
	GError *error = NULL;
	gboolean retval = FALSE;

	if (priv->device_set_up) {
		retval = isku_talk_easyshift_lock(priv->device,
				(state != FALSE) ? ISKU_TALK_EASYSHIFT_ON : ISKU_TALK_EASYSHIFT_OFF,
				&error);
		if (error) {
			g_warning(_("Could not activate easyshift lock: %s"), error->message);
			g_clear_error(&error);
		}
	}

	return retval;
}

static guint isku_eventhandler_get_talk_device(RoccatEventhandlerPlugin *self) {
	IskuEventhandlerPrivate *priv = ISKU_EVENTHANDLER(self)->priv;

	if (!priv->device_set_up)
		return 0;

	return gaminggear_device_get_product_id(GAMINGGEAR_DEVICE(priv->device));
}

static void isku_eventhandler_talk_easyshift(RoccatEventhandlerPlugin *self, gboolean state) {
	IskuEventhandler *eventhandler = ISKU_EVENTHANDLER(self);
	talk_easyshift(eventhandler, state);
}

static void isku_eventhandler_talk_easyshift_lock(RoccatEventhandlerPlugin *self, gboolean state) {
	IskuEventhandler *eventhandler = ISKU_EVENTHANDLER(self);
	talk_easyshift_lock(eventhandler, state);
}

static void talk_easyshift_cb(IskuDBusServer *server, guchar state, gpointer user_data) {
	IskuEventhandler *eventhandler = ISKU_EVENTHANDLER(user_data);
	talk_easyshift(eventhandler, state);
}

static void talk_easyshift_lock_cb(IskuDBusServer *server, guchar state, gpointer user_data) {
	IskuEventhandler *eventhandler = ISKU_EVENTHANDLER(user_data);
	talk_easyshift_lock(eventhandler, state);
}

static void open_gui_cb(gpointer source, gpointer user_data) {
	gchar *path;
	path = g_build_path("/", INSTALL_PREFIX, "bin", "roccatiskuconfig", (gchar const *)NULL);
	roccat_double_fork(path);
	g_free(path);
}

static void profile_data_changed_outside_cb(IskuDBusServer *server, guchar number, gpointer user_data) {
	IskuEventhandler *eventhandler = ISKU_EVENTHANDLER(user_data);
	IskuEventhandlerPrivate *priv = eventhandler->priv;
	guint profile_index = number - 1;
	
	isku_rkp_free(priv->rkp[profile_index]);
	priv->rkp[profile_index] = isku_rkp_load_actual(profile_index);
}

static void configuration_reload(IskuEventhandler *eventhandler) {
	IskuEventhandlerPrivate *priv = eventhandler->priv;
	if (priv->config)
		isku_configuration_free(priv->config);
	priv->config = isku_configuration_load();
}

static void configuration_changed_outside_cb(IskuDBusServer *server, gpointer user_data) {
	IskuEventhandler *eventhandler = ISKU_EVENTHANDLER(user_data);
	configuration_reload(eventhandler);
}

static void profile_changed_cb(gpointer source, guchar profile_number, gpointer user_data) {
	IskuEventhandler *eventhandler = ISKU_EVENTHANDLER(user_data);
	profile_changed(eventhandler, profile_number);
}

static void quicklaunch_cb(IskuEventhandlerChannel *channel, guchar key_index, gpointer user_data) {
	IskuEventhandler *eventhandler = ISKU_EVENTHANDLER(user_data);
	IskuEventhandlerPrivate *priv = eventhandler->priv;
	IskuRkpMacroKeyInfo *macro_key_info;

	macro_key_info = isku_rkp_get_macro_key_info(priv->rkp[priv->actual_profile_index], key_index);
	isku_play_quicklaunch_forked(macro_key_info);
	isku_rkp_macro_key_info_free(macro_key_info);
}

static guint key_type_to_key_index(guint key_type) {
	if (key_type >= ISKU_KEY_TYPE_M1 && key_type <= ISKU_KEY_TYPE_M5)
		return key_type - ISKU_KEY_TYPE_M1 + ISKU_KEY_INDEX_M1;
	if (key_type >= ISKU_KEY_TYPE_T1 && key_type <= ISKU_KEY_TYPE_T3)
		return key_type - ISKU_KEY_TYPE_T1 + ISKU_KEY_INDEX_T1;
	if (key_type == HID_UID_KB_CAPS_LOCK)
		return ISKU_KEY_INDEX_CAPSLOCK;
	g_warning(_("Got unknown key type 0x%x"), key_type);
	return key_type;
}

static void talk_cb(IskuEventhandlerChannel *channel, guchar key_type, guchar event, gpointer user_data) {
	IskuEventhandler *eventhandler = ISKU_EVENTHANDLER(user_data);
	IskuEventhandlerPrivate *priv = eventhandler->priv;
	IskuRkpMacroKeyInfo *macro_key_info;
	static gboolean state = FALSE;

	if (event == ISKU_SPECIAL_EVENT_ACTION_PRESS) {
		macro_key_info = isku_rkp_get_macro_key_info(priv->rkp[priv->actual_profile_index], key_type_to_key_index(key_type));
		priv->active_talk_type = macro_key_info->type;
		priv->active_talk_device = isku_rkp_macro_key_info_get_talk_device(macro_key_info);
		isku_rkp_macro_key_info_free(macro_key_info);
	}

	switch (priv->active_talk_type) {
	case ISKU_KEY_TYPE_EASYSHIFT:
		break;
	case ISKU_KEY_TYPE_TALK_BOTH_EASYSHIFT:
		roccat_eventhandler_plugin_emit_talk_easyshift(ROCCAT_EVENTHANDLER_PLUGIN(eventhandler),
				ROCCAT_TALK_DEVICE_MOUSE,
				(event == ISKU_SPECIAL_EVENT_ACTION_PRESS) ? TRUE : FALSE);
		break;
	case ISKU_KEY_TYPE_TALK_OTHER_EASYSHIFT:
		roccat_eventhandler_plugin_emit_talk_easyshift(ROCCAT_EVENTHANDLER_PLUGIN(eventhandler),
				priv->active_talk_device,
				(event == ISKU_SPECIAL_EVENT_ACTION_PRESS) ? TRUE : FALSE);
		break;
	case ISKU_KEY_TYPE_TALK_OTHER_EASYSHIFT_LOCK:
		if (event == ISKU_SPECIAL_EVENT_ACTION_PRESS) {
			state = !state;
			roccat_eventhandler_plugin_emit_talk_easyshift_lock(ROCCAT_EVENTHANDLER_PLUGIN(eventhandler),
					priv->active_talk_device, state);
		}
		break;
	case ISKU_KEY_TYPE_TALK_OTHER_EASYAIM_1:
		roccat_eventhandler_plugin_emit_talk_easyaim(ROCCAT_EVENTHANDLER_PLUGIN(eventhandler),
				priv->active_talk_device,
				(event == ISKU_SPECIAL_EVENT_ACTION_PRESS) ? ROCCAT_TALK_EASYAIM_1 : ROCCAT_TALK_EASYAIM_OFF);
		break;
	case ISKU_KEY_TYPE_TALK_OTHER_EASYAIM_2:
		roccat_eventhandler_plugin_emit_talk_easyaim(ROCCAT_EVENTHANDLER_PLUGIN(eventhandler),
				priv->active_talk_device,
				(event == ISKU_SPECIAL_EVENT_ACTION_PRESS) ? ROCCAT_TALK_EASYAIM_2 : ROCCAT_TALK_EASYAIM_OFF);
		break;
	case ISKU_KEY_TYPE_TALK_OTHER_EASYAIM_3:
		roccat_eventhandler_plugin_emit_talk_easyaim(ROCCAT_EVENTHANDLER_PLUGIN(eventhandler),
				priv->active_talk_device,
				(event == ISKU_SPECIAL_EVENT_ACTION_PRESS) ? ROCCAT_TALK_EASYAIM_3 : ROCCAT_TALK_EASYAIM_OFF);
		break;
	case ISKU_KEY_TYPE_TALK_OTHER_EASYAIM_4:
		roccat_eventhandler_plugin_emit_talk_easyaim(ROCCAT_EVENTHANDLER_PLUGIN(eventhandler),
				priv->active_talk_device,
				(event == ISKU_SPECIAL_EVENT_ACTION_PRESS) ? ROCCAT_TALK_EASYAIM_4 : ROCCAT_TALK_EASYAIM_OFF);
		break;
	case ISKU_KEY_TYPE_TALK_OTHER_EASYAIM_5:
		roccat_eventhandler_plugin_emit_talk_easyaim(ROCCAT_EVENTHANDLER_PLUGIN(eventhandler),
				priv->active_talk_device,
				(event == ISKU_SPECIAL_EVENT_ACTION_PRESS) ? ROCCAT_TALK_EASYAIM_5 : ROCCAT_TALK_EASYAIM_OFF);
		break;
	default:
		g_debug(_("Got unknown Talk event type 0x%02x"), priv->active_talk_type);
	}

	if (event == ISKU_SPECIAL_EVENT_ACTION_RELEASE)
		priv->active_talk_type = ISKU_KEY_TYPE_DISABLED;
}

static void light_changed_cb(IskuEventhandlerChannel *channel, guchar brightness_number, gpointer user_data) {
	IskuEventhandler *eventhandler = ISKU_EVENTHANDLER(user_data);
	IskuEventhandlerPrivate *priv = eventhandler->priv;
	guint profile_index;
	GError *error = NULL;

	profile_index = priv->actual_profile_index;

	isku_last_set_write(priv->device, &error);
	if (error) {
		g_clear_error(&error);
		return;
	}

	isku_rkp_set_light_brightness(priv->rkp[profile_index], brightness_number - 1);
	isku_rkp_save_actual(priv->rkp[profile_index], profile_index, &error);
	if (error) {
		g_clear_error(&error);
		return;
	}
	isku_rkp_set_unmodified(priv->rkp[profile_index]);

	isku_dbus_server_emit_brightness_changed(priv->dbus_server, profile_index + 1, brightness_number);
}

static gboolean recording_event_store_roccat_macro(IskuRkpMacroKeyInfo const *key_info, GError **error) {
	GaminggearMacro *gaminggear_macro;
	gboolean retval;

	gaminggear_macro = isku_rkp_macro_key_info_to_gaminggear_macro(key_info);

	/*
	 * Using device specific macroset name instead of default "MLR"
	 * because macro file is shared between devices.
	 * Macroset name stored in device is still "MLR" to reduce writes.
	 */
	gaminggear_macro_set_macroset_name(gaminggear_macro, ISKU_DEVICE_NAME " MLR");

	retval = gaminggear_macros_store_macro(gaminggear_macro, error);

	gaminggear_macro_free(gaminggear_macro);

	return retval;
}

static IskuRkpMacroKeyInfo *recording_event_get_data(RoccatDevice *device, guint profile_index, guint key_index, GError **error) {
	IskuMacro *macro;
	IskuRkpMacroKeyInfo *key_info;

	macro = isku_macro_read(device, profile_index, key_index, error);
	if (!macro)
		return NULL;

	key_info = isku_macro_to_rkp_macro_key_info(macro);
	g_free(macro);

	return key_info;
}

static gboolean handle_recording_event(IskuEventhandler *eventhandler, guint profile_index, guint key_index) {
	IskuEventhandlerPrivate *priv = eventhandler->priv;
	IskuRkpMacroKeyInfo *key_info;
	GError *error = NULL;

	key_info = recording_event_get_data(priv->device, profile_index, key_index, &error);
	if (error) {
		g_clear_error(&error);
		return FALSE;
	}

	isku_last_set_write(priv->device, &error);
	if (error) {
		g_clear_error(&error);
		g_free(key_info);
		return FALSE;
	}

	isku_rkp_set_macro_key_info(priv->rkp[profile_index], key_index, key_info);
	isku_rkp_save_actual(priv->rkp[profile_index], profile_index, &error);
	if (error) {
		g_clear_error(&error);
		g_free(key_info);
		return FALSE;
	}
	isku_rkp_set_unmodified(priv->rkp[profile_index]);

	recording_event_store_roccat_macro(key_info, &error);
	if (error) {
		g_clear_error(&error);
		g_free(key_info);
		return FALSE;
	}

	/* rkp has to be stored before this */
	isku_dbus_server_emit_macro_changed(priv->dbus_server, profile_index + 1, key_index + 1, key_info);

	g_free(key_info);
	return TRUE;
}

static void live_recording_cb(IskuEventhandlerChannel *channel, guchar key_index, guchar event, gpointer user_data) {
	IskuEventhandler *eventhandler = ISKU_EVENTHANDLER(user_data);
	IskuEventhandlerPrivate *priv = eventhandler->priv;

	switch(event) {
	case ISKU_SPECIAL_LIVE_RECORDING_START:
		roccat_notification_live_recording_select_macro_key(priv->live_recording_note,
				isku_configuration_get_live_recording_notification_type(priv->config),
				isku_configuration_get_notification_volume(priv->config));
		break;
	case ISKU_SPECIAL_LIVE_RECORDING_MACRO_KEY_SELECTED:
		roccat_notification_live_recording_record_macro(priv->live_recording_note,
				isku_configuration_get_live_recording_notification_type(priv->config),
				isku_configuration_get_notification_volume(priv->config));
		break;
	case ISKU_SPECIAL_LIVE_RECORDING_END_SUCCESS:
		roccat_notification_live_recording_end_success(priv->live_recording_note,
				isku_configuration_get_live_recording_notification_type(priv->config),
				isku_configuration_get_notification_volume(priv->config));
		handle_recording_event(eventhandler, priv->actual_profile_index, key_index);
		break;
	case ISKU_SPECIAL_LIVE_RECORDING_END_ABORT:
		roccat_notification_live_recording_end_abort(priv->live_recording_note,
				isku_configuration_get_live_recording_notification_type(priv->config),
				isku_configuration_get_notification_volume(priv->config));
		break;
	case ISKU_SPECIAL_LIVE_RECORDING_INVALID_KEY:
		roccat_notification_live_recording_invalid_key(priv->live_recording_note,
				isku_configuration_get_live_recording_notification_type(priv->config),
				isku_configuration_get_notification_volume(priv->config));
		break;
	default:
		g_debug(_("Got unknown live recording event 0x%02x"), event);
	}
}

static void timer_start_cb(IskuEventhandlerChannel *channel, guchar key_index, gpointer user_data) {
	IskuEventhandler *eventhandler = ISKU_EVENTHANDLER(user_data);
	IskuEventhandlerPrivate *priv = eventhandler->priv;
	IskuRkpMacroKeyInfo *macro_key_info;

	macro_key_info = isku_rkp_get_macro_key_info(priv->rkp[priv->actual_profile_index], key_index);
	roccat_notification_timer_start(priv->timer_note,
			isku_configuration_get_timer_notification_type(priv->config),
			isku_configuration_get_notification_volume(priv->config),
			(gchar const *)macro_key_info->timer_name,
			isku_rkp_macro_key_info_get_timer_length(macro_key_info));
	isku_rkp_macro_key_info_free(macro_key_info);
}

static void timer_stop_cb(IskuEventhandlerChannel *channel, gpointer user_data) {
	IskuEventhandler *eventhandler = ISKU_EVENTHANDLER(user_data);
	IskuEventhandlerPrivate *priv = eventhandler->priv;

	roccat_notification_timer_stop(priv->timer_note);
}

static gboolean profile_data_load(IskuEventhandler *eventhandler, GError **error) {
	IskuEventhandlerPrivate *priv = eventhandler->priv;
	guint i;

	for (i = 0; i < ISKU_PROFILE_NUM; ++i) {
		priv->rkp[i] = isku_rkp_load(priv->device, i, error);
		if (priv->rkp[i] == NULL)
			return FALSE;
	}
	return TRUE;
}

static void device_add_cb(RoccatDeviceScannerInterface *interface, RoccatDevice *device, gpointer user_data) {
	IskuEventhandler *eventhandler = ISKU_EVENTHANDLER(user_data);
	IskuEventhandlerPrivate *priv = eventhandler->priv;
	GError *local_error = NULL;
	guint driver_state;

	if (priv->device) {
		g_critical(_("Multiple devices present."));
		return;
	}

	priv->device = device;
	g_object_ref(G_OBJECT(device));
	roccat_device_debug(device);

	configuration_reload(eventhandler);

	driver_state = roccat_eventhandler_host_should_set_driver_state(priv->host);
	if (driver_state == ROCCATEVENTHANDLER_CONFIGURATION_DRIVER_STATE_ON || driver_state == ROCCATEVENTHANDLER_CONFIGURATION_DRIVER_STATE_OFF) {
		if (!isku_device_state_write(priv->device, (driver_state == ROCCATEVENTHANDLER_CONFIGURATION_DRIVER_STATE_ON) ? ISKU_DEVICE_STATE_STATE_ON : ISKU_DEVICE_STATE_STATE_OFF, &local_error)) {
			g_warning(_("Could not correct driver state: %s"), local_error->message);
			g_clear_error(&local_error);
		}
	}

	/* Notificators gets initialized only once if a device is found.
	 * Get deinited once in finalizer.
	 */
	if (!priv->profile_note)
		priv->profile_note = roccat_notification_profile_new(roccat_eventhandler_host_get_notificator(priv->host), ISKU_DEVICE_NAME);
	if (!priv->timer_note)
		priv->timer_note = roccat_notification_timer_new(roccat_eventhandler_host_get_notificator(priv->host), ISKU_DEVICE_NAME);
	if (!priv->live_recording_note)
		priv->live_recording_note = roccat_notification_live_recording_new(roccat_eventhandler_host_get_notificator(priv->host), ISKU_DEVICE_NAME);
	
	profile_data_load(eventhandler, &local_error);
	if (local_error) {
		g_warning(_("Could not read rkp: %s"), local_error->message);
		g_clear_error(&local_error);
	}

	priv->actual_profile_index = isku_actual_profile_read(priv->device, &local_error);
	if (local_error) {
		g_warning(_("Could not read actual profile: %s"), local_error->message);
		g_clear_error(&local_error);
	}
	
	priv->dbus_server = isku_dbus_server_new();
	g_signal_connect(G_OBJECT(priv->dbus_server), "talk-easyshift", G_CALLBACK(talk_easyshift_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->dbus_server), "talk-easyshift-lock", G_CALLBACK(talk_easyshift_lock_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->dbus_server), "open-gui", G_CALLBACK(open_gui_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->dbus_server), "profile-changed-outside", G_CALLBACK(profile_changed_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->dbus_server), "profile-data-changed-outside", G_CALLBACK(profile_data_changed_outside_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->dbus_server), "configuration-changed-outside", G_CALLBACK(configuration_changed_outside_cb), eventhandler);
	isku_dbus_server_connect(priv->dbus_server);

	priv->active_window_changed_handler = g_signal_connect(G_OBJECT(priv->host), "active-window-changed", G_CALLBACK(window_changed_cb), eventhandler);

	isku_eventhandler_channel_start(priv->channel, priv->device, &local_error);
	if (local_error) {
		g_warning(_("Could not start chardev eventhandler: %s"), local_error->message);
		g_clear_error(&local_error);
	}
	
	priv->active_talk_type = ISKU_KEY_TYPE_DISABLED;

	priv->device_set_up = TRUE;

	roccat_eventhandler_plugin_emit_device_added(ROCCAT_EVENTHANDLER_PLUGIN(eventhandler), gaminggear_device_get_product_id(GAMINGGEAR_DEVICE(priv->device)));
}

static void stop(IskuEventhandler *eventhandler) {
	IskuEventhandlerPrivate *priv = eventhandler->priv;
	guint i;

	roccat_eventhandler_plugin_emit_device_removed(ROCCAT_EVENTHANDLER_PLUGIN(eventhandler), gaminggear_device_get_product_id(GAMINGGEAR_DEVICE(priv->device)));

	priv->device_set_up = FALSE;

	g_clear_object(&priv->dbus_server);
	isku_eventhandler_channel_stop(priv->channel);
	g_signal_handler_disconnect(G_OBJECT(priv->host), priv->active_window_changed_handler);
	g_clear_object(&priv->device);
	g_clear_pointer(&priv->config, isku_configuration_free);

	for (i = 0; i < ISKU_PROFILE_NUM; ++i)
		g_clear_pointer(&priv->rkp[i], isku_rkp_free);
}

static void device_remove_cb(RoccatDeviceScanner *scanner, RoccatDevice *device, gpointer user_data) {
	IskuEventhandler *eventhandler = ISKU_EVENTHANDLER(user_data);
	IskuEventhandlerPrivate *priv = eventhandler->priv;
	if (priv->device && gaminggear_device_equal(GAMINGGEAR_DEVICE(device), GAMINGGEAR_DEVICE(priv->device)))
		stop(eventhandler);
}

static void isku_eventhandler_start(RoccatEventhandlerPlugin *self, RoccatEventhandlerHost *host) {
	IskuEventhandler *eventhandler = ISKU_EVENTHANDLER(self);
	IskuEventhandlerPrivate *priv = eventhandler->priv;

	priv->host = host;
	g_object_ref(priv->host);

	priv->channel = isku_eventhandler_channel_new();
	g_signal_connect(G_OBJECT(priv->channel), "open-driver", G_CALLBACK(open_gui_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->channel), "profile-changed", G_CALLBACK(profile_changed_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->channel), "quicklaunch", G_CALLBACK(quicklaunch_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->channel), "talk", G_CALLBACK(talk_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->channel), "timer-start", G_CALLBACK(timer_start_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->channel), "timer-stop", G_CALLBACK(timer_stop_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->channel), "light-changed", G_CALLBACK(light_changed_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->channel), "live-recording", G_CALLBACK(live_recording_cb), eventhandler);

	priv->device_scanner = isku_device_scanner_new();
	g_signal_connect(G_OBJECT(priv->device_scanner), "device-added", G_CALLBACK(device_add_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->device_scanner), "device-removed", G_CALLBACK(device_remove_cb), eventhandler);
	roccat_device_scanner_interface_start(ROCCAT_DEVICE_SCANNER_INTERFACE(priv->device_scanner));
}

static void isku_eventhandler_stop(RoccatEventhandlerPlugin *self) {
	IskuEventhandler *eventhandler = ISKU_EVENTHANDLER(self);
	IskuEventhandlerPrivate *priv = eventhandler->priv;

	stop(eventhandler);

	g_clear_object(&priv->device_scanner);
	g_clear_object(&priv->channel);
	g_clear_object(&priv->host);
}

static void isku_roccat_eventhandler_interface_init(RoccatEventhandlerPluginInterface *iface) {
	iface->start = isku_eventhandler_start;
	iface->stop = isku_eventhandler_stop;
	iface->get_talk_device = isku_eventhandler_get_talk_device;
	iface->talk_easyshift = isku_eventhandler_talk_easyshift;
	iface->talk_easyshift_lock = isku_eventhandler_talk_easyshift_lock;
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

G_DEFINE_TYPE_WITH_CODE(IskuEventhandler, isku_eventhandler, G_TYPE_OBJECT,
		G_IMPLEMENT_INTERFACE(ROCCAT_EVENTHANDLER_PLUGIN_TYPE, isku_roccat_eventhandler_interface_init));

static void isku_eventhandler_init(IskuEventhandler *eventhandler) {
	eventhandler->priv = ISKU_EVENTHANDLER_GET_PRIVATE(eventhandler);
}

static void dispose(GObject *object) {
	isku_eventhandler_stop(ROCCAT_EVENTHANDLER_PLUGIN(object));

	G_OBJECT_CLASS(isku_eventhandler_parent_class)->dispose(object);
}

static void finalize(GObject *object) {
	IskuEventhandlerPrivate *priv = ISKU_EVENTHANDLER(object)->priv;

	g_clear_pointer(&priv->live_recording_note, roccat_notification_live_recording_free);
	g_clear_pointer(&priv->timer_note, roccat_notification_timer_free);
	g_clear_pointer(&priv->profile_note, roccat_notification_profile_free);
	
	G_OBJECT_CLASS(isku_eventhandler_parent_class)->finalize(object);
}

static void isku_eventhandler_class_init(IskuEventhandlerClass *klass) {
	GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
	gobject_class->dispose = dispose;
	gobject_class->finalize = finalize;

	g_type_class_add_private(klass, sizeof(IskuEventhandlerPrivate));
}

ROCCAT_EVENTHANDLER_PLUGIN_API RoccatEventhandlerPlugin *roccat_eventhandler_plugin_new(void) {
	return ROCCAT_EVENTHANDLER_PLUGIN(g_object_new(ISKU_EVENTHANDLER_TYPE, NULL));
}
