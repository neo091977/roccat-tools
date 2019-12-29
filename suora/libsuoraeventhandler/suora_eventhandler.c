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

#include "suora_eventhandler_channel.h"
#include "suora_dbus_server.h"
#include "suora_config.h"
#include "suora_profile_data.h"
#include "suora_device.h"
#include "roccat_eventhandler_plugin.h"
#include "roccat_process_helper.h"
#include "suora_notification_typing.h"
#include "g_roccat_helper.h"
#include "config.h"
#include "i18n-lib.h"
#include <gaminggear/profiles.h>
#include <glib.h>

GType suora_eventhandler_get_type(void);

#define SUORA_EVENTHANDLER_TYPE (suora_eventhandler_get_type())
#define SUORA_EVENTHANDLER(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), SUORA_EVENTHANDLER_TYPE, SuoraEventhandler))
#define IS_SUORA_EVENTHANDLER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), SUORA_EVENTHANDLER_TYPE))
#define SUORA_EVENTHANDLER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), SUORA_EVENTHANDLER_TYPE, SuoraEventhandlerClass))
#define IS_SUORA_EVENTHANDLER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), SUORA_EVENTHANDLER_TYPE))
#define SUORA_EVENTHANDLER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), SUORA_EVENTHANDLER_TYPE, SuoraEventhandlerPrivate))

typedef struct _SuoraEventhandler SuoraEventhandler;
typedef struct _SuoraEventhandlerClass SuoraEventhandlerClass;
typedef struct _SuoraEventhandlerPrivate SuoraEventhandlerPrivate;

struct _SuoraEventhandler {
	GObject parent;
	SuoraEventhandlerPrivate *priv;
};

struct _SuoraEventhandlerClass {
	GObjectClass parent_class;
};

struct _SuoraEventhandlerPrivate {
	RoccatEventhandlerHost *host;
	SuoraDBusServer *dbus_server;
	RoccatDeviceScanner *device_scanner;
	RoccatDevice *device;
	gboolean device_set_up;
	RoccatKeyFile *config;
	GaminggearProfiles *profiles;
	SuoraProfileData *actual_profile_data;
	SuoraEventhandlerChannel *channel;
	SuoraNotificationTyping *typing_note;
	gulong active_window_changed_handler;
};

static guint suora_eventhandler_get_talk_device(RoccatEventhandlerPlugin *self) {
	return 0;
}

static void open_gui_cb(gpointer source, gpointer user_data) {
	gchar *path;
	path = g_build_path("/", INSTALL_PREFIX, "bin", "roccatsuoraconfig", (gchar const *)NULL);
	roccat_double_fork(path);
	g_free(path);
}

static void set_actual_profile(SuoraEventhandler *eventhandler, GaminggearProfileData *profile_data) {
	SuoraEventhandlerPrivate *priv = eventhandler->priv;

	if (profile_data)
		priv->actual_profile_data = SUORA_PROFILE_DATA(profile_data);
	else
		priv->actual_profile_data = NULL;
}

static gboolean load_profile_datas(SuoraEventhandler *eventhandler, GError **error) {
	SuoraEventhandlerPrivate *priv = eventhandler->priv;
	gchar *path;
	GaminggearProfileData *profile_data;

	path = suora_profile_data_path();
	priv->profiles = gaminggear_profiles_new(SUORA_PROFILE_DATA_TYPE, path);
	g_free(path);

	if (!gaminggear_profiles_load(priv->profiles, error))
		return FALSE;

	if (!gaminggear_profiles_read(priv->profiles, GAMINGGEAR_DEVICE(priv->device), error))
		return FALSE;

	if (!gaminggear_profiles_fill(priv->profiles, GAMINGGEAR_DEVICE(priv->device), 1, error))
		return FALSE;

	profile_data = gaminggear_profiles_find_by_hardware_index(priv->profiles, 0);
	set_actual_profile(eventhandler, profile_data);

	return TRUE;
}

static void set_profile(SuoraEventhandler *eventhandler, GaminggearProfileData *profile_data) {
	SuoraEventhandlerPrivate *priv = eventhandler->priv;
	GError *local_error = NULL;

	gaminggear_profiles_activate(priv->profiles, profile_data, 0);
	if (!gaminggear_profiles_store(priv->profiles, GAMINGGEAR_DEVICE(priv->device), &local_error)) {
		g_debug(_("Could not set profile: %s"), local_error->message);
		g_clear_error(&local_error);
	} else {
		set_actual_profile(eventhandler, profile_data);
		// FIXME notification
	}
}

static void profile_data_changed_outside_cb(SuoraDBusServer *server, gpointer user_data) {
	SuoraEventhandler *eventhandler = SUORA_EVENTHANDLER(user_data);
	SuoraEventhandlerPrivate *priv = eventhandler->priv;
	GError *local_error = NULL;

	g_clear_pointer(&priv->profiles, gaminggear_profiles_free);
	if (!load_profile_datas(eventhandler, &local_error)) {
		g_warning(_("Could not load profile data: %s"), local_error->message);
		g_clear_error(&local_error);
	}
}

static void configuration_reload(SuoraEventhandler *eventhandler) {
	SuoraEventhandlerPrivate *priv = eventhandler->priv;
	if (priv->config)
		roccat_configuration_free(priv->config);
	priv->config = suora_configuration_load();
}

static void configuration_changed_outside_cb(SuoraDBusServer *server, gpointer user_data) {
	SuoraEventhandler *eventhandler = SUORA_EVENTHANDLER(user_data);
	configuration_reload(eventhandler);
}

static void window_changed_cb(RoccatEventhandlerHost *host, gchar const *title, gpointer user_data) {
	SuoraEventhandler *eventhandler = SUORA_EVENTHANDLER(user_data);
	SuoraEventhandlerPrivate *priv = eventhandler->priv;
	GSList *iterator;
	GaminggearProfileData *profile;
	gchar const *pattern;
	gchar *desktop_name;
	guint gamefile_index;

	for (iterator = gaminggear_profiles_first(priv->profiles); iterator; iterator = g_slist_next(iterator)) {
		profile = GAMINGGEAR_PROFILE_DATA(iterator->data);

		for (gamefile_index = 0; gamefile_index < GAMINGGEAR_PROFILE_DATA_GAMEFILE_NUM; ++gamefile_index) {
			pattern = gaminggear_profile_data_get_gamefile(profile, gamefile_index);
			if (strcmp(pattern, "") == 0)
				continue;

			if (g_regex_match_simple(pattern, title, 0, 0)) {
				set_profile(eventhandler, profile);
				return;
			}
		}
	}

	desktop_name = suora_configuration_get_default_profile_name(priv->config);
	profile = gaminggear_profiles_find_by_name(priv->profiles, desktop_name);
	g_free(desktop_name);
	if (profile)
		set_profile(eventhandler, profile);
}

static void device_add_cb(RoccatDeviceScannerInterface *interface, RoccatDevice *device, gpointer user_data) {
	SuoraEventhandler *eventhandler = SUORA_EVENTHANDLER(user_data);
	SuoraEventhandlerPrivate *priv = eventhandler->priv;
	GError *local_error = NULL;

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
	if (!priv->typing_note)
		priv->typing_note = suora_notification_typing_new(roccat_eventhandler_host_get_notificator(priv->host));

	if (!load_profile_datas(eventhandler, &local_error)) {
		g_warning(_("Could not load profile data: %s"), local_error->message);
		g_clear_error(&local_error);
	}

	priv->dbus_server = suora_dbus_server_new();
	g_signal_connect(G_OBJECT(priv->dbus_server), "open-gui", G_CALLBACK(open_gui_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->dbus_server), "profile-data-changed-outside", G_CALLBACK(profile_data_changed_outside_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->dbus_server), "configuration-changed-outside", G_CALLBACK(configuration_changed_outside_cb), eventhandler);
	suora_dbus_server_connect(priv->dbus_server);

	priv->active_window_changed_handler = g_signal_connect(G_OBJECT(priv->host), "active-window-changed", G_CALLBACK(window_changed_cb), eventhandler);

	suora_eventhandler_channel_start(priv->channel, priv->device, &local_error);
	if (local_error) {
		g_warning(_("Could not start typing eventhandler: %s"), local_error->message);
		g_clear_error(&local_error);
	}

	priv->device_set_up = TRUE;

	roccat_eventhandler_plugin_emit_device_added(ROCCAT_EVENTHANDLER_PLUGIN(eventhandler), gaminggear_device_get_product_id(GAMINGGEAR_DEVICE(priv->device)));
}

static void stop(SuoraEventhandler *eventhandler) {
	SuoraEventhandlerPrivate *priv = eventhandler->priv;

	roccat_eventhandler_plugin_emit_device_removed(ROCCAT_EVENTHANDLER_PLUGIN(eventhandler), gaminggear_device_get_product_id(GAMINGGEAR_DEVICE(priv->device)));

	priv->device_set_up = FALSE;

	g_clear_object(&priv->dbus_server);
	suora_eventhandler_channel_stop(priv->channel);
	g_signal_handler_disconnect(G_OBJECT(priv->host), priv->active_window_changed_handler);
	g_clear_object(&priv->device);
	g_clear_pointer(&priv->config, roccat_configuration_free);
	g_clear_pointer(&priv->profiles, gaminggear_profiles_free);
}

static void device_remove_cb(RoccatDeviceScanner *scanner, RoccatDevice *device, gpointer user_data) {
	SuoraEventhandler *eventhandler = SUORA_EVENTHANDLER(user_data);
	SuoraEventhandlerPrivate *priv = eventhandler->priv;
	if (priv->device && gaminggear_device_equal(GAMINGGEAR_DEVICE(device), GAMINGGEAR_DEVICE(priv->device)))
		stop(eventhandler);
}

static void key_press_cb(SuoraEventhandlerChannel *eventhandler_channel, gpointer data) {
	SuoraEventhandler *eventhandler = SUORA_EVENTHANDLER(data);
	SuoraEventhandlerPrivate *priv = eventhandler->priv;

	suora_notification_typing_update(priv->typing_note,
			suora_profile_data_get_sound_feedback(priv->actual_profile_data),
			suora_profile_data_get_volume(priv->actual_profile_data));
}

static void suora_eventhandler_start(RoccatEventhandlerPlugin *self, RoccatEventhandlerHost *host) {
	SuoraEventhandler *eventhandler = SUORA_EVENTHANDLER(self);
	SuoraEventhandlerPrivate *priv = eventhandler->priv;

	priv->host = host;
	g_object_ref(priv->host);

	priv->channel = suora_eventhandler_channel_new();
	g_signal_connect(G_OBJECT(priv->channel), "key-press", G_CALLBACK(key_press_cb), eventhandler);

	priv->device_scanner = suora_device_scanner_new();
	g_signal_connect(G_OBJECT(priv->device_scanner), "device-added", G_CALLBACK(device_add_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->device_scanner), "device-removed", G_CALLBACK(device_remove_cb), eventhandler);
	roccat_device_scanner_interface_start(ROCCAT_DEVICE_SCANNER_INTERFACE(priv->device_scanner));
}

static void suora_eventhandler_stop(RoccatEventhandlerPlugin *self) {
	SuoraEventhandler *eventhandler = SUORA_EVENTHANDLER(self);
	SuoraEventhandlerPrivate *priv = eventhandler->priv;

	stop(eventhandler);

	g_clear_object(&priv->device_scanner);
	g_clear_object(&priv->channel);
	g_clear_object(&priv->host);
}

static void suora_roccat_eventhandler_interface_init(RoccatEventhandlerPluginInterface *iface) {
	iface->start = suora_eventhandler_start;
	iface->stop = suora_eventhandler_stop;
	iface->get_talk_device = suora_eventhandler_get_talk_device;
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

G_DEFINE_TYPE_WITH_CODE(SuoraEventhandler, suora_eventhandler, G_TYPE_OBJECT,
		G_IMPLEMENT_INTERFACE(ROCCAT_EVENTHANDLER_PLUGIN_TYPE, suora_roccat_eventhandler_interface_init));

static void suora_eventhandler_init(SuoraEventhandler *eventhandler) {
	eventhandler->priv = SUORA_EVENTHANDLER_GET_PRIVATE(eventhandler);
}

static void dispose(GObject *object) {
	suora_eventhandler_stop(ROCCAT_EVENTHANDLER_PLUGIN(object));

	G_OBJECT_CLASS(suora_eventhandler_parent_class)->dispose(object);
}

static void finalize(GObject *object) {
	SuoraEventhandlerPrivate *priv = SUORA_EVENTHANDLER(object)->priv;

	g_clear_pointer(&priv->typing_note, suora_notification_typing_free);

	G_OBJECT_CLASS(suora_eventhandler_parent_class)->finalize(object);
}

static void suora_eventhandler_class_init(SuoraEventhandlerClass *klass) {
	GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
	gobject_class->dispose = dispose;
	gobject_class->finalize = finalize;

	g_type_class_add_private(klass, sizeof(SuoraEventhandlerPrivate));
}

ROCCAT_EVENTHANDLER_PLUGIN_API RoccatEventhandlerPlugin *roccat_eventhandler_plugin_new(void) {
	return ROCCAT_EVENTHANDLER_PLUGIN(g_object_new(SUORA_EVENTHANDLER_TYPE, NULL));
}
