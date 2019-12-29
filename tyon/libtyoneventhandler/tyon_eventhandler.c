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

#include "tyon_eventhandler_channel.h"
#include "tyon_dbus_server.h"
#include "tyon_gfx.h"
#include "roccat_eventhandler_plugin.h"
#include "roccat_notification_cpi.h"
#include "roccat_notification_profile.h"
#include "roccat_notification_sensitivity.h"
#include "roccat_notification_timer.h"
#include "roccat_process_helper.h"
#include "roccat_talk.h"
#include "tyon.h"
#include "tyon_config.h"
#include "tyon_device.h"
#include "tyon_device_state.h"
#include "tyon_profile.h"
#include "tyon_profile_buttons.h"
#include "tyon_profile_settings.h"
#include "tyon_rmp.h"
#include "tyon_talk.h"
#include "config.h"
#include "i18n-lib.h"
#include "roccat.h"
#include "g_roccat_helper.h"
#include <glib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "../libroccattyon/tyon_special.h"

GType tyon_eventhandler_get_type(void);

#define TYON_EVENTHANDLER_TYPE (tyon_eventhandler_get_type())
#define TYON_EVENTHANDLER(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), TYON_EVENTHANDLER_TYPE, TyonEventhandler))
#define IS_TYON_EVENTHANDLER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), TYON_EVENTHANDLER_TYPE))
#define TYON_EVENTHANDLER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), TYON_EVENTHANDLER_TYPE, TyonEventhandlerClass))
#define IS_TYON_EVENTHANDLER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), TYON_EVENTHANDLER_TYPE))
#define TYON_EVENTHANDLER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), TYON_EVENTHANDLER_TYPE, TyonEventhandlerPrivate))

typedef struct _TyonEventhandler TyonEventhandler;
typedef struct _TyonEventhandlerClass TyonEventhandlerClass;
typedef struct _TyonEventhandlerPrivate TyonEventhandlerPrivate;

struct _TyonEventhandler {
	GObject parent;
	TyonEventhandlerPrivate *priv;
};

struct _TyonEventhandlerClass {
	GObjectClass parent_class;
};

struct _TyonEventhandlerPrivate {
	RoccatEventhandlerHost *host;
	TyonDBusServer *dbus_server;
	RoccatDeviceScanner *device_scanner;
	RoccatDevice *device;
	gboolean device_set_up;
	RoccatKeyFile *config;
	guint actual_profile_index;
	guint actual_sensitivity_x;
	TyonRmp *rmp[TYON_PROFILE_NUM];
	TyonGfx *gfx;
	TyonEventhandlerChannel *channel;
	gulong active_window_changed_handler;

	/*
	 * FIXME notificator für Fehlermeldungen verwenden,
	 * notificator refaktorisieren und Basis für beides verwenden oder
	 * Eigene Implementierung eines Fehlernotificators
	 */
	RoccatNotificationProfile *profile_note;
	RoccatNotificationCpi *cpi_note;
	RoccatNotificationSensitivity *sensitivity_note;
	RoccatNotificationTimer *timer_note;

	guint8 active_talk_type;
	guint16 active_talk_device;
};

static void profile_changed(TyonEventhandler *eventhandler, guint profile_number) {
	TyonEventhandlerPrivate *priv = eventhandler->priv;
	guint profile_index = profile_number - 1;

	priv->actual_profile_index = profile_index;

	roccat_notification_profile_update(priv->profile_note, tyon_configuration_get_profile_notification_type(priv->config),
			tyon_configuration_get_notification_volume(priv->config),
			profile_number, tyon_rmp_get_profile_name(priv->rmp[profile_index]));

	tyon_dbus_server_emit_profile_changed(priv->dbus_server, profile_number);
}

static gboolean set_profile(TyonEventhandler *eventhandler, guint profile_number) {
	TyonEventhandlerPrivate *priv = eventhandler->priv;
	guint profile_index;
	GError *local_error = NULL;

	if (profile_number == 0)
		return TRUE;

	profile_index = profile_number - 1;

	if (priv->actual_profile_index != profile_index) {
		if (!tyon_profile_write(priv->device, profile_index, &local_error)) {
			g_warning(_("Could not activate profile %i: %s"), profile_number, local_error->message);
			g_clear_error(&local_error);
			return FALSE;
		}
		profile_changed(eventhandler, profile_number);
	}
	return TRUE;
}

static void window_changed_cb(RoccatEventhandlerHost *host, gchar const *title, gpointer user_data) {
	TyonEventhandler *eventhandler = TYON_EVENTHANDLER(user_data);
	TyonEventhandlerPrivate *priv = eventhandler->priv;
	guint profile_index;
	guint game_file_index;
	gchar *pattern;

	for (profile_index = 0; profile_index < TYON_PROFILE_NUM; ++profile_index) {

		if (priv->rmp[profile_index] == NULL)
			continue;

		for (game_file_index = 0; game_file_index < TYON_GAMEFILE_NUM; ++game_file_index) {
			pattern = tyon_rmp_get_game_file_name(priv->rmp[profile_index], game_file_index);
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

	set_profile(eventhandler, tyon_configuration_get_default_profile_number(priv->config));
	return;
}

static gboolean talk_easyshift(TyonEventhandler *eventhandler, gboolean state) {
	TyonEventhandlerPrivate *priv = eventhandler->priv;
	GError *error = NULL;
	gboolean retval = FALSE;

	if (priv->device_set_up) {
		retval = tyon_talk_easyshift(priv->device,
				(state != FALSE) ? TYON_TALK_EASYSHIFT_ON : TYON_TALK_EASYSHIFT_OFF,
				&error);
		if (error) {
			g_warning(_("Could not activate easyshift: %s"), error->message);
			g_clear_error(&error);
		}
	}

	return retval;
}

static gboolean talk_easyshift_lock(TyonEventhandler *eventhandler, gboolean state) {
	TyonEventhandlerPrivate *priv = eventhandler->priv;
	GError *error = NULL;
	gboolean retval = FALSE;

	if (priv->device_set_up) {
		retval = tyon_talk_easyshift_lock(priv->device,
				(state != FALSE) ? TYON_TALK_EASYSHIFT_ON : TYON_TALK_EASYSHIFT_OFF,
				&error);
		if (error) {
			g_warning(_("Could not activate easyshift lock: %s"), error->message);
			g_clear_error(&error);
		}
	}

	return retval;
}

static gboolean talk_easyaim(TyonEventhandler *eventhandler, guchar state) {
	TyonEventhandlerPrivate *priv = eventhandler->priv;
	GError *error = NULL;
	gboolean retval = FALSE;

	if (priv->device_set_up) {
		retval = tyon_talk_easyaim(priv->device, state, &error);
		if (error) {
			g_warning(_("Could not activate easyaim: %s"), error->message);
			g_clear_error(&error);
		}
	}

	return retval;
}

static gboolean should_execute_fx(TyonEventhandler *eventhandler) {
	TyonEventhandlerPrivate *priv = eventhandler->priv;
	return tyon_rmp_get_talkfx(priv->rmp[priv->actual_profile_index]) == TYON_PROFILE_SETTINGS_TALKFX_ON;
}

static gboolean talkfx_set_led_rgb(TyonEventhandler *eventhandler, guint32 effect, guint32 ambient_color, guint32 event_color) {
	TyonEventhandlerPrivate *priv = eventhandler->priv;
	GError *error = NULL;
	gboolean retval;

	if (!should_execute_fx(eventhandler))
		return FALSE;

	if (!priv->device_set_up)
		return FALSE;

	retval = tyon_talkfx(priv->device, effect, ambient_color, event_color, &error);
	if (error) {
		g_warning(_("Could not activate Talk FX: %s"), error->message);
		g_clear_error(&error);
	}

	return retval;
}

static gboolean talkfx_restore_led_rgb(TyonEventhandler *eventhandler) {
	TyonEventhandlerPrivate *priv = eventhandler->priv;
	GError *error = NULL;
	gboolean retval;

	if (!should_execute_fx(eventhandler))
		return FALSE;

	if (!priv->device_set_up)
		return FALSE;

	retval = tyon_talkfx_off(priv->device, &error);
	if (error) {
		g_warning(_("Could not deactivate Talk FX: %s"), error->message);
		g_clear_error(&error);
	}

	return retval;
}

static guint tyon_eventhandler_get_talk_device(RoccatEventhandlerPlugin *self) {
	TyonEventhandlerPrivate *priv = TYON_EVENTHANDLER(self)->priv;

	if (!priv->device_set_up)
		return 0;

	return gaminggear_device_get_product_id(GAMINGGEAR_DEVICE(priv->device));
}

static void tyon_eventhandler_talk_easyshift(RoccatEventhandlerPlugin *self, gboolean state) {
	TyonEventhandler *eventhandler = TYON_EVENTHANDLER(self);
	talk_easyshift(eventhandler, state);
}

static void tyon_eventhandler_talk_easyshift_lock(RoccatEventhandlerPlugin *self, gboolean state) {
	TyonEventhandler *eventhandler = TYON_EVENTHANDLER(self);
	talk_easyshift_lock(eventhandler, state);
}

static void tyon_eventhandler_talk_easyaim(RoccatEventhandlerPlugin *self, guchar state) {
	TyonEventhandler *eventhandler = TYON_EVENTHANDLER(self);
	talk_easyaim(eventhandler, state);
}

static void tyon_eventhandler_talkfx_set_led_rgb(RoccatEventhandlerPlugin *self, guint32 effect, guint32 ambient_color, guint32 event_color) {
	TyonEventhandler *eventhandler = TYON_EVENTHANDLER(self);
	talkfx_set_led_rgb(eventhandler, effect, ambient_color, event_color);
}

static void tyon_eventhandler_talkfx_restore_led_rgb(RoccatEventhandlerPlugin *self) {
	TyonEventhandler *eventhandler = TYON_EVENTHANDLER(self);
	talkfx_restore_led_rgb(eventhandler);
}

static void talk_easyshift_cb(TyonDBusServer *server, guchar state, gpointer user_data) {
	TyonEventhandler *eventhandler = TYON_EVENTHANDLER(user_data);
	talk_easyshift(eventhandler, state);
}

static void talk_easyshift_lock_cb(TyonDBusServer *server, guchar state, gpointer user_data) {
	TyonEventhandler *eventhandler = TYON_EVENTHANDLER(user_data);
	talk_easyshift_lock(eventhandler, state);
}

static void talk_easyaim_cb(TyonDBusServer *server, guchar state, gpointer user_data) {
	TyonEventhandler *eventhandler = TYON_EVENTHANDLER(user_data);
	talk_easyaim(eventhandler, state);
}

static void talkfx_set_led_rgb_cb(TyonDBusServer *server, guint effect, guint ambient_color, guint event_color, gpointer user_data) {
	TyonEventhandler *eventhandler = TYON_EVENTHANDLER(user_data);
	talkfx_set_led_rgb(eventhandler, effect, ambient_color, event_color);
}

static void talkfx_restore_led_rgb_cb(TyonDBusServer *server, gpointer user_data) {
	TyonEventhandler *eventhandler = TYON_EVENTHANDLER(user_data);
	talkfx_restore_led_rgb(eventhandler);
}

static void gfx_set_led_rgb_cb(TyonDBusServer *server, guint index, guint color, gpointer user_data) {
	TyonEventhandler *eventhandler = TYON_EVENTHANDLER(user_data);
	if (!should_execute_fx(eventhandler))
		return;
	tyon_gfx_set_color(eventhandler->priv->gfx, index, color);
}

static void gfx_get_led_rgb_cb(TyonDBusServer *server, guint index, guint *color, gpointer user_data) {
	TyonEventhandler *eventhandler = TYON_EVENTHANDLER(user_data);
	if (!should_execute_fx(eventhandler))
		return;
	*color = tyon_gfx_get_color(eventhandler->priv->gfx, index);
}

static void gfx_update_cb(TyonDBusServer *server, gpointer user_data) {
	TyonEventhandler *eventhandler = TYON_EVENTHANDLER(user_data);
	if (!should_execute_fx(eventhandler))
		return;
	(void)tyon_gfx_update(eventhandler->priv->gfx, NULL);
}

static void open_gui_cb(gpointer source, gpointer user_data) {
	gchar *path;
	path = g_build_path("/", INSTALL_PREFIX, "bin", "roccattyonconfig", (gchar const *)NULL);
	roccat_double_fork(path);
	g_free(path);
}

static void profile_data_changed_outside_cb(TyonDBusServer *server, guchar number, gpointer user_data) {
	TyonEventhandler *eventhandler = TYON_EVENTHANDLER(user_data);
	TyonEventhandlerPrivate *priv = eventhandler->priv;
	guint profile_index = number - 1;

	/* Just load rmp without updating data from hardware */
	tyon_rmp_free(priv->rmp[profile_index]);
	priv->rmp[profile_index] = tyon_rmp_load_actual(profile_index);
}

static void configuration_reload(TyonEventhandler *eventhandler) {
	TyonEventhandlerPrivate *priv = eventhandler->priv;
	if (priv->config)
		tyon_configuration_free(priv->config);
	priv->config = tyon_configuration_load();
}

static void configuration_changed_outside_cb(TyonDBusServer *server, gpointer user_data) {
	TyonEventhandler *eventhandler = TYON_EVENTHANDLER(user_data);
	configuration_reload(eventhandler);
}

static void cpi_changed_cb(TyonEventhandlerChannel *channel, guchar cpi_index, gpointer user_data) {
	TyonEventhandler *eventhandler = TYON_EVENTHANDLER(user_data);
	TyonEventhandlerPrivate *priv = eventhandler->priv;
	guint cpi = tyon_rmp_get_cpi_level_x(priv->rmp[priv->actual_profile_index], cpi_index) * 200;

	roccat_notification_cpi_update(priv->cpi_note,
			tyon_configuration_get_cpi_notification_type(priv->config),
			tyon_configuration_get_notification_volume(priv->config),
			cpi, cpi);
}

static void profile_changed_cb(gpointer source, guchar profile_number, gpointer user_data) {
	TyonEventhandler *eventhandler = TYON_EVENTHANDLER(user_data);
	profile_changed(eventhandler, profile_number);
}

static void quicklaunch_cb(TyonEventhandlerChannel *channel, guchar key_index, gpointer user_data) {
	TyonEventhandler *eventhandler = TYON_EVENTHANDLER(user_data);
	TyonEventhandlerPrivate *priv = eventhandler->priv;
	TyonRmpMacroKeyInfo *macro_key_info;

	macro_key_info = tyon_rmp_get_macro_key_info(priv->rmp[priv->actual_profile_index], key_index);
	if (macro_key_info->type == TYON_BUTTON_TYPE_QUICKLAUNCH)
		roccat_double_fork((gchar const *)macro_key_info->filename);
	tyon_rmp_macro_key_info_free(macro_key_info);
}

static void sensitivity_changed_cb(TyonEventhandlerChannel *channel, guchar x, guchar y, gpointer user_data) {
	TyonEventhandler *eventhandler = TYON_EVENTHANDLER(user_data);
	TyonEventhandlerPrivate *priv = eventhandler->priv;
	gint notification_type;

	notification_type = tyon_configuration_get_sensitivity_notification_type(priv->config);

	if (notification_type == ROCCAT_NOTIFICATION_TYPE_OSD)
		roccat_notification_sensitivity_update(priv->sensitivity_note, notification_type, roccat_sensitivity_from_bin_to_value(x), roccat_sensitivity_from_bin_to_value(y));
	else if (x > priv->actual_sensitivity_x)
		roccat_notification_sensitivity_update_up(priv->sensitivity_note, notification_type,
				tyon_configuration_get_notification_volume(priv->config));
	else
		roccat_notification_sensitivity_update_down(priv->sensitivity_note, notification_type,
				tyon_configuration_get_notification_volume(priv->config));

	priv->actual_sensitivity_x = x;
}

static void talk_cb(TyonEventhandlerChannel *channel, guchar key_index, guchar event, gpointer user_data) {
	TyonEventhandler *eventhandler = TYON_EVENTHANDLER(user_data);
	TyonEventhandlerPrivate *priv = eventhandler->priv;
	TyonRmpMacroKeyInfo *macro_key_info;
	static gboolean state = FALSE;

	if (event == TYON_SPECIAL_ACTION_PRESS) {
		macro_key_info = tyon_rmp_get_macro_key_info(priv->rmp[priv->actual_profile_index], key_index);
		priv->active_talk_type = macro_key_info->type;
		priv->active_talk_device = tyon_rmp_macro_key_info_get_talk_device(macro_key_info);
		tyon_rmp_macro_key_info_free(macro_key_info);
	}

	switch (priv->active_talk_type) {
	case TYON_BUTTON_TYPE_EASYSHIFT_SELF:
		break;
	case TYON_BUTTON_TYPE_EASYSHIFT_ALL:
		roccat_eventhandler_plugin_emit_talk_easyshift(ROCCAT_EVENTHANDLER_PLUGIN(eventhandler),
				ROCCAT_TALK_DEVICE_KEYBOARD,
				(event == TYON_SPECIAL_ACTION_PRESS) ? TRUE : FALSE);
		break;
	case TYON_BUTTON_TYPE_EASYSHIFT_OTHER:
		roccat_eventhandler_plugin_emit_talk_easyshift(ROCCAT_EVENTHANDLER_PLUGIN(eventhandler),
				priv->active_talk_device,
				(event == TYON_SPECIAL_ACTION_PRESS) ? TRUE : FALSE);
		break;
	case TYON_BUTTON_TYPE_EASYSHIFT_LOCK_OTHER:
		if (event == TYON_SPECIAL_ACTION_PRESS) {
			state = !state;
			roccat_eventhandler_plugin_emit_talk_easyshift_lock(ROCCAT_EVENTHANDLER_PLUGIN(eventhandler),
					priv->active_talk_device,
					state);
		}
		break;
	default:
		g_debug(_("Got unknown Talk event type 0x%02x"), priv->active_talk_type);
	}

	if (event == TYON_SPECIAL_ACTION_RELEASE)
		priv->active_talk_type = TYON_BUTTON_TYPE_DISABLED;
}

static void timer_start_cb(TyonEventhandlerChannel *channel, guchar key_index, gpointer user_data) {
	TyonEventhandler *eventhandler = TYON_EVENTHANDLER(user_data);
	TyonEventhandlerPrivate *priv = eventhandler->priv;
	TyonRmpMacroKeyInfo *macro_key_info;

	macro_key_info = tyon_rmp_get_macro_key_info(priv->rmp[priv->actual_profile_index], key_index);
	roccat_notification_timer_start(priv->timer_note,
			tyon_configuration_get_timer_notification_type(priv->config),
			tyon_configuration_get_notification_volume(priv->config),
			(gchar const *)macro_key_info->timer_name,
			tyon_rmp_macro_key_info_get_timer_length(macro_key_info));
	tyon_rmp_macro_key_info_free(macro_key_info);
}

static void timer_stop_cb(TyonEventhandlerChannel *channel, gpointer user_data) {
	TyonEventhandler *eventhandler = TYON_EVENTHANDLER(user_data);
	TyonEventhandlerPrivate *priv = eventhandler->priv;

	roccat_notification_timer_stop(priv->timer_note);
}

static gboolean profile_data_load(TyonEventhandler *eventhandler, GError **error) {
	TyonEventhandlerPrivate *priv = eventhandler->priv;
	guint i;

	for (i = 0; i < TYON_PROFILE_NUM; ++i) {
		priv->rmp[i] = tyon_rmp_load(priv->device, i, error);
		if (priv->rmp[i] == NULL)
			return FALSE;
	}
	return TRUE;
}

static void device_add_cb(RoccatDeviceScannerInterface *interface, RoccatDevice *device, gpointer user_data) {
	TyonEventhandler *eventhandler = TYON_EVENTHANDLER(user_data);
	TyonEventhandlerPrivate *priv = eventhandler->priv;
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
		if (!tyon_device_state_write(priv->device, (driver_state == ROCCATEVENTHANDLER_CONFIGURATION_DRIVER_STATE_ON) ? TYON_DEVICE_STATE_STATE_ON : TYON_DEVICE_STATE_STATE_OFF, &local_error)) {
			g_warning(_("Could not correct driver state: %s"), local_error->message);
			g_clear_error(&local_error);
		}
	}

	/* Notificator gets initialized only once if a device is found.
	 * Gets deinited once in finalizer.
	 */
	if (!priv->profile_note)
		priv->profile_note = roccat_notification_profile_new(roccat_eventhandler_host_get_notificator(priv->host), TYON_DEVICE_NAME);
	if (!priv->cpi_note)
		priv->cpi_note = roccat_notification_cpi_new(roccat_eventhandler_host_get_notificator(priv->host), TYON_DEVICE_NAME);
	if (!priv->sensitivity_note)
		priv->sensitivity_note = roccat_notification_sensitivity_new(roccat_eventhandler_host_get_notificator(priv->host), TYON_DEVICE_NAME);
	if (!priv->timer_note)
		priv->timer_note = roccat_notification_timer_new(roccat_eventhandler_host_get_notificator(priv->host), TYON_DEVICE_NAME);

	profile_data_load(eventhandler, &local_error);
	if (local_error) {
		g_warning(_("Could not read profile data: %s"), local_error->message);
		g_clear_error(&local_error);
	}

	priv->actual_profile_index = tyon_profile_read(priv->device, &local_error);
	if (local_error) {
		g_warning(_("Could not read actual profile: %s"), local_error->message);
		g_clear_error(&local_error);
		priv->actual_sensitivity_x = (ROCCAT_SENSITIVITY_MIN + ROCCAT_SENSITIVITY_MAX) >> 1;
	} else
		priv->actual_sensitivity_x = tyon_rmp_get_sensitivity_x(priv->rmp[priv->actual_profile_index]);

	priv->gfx = tyon_gfx_new(priv->device);

	// FIXME create in init and just disconnect/reconnect?
	priv->dbus_server = tyon_dbus_server_new();
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
	tyon_dbus_server_connect(priv->dbus_server);

	priv->active_window_changed_handler = g_signal_connect(G_OBJECT(priv->host), "active-window-changed", G_CALLBACK(window_changed_cb), eventhandler);

	tyon_eventhandler_channel_start(priv->channel, priv->device, &local_error);
	if (local_error) {
		g_warning(_("Could not start chardev eventhandler: %s"), local_error->message);
		g_clear_error(&local_error);
	}

	priv->active_talk_type = TYON_BUTTON_TYPE_DISABLED;

	priv->device_set_up = TRUE;

	roccat_eventhandler_plugin_emit_device_added(ROCCAT_EVENTHANDLER_PLUGIN(eventhandler), gaminggear_device_get_product_id(GAMINGGEAR_DEVICE(priv->device)));
}

static void stop(TyonEventhandler *eventhandler) {
	TyonEventhandlerPrivate *priv = eventhandler->priv;
	guint i;

	roccat_eventhandler_plugin_emit_device_removed(ROCCAT_EVENTHANDLER_PLUGIN(eventhandler), gaminggear_device_get_product_id(GAMINGGEAR_DEVICE(priv->device)));

	priv->device_set_up = FALSE;

	g_clear_object(&priv->dbus_server);
	tyon_eventhandler_channel_stop(priv->channel);
	g_signal_handler_disconnect(G_OBJECT(priv->host), priv->active_window_changed_handler);
	g_clear_object(&priv->device);

	g_clear_pointer(&priv->config, tyon_configuration_free);

	for (i = 0; i < TYON_PROFILE_NUM; ++i)
		g_clear_pointer(&priv->rmp[i], tyon_rmp_free);

	g_clear_object(&priv->gfx);
}

static void device_remove_cb(RoccatDeviceScanner *scanner, RoccatDevice *device, gpointer user_data) {
	TyonEventhandler *eventhandler = TYON_EVENTHANDLER(user_data);
	TyonEventhandlerPrivate *priv = eventhandler->priv;
	if (priv->device && gaminggear_device_equal(GAMINGGEAR_DEVICE(device), GAMINGGEAR_DEVICE(priv->device)))
		stop(eventhandler);
}

static void tyon_eventhandler_start(RoccatEventhandlerPlugin *self, RoccatEventhandlerHost *host) {
	TyonEventhandler *eventhandler = TYON_EVENTHANDLER(self);
	TyonEventhandlerPrivate *priv = eventhandler->priv;

	priv->host = host;
	g_object_ref(priv->host);

	priv->channel = tyon_eventhandler_channel_new();
	g_signal_connect(G_OBJECT(priv->channel), "cpi-changed", G_CALLBACK(cpi_changed_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->channel), "open-driver", G_CALLBACK(open_gui_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->channel), "profile-changed", G_CALLBACK(profile_changed_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->channel), "quicklaunch", G_CALLBACK(quicklaunch_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->channel), "sensitivity-changed", G_CALLBACK(sensitivity_changed_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->channel), "talk", G_CALLBACK(talk_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->channel), "timer-start", G_CALLBACK(timer_start_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->channel), "timer-stop", G_CALLBACK(timer_stop_cb), eventhandler);

	priv->device_scanner = tyon_device_scanner_new();
	g_signal_connect(G_OBJECT(priv->device_scanner), "device-added", G_CALLBACK(device_add_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->device_scanner), "device-removed", G_CALLBACK(device_remove_cb), eventhandler);
	roccat_device_scanner_interface_start(ROCCAT_DEVICE_SCANNER_INTERFACE(priv->device_scanner));
}

static void tyon_eventhandler_stop(RoccatEventhandlerPlugin *self) {
	TyonEventhandler *eventhandler = TYON_EVENTHANDLER(self);
	TyonEventhandlerPrivate *priv = eventhandler->priv;

	stop(eventhandler);

	g_clear_object(&priv->device_scanner);
	g_clear_object(&priv->channel);
	g_clear_object(&priv->host);
}

static void tyon_roccat_eventhandler_interface_init(RoccatEventhandlerPluginInterface *iface) {
	iface->start = tyon_eventhandler_start;
	iface->stop = tyon_eventhandler_stop;
	iface->get_talk_device = tyon_eventhandler_get_talk_device;
	iface->talk_easyshift = tyon_eventhandler_talk_easyshift;
	iface->talk_easyshift_lock = tyon_eventhandler_talk_easyshift_lock;
	iface->talk_easyaim = tyon_eventhandler_talk_easyaim;
	iface->talkfx_set_led_rgb = tyon_eventhandler_talkfx_set_led_rgb;
	iface->talkfx_restore_led_rgb = tyon_eventhandler_talkfx_restore_led_rgb;
	iface->talkfx_ryos_set_sdk_mode = NULL;
	iface->talkfx_ryos_set_all_leds = NULL;
	iface->talkfx_ryos_turn_on_all_leds = NULL;
	iface->talkfx_ryos_turn_off_all_leds = NULL;
	iface->talkfx_ryos_set_led_on = NULL;
	iface->talkfx_ryos_set_led_off = NULL;
	iface->talkfx_ryos_all_key_blinking = NULL;
}

G_DEFINE_TYPE_WITH_CODE(TyonEventhandler, tyon_eventhandler, G_TYPE_OBJECT,
		G_IMPLEMENT_INTERFACE(ROCCAT_EVENTHANDLER_PLUGIN_TYPE, tyon_roccat_eventhandler_interface_init));

static void tyon_eventhandler_init(TyonEventhandler *eventhandler) {
	eventhandler->priv = TYON_EVENTHANDLER_GET_PRIVATE(eventhandler);
}

static void dispose(GObject *object) {
	tyon_eventhandler_stop(ROCCAT_EVENTHANDLER_PLUGIN(object));

	G_OBJECT_CLASS(tyon_eventhandler_parent_class)->dispose(object);
}

static void finalize(GObject *object) {
	TyonEventhandlerPrivate *priv = TYON_EVENTHANDLER(object)->priv;

	g_clear_pointer(&priv->timer_note, roccat_notification_timer_free);
	g_clear_pointer(&priv->profile_note, roccat_notification_profile_free);
	g_clear_pointer(&priv->cpi_note, roccat_notification_cpi_free);
	g_clear_pointer(&priv->sensitivity_note, roccat_notification_sensitivity_free);

	G_OBJECT_CLASS(tyon_eventhandler_parent_class)->finalize(object);
}

static void tyon_eventhandler_class_init(TyonEventhandlerClass *klass) {
	GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
	gobject_class->dispose = dispose;
	gobject_class->finalize = finalize;

	g_type_class_add_private(klass, sizeof(TyonEventhandlerPrivate));
}

ROCCAT_EVENTHANDLER_PLUGIN_API RoccatEventhandlerPlugin *roccat_eventhandler_plugin_new(void) {
	return ROCCAT_EVENTHANDLER_PLUGIN(g_object_new(TYON_EVENTHANDLER_TYPE, NULL));
}
