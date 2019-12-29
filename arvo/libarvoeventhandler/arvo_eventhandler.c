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

#include <gaminggear/macro_threads.h>
#include <gaminggear/input_events.h>
#include <gaminggear/hid_uid.h>
#include "roccat_eventhandler_plugin.h"
#include "arvo_eventhandler_channel.h"
#include "arvo_dbus_server.h"
#include "roccat_notification_profile.h"
#include "roccat_notification_timer.h"
#include "roccat_process_helper.h"
#include "roccat_talk.h"
#include "arvo.h"
#include "config.h"
#include "i18n-lib.h"
#include "g_roccat_helper.h"
#include <glib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <linux/input.h>

GType arvo_eventhandler_get_type(void);

#define ARVO_EVENTHANDLER_TYPE (arvo_eventhandler_get_type())
#define ARVO_EVENTHANDLER(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), ARVO_EVENTHANDLER_TYPE, ArvoEventhandler))
#define IS_ARVO_EVENTHANDLER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), ARVO_EVENTHANDLER_TYPE))
#define ARVO_EVENTHANDLER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), ARVO_EVENTHANDLER_TYPE, ArvoEventhandlerClass))
#define IS_ARVO_EVENTHANDLER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), ARVO_EVENTHANDLER_TYPE))
#define ARVO_EVENTHANDLER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), ARVO_EVENTHANDLER_TYPE, ArvoEventhandlerPrivate))

typedef struct _ArvoEventhandler ArvoEventhandler;
typedef struct _ArvoEventhandlerClass ArvoEventhandlerClass;
typedef struct _ArvoEventhandlerPrivate ArvoEventhandlerPrivate;

struct _ArvoEventhandler {
	GObject parent;
	ArvoEventhandlerPrivate *priv;
};

struct _ArvoEventhandlerClass {
	GObjectClass parent_class;
};

struct _ArvoEventhandlerPrivate {
	RoccatEventhandlerHost *host;
	ArvoDBusServer *dbus_server;
	RoccatDeviceScanner *device_scanner;
	RoccatDevice *device;
	gboolean device_set_up;
	RoccatKeyFile *config;
	guint actual_profile_index;
	ArvoRkp *rkp[ARVO_PROFILE_NUM];
	ArvoEventhandlerChannel *channel;
	gulong active_window_changed_handler;

	RoccatNotificationProfile *profile_note;
	RoccatNotificationTimer *timer_note;

	GaminggearMacroThreads *threads;
};

static void profile_changed(ArvoEventhandler *eventhandler, guint profile_number) {
	ArvoEventhandlerPrivate *priv = eventhandler->priv;
	guint profile_index = profile_number - 1;

	priv->actual_profile_index = profile_index;

	roccat_notification_profile_update(priv->profile_note, arvo_configuration_get_profile_notification_type(priv->config),
			arvo_configuration_get_notification_volume(priv->config),
			profile_number, arvo_rkp_get_profile_name(priv->rkp[profile_index]));

	arvo_dbus_server_emit_profile_changed(priv->dbus_server, profile_number);
}

static gboolean set_profile(ArvoEventhandler *eventhandler, guint profile_number) {
	ArvoEventhandlerPrivate *priv = eventhandler->priv;
	guint profile_index;
	GError *local_error = NULL;

	if (profile_number == 0)
		return TRUE;

	profile_index = profile_number - 1;

	if (priv->actual_profile_index != profile_index) {
		if (!arvo_actual_profile_write(priv->device, profile_number, &local_error)) {
			g_warning(_("Could not activate profile %i: %s"), profile_number, local_error->message);
			g_clear_error(&local_error);
			return FALSE;
		}
		profile_changed(eventhandler, profile_number);
	}
	return TRUE;
}

static void window_changed_cb(RoccatEventhandlerHost *host, gchar const *title, gpointer user_data) {
	ArvoEventhandler *eventhandler = ARVO_EVENTHANDLER(user_data);
	ArvoEventhandlerPrivate *priv = eventhandler->priv;
	guint profile_index;
	guint game_file_index;
	gchar *pattern;

	for (profile_index = 0; profile_index < ARVO_PROFILE_NUM; ++profile_index) {

		if (priv->rkp[profile_index] == NULL)
			continue;

		for (game_file_index = 0; game_file_index < ARVO_GAMEFILE_NUM; ++game_file_index) {
			pattern = arvo_rkp_get_game_file_name(priv->rkp[profile_index], game_file_index);
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

	set_profile(eventhandler, arvo_configuration_get_default_profile_number(priv->config));
	return;
}

static guint arvo_eventhandler_get_talk_device(RoccatEventhandlerPlugin *self) {
	return 0;
}

static void open_gui(void) {
	gchar *path;
	path = g_build_path("/", INSTALL_PREFIX, "bin", "roccatarvoconfig", (gchar const *)NULL);
	roccat_double_fork(path);
	g_free(path);
}

static void open_gui_cb(gpointer source, gpointer user_data) {
	open_gui();
}

static void profile_data_changed_outside_cb(ArvoDBusServer *server, guchar profile_number, gpointer user_data) {
	ArvoEventhandler *eventhandler = ARVO_EVENTHANDLER(user_data);
	ArvoEventhandlerPrivate *priv = eventhandler->priv;
	guint profile_index = profile_number - 1;
	
	arvo_rkp_free(priv->rkp[profile_index]);
	priv->rkp[profile_index] = arvo_rkp_load_actual(profile_number);
}

static void configuration_reload(ArvoEventhandler *eventhandler) {
	ArvoEventhandlerPrivate *priv = eventhandler->priv;
	if (priv->config)
		arvo_configuration_free(priv->config);
	priv->config = arvo_configuration_load();
}

static void configuration_changed_outside_cb(ArvoDBusServer *server, gpointer user_data) {
	ArvoEventhandler *eventhandler = ARVO_EVENTHANDLER(user_data);
	configuration_reload(eventhandler);
}

static void profile_changed_cb(gpointer source, guchar profile_number, gpointer user_data) {
	ArvoEventhandler *eventhandler = ARVO_EVENTHANDLER(user_data);
	profile_changed(eventhandler, profile_number);
}

static gboolean arvo_profile_increment(ArvoEventhandler *eventhandler, guint profile_index) {
	++profile_index;
	if (profile_index >= ARVO_PROFILE_NUM)
		return TRUE;
	return set_profile(eventhandler, profile_index + 1);
}

static gboolean arvo_profile_decrement(ArvoEventhandler *eventhandler, guint profile_index) {
	if (profile_index == 0)
		return TRUE;
	--profile_index;
	return set_profile(eventhandler, profile_index + 1);
}

static gboolean arvo_profile_cycle_up(ArvoEventhandler *eventhandler, guint profile_index) {
	++profile_index;
	if (profile_index >= ARVO_PROFILE_NUM)
		profile_index = 0;

	return set_profile(eventhandler, profile_index + 1);
}

static void button_cb(ArvoEventhandlerChannel *channel, guchar action, guchar button, gpointer user_data) {
	ArvoEventhandler *eventhandler = ARVO_EVENTHANDLER(user_data);
	ArvoEventhandlerPrivate *priv = eventhandler->priv;
	ArvoRkpButtonInfo *button_info;
	GaminggearMacro *gaminggear_macro;
	static gboolean state = FALSE;

	button_info = arvo_rkp_get_button_info(priv->rkp[priv->actual_profile_index], button - 1);

	switch (button_info->type) {
	case ARVO_RKP_BUTTON_INFO_TYPE_MULTIMEDIA_PREV_TRACK:
		if (action == ARVO_SPECIAL_ACTION_PRESS)
			gaminggear_input_event_write_multimedia_single(HID_UID_CP_PREVIOUSSONG);
		break;
	case ARVO_RKP_BUTTON_INFO_TYPE_MULTIMEDIA_NEXT_TRACK:
		if (action == ARVO_SPECIAL_ACTION_PRESS)
			gaminggear_input_event_write_multimedia_single(HID_UID_CP_NEXTSONG);
		break;
	case ARVO_RKP_BUTTON_INFO_TYPE_MULTIMEDIA_PLAY_PAUSE:
		if (action == ARVO_SPECIAL_ACTION_PRESS)
			gaminggear_input_event_write_multimedia_single(HID_UID_CP_PLAYPAUSE);
		break;
	case ARVO_RKP_BUTTON_INFO_TYPE_MULTIMEDIA_STOP:
		if (action == ARVO_SPECIAL_ACTION_PRESS)
			gaminggear_input_event_write_multimedia_single(HID_UID_CP_STOP);
		break;
	case ARVO_RKP_BUTTON_INFO_TYPE_MULTIMEDIA_MUTE:
		if (action == ARVO_SPECIAL_ACTION_PRESS)
			gaminggear_input_event_write_multimedia_single(HID_UID_CP_MUTE);
		break;
	case ARVO_RKP_BUTTON_INFO_TYPE_MULTIMEDIA_VOLUME_UP:
		if (action == ARVO_SPECIAL_ACTION_PRESS)
			gaminggear_input_event_write_multimedia_single(HID_UID_CP_VOLUMEUP);
		break;
	case ARVO_RKP_BUTTON_INFO_TYPE_MULTIMEDIA_VOLUME_DOWN:
		if (action == ARVO_SPECIAL_ACTION_PRESS)
			gaminggear_input_event_write_multimedia_single(HID_UID_CP_VOLUMEDOWN);
		break;
	case ARVO_RKP_BUTTON_INFO_TYPE_OFF:
		/* just ignore */
		break;
	case ARVO_RKP_BUTTON_INFO_TYPE_MACRO:
		if (action == ARVO_SPECIAL_ACTION_PRESS) {
			gaminggear_macro = arvo_rkp_button_info_to_gaminggear_macro(button_info);
			gaminggear_play_macro_threaded(priv->threads, button - 1, gaminggear_macro);
			gaminggear_macro_free(gaminggear_macro);
		}
		break;
	case ARVO_RKP_BUTTON_INFO_TYPE_TIMER_START:
		if (action == ARVO_SPECIAL_ACTION_PRESS)
			roccat_notification_timer_start(priv->timer_note,
					arvo_configuration_get_timer_notification_type(priv->config),
					arvo_configuration_get_notification_volume(priv->config),
					(gchar const *)button_info->timer_name,
					arvo_rkp_button_info_get_timer_length(button_info));
		break;
	case ARVO_RKP_BUTTON_INFO_TYPE_TIMER_STOP:
		if (action == ARVO_SPECIAL_ACTION_PRESS)
			roccat_notification_timer_stop(priv->timer_note);
		break;
	case ARVO_RKP_BUTTON_INFO_TYPE_QUICKLAUNCH_NS:
		if (action == ARVO_SPECIAL_ACTION_PRESS)
			arvo_play_quicklaunch_forked_ns(button_info);
		break;
	case ARVO_RKP_BUTTON_INFO_TYPE_OPEN_DRIVER_NS:
		if (action == ARVO_SPECIAL_ACTION_PRESS)
			open_gui();
		break;
	case ARVO_RKP_BUTTON_INFO_TYPE_PROFILE_CYCLE_NS:
		if (action == ARVO_SPECIAL_ACTION_PRESS)
			arvo_profile_cycle_up(eventhandler, priv->actual_profile_index);
		break;
	case ARVO_RKP_BUTTON_INFO_TYPE_PROFILE_UP_NS:
		if (action == ARVO_SPECIAL_ACTION_PRESS)
			arvo_profile_increment(eventhandler, priv->actual_profile_index);
		break;
	case ARVO_RKP_BUTTON_INFO_TYPE_PROFILE_DOWN_NS:
		if (action == ARVO_SPECIAL_ACTION_PRESS)
			arvo_profile_decrement(eventhandler, priv->actual_profile_index);
		break;
	case ARVO_RKP_BUTTON_INFO_TYPE_TALK_OTHER_EASYSHIFT_NS:
		roccat_eventhandler_plugin_emit_talk_easyshift(ROCCAT_EVENTHANDLER_PLUGIN(eventhandler),
				ROCCAT_TALK_DEVICE_MOUSE,
				(action == ARVO_SPECIAL_ACTION_PRESS) ? TRUE : FALSE);
		break;
	case ARVO_RKP_BUTTON_INFO_TYPE_TALK_OTHER_EASYSHIFT_LOCK_NS:
		if (action == ARVO_SPECIAL_ACTION_PRESS) {
			state = !state;
			roccat_eventhandler_plugin_emit_talk_easyshift(ROCCAT_EVENTHANDLER_PLUGIN(eventhandler),
					ROCCAT_TALK_DEVICE_MOUSE, state);
		}
		break;
	case ARVO_RKP_BUTTON_INFO_TYPE_TALK_OTHER_EASYAIM_1_NS:
		roccat_eventhandler_plugin_emit_talk_easyaim(ROCCAT_EVENTHANDLER_PLUGIN(eventhandler),
				ROCCAT_TALK_DEVICE_MOUSE,
				(action == ARVO_SPECIAL_ACTION_PRESS) ? ROCCAT_TALK_EASYAIM_1 : ROCCAT_TALK_EASYAIM_OFF);
		break;
	case ARVO_RKP_BUTTON_INFO_TYPE_TALK_OTHER_EASYAIM_2_NS:
		roccat_eventhandler_plugin_emit_talk_easyaim(ROCCAT_EVENTHANDLER_PLUGIN(eventhandler),
				ROCCAT_TALK_DEVICE_MOUSE,
				(action == ARVO_SPECIAL_ACTION_PRESS) ? ROCCAT_TALK_EASYAIM_2 : ROCCAT_TALK_EASYAIM_OFF);
		break;
	case ARVO_RKP_BUTTON_INFO_TYPE_TALK_OTHER_EASYAIM_3_NS:
		roccat_eventhandler_plugin_emit_talk_easyaim(ROCCAT_EVENTHANDLER_PLUGIN(eventhandler),
				ROCCAT_TALK_DEVICE_MOUSE,
				(action == ARVO_SPECIAL_ACTION_PRESS) ? ROCCAT_TALK_EASYAIM_3 : ROCCAT_TALK_EASYAIM_OFF);
		break;
	case ARVO_RKP_BUTTON_INFO_TYPE_TALK_OTHER_EASYAIM_4_NS:
		roccat_eventhandler_plugin_emit_talk_easyaim(ROCCAT_EVENTHANDLER_PLUGIN(eventhandler),
				ROCCAT_TALK_DEVICE_MOUSE,
				(action == ARVO_SPECIAL_ACTION_PRESS) ? ROCCAT_TALK_EASYAIM_4 : ROCCAT_TALK_EASYAIM_OFF);
		break;
	case ARVO_RKP_BUTTON_INFO_TYPE_TALK_OTHER_EASYAIM_5_NS:
		roccat_eventhandler_plugin_emit_talk_easyaim(ROCCAT_EVENTHANDLER_PLUGIN(eventhandler),
				ROCCAT_TALK_DEVICE_MOUSE,
				(action == ARVO_SPECIAL_ACTION_PRESS) ? ROCCAT_TALK_EASYAIM_5 : ROCCAT_TALK_EASYAIM_OFF);
		break;
	default:
		g_debug(_("Got unknown event type 0x%02x"), button_info->type);
		break;
	}
	arvo_rkp_button_info_free(button_info);
}

static gboolean profile_data_load(ArvoEventhandler *eventhandler, GError **error) {
	ArvoEventhandlerPrivate *priv = eventhandler->priv;
	guint i;

	for (i = 0; i < ARVO_PROFILE_NUM; ++i) {
		priv->rkp[i] = arvo_rkp_load(priv->device, i + 1, error);
		if (priv->rkp[i] == NULL)
			return FALSE;
	}
	return TRUE;
}

static void device_add_cb(RoccatDeviceScannerInterface *interface, RoccatDevice *device, gpointer user_data) {
	ArvoEventhandler *eventhandler = ARVO_EVENTHANDLER(user_data);
	ArvoEventhandlerPrivate *priv = eventhandler->priv;
	GError *error = NULL;

	if (priv->device) {
		g_critical(_("Multiple devices present."));
		return;
	}

	priv->device = device;
	g_object_ref(G_OBJECT(device));
	roccat_device_debug(device);

	configuration_reload(eventhandler);

	/* Notificator gets initialized only once if a device is found.
	 * Gets deinited once in finalizer.
	 */
	if (!priv->profile_note)
		priv->profile_note = roccat_notification_profile_new(roccat_eventhandler_host_get_notificator(priv->host), ARVO_DEVICE_NAME);
	if (!priv->timer_note)
		priv->timer_note = roccat_notification_timer_new(roccat_eventhandler_host_get_notificator(priv->host), ARVO_DEVICE_NAME);
	
	/* Opening the files and caching the fd to provide error messages on
	 * startup and increase speed on access.
	 * These still are closed in RoccatDevice finalization.
	 */

	profile_data_load(eventhandler, &error);
	if (error) {
		g_warning(_("Could not read rkp: %s"), error->message);
		g_clear_error(&error);
	}

	priv->actual_profile_index = arvo_actual_profile_read(priv->device, &error) - 1;
	if (error) {
		g_warning(_("Could not read actual profile: %s"), error->message);
		g_clear_error(&error);
	}

	priv->dbus_server = arvo_dbus_server_new();
	g_signal_connect(G_OBJECT(priv->dbus_server), "open-gui", G_CALLBACK(open_gui_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->dbus_server), "profile-changed-outside", G_CALLBACK(profile_changed_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->dbus_server), "profile-data-changed-outside", G_CALLBACK(profile_data_changed_outside_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->dbus_server), "configuration-changed-outside", G_CALLBACK(configuration_changed_outside_cb), eventhandler);
	arvo_dbus_server_connect(priv->dbus_server);

	priv->threads = gaminggear_macro_threads_new(ARVO_BUTTON_NUM);
	
	priv->active_window_changed_handler = g_signal_connect(G_OBJECT(priv->host), "active-window-changed", G_CALLBACK(window_changed_cb), eventhandler);

	arvo_eventhandler_channel_start(priv->channel, priv->device, &error);
	if (error) {
		g_warning(_("Could not start chardev eventhandler: %s"), error->message);
		g_clear_error(&error);
	}
	
	priv->device_set_up = TRUE;

	roccat_eventhandler_plugin_emit_device_added(ROCCAT_EVENTHANDLER_PLUGIN(eventhandler), gaminggear_device_get_product_id(GAMINGGEAR_DEVICE(priv->device)));
}

static void stop(ArvoEventhandler *eventhandler) {
	ArvoEventhandlerPrivate *priv = eventhandler->priv;
	guint i;

	roccat_eventhandler_plugin_emit_device_removed(ROCCAT_EVENTHANDLER_PLUGIN(eventhandler), gaminggear_device_get_product_id(GAMINGGEAR_DEVICE(priv->device)));

	priv->device_set_up = FALSE;

	g_clear_object(&priv->dbus_server);
	arvo_eventhandler_channel_stop(priv->channel);
	g_signal_handler_disconnect(G_OBJECT(priv->host), priv->active_window_changed_handler);
	g_clear_pointer(&priv->threads, gaminggear_macro_threads_free);
	g_clear_object(&priv->device);

	g_clear_pointer(&priv->config, arvo_configuration_free);

	for (i = 0; i < ARVO_PROFILE_NUM; ++i)
		g_clear_pointer(&priv->rkp[i], arvo_rkp_free);
}

static void device_remove_cb(RoccatDeviceScannerInterface *interface, RoccatDevice *device, gpointer user_data) {
	ArvoEventhandler *eventhandler = ARVO_EVENTHANDLER(user_data);
	ArvoEventhandlerPrivate *priv = eventhandler->priv;

	if (priv->device && gaminggear_device_equal(GAMINGGEAR_DEVICE(device), GAMINGGEAR_DEVICE(priv->device)))
		stop(eventhandler);
}

static void arvo_eventhandler_start(RoccatEventhandlerPlugin *self, RoccatEventhandlerHost *host) {
	ArvoEventhandler *eventhandler = ARVO_EVENTHANDLER(self);
	ArvoEventhandlerPrivate *priv = eventhandler->priv;

	priv->host = host;
	g_object_ref(priv->host);

	priv->channel = arvo_eventhandler_channel_new();
	g_signal_connect(G_OBJECT(priv->channel), "button", G_CALLBACK(button_cb), eventhandler);

	priv->device_scanner = arvo_device_scanner_new();
	g_signal_connect(G_OBJECT(priv->device_scanner), "device-added", G_CALLBACK(device_add_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->device_scanner), "device-removed", G_CALLBACK(device_remove_cb), eventhandler);
	roccat_device_scanner_interface_start(ROCCAT_DEVICE_SCANNER_INTERFACE(priv->device_scanner));
}

static void arvo_eventhandler_stop(RoccatEventhandlerPlugin *self) {
	ArvoEventhandler *eventhandler = ARVO_EVENTHANDLER(self);
	ArvoEventhandlerPrivate *priv = eventhandler->priv;

	stop(eventhandler);

	g_clear_object(&priv->device_scanner);
	g_clear_object(&priv->channel);
	g_clear_object(&priv->host);
}

static void arvo_roccat_eventhandler_interface_init(RoccatEventhandlerPluginInterface *iface) {
	iface->start = arvo_eventhandler_start;
	iface->stop = arvo_eventhandler_stop;
	iface->get_talk_device = arvo_eventhandler_get_talk_device;
	iface->talk_easyshift = NULL;
	iface->talk_easyshift_lock = NULL;
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

G_DEFINE_TYPE_WITH_CODE(ArvoEventhandler, arvo_eventhandler, G_TYPE_OBJECT,
		G_IMPLEMENT_INTERFACE(ROCCAT_EVENTHANDLER_PLUGIN_TYPE, arvo_roccat_eventhandler_interface_init));

static void arvo_eventhandler_init(ArvoEventhandler *eventhandler) {
	eventhandler->priv = ARVO_EVENTHANDLER_GET_PRIVATE(eventhandler);
}

static void dispose(GObject *object) {
	arvo_eventhandler_stop(ROCCAT_EVENTHANDLER_PLUGIN(object));

	G_OBJECT_CLASS(arvo_eventhandler_parent_class)->dispose(object);
}

static void finalize(GObject *object) {
	ArvoEventhandlerPrivate *priv = ARVO_EVENTHANDLER(object)->priv;

	g_clear_pointer(&priv->timer_note, roccat_notification_timer_free);
	g_clear_pointer(&priv->profile_note, roccat_notification_profile_free);
	
	G_OBJECT_CLASS(arvo_eventhandler_parent_class)->finalize(object);
}

static void arvo_eventhandler_class_init(ArvoEventhandlerClass *klass) {
	GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
	gobject_class->dispose = dispose;
	gobject_class->finalize = finalize;

	g_type_class_add_private(klass, sizeof(ArvoEventhandlerPrivate));
}

ROCCAT_EVENTHANDLER_PLUGIN_API RoccatEventhandlerPlugin *roccat_eventhandler_plugin_new(void) {
	return ROCCAT_EVENTHANDLER_PLUGIN(g_object_new(ARVO_EVENTHANDLER_TYPE, NULL));
}
