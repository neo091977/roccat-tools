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

#include "nyth_eventhandler_channel.h"
#include "nyth_dbus_server.h"
#include "nyth_gfx.h"
#include "roccat_eventhandler_plugin.h"
#include "roccat_notification_cpi.h"
#include "roccat_notification_profile.h"
#include "roccat_notification_sensitivity.h"
#include "roccat_notification_timer.h"
#include "roccat_process_helper.h"
#include "roccat_talk.h"
#include "roccat_helper.h"
#include "roccat.h"
#include "nyth.h"
#include "nyth_config.h"
#include "nyth_device.h"
#include "nyth_device_state.h"
#include "nyth_profile.h"
#include "nyth_profile_data.h"
#include "nyth_talk.h"
#include "config.h"
#include "i18n-lib.h"
#include "g_roccat_helper.h"
#include <gtk/gtk.h>
#include <glib.h>
#include <string.h>
#include <unistd.h>
#include "../libroccatnyth/nyth_special.h"

GType nyth_eventhandler_get_type(void);

#define NYTH_EVENTHANDLER_TYPE (nyth_eventhandler_get_type())
#define NYTH_EVENTHANDLER(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), NYTH_EVENTHANDLER_TYPE, NythEventhandler))
#define IS_NYTH_EVENTHANDLER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), NYTH_EVENTHANDLER_TYPE))
#define NYTH_EVENTHANDLER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), NYTH_EVENTHANDLER_TYPE, NythEventhandlerClass))
#define IS_NYTH_EVENTHANDLER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), NYTH_EVENTHANDLER_TYPE))
#define NYTH_EVENTHANDLER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), NYTH_EVENTHANDLER_TYPE, NythEventhandlerPrivate))

typedef struct _NythEventhandler NythEventhandler;
typedef struct _NythEventhandlerClass NythEventhandlerClass;
typedef struct _NythEventhandlerPrivate NythEventhandlerPrivate;

struct _NythEventhandler {
	GObject parent;
	NythEventhandlerPrivate *priv;
};

struct _NythEventhandlerClass {
	GObjectClass parent_class;
};

struct _NythEventhandlerPrivate {
	RoccatEventhandlerHost *host;
	NythDBusServer *dbus_server;
	RoccatDeviceScanner *device_scanner;
	RoccatDevice *device;
	gboolean device_set_up;
	RoccatKeyFile *config;
	guint actual_profile_index;
	guint actual_profile_count;
	guint actual_sensitivity_x;
	NythProfileData *profile_data[NYTH_PROFILE_NUM];
	NythGfx *gfx;
	NythEventhandlerChannel *channel;
	gulong active_window_changed_handler;
	RoccatNotificationProfile *profile_note;
	RoccatNotificationCpi *cpi_note;
	RoccatNotificationSensitivity *sensitivity_note;
	RoccatNotificationTimer *timer_note;
	guint8 active_talk_type;
	guint16 active_talk_target;
};

static void profile_changed(NythEventhandler *eventhandler, guint profile_number) {
	NythEventhandlerPrivate *priv = eventhandler->priv;
	guint profile_index = profile_number - 1;
	NythProfileDataEventhandler const *profile_data_eventhandler = &priv->profile_data[profile_index]->eventhandler;

	priv->actual_profile_index = profile_index;

	roccat_notification_profile_update(priv->profile_note, profile_data_eventhandler->profile_notification_type,
			profile_data_eventhandler->notification_volume,
			profile_number, profile_data_eventhandler->profile_name);

	nyth_dbus_server_emit_profile_changed(priv->dbus_server, profile_number);
}

static gboolean set_profile(NythEventhandler *eventhandler, guint profile_number) {
	NythEventhandlerPrivate *priv = eventhandler->priv;
	guint profile_index = profile_number - 1;
	GError *local_error = NULL;

	if (profile_number == 0)
		return TRUE;

	if (priv->actual_profile_index != profile_index) {
		if (!nyth_profile_write_index(priv->device, profile_index, priv->actual_profile_count, &local_error)) {
			g_warning(_("Could not activate profile %i: %s"), profile_number, local_error->message);
			g_clear_error(&local_error);
			return FALSE;
		}
		profile_changed(eventhandler, profile_number);
	}
	return TRUE;
}

static void window_changed_cb(RoccatEventhandlerHost *host, gchar const *title, gpointer user_data) {
	NythEventhandler *eventhandler = NYTH_EVENTHANDLER(user_data);
	NythEventhandlerPrivate *priv = eventhandler->priv;
	guint profile_index;
	guint gamefile_index;
	gchar const *pattern;

	for (profile_index = 0; profile_index < NYTH_PROFILE_NUM; ++profile_index) {

		if (priv->profile_data[profile_index] == NULL)
			continue;

		for (gamefile_index = 0; gamefile_index < ROCCAT_SWARM_RMP_GAMEFILE_NUM; ++gamefile_index) {
			pattern = priv->profile_data[profile_index]->eventhandler.gamefile_names[gamefile_index];
			if (strcmp(pattern, "") == 0)
				continue;

			if (g_regex_match_simple(pattern, title, 0, 0)) {
				set_profile(eventhandler, profile_index + 1);
				return;
			}
		}
	}

	set_profile(eventhandler, nyth_configuration_get_default_profile_number(priv->config));
	return;
}

static gboolean talk_easyshift(NythEventhandler *eventhandler, gboolean state) {
	NythEventhandlerPrivate *priv = eventhandler->priv;
	GError *error = NULL;
	gboolean retval = FALSE;

	if (priv->device_set_up) {
		retval = nyth_talk_easyshift(priv->device,
				(state != FALSE) ? NYTH_TALK_EASYSHIFT_ON : NYTH_TALK_EASYSHIFT_OFF,
				&error);
		if (error) {
			g_warning(_("Could not activate easyshift: %s"), error->message);
			g_clear_error(&error);
		}
	}

	return retval;
}

static gboolean talk_easyshift_lock(NythEventhandler *eventhandler, gboolean state) {
	NythEventhandlerPrivate *priv = eventhandler->priv;
	GError *error = NULL;
	gboolean retval = FALSE;

	if (priv->device_set_up) {
		retval = nyth_talk_easyshift_lock(priv->device,
				(state != FALSE) ? NYTH_TALK_EASYSHIFT_ON : NYTH_TALK_EASYSHIFT_OFF,
				&error);
		if (error) {
			g_warning(_("Could not activate easyshift lock: %s"), error->message);
			g_clear_error(&error);
		}
	}

	return retval;
}

static gboolean talk_easyaim(NythEventhandler *eventhandler, guchar state) {
	NythEventhandlerPrivate *priv = eventhandler->priv;
	GError *error = NULL;
	gboolean retval = FALSE;

	if (priv->device_set_up) {
		retval = nyth_talk_easyaim(priv->device, state, &error);
		if (error) {
			g_warning(_("Could not activate easyaim: %s"), error->message);
			g_clear_error(&error);
		}
	}

	return retval;
}

static gboolean should_execute_fx(NythEventhandler *eventhandler) {
	return TRUE; // FIXME
}

static gboolean talkfx_set_led_rgb(NythEventhandler *eventhandler, guint32 effect, guint32 ambient_color, guint32 event_color) {
	NythEventhandlerPrivate *priv = eventhandler->priv;
	GError *error = NULL;
	gboolean retval;

	if (!should_execute_fx(eventhandler))
		return FALSE;

	if (!priv->device_set_up)
		return FALSE;

	retval = nyth_talkfx(priv->device, effect, ambient_color, event_color, &error);
	if (error) {
		g_warning(_("Could not activate Talk FX: %s"), error->message);
		g_clear_error(&error);
	}

	return retval;
}

static gboolean talkfx_restore_led_rgb(NythEventhandler *eventhandler) {
	NythEventhandlerPrivate *priv = eventhandler->priv;
	GError *error = NULL;
	gboolean retval;

	if (!should_execute_fx(eventhandler))
		return FALSE;

	if (!priv->device_set_up)
		return FALSE;

	retval = nyth_talkfx_off(priv->device, &error);
	if (error) {
		g_warning(_("Could not deactivate Talk FX: %s"), error->message);
		g_clear_error(&error);
	}

	return retval;
}

static guint nyth_eventhandler_get_talk_device(RoccatEventhandlerPlugin *self) {
	NythEventhandlerPrivate *priv = NYTH_EVENTHANDLER(self)->priv;

	if (!priv->device_set_up)
		return 0;

	return gaminggear_device_get_product_id(GAMINGGEAR_DEVICE(priv->device));
}

static void nyth_eventhandler_talk_easyshift(RoccatEventhandlerPlugin *self, gboolean state) {
	NythEventhandler *eventhandler = NYTH_EVENTHANDLER(self);
	talk_easyshift(eventhandler, state);
}

static void nyth_eventhandler_talk_easyshift_lock(RoccatEventhandlerPlugin *self, gboolean state) {
	NythEventhandler *eventhandler = NYTH_EVENTHANDLER(self);
	talk_easyshift_lock(eventhandler, state);
}

static void nyth_eventhandler_talk_easyaim(RoccatEventhandlerPlugin *self, guchar state) {
	NythEventhandler *eventhandler = NYTH_EVENTHANDLER(self);
	talk_easyaim(eventhandler, state);
}

static void nyth_eventhandler_talkfx_set_led_rgb(RoccatEventhandlerPlugin *self, guint32 effect, guint32 ambient_color, guint32 event_color) {
	NythEventhandler *eventhandler = NYTH_EVENTHANDLER(self);
	talkfx_set_led_rgb(eventhandler, effect, ambient_color, event_color);
}

static void nyth_eventhandler_talkfx_restore_led_rgb(RoccatEventhandlerPlugin *self) {
	NythEventhandler *eventhandler = NYTH_EVENTHANDLER(self);
	talkfx_restore_led_rgb(eventhandler);
}

static void talk_easyshift_cb(NythDBusServer *server, guchar state, gpointer user_data) {
	NythEventhandler *eventhandler = NYTH_EVENTHANDLER(user_data);
	talk_easyshift(eventhandler, state);
}

static void talk_easyshift_lock_cb(NythDBusServer *server, guchar state, gpointer user_data) {
	NythEventhandler *eventhandler = NYTH_EVENTHANDLER(user_data);
	talk_easyshift_lock(eventhandler, state);
}

static void talk_easyaim_cb(NythDBusServer *server, guchar state, gpointer user_data) {
	NythEventhandler *eventhandler = NYTH_EVENTHANDLER(user_data);
	talk_easyaim(eventhandler, state);
}

static void talkfx_set_led_rgb_cb(NythDBusServer *server, guint effect, guint ambient_color, guint event_color, gpointer user_data) {
	NythEventhandler *eventhandler = NYTH_EVENTHANDLER(user_data);
	talkfx_set_led_rgb(eventhandler, effect, ambient_color, event_color);
}

static void talkfx_restore_led_rgb_cb(NythDBusServer *server, gpointer user_data) {
	NythEventhandler *eventhandler = NYTH_EVENTHANDLER(user_data);
	talkfx_restore_led_rgb(eventhandler);
}

static void gfx_set_led_rgb_cb(NythDBusServer *server, guint index, guint color, gpointer user_data) {
	NythEventhandler *eventhandler = NYTH_EVENTHANDLER(user_data);
	if (!should_execute_fx(eventhandler))
		return;
	nyth_gfx_set_color(eventhandler->priv->gfx, index, color);
}

static void gfx_get_led_rgb_cb(NythDBusServer *server, guint index, guint *color, gpointer user_data) {
	NythEventhandler *eventhandler = NYTH_EVENTHANDLER(user_data);
	if (!should_execute_fx(eventhandler))
		return;
	*color = nyth_gfx_get_color(eventhandler->priv->gfx, index);
}

static void gfx_update_cb(NythDBusServer *server, gpointer user_data) {
	NythEventhandler *eventhandler = NYTH_EVENTHANDLER(user_data);
	if (!should_execute_fx(eventhandler))
		return;
	(void)nyth_gfx_update(eventhandler->priv->gfx, NULL);
}

static void open_gui_cb(gpointer source, gpointer user_data) {
	gchar *path;
	path = g_build_path("/", INSTALL_PREFIX, "bin", "roccatnythconfig", (gchar const *)NULL);
	roccat_double_fork(path);
	g_free(path);
}

static NythProfileData *single_profile_data_load(RoccatDevice *device, guint profile_index) {
	NythProfileData *profile_data;
	GError *local_error = NULL;

	profile_data = nyth_profile_data_new();

	if (!nyth_profile_data_update_filesystem(profile_data, profile_index, &local_error)) {
		g_warning(_("Could not load filesystem data for profile %i: %s"), profile_index, local_error->message);
		g_clear_error(&local_error);
	}

	if (!nyth_profile_data_update_hardware(profile_data, device, profile_index, &local_error)) {
		g_warning(_("Could not load hardware data for profile %i: %s"), profile_index, local_error->message);
		g_clear_error(&local_error);
	}

	return profile_data;
}

static void profile_data_changed_outside_cb(NythDBusServer *server, guchar profile_number, gpointer user_data) {
	NythEventhandler *eventhandler = NYTH_EVENTHANDLER(user_data);
	NythEventhandlerPrivate *priv = eventhandler->priv;
	guint profile_index = profile_number - 1;

	g_free(priv->profile_data[profile_index]);
	priv->profile_data[profile_index] = single_profile_data_load(priv->device, profile_index);
}

static void configuration_reload(NythEventhandler *eventhandler) {
	NythEventhandlerPrivate *priv = eventhandler->priv;
	if (priv->config)
		nyth_configuration_free(priv->config);
	priv->config = nyth_configuration_load();
}

static void configuration_changed_outside_cb(NythDBusServer *server, gpointer user_data) {
	NythEventhandler *eventhandler = NYTH_EVENTHANDLER(user_data);
	configuration_reload(eventhandler);
}

static void cpi_changed_cb(NythEventhandlerChannel *channel, guchar cpi_index, gpointer user_data) {
	NythEventhandlerPrivate *priv = NYTH_EVENTHANDLER(user_data)->priv;
	NythProfileDataEventhandler const *profile_data_eventhandler = &priv->profile_data[priv->actual_profile_index]->eventhandler;
	guint cpi = nyth_profile_settings_get_cpi_level(&priv->profile_data[priv->actual_profile_index]->hardware.profile_settings, cpi_index);

	roccat_notification_cpi_update(priv->cpi_note,
			profile_data_eventhandler->cpi_notification_type,
			profile_data_eventhandler->notification_volume,
			cpi, cpi);
}

static void profile_changed_cb(gpointer source, guchar profile_number, gpointer user_data) {
	NythEventhandler *eventhandler = NYTH_EVENTHANDLER(user_data);
	profile_changed(eventhandler, profile_number);
}

static void open_application_cb(NythEventhandlerChannel *channel, guchar key_index, gpointer user_data) {
	NythEventhandler *eventhandler = NYTH_EVENTHANDLER(user_data);
	NythEventhandlerPrivate *priv = eventhandler->priv;

	roccat_double_fork(priv->profile_data[priv->actual_profile_index]->eventhandler.openers[key_index]);
}

static void sensitivity_changed_cb(NythEventhandlerChannel *channel, guchar x, guchar y, gpointer user_data) {
	NythEventhandlerPrivate *priv = NYTH_EVENTHANDLER(user_data)->priv;
	NythProfileDataEventhandler const *profile_data_eventhandler = &priv->profile_data[priv->actual_profile_index]->eventhandler;
	gint notification_type;

	notification_type = profile_data_eventhandler->sensitivity_notification_type;

	if (notification_type == ROCCAT_NOTIFICATION_TYPE_OSD)
		roccat_notification_sensitivity_update(priv->sensitivity_note, notification_type, roccat_sensitivity_from_bin_to_value(x), roccat_sensitivity_from_bin_to_value(y));
	else if (x > priv->actual_sensitivity_x)
		roccat_notification_sensitivity_update_up(priv->sensitivity_note, notification_type,
				profile_data_eventhandler->notification_volume);
	else
		roccat_notification_sensitivity_update_down(priv->sensitivity_note, notification_type,
				profile_data_eventhandler->notification_volume);

	priv->actual_sensitivity_x = x;
}

static void windows_cb(NythEventhandlerChannel *channel, guchar subtype, gpointer user_data) {
	switch(subtype) {
	case NYTH_SPECIAL_SUBTYPE_WINDOWS_FOLDER_DESKTOP:
		roccat_open_default_folder("DESKTOP");
		break;
	case NYTH_SPECIAL_SUBTYPE_WINDOWS_FOLDER_MY_DOCUMENTS:
		roccat_open_default_folder("DOCUMENTS");
		break;
	case NYTH_SPECIAL_SUBTYPE_WINDOWS_FOLDER_MY_DOWNLOADS:
		roccat_open_default_folder("DOWNLOAD");
		break;
	case NYTH_SPECIAL_SUBTYPE_WINDOWS_FOLDER_MY_MUSIC:
		roccat_open_default_folder("MUSIC");
		break;
	case NYTH_SPECIAL_SUBTYPE_WINDOWS_FOLDER_MY_PICTURES:
		roccat_open_default_folder("PICTURES");
		break;
	case NYTH_SPECIAL_SUBTYPE_WINDOWS_MUTE_MICROPHONE:
	case NYTH_SPECIAL_SUBTYPE_WINDOWS_FOLDER_FAVORITES:
	case NYTH_SPECIAL_SUBTYPE_WINDOWS_FOLDER_FONTS:
	case NYTH_SPECIAL_SUBTYPE_WINDOWS_FOLDER_NETWORK:
	case NYTH_SPECIAL_SUBTYPE_WINDOWS_FOLDER_PRINTERS:
	case NYTH_SPECIAL_SUBTYPE_WINDOWS_FOLDER_SYSTEM:
	case NYTH_SPECIAL_SUBTYPE_WINDOWS_FUNCTION_CONTROL_PANEL:
	case NYTH_SPECIAL_SUBTYPE_WINDOWS_FUNCTION_SYSTEM_PANEL:
	case NYTH_SPECIAL_SUBTYPE_WINDOWS_FUNCTION_TASK_MANAGER:
	case NYTH_SPECIAL_SUBTYPE_WINDOWS_FUNCTION_SCREEN_SETTINGS:
	case NYTH_SPECIAL_SUBTYPE_WINDOWS_FUNCTION_SCREENSAVER:
	case NYTH_SPECIAL_SUBTYPE_WINDOWS_FUNCTION_THEMES:
	case NYTH_SPECIAL_SUBTYPE_WINDOWS_FUNCTION_DATE_TIME:
	case NYTH_SPECIAL_SUBTYPE_WINDOWS_FUNCTION_NETWORK_SETTINGS:
	case NYTH_SPECIAL_SUBTYPE_WINDOWS_FUNCTION_ADMIN_PANEL:
	case NYTH_SPECIAL_SUBTYPE_WINDOWS_FUNCTION_FIREWALL:
	case NYTH_SPECIAL_SUBTYPE_WINDOWS_FUNCTION_REGEDIT:
	case NYTH_SPECIAL_SUBTYPE_WINDOWS_FUNCTION_EVENT_MONITOR:
	case NYTH_SPECIAL_SUBTYPE_WINDOWS_FUNCTION_PERFORMANCE_MONITOR:
	case NYTH_SPECIAL_SUBTYPE_WINDOWS_FUNCTION_SOUND_SETTINGS:
	case NYTH_SPECIAL_SUBTYPE_WINDOWS_FUNCTION_INTERNET_SETTINGS:
	case NYTH_SPECIAL_SUBTYPE_WINDOWS_FUNCTION_DIREKTX_DIAG:
	case NYTH_SPECIAL_SUBTYPE_WINDOWS_FUNCTION_COMMAND_LINE:
		g_roccat_info(_("Unsupported windows subtype %i"), subtype);
		break;
	default:
		g_warning(_("Unknown windows subtype %i"), subtype);
		break;
	}
}

static void open_cb(NythEventhandlerChannel *channel, guchar subtype, guchar key_index, gpointer user_data) {
	NythEventhandlerPrivate *priv = NYTH_EVENTHANDLER(user_data)->priv;
	NythProfileDataEventhandler *eventhandler = &priv->profile_data[priv->actual_profile_index]->eventhandler;

	switch(subtype) {
	case NYTH_SPECIAL_SUBTYPE_OPEN_DOCUMENT:
	case NYTH_SPECIAL_SUBTYPE_OPEN_FOLDER:
	case NYTH_SPECIAL_SUBTYPE_OPEN_WEBSITE:
		roccat_open_url(eventhandler->openers[key_index]);
		break;
	default:
		g_warning(_("Unknown open subtype %i"), subtype);
		break;
	}
}

static void easyshift_cb(NythEventhandlerChannel *channel, guchar key_index, guchar action, gpointer user_data) {
	NythEventhandler *eventhandler = NYTH_EVENTHANDLER(user_data);
	NythEventhandlerPrivate *priv = eventhandler->priv;
	NythProfileData *profile_data = priv->profile_data[priv->actual_profile_index];
	static gboolean state = FALSE;

	if (action == NYTH_SPECIAL_ACTION_PRESS) {
		priv->active_talk_type = profile_data->hardware.profile_buttons.buttons[key_index].type;
		priv->active_talk_target = profile_data->eventhandler.talk_targets[key_index];
	}
	
	switch (priv->active_talk_type) {
	case NYTH_BUTTON_TYPE_EASYSHIFT_ALL:
		roccat_eventhandler_plugin_emit_talk_easyshift(ROCCAT_EVENTHANDLER_PLUGIN(eventhandler),
				ROCCAT_TALK_DEVICE_KEYBOARD,
				(action == NYTH_SPECIAL_ACTION_PRESS) ? TRUE : FALSE);
		break;
	case NYTH_BUTTON_TYPE_EASYSHIFT_OTHER:
		roccat_eventhandler_plugin_emit_talk_easyshift(ROCCAT_EVENTHANDLER_PLUGIN(eventhandler),
				priv->active_talk_target,
				(action == NYTH_SPECIAL_ACTION_PRESS) ? TRUE : FALSE);
		break;
	case NYTH_BUTTON_TYPE_EASYSHIFT_LOCK_OTHER:
		if (action == NYTH_SPECIAL_ACTION_PRESS) {
			state = !state;
			roccat_eventhandler_plugin_emit_talk_easyshift_lock(ROCCAT_EVENTHANDLER_PLUGIN(eventhandler),
					priv->active_talk_target,
					state);
		}
		break;
	default:
		g_debug(_("Got unknown Talk event type 0x%02x"), priv->active_talk_type);
	}

	if (action == NYTH_SPECIAL_ACTION_RELEASE) {
		priv->active_talk_type = NYTH_BUTTON_TYPE_DISABLED;
		priv->active_talk_target = 0;
	}
}

static void timer_start_cb(NythEventhandlerChannel *channel, guchar key_index, gpointer user_data) {
	NythEventhandler *eventhandler = NYTH_EVENTHANDLER(user_data);
	NythEventhandlerPrivate *priv = eventhandler->priv;
	NythProfileDataEventhandler const *profile_data_eventhandler = &priv->profile_data[priv->actual_profile_index]->eventhandler;

	roccat_notification_timer_start(priv->timer_note,
			profile_data_eventhandler->timer_notification_type,
			profile_data_eventhandler->notification_volume,
			(gchar const *)profile_data_eventhandler->timers[key_index].name,
			roccat_timer_get_seconds(&profile_data_eventhandler->timers[key_index]));
}

static void timer_stop_cb(NythEventhandlerChannel *channel, gpointer user_data) {
	NythEventhandler *eventhandler = NYTH_EVENTHANDLER(user_data);
	NythEventhandlerPrivate *priv = eventhandler->priv;

	roccat_notification_timer_stop(priv->timer_note);
}

static void profile_data_load(NythEventhandler *eventhandler, guint count) {
	NythEventhandlerPrivate *priv = eventhandler->priv;
	guint i;

	for (i = 0; i < count; ++i)
		priv->profile_data[i] = single_profile_data_load(priv->device, i);
}

static void device_add_cb(RoccatDeviceScannerInterface *interface, RoccatDevice *device, gpointer user_data) {
	NythEventhandler *eventhandler = NYTH_EVENTHANDLER(user_data);
	NythEventhandlerPrivate *priv = eventhandler->priv;
	GError *local_error = NULL;
	guint driver_state;
	NythProfile *profile;

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
		if (!nyth_device_state_write(priv->device, (driver_state == ROCCATEVENTHANDLER_CONFIGURATION_DRIVER_STATE_ON) ? NYTH_DEVICE_STATE_STATE_ON : NYTH_DEVICE_STATE_STATE_OFF, &local_error)) {
			g_warning(_("Could not correct driver state: %s"), local_error->message);
			g_clear_error(&local_error);
		}
	}

	/* Notificator gets initialized only once if a device is found.
	 * Gets deinited once in finalizer.
	 */
	if (!priv->profile_note)
		priv->profile_note = roccat_notification_profile_new(roccat_eventhandler_host_get_notificator(priv->host), NYTH_DEVICE_NAME);
	if (!priv->cpi_note)
		priv->cpi_note = roccat_notification_cpi_new(roccat_eventhandler_host_get_notificator(priv->host), NYTH_DEVICE_NAME);
	if (!priv->sensitivity_note)
		priv->sensitivity_note = roccat_notification_sensitivity_new(roccat_eventhandler_host_get_notificator(priv->host), NYTH_DEVICE_NAME);
	if (!priv->timer_note)
		priv->timer_note = roccat_notification_timer_new(roccat_eventhandler_host_get_notificator(priv->host), NYTH_DEVICE_NAME);

	profile = nyth_profile_read(priv->device, &local_error);
	if (local_error) {
		g_warning(_("Could not read actual profile: %s"), local_error->message);
		g_clear_error(&local_error);
		priv->actual_profile_index = 0;
		priv->actual_profile_count = 5;
		profile_data_load(eventhandler, priv->actual_profile_count);
		priv->actual_sensitivity_x = (ROCCAT_SENSITIVITY_MIN + ROCCAT_SENSITIVITY_MAX) >> 1;
	} else {
		priv->actual_profile_index = profile->index;
		priv->actual_profile_count = MAX(1, profile->count);
		profile_data_load(eventhandler, priv->actual_profile_count);
		priv->actual_sensitivity_x = priv->profile_data[priv->actual_profile_index]->hardware.profile_settings.sensitivity_x;
		g_free(profile);
	}

	priv->gfx = nyth_gfx_new(priv->device);

	// FIXME create in init and just disconnect/reconnect?
	priv->dbus_server = nyth_dbus_server_new();
	g_signal_connect(G_OBJECT(priv->dbus_server), "talk-easyshift", G_CALLBACK(talk_easyshift_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->dbus_server), "talk-easyshift-lock", G_CALLBACK(talk_easyshift_lock_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->dbus_server), "talk-easyaim", G_CALLBACK(talk_easyaim_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->dbus_server), "talkfx-set-led-rgb", G_CALLBACK(talkfx_set_led_rgb_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->dbus_server), "talkfx-restore-led-rgb", G_CALLBACK(talkfx_restore_led_rgb_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->dbus_server), "gfx-set-led-rgb", G_CALLBACK(gfx_set_led_rgb_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->dbus_server), "gfx-get-led-rgb", G_CALLBACK(gfx_get_led_rgb_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->dbus_server), "gfx-update", G_CALLBACK(gfx_update_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->dbus_server), "open-gui", G_CALLBACK(open_gui_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->dbus_server), "profile-changed-outside", G_CALLBACK(profile_changed_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->dbus_server), "profile-data-changed-outside", G_CALLBACK(profile_data_changed_outside_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->dbus_server), "configuration-changed-outside", G_CALLBACK(configuration_changed_outside_cb), eventhandler);
	nyth_dbus_server_connect(priv->dbus_server);

	priv->active_window_changed_handler = g_signal_connect(G_OBJECT(priv->host), "active-window-changed", G_CALLBACK(window_changed_cb), eventhandler);

	nyth_eventhandler_channel_start(priv->channel, priv->device, &local_error);
	if (local_error) {
		g_warning(_("Could not start chardev eventhandler: %s"), local_error->message);
		g_clear_error(&local_error);
	}

	priv->active_talk_type = NYTH_BUTTON_TYPE_DISABLED;
	priv->active_talk_target = 0;

	priv->device_set_up = TRUE;

	roccat_eventhandler_plugin_emit_device_added(ROCCAT_EVENTHANDLER_PLUGIN(eventhandler), gaminggear_device_get_product_id(GAMINGGEAR_DEVICE(priv->device)));
}

static void stop(NythEventhandler *eventhandler) {
	NythEventhandlerPrivate *priv = eventhandler->priv;
	guint i;

	roccat_eventhandler_plugin_emit_device_removed(ROCCAT_EVENTHANDLER_PLUGIN(eventhandler), gaminggear_device_get_product_id(GAMINGGEAR_DEVICE(priv->device)));

	priv->device_set_up = FALSE;

	g_clear_object(&priv->dbus_server);
	nyth_eventhandler_channel_stop(priv->channel);
	g_signal_handler_disconnect(G_OBJECT(priv->host), priv->active_window_changed_handler);
	g_clear_object(&priv->device);
	g_clear_pointer(&priv->config, nyth_configuration_free);

	for (i = 0; i < NYTH_PROFILE_NUM; ++i)
		g_clear_pointer(&priv->profile_data[i], g_free);

	g_clear_object(&priv->gfx);
}

static void device_remove_cb(RoccatDeviceScanner *scanner, RoccatDevice *device, gpointer user_data) {
	NythEventhandler *eventhandler = NYTH_EVENTHANDLER(user_data);
	NythEventhandlerPrivate *priv = eventhandler->priv;
	if (priv->device && gaminggear_device_equal(GAMINGGEAR_DEVICE(device), GAMINGGEAR_DEVICE(priv->device)))
		stop(eventhandler);
}

static void nyth_eventhandler_start(RoccatEventhandlerPlugin *self, RoccatEventhandlerHost *host) {
	NythEventhandler *eventhandler = NYTH_EVENTHANDLER(self);
	NythEventhandlerPrivate *priv = eventhandler->priv;

	priv->host = host;
	g_object_ref(priv->host);

	priv->channel = nyth_eventhandler_channel_new();
	g_signal_connect(G_OBJECT(priv->channel), "profile-changed", G_CALLBACK(profile_changed_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->channel), "open-application", G_CALLBACK(open_application_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->channel), "timer-start", G_CALLBACK(timer_start_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->channel), "timer-stop", G_CALLBACK(timer_stop_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->channel), "open-driver", G_CALLBACK(open_gui_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->channel), "cpi-changed", G_CALLBACK(cpi_changed_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->channel), "sensitivity-changed", G_CALLBACK(sensitivity_changed_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->channel), "windows", G_CALLBACK(windows_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->channel), "open", G_CALLBACK(open_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->channel), "easyshift", G_CALLBACK(easyshift_cb), eventhandler);

	priv->device_scanner = nyth_device_scanner_new();
	g_signal_connect(G_OBJECT(priv->device_scanner), "device-added", G_CALLBACK(device_add_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->device_scanner), "device-removed", G_CALLBACK(device_remove_cb), eventhandler);
	roccat_device_scanner_interface_start(ROCCAT_DEVICE_SCANNER_INTERFACE(priv->device_scanner));
}

static void nyth_eventhandler_stop(RoccatEventhandlerPlugin *self) {
	NythEventhandler *eventhandler = NYTH_EVENTHANDLER(self);
	NythEventhandlerPrivate *priv = eventhandler->priv;

	stop(eventhandler);

	g_clear_object(&priv->device_scanner);
	g_clear_object(&priv->channel);
	g_clear_object(&priv->host);
}

static void nyth_roccat_eventhandler_interface_init(RoccatEventhandlerPluginInterface *iface) {
	iface->start = nyth_eventhandler_start;
	iface->stop = nyth_eventhandler_stop;
	iface->get_talk_device = nyth_eventhandler_get_talk_device;
	iface->talk_easyshift = nyth_eventhandler_talk_easyshift;
	iface->talk_easyshift_lock = nyth_eventhandler_talk_easyshift_lock;
	iface->talk_easyaim = nyth_eventhandler_talk_easyaim;
	iface->talkfx_set_led_rgb = nyth_eventhandler_talkfx_set_led_rgb;
	iface->talkfx_restore_led_rgb = nyth_eventhandler_talkfx_restore_led_rgb;
	iface->talkfx_ryos_set_sdk_mode = NULL;
	iface->talkfx_ryos_set_all_leds = NULL;
	iface->talkfx_ryos_turn_on_all_leds = NULL;
	iface->talkfx_ryos_turn_off_all_leds = NULL;
	iface->talkfx_ryos_set_led_on = NULL;
	iface->talkfx_ryos_set_led_off = NULL;
	iface->talkfx_ryos_all_key_blinking = NULL;
}

G_DEFINE_TYPE_WITH_CODE(NythEventhandler, nyth_eventhandler, G_TYPE_OBJECT,
		G_IMPLEMENT_INTERFACE(ROCCAT_EVENTHANDLER_PLUGIN_TYPE, nyth_roccat_eventhandler_interface_init));

static void nyth_eventhandler_init(NythEventhandler *eventhandler) {
	eventhandler->priv = NYTH_EVENTHANDLER_GET_PRIVATE(eventhandler);
}

static void dispose(GObject *object) {
	nyth_eventhandler_stop(ROCCAT_EVENTHANDLER_PLUGIN(object));

	G_OBJECT_CLASS(nyth_eventhandler_parent_class)->dispose(object);
}

static void finalize(GObject *object) {
	NythEventhandlerPrivate *priv = NYTH_EVENTHANDLER(object)->priv;

	g_clear_pointer(&priv->timer_note, roccat_notification_timer_free);
	g_clear_pointer(&priv->profile_note, roccat_notification_profile_free);
	g_clear_pointer(&priv->cpi_note, roccat_notification_cpi_free);
	g_clear_pointer(&priv->sensitivity_note, roccat_notification_sensitivity_free);

	G_OBJECT_CLASS(nyth_eventhandler_parent_class)->finalize(object);
}

static void nyth_eventhandler_class_init(NythEventhandlerClass *klass) {
	GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
	gobject_class->dispose = dispose;
	gobject_class->finalize = finalize;

	g_type_class_add_private(klass, sizeof(NythEventhandlerPrivate));
}

ROCCAT_EVENTHANDLER_PLUGIN_API RoccatEventhandlerPlugin *roccat_eventhandler_plugin_new(void) {
	return ROCCAT_EVENTHANDLER_PLUGIN(g_object_new(NYTH_EVENTHANDLER_TYPE, NULL));
}
