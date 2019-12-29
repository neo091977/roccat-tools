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

#include "kova2016_eventhandler_channel.h"
#include "kova2016_dbus_server.h"
#include "kova2016_gfx.h"
#include "roccat_eventhandler_plugin.h"
#include "roccat_notification_cpi.h"
#include "roccat_notification_profile.h"
#include "roccat_notification_timer.h"
#include "roccat_process_helper.h"
#include "roccat_talk.h"
#include "roccat_helper.h"
#include "roccat.h"
#include "kova2016.h"
#include "kova2016_config.h"
#include "kova2016_device.h"
#include "kova2016_device_state.h"
#include "kova2016_profile.h"
#include "kova2016_profile_data.h"
#include "kova2016_talk.h"
#include "config.h"
#include "i18n-lib.h"
#include "g_roccat_helper.h"
#include <gtk/gtk.h>
#include <glib.h>
#include <string.h>
#include <unistd.h>
#include "../libroccatkova2016/kova2016_special.h"

GType kova2016_eventhandler_get_type(void);

#define KOVA2016_EVENTHANDLER_TYPE (kova2016_eventhandler_get_type())
#define KOVA2016_EVENTHANDLER(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), KOVA2016_EVENTHANDLER_TYPE, Kova2016Eventhandler))
#define IS_KOVA2016_EVENTHANDLER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), KOVA2016_EVENTHANDLER_TYPE))
#define KOVA2016_EVENTHANDLER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), KOVA2016_EVENTHANDLER_TYPE, Kova2016EventhandlerClass))
#define IS_KOVA2016_EVENTHANDLER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), KOVA2016_EVENTHANDLER_TYPE))
#define KOVA2016_EVENTHANDLER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), KOVA2016_EVENTHANDLER_TYPE, Kova2016EventhandlerPrivate))

typedef struct _Kova2016Eventhandler Kova2016Eventhandler;
typedef struct _Kova2016EventhandlerClass Kova2016EventhandlerClass;
typedef struct _Kova2016EventhandlerPrivate Kova2016EventhandlerPrivate;

struct _Kova2016Eventhandler {
	GObject parent;
	Kova2016EventhandlerPrivate *priv;
};

struct _Kova2016EventhandlerClass {
	GObjectClass parent_class;
};

struct _Kova2016EventhandlerPrivate {
	RoccatEventhandlerHost *host;
	Kova2016DBusServer *dbus_server;
	RoccatDeviceScanner *device_scanner;
	RoccatDevice *device;
	gboolean device_set_up;
	RoccatKeyFile *config;
	guint actual_profile_index;
	guint actual_profile_count;
	Kova2016ProfileData *profile_data[KOVA2016_PROFILE_NUM];
	Kova2016Gfx *gfx;
	Kova2016EventhandlerChannel *channel;
	gulong active_window_changed_handler;
	RoccatNotificationProfile *profile_note;
	RoccatNotificationCpi *cpi_note;
	RoccatNotificationTimer *timer_note;
	guint8 active_talk_type;
	guint16 active_talk_target;
};

static void profile_changed(Kova2016Eventhandler *eventhandler, guint profile_number) {
	Kova2016EventhandlerPrivate *priv = eventhandler->priv;
	guint profile_index = profile_number - 1;
	Kova2016ProfileDataEventhandler const *profile_data_eventhandler = &priv->profile_data[profile_index]->eventhandler;

	priv->actual_profile_index = profile_index;

	roccat_notification_profile_update(priv->profile_note, profile_data_eventhandler->profile_notification_type,
			profile_data_eventhandler->notification_volume,
			profile_number, profile_data_eventhandler->profile_name);

	kova2016_dbus_server_emit_profile_changed(priv->dbus_server, profile_number);
}

static gboolean set_profile(Kova2016Eventhandler *eventhandler, guint profile_number) {
	Kova2016EventhandlerPrivate *priv = eventhandler->priv;
	guint profile_index = profile_number - 1;
	GError *local_error = NULL;

	if (profile_number == 0)
		return TRUE;

	if (priv->actual_profile_index != profile_index) {
		if (!kova2016_profile_write_index(priv->device, profile_index, priv->actual_profile_count, &local_error)) {
			g_warning(_("Could not activate profile %i: %s"), profile_number, local_error->message);
			g_clear_error(&local_error);
			return FALSE;
		}
		profile_changed(eventhandler, profile_number);
	}
	return TRUE;
}

static void window_changed_cb(RoccatEventhandlerHost *host, gchar const *title, gpointer user_data) {
	Kova2016Eventhandler *eventhandler = KOVA2016_EVENTHANDLER(user_data);
	Kova2016EventhandlerPrivate *priv = eventhandler->priv;
	guint profile_index;
	guint gamefile_index;
	gchar const *pattern;

	for (profile_index = 0; profile_index < KOVA2016_PROFILE_NUM; ++profile_index) {

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

	set_profile(eventhandler, kova2016_configuration_get_default_profile_number(priv->config));
	return;
}

static gboolean talk_easyshift(Kova2016Eventhandler *eventhandler, gboolean state) {
	Kova2016EventhandlerPrivate *priv = eventhandler->priv;
	GError *error = NULL;
	gboolean retval = FALSE;

	if (priv->device_set_up) {
		retval = kova2016_talk_easyshift(priv->device,
				(state != FALSE) ? KOVA2016_TALK_EASYSHIFT_ON : KOVA2016_TALK_EASYSHIFT_OFF,
				&error);
		if (error) {
			g_warning(_("Could not activate easyshift: %s"), error->message);
			g_clear_error(&error);
		}
	}

	return retval;
}

static gboolean talk_easyshift_lock(Kova2016Eventhandler *eventhandler, gboolean state) {
	Kova2016EventhandlerPrivate *priv = eventhandler->priv;
	GError *error = NULL;
	gboolean retval = FALSE;

	if (priv->device_set_up) {
		retval = kova2016_talk_easyshift_lock(priv->device,
				(state != FALSE) ? KOVA2016_TALK_EASYSHIFT_ON : KOVA2016_TALK_EASYSHIFT_OFF,
				&error);
		if (error) {
			g_warning(_("Could not activate easyshift lock: %s"), error->message);
			g_clear_error(&error);
		}
	}

	return retval;
}

static gboolean talk_easyaim(Kova2016Eventhandler *eventhandler, guchar state) {
	Kova2016EventhandlerPrivate *priv = eventhandler->priv;
	GError *error = NULL;
	gboolean retval = FALSE;

	if (priv->device_set_up) {
		retval = kova2016_talk_easyaim(priv->device, state, &error);
		if (error) {
			g_warning(_("Could not activate easyaim: %s"), error->message);
			g_clear_error(&error);
		}
	}

	return retval;
}

static gboolean should_execute_fx(Kova2016Eventhandler *eventhandler) {
	return TRUE; // FIXME
}

static gboolean talkfx_set_led_rgb(Kova2016Eventhandler *eventhandler, guint32 effect, guint32 ambient_color, guint32 event_color) {
	Kova2016EventhandlerPrivate *priv = eventhandler->priv;
	GError *error = NULL;
	gboolean retval;

	if (!should_execute_fx(eventhandler))
		return FALSE;

	if (!priv->device_set_up)
		return FALSE;

	retval = kova2016_talkfx(priv->device, effect, ambient_color, event_color, &error);
	if (error) {
		g_warning(_("Could not activate Talk FX: %s"), error->message);
		g_clear_error(&error);
	}

	return retval;
}

static gboolean talkfx_restore_led_rgb(Kova2016Eventhandler *eventhandler) {
	Kova2016EventhandlerPrivate *priv = eventhandler->priv;
	GError *error = NULL;
	gboolean retval;

	if (!should_execute_fx(eventhandler))
		return FALSE;

	if (!priv->device_set_up)
		return FALSE;

	retval = kova2016_talkfx_off(priv->device, &error);
	if (error) {
		g_warning(_("Could not deactivate Talk FX: %s"), error->message);
		g_clear_error(&error);
	}

	return retval;
}

static guint kova2016_eventhandler_get_talk_device(RoccatEventhandlerPlugin *self) {
	Kova2016EventhandlerPrivate *priv = KOVA2016_EVENTHANDLER(self)->priv;

	if (!priv->device_set_up)
		return 0;

	return gaminggear_device_get_product_id(GAMINGGEAR_DEVICE(priv->device));
}

static void kova2016_eventhandler_talk_easyshift(RoccatEventhandlerPlugin *self, gboolean state) {
	Kova2016Eventhandler *eventhandler = KOVA2016_EVENTHANDLER(self);
	talk_easyshift(eventhandler, state);
}

static void kova2016_eventhandler_talk_easyshift_lock(RoccatEventhandlerPlugin *self, gboolean state) {
	Kova2016Eventhandler *eventhandler = KOVA2016_EVENTHANDLER(self);
	talk_easyshift_lock(eventhandler, state);
}

static void kova2016_eventhandler_talk_easyaim(RoccatEventhandlerPlugin *self, guchar state) {
	Kova2016Eventhandler *eventhandler = KOVA2016_EVENTHANDLER(self);
	talk_easyaim(eventhandler, state);
}

static void kova2016_eventhandler_talkfx_set_led_rgb(RoccatEventhandlerPlugin *self, guint32 effect, guint32 ambient_color, guint32 event_color) {
	Kova2016Eventhandler *eventhandler = KOVA2016_EVENTHANDLER(self);
	talkfx_set_led_rgb(eventhandler, effect, ambient_color, event_color);
}

static void kova2016_eventhandler_talkfx_restore_led_rgb(RoccatEventhandlerPlugin *self) {
	Kova2016Eventhandler *eventhandler = KOVA2016_EVENTHANDLER(self);
	talkfx_restore_led_rgb(eventhandler);
}

static void talk_easyshift_cb(Kova2016DBusServer *server, guchar state, gpointer user_data) {
	Kova2016Eventhandler *eventhandler = KOVA2016_EVENTHANDLER(user_data);
	talk_easyshift(eventhandler, state);
}

static void talk_easyshift_lock_cb(Kova2016DBusServer *server, guchar state, gpointer user_data) {
	Kova2016Eventhandler *eventhandler = KOVA2016_EVENTHANDLER(user_data);
	talk_easyshift_lock(eventhandler, state);
}

static void talk_easyaim_cb(Kova2016DBusServer *server, guchar state, gpointer user_data) {
	Kova2016Eventhandler *eventhandler = KOVA2016_EVENTHANDLER(user_data);
	talk_easyaim(eventhandler, state);
}

static void talkfx_set_led_rgb_cb(Kova2016DBusServer *server, guint effect, guint ambient_color, guint event_color, gpointer user_data) {
	Kova2016Eventhandler *eventhandler = KOVA2016_EVENTHANDLER(user_data);
	talkfx_set_led_rgb(eventhandler, effect, ambient_color, event_color);
}

static void talkfx_restore_led_rgb_cb(Kova2016DBusServer *server, gpointer user_data) {
	Kova2016Eventhandler *eventhandler = KOVA2016_EVENTHANDLER(user_data);
	talkfx_restore_led_rgb(eventhandler);
}

static void gfx_set_led_rgb_cb(Kova2016DBusServer *server, guint index, guint color, gpointer user_data) {
	Kova2016Eventhandler *eventhandler = KOVA2016_EVENTHANDLER(user_data);
	if (!should_execute_fx(eventhandler))
		return;
	kova2016_gfx_set_color(eventhandler->priv->gfx, index, color);
}

static void gfx_get_led_rgb_cb(Kova2016DBusServer *server, guint index, guint *color, gpointer user_data) {
	Kova2016Eventhandler *eventhandler = KOVA2016_EVENTHANDLER(user_data);
	if (!should_execute_fx(eventhandler))
		return;
	*color = kova2016_gfx_get_color(eventhandler->priv->gfx, index);
}

static void gfx_update_cb(Kova2016DBusServer *server, gpointer user_data) {
	Kova2016Eventhandler *eventhandler = KOVA2016_EVENTHANDLER(user_data);
	if (!should_execute_fx(eventhandler))
		return;
	(void)kova2016_gfx_update(eventhandler->priv->gfx, NULL);
}

static void open_gui_cb(gpointer source, gpointer user_data) {
	gchar *path;
	path = g_build_path("/", INSTALL_PREFIX, "bin", "roccatkova2016config", (gchar const *)NULL);
	roccat_double_fork(path);
	g_free(path);
}

static Kova2016ProfileData *single_profile_data_load(RoccatDevice *device, guint profile_index) {
	Kova2016ProfileData *profile_data;
	GError *local_error = NULL;

	profile_data = kova2016_profile_data_new();

	if (!kova2016_profile_data_update_filesystem(profile_data, profile_index, &local_error)) {
		g_warning(_("Could not load filesystem data for profile %i: %s"), profile_index, local_error->message);
		g_clear_error(&local_error);
	}

	if (!kova2016_profile_data_update_hardware(profile_data, device, profile_index, &local_error)) {
		g_warning(_("Could not load hardware data for profile %i: %s"), profile_index, local_error->message);
		g_clear_error(&local_error);
	}

	return profile_data;
}

static void profile_data_changed_outside_cb(Kova2016DBusServer *server, guchar profile_number, gpointer user_data) {
	Kova2016Eventhandler *eventhandler = KOVA2016_EVENTHANDLER(user_data);
	Kova2016EventhandlerPrivate *priv = eventhandler->priv;
	guint profile_index = profile_number - 1;

	g_free(priv->profile_data[profile_index]);
	priv->profile_data[profile_index] = single_profile_data_load(priv->device, profile_index);
}

static void configuration_reload(Kova2016Eventhandler *eventhandler) {
	Kova2016EventhandlerPrivate *priv = eventhandler->priv;
	if (priv->config)
		kova2016_configuration_free(priv->config);
	priv->config = kova2016_configuration_load();
}

static void configuration_changed_outside_cb(Kova2016DBusServer *server, gpointer user_data) {
	Kova2016Eventhandler *eventhandler = KOVA2016_EVENTHANDLER(user_data);
	configuration_reload(eventhandler);
}

static void cpi_changed_cb(Kova2016EventhandlerChannel *channel, guchar cpi_index, gpointer user_data) {
	Kova2016EventhandlerPrivate *priv = KOVA2016_EVENTHANDLER(user_data)->priv;
	Kova2016ProfileDataEventhandler const *profile_data_eventhandler = &priv->profile_data[priv->actual_profile_index]->eventhandler;
	guint cpi = kova2016_profile_settings_cpi_level_to_cpi(priv->profile_data[priv->actual_profile_index]->hardware.profile_settings.cpi_levels[cpi_index]);

	roccat_notification_cpi_update(priv->cpi_note,
			profile_data_eventhandler->cpi_notification_type,
			profile_data_eventhandler->notification_volume,
			cpi, cpi);
}

static void profile_changed_cb(gpointer source, guchar profile_number, gpointer user_data) {
	Kova2016Eventhandler *eventhandler = KOVA2016_EVENTHANDLER(user_data);
	profile_changed(eventhandler, profile_number);
}

static void open_application_cb(Kova2016EventhandlerChannel *channel, guchar key_index, gpointer user_data) {
	Kova2016Eventhandler *eventhandler = KOVA2016_EVENTHANDLER(user_data);
	Kova2016EventhandlerPrivate *priv = eventhandler->priv;

	roccat_double_fork(priv->profile_data[priv->actual_profile_index]->eventhandler.openers[key_index]);
}

static void open_cb(Kova2016EventhandlerChannel *channel, guchar subtype, guchar key_index, gpointer user_data) {
	Kova2016EventhandlerPrivate *priv = KOVA2016_EVENTHANDLER(user_data)->priv;
	Kova2016ProfileDataEventhandler *eventhandler = &priv->profile_data[priv->actual_profile_index]->eventhandler;

	switch(subtype) {
	case KOVA2016_SPECIAL_SUBTYPE_OPEN_DOCUMENT:
	case KOVA2016_SPECIAL_SUBTYPE_OPEN_FOLDER:
	case KOVA2016_SPECIAL_SUBTYPE_OPEN_WEBSITE:
		roccat_open_url(eventhandler->openers[key_index]);
		break;
	default:
		g_warning(_("Unknown open subtype %i"), subtype);
		break;
	}
}

static void easyshift_cb(Kova2016EventhandlerChannel *channel, guchar key_index, guchar action, gpointer user_data) {
	Kova2016Eventhandler *eventhandler = KOVA2016_EVENTHANDLER(user_data);
	Kova2016EventhandlerPrivate *priv = eventhandler->priv;
	Kova2016ProfileData *profile_data = priv->profile_data[priv->actual_profile_index];
	static gboolean state = FALSE;

	if (action == KOVA2016_SPECIAL_ACTION_PRESS) {
		priv->active_talk_type = profile_data->hardware.profile_buttons.buttons[key_index].type;
		priv->active_talk_target = profile_data->eventhandler.talk_targets[key_index];
	}
	
	switch (priv->active_talk_type) {
	case KOVA2016_BUTTON_TYPE_EASYSHIFT_ALL:
		roccat_eventhandler_plugin_emit_talk_easyshift(ROCCAT_EVENTHANDLER_PLUGIN(eventhandler),
				ROCCAT_TALK_DEVICE_KEYBOARD,
				(action == KOVA2016_SPECIAL_ACTION_PRESS) ? TRUE : FALSE);
		break;
	case KOVA2016_BUTTON_TYPE_EASYSHIFT_OTHER:
		roccat_eventhandler_plugin_emit_talk_easyshift(ROCCAT_EVENTHANDLER_PLUGIN(eventhandler),
				priv->active_talk_target,
				(action == KOVA2016_SPECIAL_ACTION_PRESS) ? TRUE : FALSE);
		break;
	case KOVA2016_BUTTON_TYPE_EASYSHIFT_LOCK_OTHER:
		if (action == KOVA2016_SPECIAL_ACTION_PRESS) {
			state = !state;
			roccat_eventhandler_plugin_emit_talk_easyshift_lock(ROCCAT_EVENTHANDLER_PLUGIN(eventhandler),
					priv->active_talk_target,
					state);
		}
		break;
	default:
		g_debug(_("Got unknown Talk event type 0x%02x"), priv->active_talk_type);
	}

	if (action == KOVA2016_SPECIAL_ACTION_RELEASE) {
		priv->active_talk_type = KOVA2016_BUTTON_TYPE_DISABLED;
		priv->active_talk_target = 0;
	}
}

static void timer_start_cb(Kova2016EventhandlerChannel *channel, guchar key_index, gpointer user_data) {
	Kova2016Eventhandler *eventhandler = KOVA2016_EVENTHANDLER(user_data);
	Kova2016EventhandlerPrivate *priv = eventhandler->priv;
	Kova2016ProfileDataEventhandler const *profile_data_eventhandler = &priv->profile_data[priv->actual_profile_index]->eventhandler;

	roccat_notification_timer_start(priv->timer_note,
			profile_data_eventhandler->timer_notification_type,
			profile_data_eventhandler->notification_volume,
			(gchar const *)profile_data_eventhandler->timers[key_index].name,
			roccat_timer_get_seconds(&profile_data_eventhandler->timers[key_index]));
}

static void timer_stop_cb(Kova2016EventhandlerChannel *channel, gpointer user_data) {
	Kova2016Eventhandler *eventhandler = KOVA2016_EVENTHANDLER(user_data);
	Kova2016EventhandlerPrivate *priv = eventhandler->priv;

	roccat_notification_timer_stop(priv->timer_note);
}

static void profile_data_load(Kova2016Eventhandler *eventhandler, guint count) {
	Kova2016EventhandlerPrivate *priv = eventhandler->priv;
	guint i;

	for (i = 0; i < count; ++i)
		priv->profile_data[i] = single_profile_data_load(priv->device, i);
}

static void device_add_cb(RoccatDeviceScannerInterface *interface, RoccatDevice *device, gpointer user_data) {
	Kova2016Eventhandler *eventhandler = KOVA2016_EVENTHANDLER(user_data);
	Kova2016EventhandlerPrivate *priv = eventhandler->priv;
	GError *local_error = NULL;
	guint driver_state;
	Kova2016Profile *profile;

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
		if (!kova2016_device_state_write(priv->device, (driver_state == ROCCATEVENTHANDLER_CONFIGURATION_DRIVER_STATE_ON) ? KOVA2016_DEVICE_STATE_STATE_ON : KOVA2016_DEVICE_STATE_STATE_OFF, &local_error)) {
			g_warning(_("Could not correct driver state: %s"), local_error->message);
			g_clear_error(&local_error);
		}
	}

	/* Notificator gets initialized only once if a device is found.
	 * Gets deinited once in finalizer.
	 */
	if (!priv->profile_note)
		priv->profile_note = roccat_notification_profile_new(roccat_eventhandler_host_get_notificator(priv->host), KOVA2016_DEVICE_NAME);
	if (!priv->cpi_note)
		priv->cpi_note = roccat_notification_cpi_new(roccat_eventhandler_host_get_notificator(priv->host), KOVA2016_DEVICE_NAME);
	if (!priv->timer_note)
		priv->timer_note = roccat_notification_timer_new(roccat_eventhandler_host_get_notificator(priv->host), KOVA2016_DEVICE_NAME);

	profile = kova2016_profile_read(priv->device, &local_error);
	if (local_error) {
		g_warning(_("Could not read actual profile: %s"), local_error->message);
		g_clear_error(&local_error);
		priv->actual_profile_index = 0;
		priv->actual_profile_count = 5;
	} else {
		priv->actual_profile_index = profile->index;
		priv->actual_profile_count = MAX(1, profile->count);
		g_free(profile);
	}
	profile_data_load(eventhandler, priv->actual_profile_count);

	priv->gfx = kova2016_gfx_new(priv->device);

	// FIXME create in init and just disconnect/reconnect?
	priv->dbus_server = kova2016_dbus_server_new();
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
	kova2016_dbus_server_connect(priv->dbus_server);

	priv->active_window_changed_handler = g_signal_connect(G_OBJECT(priv->host), "active-window-changed", G_CALLBACK(window_changed_cb), eventhandler);

	kova2016_eventhandler_channel_start(priv->channel, priv->device, &local_error);
	if (local_error) {
		g_warning(_("Could not start chardev eventhandler: %s"), local_error->message);
		g_clear_error(&local_error);
	}

	priv->active_talk_type = KOVA2016_BUTTON_TYPE_DISABLED;
	priv->active_talk_target = 0;

	priv->device_set_up = TRUE;

	roccat_eventhandler_plugin_emit_device_added(ROCCAT_EVENTHANDLER_PLUGIN(eventhandler), gaminggear_device_get_product_id(GAMINGGEAR_DEVICE(priv->device)));
}

static void stop(Kova2016Eventhandler *eventhandler) {
	Kova2016EventhandlerPrivate *priv = eventhandler->priv;
	guint i;

	roccat_eventhandler_plugin_emit_device_removed(ROCCAT_EVENTHANDLER_PLUGIN(eventhandler), gaminggear_device_get_product_id(GAMINGGEAR_DEVICE(priv->device)));

	priv->device_set_up = FALSE;

	g_clear_object(&priv->dbus_server);
	kova2016_eventhandler_channel_stop(priv->channel);
	g_signal_handler_disconnect(G_OBJECT(priv->host), priv->active_window_changed_handler);
	g_clear_object(&priv->device);
	g_clear_pointer(&priv->config, kova2016_configuration_free);

	for (i = 0; i < KOVA2016_PROFILE_NUM; ++i)
		g_clear_pointer(&priv->profile_data[i], g_free);

	g_clear_object(&priv->gfx);
}

static void device_remove_cb(RoccatDeviceScanner *scanner, RoccatDevice *device, gpointer user_data) {
	Kova2016Eventhandler *eventhandler = KOVA2016_EVENTHANDLER(user_data);
	Kova2016EventhandlerPrivate *priv = eventhandler->priv;
	if (priv->device && gaminggear_device_equal(GAMINGGEAR_DEVICE(device), GAMINGGEAR_DEVICE(priv->device)))
		stop(eventhandler);
}

static void kova2016_eventhandler_start(RoccatEventhandlerPlugin *self, RoccatEventhandlerHost *host) {
	Kova2016Eventhandler *eventhandler = KOVA2016_EVENTHANDLER(self);
	Kova2016EventhandlerPrivate *priv = eventhandler->priv;

	priv->host = host;
	g_object_ref(priv->host);

	priv->channel = kova2016_eventhandler_channel_new();
	g_signal_connect(G_OBJECT(priv->channel), "profile-changed", G_CALLBACK(profile_changed_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->channel), "open-application", G_CALLBACK(open_application_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->channel), "timer-start", G_CALLBACK(timer_start_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->channel), "timer-stop", G_CALLBACK(timer_stop_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->channel), "open-driver", G_CALLBACK(open_gui_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->channel), "cpi-changed", G_CALLBACK(cpi_changed_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->channel), "open", G_CALLBACK(open_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->channel), "easyshift", G_CALLBACK(easyshift_cb), eventhandler);

	priv->device_scanner = kova2016_device_scanner_new();
	g_signal_connect(G_OBJECT(priv->device_scanner), "device-added", G_CALLBACK(device_add_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->device_scanner), "device-removed", G_CALLBACK(device_remove_cb), eventhandler);
	roccat_device_scanner_interface_start(ROCCAT_DEVICE_SCANNER_INTERFACE(priv->device_scanner));
}

static void kova2016_eventhandler_stop(RoccatEventhandlerPlugin *self) {
	Kova2016Eventhandler *eventhandler = KOVA2016_EVENTHANDLER(self);
	Kova2016EventhandlerPrivate *priv = eventhandler->priv;

	stop(eventhandler);

	g_clear_object(&priv->device_scanner);
	g_clear_object(&priv->channel);
	g_clear_object(&priv->host);
}

static void kova2016_roccat_eventhandler_interface_init(RoccatEventhandlerPluginInterface *iface) {
	iface->start = kova2016_eventhandler_start;
	iface->stop = kova2016_eventhandler_stop;
	iface->get_talk_device = kova2016_eventhandler_get_talk_device;
	iface->talk_easyshift = kova2016_eventhandler_talk_easyshift;
	iface->talk_easyshift_lock = kova2016_eventhandler_talk_easyshift_lock;
	iface->talk_easyaim = kova2016_eventhandler_talk_easyaim;
	iface->talkfx_set_led_rgb = kova2016_eventhandler_talkfx_set_led_rgb;
	iface->talkfx_restore_led_rgb = kova2016_eventhandler_talkfx_restore_led_rgb;
	iface->talkfx_ryos_set_sdk_mode = NULL;
	iface->talkfx_ryos_set_all_leds = NULL;
	iface->talkfx_ryos_turn_on_all_leds = NULL;
	iface->talkfx_ryos_turn_off_all_leds = NULL;
	iface->talkfx_ryos_set_led_on = NULL;
	iface->talkfx_ryos_set_led_off = NULL;
	iface->talkfx_ryos_all_key_blinking = NULL;
}

G_DEFINE_TYPE_WITH_CODE(Kova2016Eventhandler, kova2016_eventhandler, G_TYPE_OBJECT,
		G_IMPLEMENT_INTERFACE(ROCCAT_EVENTHANDLER_PLUGIN_TYPE, kova2016_roccat_eventhandler_interface_init));

static void kova2016_eventhandler_init(Kova2016Eventhandler *eventhandler) {
	eventhandler->priv = KOVA2016_EVENTHANDLER_GET_PRIVATE(eventhandler);
}

static void dispose(GObject *object) {
	kova2016_eventhandler_stop(ROCCAT_EVENTHANDLER_PLUGIN(object));

	G_OBJECT_CLASS(kova2016_eventhandler_parent_class)->dispose(object);
}

static void finalize(GObject *object) {
	Kova2016EventhandlerPrivate *priv = KOVA2016_EVENTHANDLER(object)->priv;

	g_clear_pointer(&priv->timer_note, roccat_notification_timer_free);
	g_clear_pointer(&priv->profile_note, roccat_notification_profile_free);
	g_clear_pointer(&priv->cpi_note, roccat_notification_cpi_free);

	G_OBJECT_CLASS(kova2016_eventhandler_parent_class)->finalize(object);
}

static void kova2016_eventhandler_class_init(Kova2016EventhandlerClass *klass) {
	GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
	gobject_class->dispose = dispose;
	gobject_class->finalize = finalize;

	g_type_class_add_private(klass, sizeof(Kova2016EventhandlerPrivate));
}

ROCCAT_EVENTHANDLER_PLUGIN_API RoccatEventhandlerPlugin *roccat_eventhandler_plugin_new(void) {
	return ROCCAT_EVENTHANDLER_PLUGIN(g_object_new(KOVA2016_EVENTHANDLER_TYPE, NULL));
}
