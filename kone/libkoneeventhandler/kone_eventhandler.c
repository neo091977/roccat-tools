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
#include "kone_eventhandler_channel.h"
#include "kone_dbus_server.h"
#include "kone_device_scanner.h"
#include "roccat_eventhandler_plugin.h"
#include "roccat_notification_profile.h"
#include "roccat_notification_cpi.h"
#include "roccat_process_helper.h"
#include "kone.h"
#include "config.h"
#include "i18n-lib.h"
#include "g_roccat_helper.h"
#include <glib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <linux/input.h>

GType kone_eventhandler_get_type(void);

#define KONE_EVENTHANDLER_TYPE (kone_eventhandler_get_type())
#define KONE_EVENTHANDLER(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), KONE_EVENTHANDLER_TYPE, KoneEventhandler))
#define IS_KONE_EVENTHANDLER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), KONE_EVENTHANDLER_TYPE))
#define KONE_EVENTHANDLER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), KONE_EVENTHANDLER_TYPE, KoneEventhandlerClass))
#define IS_KONE_EVENTHANDLER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), KONE_EVENTHANDLER_TYPE))
#define KONE_EVENTHANDLER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), KONE_EVENTHANDLER_TYPE, KoneEventhandlerPrivate))

typedef struct _KoneEventhandler KoneEventhandler;
typedef struct _KoneEventhandlerClass KoneEventhandlerClass;
typedef struct _KoneEventhandlerPrivate KoneEventhandlerPrivate;

struct _KoneEventhandler {
	GObject parent;
	KoneEventhandlerPrivate *priv;
};

struct _KoneEventhandlerClass {
	GObjectClass parent_class;
};

struct _KoneEventhandlerPrivate {
	RoccatEventhandlerHost *host;
	KoneDBusServer *dbus_server;
	KoneDeviceScanner *device_scanner;
	RoccatDevice *device;
	gboolean device_set_up;
	RoccatKeyFile *config;
	guint actual_profile_index;
	KoneRMP *rmp[KONE_PROFILE_NUM];
	KoneEventhandlerChannel *channel;
	gulong active_window_changed_handler;

	/*
	 * FIXME notificator für Fehlermeldungen verwenden,
	 * notificator refaktorisieren und Basis für beides verwenden oder
	 * Eigene Implementierung eines Fehlernotificators
	 */
	RoccatNotificationProfile *profile_note;
	RoccatNotificationCpi *cpi_note;

	GaminggearMacroThreads *threads;
};

static void profile_osd(KoneEventhandler *eventhandler, guint profile_number) {
	KoneEventhandlerPrivate *priv = eventhandler->priv;
	guint profile_index = profile_number - 1;
	
	roccat_notification_profile_update(priv->profile_note, kone_configuration_get_profile_notification_type(priv->config),
			kone_configuration_get_notification_volume(priv->config),
			profile_number, kone_rmp_get_profile_name(priv->rmp[profile_index]));
}

static void profile_changed(KoneEventhandler *eventhandler, guint profile_number) {
	KoneEventhandlerPrivate *priv = eventhandler->priv;
	guint profile_index = profile_number - 1;

	priv->actual_profile_index = profile_index;

	profile_osd(eventhandler, profile_number);

	kone_dbus_server_emit_profile_changed(priv->dbus_server, profile_number);
}

static gboolean set_profile(KoneEventhandler *eventhandler, guint profile_number) {
	KoneEventhandlerPrivate *priv = eventhandler->priv;
	guint profile_index;
	GError *local_error = NULL;

	if (profile_number == 0)
		return TRUE;

	profile_index = profile_number - 1;

	if (priv->actual_profile_index != profile_index) {
		if (!kone_profile_activate(priv->device, profile_number, &local_error)) {
			g_warning(_("Could not activate profile %i: %s"), profile_number, local_error->message);
			g_clear_error(&local_error);
			return FALSE;
		}
		profile_changed(eventhandler, profile_number);
	}
	return TRUE;
}

static void window_changed_cb(RoccatEventhandlerHost *host, gchar const *title, gpointer user_data) {
	KoneEventhandler *eventhandler = KONE_EVENTHANDLER(user_data);
	KoneEventhandlerPrivate *priv = eventhandler->priv;
	guint profile_index;
	gchar *pattern;

	for (profile_index = 0; profile_index < KONE_PROFILE_NUM; ++profile_index) {

		if (priv->rmp[profile_index] == NULL)
			continue;

		pattern = kone_rmp_get_game_file_name(priv->rmp[profile_index]);
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

	set_profile(eventhandler, kone_configuration_get_default_profile_number(priv->config));
	return;
}

static guint kone_eventhandler_get_talk_device(RoccatEventhandlerPlugin *self) {
	return 0;
}

static void open_gui_cb(gpointer source, gpointer user_data) {
	gchar *path;
	path = g_build_path("/", INSTALL_PREFIX, "bin", "roccatkoneconfig", (gchar const *)NULL);
	roccat_double_fork(path);
	g_free(path);
}

static void profile_data_changed_outside_cb(KoneDBusServer *server, guchar profile_number, gpointer user_data) {
	KoneEventhandler *eventhandler = KONE_EVENTHANDLER(user_data);
	KoneEventhandlerPrivate *priv = eventhandler->priv;
	guint profile_index = profile_number - 1;

	/* Just load rmp without updating data from hardware */
	kone_rmp_free(priv->rmp[profile_index]);
	priv->rmp[profile_index] = kone_rmp_load_actual(profile_number);
}

static void configuration_reload(KoneEventhandler *eventhandler) {
	KoneEventhandlerPrivate *priv = eventhandler->priv;
	if (priv->config)
		kone_configuration_free(priv->config);
	priv->config = kone_configuration_load();
}

static void configuration_changed_outside_cb(KoneDBusServer *server, gpointer user_data) {
	KoneEventhandler *eventhandler = KONE_EVENTHANDLER(user_data);
	configuration_reload(eventhandler);
}

static void cpi_osd(KoneEventhandler *eventhandler, guchar cpi_raw) {
	KoneEventhandlerPrivate *priv = eventhandler->priv;
	guint cpi_value;
	
	cpi_value = kone_dpi_raw_to_dpi(cpi_raw);
	roccat_notification_cpi_update(priv->cpi_note,
			kone_configuration_get_cpi_notification_type(priv->config),
			kone_configuration_get_notification_volume(priv->config),
			cpi_value, cpi_value);
}

static void cpi_osd_cb(KoneEventhandlerChannel *channel, guchar cpi_raw, gpointer user_data) {
	KoneEventhandler *eventhandler = KONE_EVENTHANDLER(user_data);
	cpi_osd(eventhandler, cpi_raw);
}

static void cpi_changed_cb(KoneEventhandlerChannel *channel, guchar cpi_raw, gpointer user_data) {
	KoneEventhandler *eventhandler = KONE_EVENTHANDLER(user_data);
	KoneEventhandlerPrivate *priv = eventhandler->priv;

	if (!kone_configuration_get_only_notify_on_osd(priv->config))
		cpi_osd(eventhandler, cpi_raw);
}

static void profile_osd_cb(gpointer source, guchar profile_number, gpointer user_data) {
	KoneEventhandler *eventhandler = KONE_EVENTHANDLER(user_data);
	profile_osd(eventhandler, profile_number);
}

static void profile_changed_cb(gpointer source, guchar profile_number, gpointer user_data) {
	KoneEventhandler *eventhandler = KONE_EVENTHANDLER(user_data);
	profile_changed(eventhandler, profile_number);
}

static void multimedia_cb(KoneEventhandlerChannel *channel, guchar button_type, gpointer user_data) {
	switch (button_type) {
	case KONE_BUTTON_INFO_TYPE_MULTIMEDIA_NEXT_TRACK:
		gaminggear_input_event_write_multimedia_single(HID_UID_CP_NEXTSONG);
		break;
	case KONE_BUTTON_INFO_TYPE_MULTIMEDIA_PREV_TRACK:
		gaminggear_input_event_write_multimedia_single(HID_UID_CP_PREVIOUSSONG);
		break;
	case KONE_BUTTON_INFO_TYPE_MULTIMEDIA_PLAY_PAUSE:
		gaminggear_input_event_write_multimedia_single(HID_UID_CP_PLAYPAUSE);
		break;
	case KONE_BUTTON_INFO_TYPE_MULTIMEDIA_STOP:
		gaminggear_input_event_write_multimedia_single(HID_UID_CP_STOP);
		break;
	case KONE_BUTTON_INFO_TYPE_MULTIMEDIA_MUTE:
		gaminggear_input_event_write_multimedia_single(HID_UID_CP_MUTE);
		break;
	case KONE_BUTTON_INFO_TYPE_MULTIMEDIA_VOLUME_UP:
		gaminggear_input_event_write_multimedia_single(HID_UID_CP_VOLUMEUP);
		break;
	case KONE_BUTTON_INFO_TYPE_MULTIMEDIA_VOLUME_DOWN:
		gaminggear_input_event_write_multimedia_single(HID_UID_CP_VOLUMEDOWN);
		break;
	default:
		g_debug(_("Got unknown multimedia event type 0x%02x"), button_type);
		break;
	}
}

static void macro_cb(KoneEventhandlerChannel *channel, guchar key_number, gpointer user_data) {
	KoneEventhandler *eventhandler = KONE_EVENTHANDLER(user_data);
	KoneEventhandlerPrivate *priv = eventhandler->priv;
	KoneRMPButtonInfo *button_info;
	GaminggearMacro *gaminggear_macro;
	
	button_info = kone_rmp_get_rmp_button_info(priv->rmp[priv->actual_profile_index], key_number - 1);
	gaminggear_macro = kone_rmp_button_info_to_gaminggear_macro(button_info);
	g_free(button_info);
	gaminggear_play_macro_threaded(priv->threads, key_number - 1, gaminggear_macro);
	gaminggear_macro_free(gaminggear_macro);
}

static gboolean profile_data_load(KoneEventhandler *eventhandler, GError **error) {
	KoneEventhandlerPrivate *priv = eventhandler->priv;
	guint i;

	for (i = 0; i < KONE_PROFILE_NUM; ++i) {
		priv->rmp[i] = kone_rmp_load(priv->device, i + 1, error);
		if (priv->rmp[i] == NULL)
			return FALSE;
	}
	return TRUE;
}

static void device_add_cb(RoccatDeviceScannerInterface *interface, RoccatDevice *device, gpointer user_data) {
	KoneEventhandler *eventhandler = KONE_EVENTHANDLER(user_data);
	KoneEventhandlerPrivate *priv = eventhandler->priv;
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
		priv->profile_note = roccat_notification_profile_new(roccat_eventhandler_host_get_notificator(priv->host), KONE_DEVICE_NAME);
	if (!priv->cpi_note)
		priv->cpi_note = roccat_notification_cpi_new(roccat_eventhandler_host_get_notificator(priv->host), KONE_DEVICE_NAME);
	
	profile_data_load(eventhandler, &error);
	if (error) {
		g_warning(_("Could not read rmp: %s"), error->message);
		g_clear_error(&error);
	}

	priv->actual_profile_index = kone_actual_profile_read(priv->device, &error) - 1;
	if (error) {
		g_warning(_("Could not read actual profile: %s"), error->message);
		g_clear_error(&error);
	}
	
	// FIXME create in init and just disconnect/reconnect?
	priv->dbus_server = kone_dbus_server_new();
	g_signal_connect(G_OBJECT(priv->dbus_server), "open-gui", G_CALLBACK(open_gui_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->dbus_server), "profile-changed-outside", G_CALLBACK(profile_changed_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->dbus_server), "profile-data-changed-outside", G_CALLBACK(profile_data_changed_outside_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->dbus_server), "configuration-changed-outside", G_CALLBACK(configuration_changed_outside_cb), eventhandler);
	kone_dbus_server_connect(priv->dbus_server);

	priv->threads = gaminggear_macro_threads_new(KONE_BUTTON_INFO_NUM);

	priv->active_window_changed_handler = g_signal_connect(G_OBJECT(priv->host), "active-window-changed", G_CALLBACK(window_changed_cb), eventhandler);
	
	kone_eventhandler_channel_start(priv->channel, roccat_device_get_roccat_dev(priv->device), &error);
	if (error) {
		g_warning(_("Could not start chardev eventhandler: %s"), error->message);
		g_clear_error(&error);
	}

	priv->device_set_up = TRUE;

	roccat_eventhandler_plugin_emit_device_added(ROCCAT_EVENTHANDLER_PLUGIN(eventhandler), gaminggear_device_get_product_id(GAMINGGEAR_DEVICE(priv->device)));
}

static void stop(KoneEventhandler *eventhandler) {
	KoneEventhandlerPrivate *priv = eventhandler->priv;
	guint i;

	roccat_eventhandler_plugin_emit_device_removed(ROCCAT_EVENTHANDLER_PLUGIN(eventhandler), gaminggear_device_get_product_id(GAMINGGEAR_DEVICE(priv->device)));

	priv->device_set_up = FALSE;

	g_clear_object(&priv->dbus_server);
	kone_eventhandler_channel_stop(priv->channel);

	g_signal_handler_disconnect(G_OBJECT(priv->host), priv->active_window_changed_handler);

	g_clear_pointer(&priv->threads, gaminggear_macro_threads_free);
	g_clear_object(&priv->device);
	g_clear_pointer(&priv->config, kone_configuration_free);

	for (i = 0; i < KONE_PROFILE_NUM; ++i)
		g_clear_pointer(&priv->rmp[i], kone_rmp_free);
}

static void device_remove_cb(KoneDeviceScanner *scanner, RoccatDevice *device, gpointer user_data) {
	KoneEventhandler *eventhandler = KONE_EVENTHANDLER(user_data);
	KoneEventhandlerPrivate *priv = eventhandler->priv;
	if (priv->device && gaminggear_device_equal(GAMINGGEAR_DEVICE(device), GAMINGGEAR_DEVICE(priv->device)))
		stop(eventhandler);
}

static void kone_eventhandler_start(RoccatEventhandlerPlugin *self, RoccatEventhandlerHost *host) {
	KoneEventhandler *eventhandler = KONE_EVENTHANDLER(self);
	KoneEventhandlerPrivate *priv = eventhandler->priv;

	priv->host = host;
	g_object_ref(priv->host);

	priv->channel = kone_eventhandler_channel_new();
	g_signal_connect(G_OBJECT(priv->channel), "cpi-changed", G_CALLBACK(cpi_changed_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->channel), "cpi-osd", G_CALLBACK(cpi_osd_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->channel), "profile-changed", G_CALLBACK(profile_changed_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->channel), "profile-osd", G_CALLBACK(profile_osd_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->channel), "macro", G_CALLBACK(macro_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->channel), "multimedia", G_CALLBACK(multimedia_cb), eventhandler);

	priv->device_scanner = kone_device_scanner_new();
	g_signal_connect(G_OBJECT(priv->device_scanner), "device-added", G_CALLBACK(device_add_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->device_scanner), "device-removed", G_CALLBACK(device_remove_cb), eventhandler);
	roccat_device_scanner_interface_start(ROCCAT_DEVICE_SCANNER_INTERFACE(priv->device_scanner));
}

static void kone_eventhandler_stop(RoccatEventhandlerPlugin *self) {
	KoneEventhandler *eventhandler = KONE_EVENTHANDLER(self);
	KoneEventhandlerPrivate *priv = eventhandler->priv;

	stop(eventhandler);

	g_clear_object(&priv->device_scanner);
	g_clear_object(&priv->channel);
	g_clear_object(&priv->host);
}

static void kone_roccat_eventhandler_interface_init(RoccatEventhandlerPluginInterface *iface) {
	iface->start = kone_eventhandler_start;
	iface->stop = kone_eventhandler_stop;
	iface->get_talk_device = kone_eventhandler_get_talk_device;
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

G_DEFINE_TYPE_WITH_CODE(KoneEventhandler, kone_eventhandler, G_TYPE_OBJECT,
		G_IMPLEMENT_INTERFACE(ROCCAT_EVENTHANDLER_PLUGIN_TYPE, kone_roccat_eventhandler_interface_init));

static void kone_eventhandler_init(KoneEventhandler *eventhandler) {
	eventhandler->priv = KONE_EVENTHANDLER_GET_PRIVATE(eventhandler);
}

static void dispose(GObject *object) {
	kone_eventhandler_stop(ROCCAT_EVENTHANDLER_PLUGIN(object));

	G_OBJECT_CLASS(kone_eventhandler_parent_class)->dispose(object);
}

static void finalize(GObject *object) {
	KoneEventhandlerPrivate *priv = KONE_EVENTHANDLER(object)->priv;

	g_clear_pointer(&priv->profile_note, roccat_notification_profile_free);
	g_clear_pointer(&priv->cpi_note, roccat_notification_cpi_free);
	
	G_OBJECT_CLASS(kone_eventhandler_parent_class)->finalize(object);
}

static void kone_eventhandler_class_init(KoneEventhandlerClass *klass) {
	GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
	gobject_class->dispose = dispose;
	gobject_class->finalize = finalize;

	g_type_class_add_private(klass, sizeof(KoneEventhandlerPrivate));
}

ROCCAT_EVENTHANDLER_PLUGIN_API RoccatEventhandlerPlugin *roccat_eventhandler_plugin_new(void) {
	return ROCCAT_EVENTHANDLER_PLUGIN(g_object_new(KONE_EVENTHANDLER_TYPE, NULL));
}
