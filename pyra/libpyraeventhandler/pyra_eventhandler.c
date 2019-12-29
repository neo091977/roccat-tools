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
#include "pyra_eventhandler_channel.h"
#include "pyra_dbus_server.h"
#include "roccat_eventhandler_plugin.h"
#include "roccat_notification_cpi.h"
#include "roccat_notification_profile.h"
#include "roccat_process_helper.h"
#include "pyra.h"
#include "config.h"
#include "i18n-lib.h"
#include "g_roccat_helper.h"
#include <glib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

GType pyra_eventhandler_get_type(void);

#define PYRA_EVENTHANDLER_TYPE (pyra_eventhandler_get_type())
#define PYRA_EVENTHANDLER(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), PYRA_EVENTHANDLER_TYPE, PyraEventhandler))
#define IS_PYRA_EVENTHANDLER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), PYRA_EVENTHANDLER_TYPE))
#define PYRA_EVENTHANDLER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), PYRA_EVENTHANDLER_TYPE, PyraEventhandlerClass))
#define IS_PYRA_EVENTHANDLER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), PYRA_EVENTHANDLER_TYPE))
#define PYRA_EVENTHANDLER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), PYRA_EVENTHANDLER_TYPE, PyraEventhandlerPrivate))

typedef struct _PyraEventhandler PyraEventhandler;
typedef struct _PyraEventhandlerClass PyraEventhandlerClass;
typedef struct _PyraEventhandlerPrivate PyraEventhandlerPrivate;

struct _PyraEventhandler {
	GObject parent;
	PyraEventhandlerPrivate *priv;
};

struct _PyraEventhandlerClass {
	GObjectClass parent_class;
};

struct _PyraEventhandlerPrivate {
	RoccatEventhandlerHost *host;
	PyraDBusServer *dbus_server;
	RoccatDeviceScanner *device_scanner;
	RoccatDevice *device;
	gboolean device_set_up;
	RoccatKeyFile *config;
	guint actual_profile_index;
	PyraRMP *rmp[PYRA_PROFILE_NUM];
	PyraEventhandlerChannel *channel;
	gulong active_window_changed_handler;
	
	/*
	 * FIXME notificator für Fehlermeldungen verwenden,
	 * notificator refaktorisieren und Basis für beides verwenden oder
	 * Eigene Implementierung eines Fehlernotificators
	 */
	RoccatNotificationProfile *profile_note;
	RoccatNotificationCpi *cpi_note;

	GaminggearMacroThreads *threads;
	PyraRMPMacroKeyInfo *active_shortcuts[PYRA_BUTTONS_NUM];
};

static void profile_changed(PyraEventhandler *eventhandler, guint profile_number) {
	PyraEventhandlerPrivate *priv = eventhandler->priv;
	guint profile_index = profile_number - 1;

	priv->actual_profile_index = profile_index;

	roccat_notification_profile_update(priv->profile_note, pyra_configuration_get_profile_notification_type(priv->config),
			pyra_configuration_get_notification_volume(priv->config),
			profile_number, pyra_rmp_get_profile_name(priv->rmp[profile_index]));

	pyra_dbus_server_emit_profile_changed(priv->dbus_server, profile_number);
}

static gboolean set_profile(PyraEventhandler *eventhandler, guint profile_number) {
	PyraEventhandlerPrivate *priv = eventhandler->priv;
	guint profile_index;
	GError *local_error = NULL;

	if (profile_number == 0)
		return TRUE;

	profile_index = profile_number - 1;

	if (priv->actual_profile_index != profile_index) {
		if (!pyra_actual_profile_write(priv->device, profile_index, &local_error)) {
			g_warning(_("Could not activate profile %i: %s"), profile_number, local_error->message);
			g_clear_error(&local_error);
			return FALSE;
		}
		profile_changed(eventhandler, profile_number);
	}
	return TRUE;
}

static void window_changed_cb(RoccatEventhandlerHost *host, gchar const *title, gpointer user_data) {
	PyraEventhandler *eventhandler = PYRA_EVENTHANDLER(user_data);
	PyraEventhandlerPrivate *priv = eventhandler->priv;
	guint profile_index;
	guint game_file_index;
	gchar *pattern;

	for (profile_index = 0; profile_index < PYRA_PROFILE_NUM; ++profile_index) {

		if (priv->rmp[profile_index] == NULL)
			continue;

		for (game_file_index = 0; game_file_index < PYRA_GAMEFILE_NUM; ++game_file_index) {
			pattern = pyra_rmp_get_game_file_name(priv->rmp[profile_index], game_file_index);
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

	set_profile(eventhandler, pyra_configuration_get_default_profile_number(priv->config));
	return;
}

static guint pyra_eventhandler_get_talk_device(RoccatEventhandlerPlugin *self) {
	return 0;
}

static void open_gui_cb(gpointer source, gpointer user_data) {
	gchar *path;
	path = g_build_path("/", INSTALL_PREFIX, "bin", "roccatpyraconfig", (gchar const *)NULL);
	roccat_double_fork(path);
	g_free(path);
}

static void profile_data_changed_outside_cb(PyraDBusServer *server, guchar number, gpointer user_data) {
	PyraEventhandler *eventhandler = PYRA_EVENTHANDLER(user_data);
	PyraEventhandlerPrivate *priv = eventhandler->priv;
	guint profile_index = number - 1;

	/* Just load rmp without updating data from hardware */
	pyra_rmp_free(priv->rmp[profile_index]);
	priv->rmp[profile_index] = pyra_rmp_load_actual(profile_index);
}

static void configuration_reload(PyraEventhandler *eventhandler) {
	PyraEventhandlerPrivate *priv = eventhandler->priv;
	if (priv->config)
		pyra_configuration_free(priv->config);
	priv->config = pyra_configuration_load();
}

static void configuration_changed_outside_cb(PyraDBusServer *server, gpointer user_data) {
	PyraEventhandler *eventhandler = PYRA_EVENTHANDLER(user_data);
	configuration_reload(eventhandler);
}

static void cpi_changed_cb(PyraEventhandlerChannel *channel, guchar cpi_raw, gpointer user_data) {
	PyraEventhandler *eventhandler = PYRA_EVENTHANDLER(user_data);
	PyraEventhandlerPrivate *priv = eventhandler->priv;
	guint cpi_value;
	
	cpi_value = pyra_cpi_raw_to_cpi(cpi_raw);
	roccat_notification_cpi_update(priv->cpi_note,
			pyra_configuration_get_cpi_notification_type(priv->config),
			pyra_configuration_get_notification_volume(priv->config),
			cpi_value, cpi_value);
}

static void profile_changed_cb(gpointer source, guchar profile_number, gpointer user_data) {
	PyraEventhandler *eventhandler = PYRA_EVENTHANDLER(user_data);
	profile_changed(eventhandler, profile_number);
}

static void macro_cb(PyraEventhandlerChannel *channel, guchar key_number, gpointer user_data) {
	PyraEventhandler *eventhandler = PYRA_EVENTHANDLER(user_data);
	PyraEventhandlerPrivate *priv = eventhandler->priv;
	PyraRMPMacroKeyInfo *macro_key_info;
	GaminggearMacro *gaminggear_macro;
	
	macro_key_info = pyra_rmp_get_macro_key_info(priv->rmp[priv->actual_profile_index], key_number - 1);
	gaminggear_macro = pyra_rmp_macro_key_info_to_gaminggear_macro(macro_key_info);
	g_free(macro_key_info);
	gaminggear_play_macro_threaded(priv->threads, key_number - 1, gaminggear_macro);
	gaminggear_macro_free(gaminggear_macro);
}

static void shortcut_press_cb(PyraEventhandlerChannel *channel, guchar key_number, gpointer user_data) {
	PyraEventhandlerPrivate *priv = PYRA_EVENTHANDLER(user_data)->priv;
	guint key_index = key_number - 1;

	if (priv->active_shortcuts[key_index]) {
		g_warning(_("There is already an active shortcut for key index %u"), key_index);
		return;
	}

	priv->active_shortcuts[key_index] = pyra_rmp_get_macro_key_info(priv->rmp[priv->actual_profile_index], key_number - 1);
	pyra_play_shortcut_press(priv->active_shortcuts[key_index]);
}

static void shortcut_release(PyraEventhandler *eventhandler, guint key_index) {
	PyraEventhandlerPrivate *priv = eventhandler->priv;

	if (!priv->active_shortcuts[key_index]) {
		g_warning(_("There is no active shortcut for key index %u"), key_index);
		return;
	}

	pyra_play_shortcut_release(priv->active_shortcuts[key_index]);
	g_clear_pointer(&priv->active_shortcuts[key_index], g_free);
}

static void shortcut_release_cb(PyraEventhandlerChannel *channel, guchar key_number, gpointer user_data) {
	shortcut_release(PYRA_EVENTHANDLER(user_data), key_number - 1);
}

static void quicklaunch_cb(PyraEventhandlerChannel *channel, guchar key_number, gpointer user_data) {
	PyraEventhandler *eventhandler = PYRA_EVENTHANDLER(user_data);
	PyraEventhandlerPrivate *priv = eventhandler->priv;
	PyraRMPMacroKeyInfo *macro_key_info;

	macro_key_info = pyra_rmp_get_macro_key_info(priv->rmp[priv->actual_profile_index], key_number - 1);
	pyra_play_quicklaunch_forked(macro_key_info);
	g_free(macro_key_info);
}

static gboolean profile_data_load(PyraEventhandler *eventhandler, GError **error) {
	PyraEventhandlerPrivate *priv = eventhandler->priv;
	guint i;

	for (i = 0; i < PYRA_PROFILE_NUM; ++i) {
		priv->rmp[i] = pyra_rmp_load(priv->device, i, error);
		if (priv->rmp[i] == NULL)
			return FALSE;
	}
	return TRUE;
}

static void device_add_cb(RoccatDeviceScannerInterface *interface, RoccatDevice *device, gpointer user_data) {
	PyraEventhandler *eventhandler = PYRA_EVENTHANDLER(user_data);
	PyraEventhandlerPrivate *priv = eventhandler->priv;
	GError *error = NULL;
	guint i;

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
		priv->profile_note = roccat_notification_profile_new(roccat_eventhandler_host_get_notificator(priv->host), PYRA_DEVICE_NAME);
	if (!priv->cpi_note)
		priv->cpi_note = roccat_notification_cpi_new(roccat_eventhandler_host_get_notificator(priv->host), PYRA_DEVICE_NAME);
	
	profile_data_load(eventhandler, &error);
	if (error) {
		g_warning(_("Could not read rmp: %s"), error->message);
		g_clear_error(&error);
	}

	priv->actual_profile_index = pyra_actual_profile_read(priv->device, &error);
	if (error) {
		g_warning(_("Could not read actual profile: %s"), error->message);
		g_clear_error(&error);
	}
	
	// FIXME create in init and just disconnect/reconnect?
	priv->dbus_server = pyra_dbus_server_new();
	g_signal_connect(G_OBJECT(priv->dbus_server), "open-gui", G_CALLBACK(open_gui_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->dbus_server), "profile-changed-outside", G_CALLBACK(profile_changed_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->dbus_server), "profile-data-changed-outside", G_CALLBACK(profile_data_changed_outside_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->dbus_server), "configuration-changed-outside", G_CALLBACK(configuration_changed_outside_cb), eventhandler);
	pyra_dbus_server_connect(priv->dbus_server);

	priv->active_window_changed_handler = g_signal_connect(G_OBJECT(priv->host), "active-window-changed", G_CALLBACK(window_changed_cb), eventhandler);

	priv->threads = gaminggear_macro_threads_new(PYRA_BUTTONS_NUM);
	
	for (i = 0; i < PYRA_BUTTONS_NUM; ++i)
		priv->active_shortcuts[i] = NULL;

	pyra_eventhandler_channel_start(priv->channel, priv->device, &error);
	if (error) {
		g_warning(_("Could not start chardev eventhandler: %s"), error->message);
		g_clear_error(&error);
	}

	priv->device_set_up = TRUE;

	roccat_eventhandler_plugin_emit_device_added(ROCCAT_EVENTHANDLER_PLUGIN(eventhandler), gaminggear_device_get_product_id(GAMINGGEAR_DEVICE(priv->device)));
}

static void stop(PyraEventhandler *eventhandler) {
	PyraEventhandlerPrivate *priv = eventhandler->priv;
	guint i;

	roccat_eventhandler_plugin_emit_device_removed(ROCCAT_EVENTHANDLER_PLUGIN(eventhandler), gaminggear_device_get_product_id(GAMINGGEAR_DEVICE(priv->device)));

	priv->device_set_up = FALSE;

	g_clear_object(&priv->dbus_server);
	pyra_eventhandler_channel_stop(priv->channel);
	g_signal_handler_disconnect(G_OBJECT(priv->host), priv->active_window_changed_handler);
	g_clear_pointer(&priv->threads, gaminggear_macro_threads_free);
	
	for (i = 0; i < PYRA_BUTTONS_NUM; ++i) {
		if (priv->active_shortcuts[i])
			shortcut_release(eventhandler, i);
	}

	g_clear_object(&priv->device);
	g_clear_pointer(&priv->config, pyra_configuration_free);

	for (i = 0; i < PYRA_PROFILE_NUM; ++i)
		g_clear_pointer(&priv->rmp[i], pyra_rmp_free);
}

static void device_remove_cb(RoccatDeviceScanner *scanner, RoccatDevice *device, gpointer user_data) {
	PyraEventhandler *eventhandler = PYRA_EVENTHANDLER(user_data);
	PyraEventhandlerPrivate *priv = eventhandler->priv;
	if (priv->device && gaminggear_device_equal(GAMINGGEAR_DEVICE(device), GAMINGGEAR_DEVICE(priv->device)))
		stop(eventhandler);
}

static void pyra_eventhandler_start(RoccatEventhandlerPlugin *self, RoccatEventhandlerHost *host) {
	PyraEventhandler *eventhandler = PYRA_EVENTHANDLER(self);
	PyraEventhandlerPrivate *priv = eventhandler->priv;

	priv->host = host;
	g_object_ref(priv->host);

	priv->channel = pyra_eventhandler_channel_new();
	g_signal_connect(G_OBJECT(priv->channel), "cpi-changed", G_CALLBACK(cpi_changed_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->channel), "profile-changed", G_CALLBACK(profile_changed_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->channel), "quicklaunch", G_CALLBACK(quicklaunch_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->channel), "macro", G_CALLBACK(macro_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->channel), "shortcut-press", G_CALLBACK(shortcut_press_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->channel), "shortcut-release", G_CALLBACK(shortcut_release_cb), eventhandler);

	priv->device_scanner = pyra_device_scanner_new();
	g_signal_connect(G_OBJECT(priv->device_scanner), "device-added", G_CALLBACK(device_add_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->device_scanner), "device-removed", G_CALLBACK(device_remove_cb), eventhandler);
	roccat_device_scanner_interface_start(ROCCAT_DEVICE_SCANNER_INTERFACE(priv->device_scanner));
}

static void pyra_eventhandler_stop(RoccatEventhandlerPlugin *self) {
	PyraEventhandler *eventhandler = PYRA_EVENTHANDLER(self);
	PyraEventhandlerPrivate *priv = eventhandler->priv;

	stop(eventhandler);

	g_clear_object(&priv->device_scanner);
	g_clear_object(&priv->channel);
	g_clear_object(&priv->host);
}

static void pyra_roccat_eventhandler_interface_init(RoccatEventhandlerPluginInterface *iface) {
	iface->start = pyra_eventhandler_start;
	iface->stop = pyra_eventhandler_stop;
	iface->get_talk_device = pyra_eventhandler_get_talk_device;
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

G_DEFINE_TYPE_WITH_CODE(PyraEventhandler, pyra_eventhandler, G_TYPE_OBJECT,
		G_IMPLEMENT_INTERFACE(ROCCAT_EVENTHANDLER_PLUGIN_TYPE, pyra_roccat_eventhandler_interface_init));

static void pyra_eventhandler_init(PyraEventhandler *eventhandler) {
	eventhandler->priv = PYRA_EVENTHANDLER_GET_PRIVATE(eventhandler);
}

static void dispose(GObject *object) {
	pyra_eventhandler_stop(ROCCAT_EVENTHANDLER_PLUGIN(object));

	G_OBJECT_CLASS(pyra_eventhandler_parent_class)->dispose(object);
}

static void finalize(GObject *object) {
	PyraEventhandlerPrivate *priv = PYRA_EVENTHANDLER(object)->priv;

	g_clear_pointer(&priv->profile_note, roccat_notification_profile_free);
	g_clear_pointer(&priv->cpi_note, roccat_notification_cpi_free);
	
	G_OBJECT_CLASS(pyra_eventhandler_parent_class)->finalize(object);
}

static void pyra_eventhandler_class_init(PyraEventhandlerClass *klass) {
	GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
	gobject_class->dispose = dispose;
	gobject_class->finalize = finalize;

	g_type_class_add_private(klass, sizeof(PyraEventhandlerPrivate));
}

ROCCAT_EVENTHANDLER_PLUGIN_API RoccatEventhandlerPlugin *roccat_eventhandler_plugin_new(void) {
	return ROCCAT_EVENTHANDLER_PLUGIN(g_object_new(PYRA_EVENTHANDLER_TYPE, NULL));
}
