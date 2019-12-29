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

#include "kiro_eventhandler_channel.h"
#include "kiro_dbus_server.h"
#include "kiro_gfx.h"
#include "roccat_eventhandler_plugin.h"
#include "roccat_notification_cpi.h"
#include "roccat_notification_sensitivity.h"
#include "roccat_process_helper.h"
#include "roccat_helper.h"
#include "roccat.h"
#include "kiro.h"
#include "kiro_config.h"
#include "kiro_device.h"
#include "kiro_device_state.h"
#include "kiro_profile.h"
#include "kiro_profile_data.h"
#include "kiro_talkfx.h"
#include "config.h"
#include "i18n-lib.h"
#include "g_roccat_helper.h"
#include <gtk/gtk.h>
#include <glib.h>
#include <string.h>
#include <unistd.h>
#include "../libroccatkiro/kiro_special.h"

GType kiro_eventhandler_get_type(void);

#define KIRO_EVENTHANDLER_TYPE (kiro_eventhandler_get_type())
#define KIRO_EVENTHANDLER(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), KIRO_EVENTHANDLER_TYPE, KiroEventhandler))
#define IS_KIRO_EVENTHANDLER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), KIRO_EVENTHANDLER_TYPE))
#define KIRO_EVENTHANDLER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), KIRO_EVENTHANDLER_TYPE, KiroEventhandlerClass))
#define IS_KIRO_EVENTHANDLER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), KIRO_EVENTHANDLER_TYPE))
#define KIRO_EVENTHANDLER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), KIRO_EVENTHANDLER_TYPE, KiroEventhandlerPrivate))

typedef struct _KiroEventhandler KiroEventhandler;
typedef struct _KiroEventhandlerClass KiroEventhandlerClass;
typedef struct _KiroEventhandlerPrivate KiroEventhandlerPrivate;

struct _KiroEventhandler {
	GObject parent;
	KiroEventhandlerPrivate *priv;
};

struct _KiroEventhandlerClass {
	GObjectClass parent_class;
};

struct _KiroEventhandlerPrivate {
	RoccatEventhandlerHost *host;
	KiroDBusServer *dbus_server;
	RoccatDeviceScanner *device_scanner;
	RoccatDevice *device;
	gboolean device_set_up;
	RoccatKeyFile *config;
	guint actual_sensitivity;
	KiroProfileData *profile_data;
	KiroGfx *gfx;
	KiroEventhandlerChannel *channel;
	RoccatNotificationCpi *cpi_note;
	RoccatNotificationSensitivity *sensitivity_note;
};

static gboolean should_execute_fx(KiroEventhandler *eventhandler) {
	return TRUE; // FIXME
}

static gboolean talkfx_set_led_rgb(KiroEventhandler *eventhandler, guint32 effect, guint32 ambient_color, guint32 event_color) {
	KiroEventhandlerPrivate *priv = eventhandler->priv;
	GError *error = NULL;
	gboolean retval;

	if (!should_execute_fx(eventhandler))
		return FALSE;

	if (!priv->device_set_up)
		return FALSE;

	retval = kiro_talkfx(priv->device, effect, ambient_color, event_color, &error);
	if (error) {
		g_warning(_("Could not activate Talk FX: %s"), error->message);
		g_clear_error(&error);
	}

	return retval;
}

static gboolean talkfx_restore_led_rgb(KiroEventhandler *eventhandler) {
	KiroEventhandlerPrivate *priv = eventhandler->priv;
	GError *error = NULL;
	gboolean retval;

	if (!should_execute_fx(eventhandler))
		return FALSE;

	if (!priv->device_set_up)
		return FALSE;

	retval = kiro_talkfx_off(priv->device, &error);
	if (error) {
		g_warning(_("Could not deactivate Talk FX: %s"), error->message);
		g_clear_error(&error);
	}

	return retval;
}

static guint kiro_eventhandler_get_talk_device(RoccatEventhandlerPlugin *self) {
	return 0;
}

static void kiro_eventhandler_talkfx_set_led_rgb(RoccatEventhandlerPlugin *self, guint32 effect, guint32 ambient_color, guint32 event_color) {
	KiroEventhandler *eventhandler = KIRO_EVENTHANDLER(self);
	talkfx_set_led_rgb(eventhandler, effect, ambient_color, event_color);
}

static void kiro_eventhandler_talkfx_restore_led_rgb(RoccatEventhandlerPlugin *self) {
	KiroEventhandler *eventhandler = KIRO_EVENTHANDLER(self);
	talkfx_restore_led_rgb(eventhandler);
}

static void talkfx_set_led_rgb_cb(KiroDBusServer *server, guint effect, guint ambient_color, guint event_color, gpointer user_data) {
	KiroEventhandler *eventhandler = KIRO_EVENTHANDLER(user_data);
	talkfx_set_led_rgb(eventhandler, effect, ambient_color, event_color);
}

static void talkfx_restore_led_rgb_cb(KiroDBusServer *server, gpointer user_data) {
	KiroEventhandler *eventhandler = KIRO_EVENTHANDLER(user_data);
	talkfx_restore_led_rgb(eventhandler);
}

static void gfx_set_led_rgb_cb(KiroDBusServer *server, guint index, guint color, gpointer user_data) {
	KiroEventhandler *eventhandler = KIRO_EVENTHANDLER(user_data);
	if (!should_execute_fx(eventhandler))
		return;
	kiro_gfx_set_color(eventhandler->priv->gfx, index, color);
}

static void gfx_get_led_rgb_cb(KiroDBusServer *server, guint index, guint *color, gpointer user_data) {
	KiroEventhandler *eventhandler = KIRO_EVENTHANDLER(user_data);
	if (!should_execute_fx(eventhandler))
		return;
	*color = kiro_gfx_get_color(eventhandler->priv->gfx, index);
}

static void gfx_update_cb(KiroDBusServer *server, gpointer user_data) {
	KiroEventhandler *eventhandler = KIRO_EVENTHANDLER(user_data);
	if (!should_execute_fx(eventhandler))
		return;
	(void)kiro_gfx_update(eventhandler->priv->gfx, NULL);
}

static void open_gui_cb(gpointer source, gpointer user_data) {
	gchar *path;
	path = g_build_path("/", INSTALL_PREFIX, "bin", "roccatkiroconfig", (gchar const *)NULL);
	roccat_double_fork(path);
	g_free(path);
}

static KiroProfileData *single_profile_data_load(RoccatDevice *device) {
	KiroProfileData *profile_data;
	GError *local_error = NULL;

	profile_data = kiro_profile_data_new();

	if (!kiro_profile_data_update_filesystem(profile_data, &local_error)) {
		g_warning(_("Could not load filesystem data for profile: %s"), local_error->message);
		g_clear_error(&local_error);
	}

	if (!kiro_profile_data_update_hardware(profile_data, device, &local_error)) {
		g_warning(_("Could not load hardware data for profile: %s"), local_error->message);
		g_clear_error(&local_error);
	}

	return profile_data;
}

static void profile_data_changed_outside_cb(KiroDBusServer *server, guchar profile_number, gpointer user_data) {
	KiroEventhandler *eventhandler = KIRO_EVENTHANDLER(user_data);
	KiroEventhandlerPrivate *priv = eventhandler->priv;

	g_free(priv->profile_data);
	priv->profile_data = single_profile_data_load(priv->device);
}

static void configuration_reload(KiroEventhandler *eventhandler) {
	KiroEventhandlerPrivate *priv = eventhandler->priv;
	if (priv->config)
		kiro_configuration_free(priv->config);
	priv->config = kiro_configuration_load();
}

static void configuration_changed_outside_cb(KiroDBusServer *server, gpointer user_data) {
	KiroEventhandler *eventhandler = KIRO_EVENTHANDLER(user_data);
	configuration_reload(eventhandler);
}

static void cpi_changed_cb(KiroEventhandlerChannel *channel, guchar cpi_index, gpointer user_data) {
	KiroEventhandlerPrivate *priv = KIRO_EVENTHANDLER(user_data)->priv;
	KiroProfileDataEventhandler const *profile_data_eventhandler = &priv->profile_data->eventhandler;
	guint cpi = kiro_profile_cpi_level_to_cpi(priv->profile_data->hardware.profile.cpi_levels[cpi_index]);

	roccat_notification_cpi_update(priv->cpi_note,
			profile_data_eventhandler->cpi_notification_type,
			profile_data_eventhandler->notification_volume,
			cpi, cpi);
}

static void open_application_cb(KiroEventhandlerChannel *channel, guchar key_index, gpointer user_data) {
	KiroEventhandler *eventhandler = KIRO_EVENTHANDLER(user_data);
	KiroEventhandlerPrivate *priv = eventhandler->priv;

	roccat_double_fork(priv->profile_data->eventhandler.openers[key_index]);
}

static void sensitivity_changed_cb(KiroEventhandlerChannel *channel, guchar sense, gpointer user_data) {
	KiroEventhandlerPrivate *priv = KIRO_EVENTHANDLER(user_data)->priv;
	KiroProfileDataEventhandler const *profile_data_eventhandler = &priv->profile_data->eventhandler;
	gint notification_type;

	notification_type = profile_data_eventhandler->sensitivity_notification_type;

	if (notification_type == ROCCAT_NOTIFICATION_TYPE_OSD)
		roccat_notification_sensitivity_update(priv->sensitivity_note, notification_type, roccat_sensitivity_from_bin_to_value(sense), roccat_sensitivity_from_bin_to_value(sense));
	else if (sense > priv->actual_sensitivity)
		roccat_notification_sensitivity_update_up(priv->sensitivity_note, notification_type,
				profile_data_eventhandler->notification_volume);
	else
		roccat_notification_sensitivity_update_down(priv->sensitivity_note, notification_type,
				profile_data_eventhandler->notification_volume);

	priv->actual_sensitivity = sense;
}

static void device_add_cb(RoccatDeviceScannerInterface *interface, RoccatDevice *device, gpointer user_data) {
	KiroEventhandler *eventhandler = KIRO_EVENTHANDLER(user_data);
	KiroEventhandlerPrivate *priv = eventhandler->priv;
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
		if (!kiro_device_state_write(priv->device, (driver_state == ROCCATEVENTHANDLER_CONFIGURATION_DRIVER_STATE_ON) ? KIRO_DEVICE_STATE_STATE_ON : KIRO_DEVICE_STATE_STATE_OFF, &local_error)) {
			g_warning(_("Could not correct driver state: %s"), local_error->message);
			g_clear_error(&local_error);
		}
	}

	/* Notificator gets initialized only once if a device is found.
	 * Gets deinited once in finalizer.
	 */
	if (!priv->cpi_note)
		priv->cpi_note = roccat_notification_cpi_new(roccat_eventhandler_host_get_notificator(priv->host), KIRO_DEVICE_NAME);
	if (!priv->sensitivity_note)
		priv->sensitivity_note = roccat_notification_sensitivity_new(roccat_eventhandler_host_get_notificator(priv->host), KIRO_DEVICE_NAME);

	priv->profile_data = single_profile_data_load(priv->device);
	priv->actual_sensitivity = priv->profile_data->hardware.profile.sensitivity;

	priv->gfx = kiro_gfx_new(priv->device);

	// FIXME create in init and just disconnect/reconnect?
	priv->dbus_server = kiro_dbus_server_new();
	g_signal_connect(G_OBJECT(priv->dbus_server), "talkfx-set-led-rgb", G_CALLBACK(talkfx_set_led_rgb_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->dbus_server), "talkfx-restore-led-rgb", G_CALLBACK(talkfx_restore_led_rgb_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->dbus_server), "gfx-set-led-rgb", G_CALLBACK(gfx_set_led_rgb_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->dbus_server), "gfx-get-led-rgb", G_CALLBACK(gfx_get_led_rgb_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->dbus_server), "gfx-update", G_CALLBACK(gfx_update_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->dbus_server), "open-gui", G_CALLBACK(open_gui_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->dbus_server), "profile-data-changed-outside", G_CALLBACK(profile_data_changed_outside_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->dbus_server), "configuration-changed-outside", G_CALLBACK(configuration_changed_outside_cb), eventhandler);
	kiro_dbus_server_connect(priv->dbus_server);

	kiro_eventhandler_channel_start(priv->channel, priv->device, &local_error);
	if (local_error) {
		g_warning(_("Could not start chardev eventhandler: %s"), local_error->message);
		g_clear_error(&local_error);
	}

	priv->device_set_up = TRUE;

	roccat_eventhandler_plugin_emit_device_added(ROCCAT_EVENTHANDLER_PLUGIN(eventhandler), gaminggear_device_get_product_id(GAMINGGEAR_DEVICE(priv->device)));
}

static void stop(KiroEventhandler *eventhandler) {
	KiroEventhandlerPrivate *priv = eventhandler->priv;

	roccat_eventhandler_plugin_emit_device_removed(ROCCAT_EVENTHANDLER_PLUGIN(eventhandler), gaminggear_device_get_product_id(GAMINGGEAR_DEVICE(priv->device)));

	priv->device_set_up = FALSE;

	g_clear_object(&priv->dbus_server);
	kiro_eventhandler_channel_stop(priv->channel);
	g_clear_object(&priv->device);
	g_clear_pointer(&priv->config, kiro_configuration_free);
	g_clear_pointer(&priv->profile_data, g_free);
	g_clear_object(&priv->gfx);
}

static void device_remove_cb(RoccatDeviceScanner *scanner, RoccatDevice *device, gpointer user_data) {
	KiroEventhandler *eventhandler = KIRO_EVENTHANDLER(user_data);
	KiroEventhandlerPrivate *priv = eventhandler->priv;
	if (priv->device && gaminggear_device_equal(GAMINGGEAR_DEVICE(device), GAMINGGEAR_DEVICE(priv->device)))
		stop(eventhandler);
}

static void kiro_eventhandler_start(RoccatEventhandlerPlugin *self, RoccatEventhandlerHost *host) {
	KiroEventhandler *eventhandler = KIRO_EVENTHANDLER(self);
	KiroEventhandlerPrivate *priv = eventhandler->priv;

	priv->host = host;
	g_object_ref(priv->host);

	priv->channel = kiro_eventhandler_channel_new();
	g_signal_connect(G_OBJECT(priv->channel), "open-application", G_CALLBACK(open_application_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->channel), "open-driver", G_CALLBACK(open_gui_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->channel), "cpi-changed", G_CALLBACK(cpi_changed_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->channel), "sensitivity-changed", G_CALLBACK(sensitivity_changed_cb), eventhandler);

	priv->device_scanner = kiro_device_scanner_new();
	g_signal_connect(G_OBJECT(priv->device_scanner), "device-added", G_CALLBACK(device_add_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->device_scanner), "device-removed", G_CALLBACK(device_remove_cb), eventhandler);
	roccat_device_scanner_interface_start(ROCCAT_DEVICE_SCANNER_INTERFACE(priv->device_scanner));
}

static void kiro_eventhandler_stop(RoccatEventhandlerPlugin *self) {
	KiroEventhandler *eventhandler = KIRO_EVENTHANDLER(self);
	KiroEventhandlerPrivate *priv = eventhandler->priv;

	stop(eventhandler);

	g_clear_object(&priv->device_scanner);
	g_clear_object(&priv->channel);
	g_clear_object(&priv->host);
}

static void kiro_roccat_eventhandler_interface_init(RoccatEventhandlerPluginInterface *iface) {
	iface->start = kiro_eventhandler_start;
	iface->stop = kiro_eventhandler_stop;
	iface->get_talk_device = kiro_eventhandler_get_talk_device;
	iface->talk_easyshift = NULL;
	iface->talk_easyshift_lock = NULL;
	iface->talk_easyaim = NULL;
	iface->talkfx_set_led_rgb = kiro_eventhandler_talkfx_set_led_rgb;
	iface->talkfx_restore_led_rgb = kiro_eventhandler_talkfx_restore_led_rgb;
	iface->talkfx_ryos_set_sdk_mode = NULL;
	iface->talkfx_ryos_set_all_leds = NULL;
	iface->talkfx_ryos_turn_on_all_leds = NULL;
	iface->talkfx_ryos_turn_off_all_leds = NULL;
	iface->talkfx_ryos_set_led_on = NULL;
	iface->talkfx_ryos_set_led_off = NULL;
	iface->talkfx_ryos_all_key_blinking = NULL;
}

G_DEFINE_TYPE_WITH_CODE(KiroEventhandler, kiro_eventhandler, G_TYPE_OBJECT,
		G_IMPLEMENT_INTERFACE(ROCCAT_EVENTHANDLER_PLUGIN_TYPE, kiro_roccat_eventhandler_interface_init));

static void kiro_eventhandler_init(KiroEventhandler *eventhandler) {
	eventhandler->priv = KIRO_EVENTHANDLER_GET_PRIVATE(eventhandler);
}

static void dispose(GObject *object) {
	kiro_eventhandler_stop(ROCCAT_EVENTHANDLER_PLUGIN(object));

	G_OBJECT_CLASS(kiro_eventhandler_parent_class)->dispose(object);
}

static void finalize(GObject *object) {
	KiroEventhandlerPrivate *priv = KIRO_EVENTHANDLER(object)->priv;

	g_clear_pointer(&priv->cpi_note, roccat_notification_cpi_free);
	g_clear_pointer(&priv->sensitivity_note, roccat_notification_sensitivity_free);

	G_OBJECT_CLASS(kiro_eventhandler_parent_class)->finalize(object);
}

static void kiro_eventhandler_class_init(KiroEventhandlerClass *klass) {
	GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
	gobject_class->dispose = dispose;
	gobject_class->finalize = finalize;

	g_type_class_add_private(klass, sizeof(KiroEventhandlerPrivate));
}

ROCCAT_EVENTHANDLER_PLUGIN_API RoccatEventhandlerPlugin *roccat_eventhandler_plugin_new(void) {
	return ROCCAT_EVENTHANDLER_PLUGIN(g_object_new(KIRO_EVENTHANDLER_TYPE, NULL));
}
