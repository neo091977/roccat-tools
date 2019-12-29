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

#include "roccat_eventhandler_application.h"
#include "roccat_eventhandler_plugin.h"
#include "roccat_eventhandler_host.h"
#include "roccateventhandler_config.h"
#include "roccat_window_monitor.h"
#include "roccat_dbus_server.h"
#include "roccat_talk.h"
#include "config.h"
#include "g_roccat_helper.h"
#include "i18n.h"
#include "roccat.h"
#include <gaminggear/input_events.h>
#include <gmodule.h>
#include <dbus/dbus-glib.h>

#define ROCCAT_EVENTHANDLER_APPLICATION_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), ROCCAT_EVENTHANDLER_APPLICATION_TYPE, RoccatEventhandlerApplicationClass))
#define IS_ROCCAT_EVENTHANDLER_APPLICATION_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), ROCCAT_EVENTHANDLER_APPLICATION_TYPE))
#define ROCCAT_EVENTHANDLER_APPLICATION_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), ROCCAT_EVENTHANDLER_APPLICATION_TYPE, RoccatEventhandlerApplicationPrivate))

typedef struct _RoccatEventhandlerApplicationClass RoccatEventhandlerApplicationClass;
typedef struct _RoccatEventhandlerApplicationPrivate RoccatEventhandlerApplicationPrivate;
typedef struct _PluginListData PluginListData;

struct _RoccatEventhandlerApplication {
	GObject parent;
	RoccatEventhandlerApplicationPrivate *priv;
};

struct _RoccatEventhandlerApplicationClass {
	GObjectClass parent_class;
};

struct _RoccatEventhandlerApplicationPrivate {
	RoccatDBusServer *dbus_server;
	GSList *plugins;
	RoccatNotificator *notificator;
	RoccatWindowMonitor *window_monitor;
	RoccatKeyFile *config;
};

struct _PluginListData {
	GModule *module;
	RoccatEventhandlerPlugin *plugin;
};

static RoccatNotificator *get_notificator(RoccatEventhandlerHost *self) {
	RoccatEventhandlerApplication *app = ROCCAT_EVENTHANDLER_APPLICATION(self);
	RoccatEventhandlerApplicationPrivate *priv = app->priv;
	gchar *driver;

	if (priv->notificator == NULL) {
		driver = roccateventhandler_configuration_get_sound_driver(priv->config);
		priv->notificator = roccat_notificator_init(PROJECT_NAME, driver);
		g_free(driver);
	}

	return priv->notificator;
}

static guint should_set_driver_state(RoccatEventhandlerHost *self) {
	return roccateventhandler_configuration_get_driver_state(ROCCAT_EVENTHANDLER_APPLICATION(self)->priv->config);
}

static void roccat_eventhandler_host_interface_init(RoccatEventhandlerHostInterface *iface) {
	iface->get_notificator = get_notificator;
	iface->should_set_driver_state = should_set_driver_state;
}

G_DEFINE_TYPE_WITH_CODE(RoccatEventhandlerApplication, roccat_eventhandler_application, G_TYPE_OBJECT,
		G_IMPLEMENT_INTERFACE(ROCCAT_EVENTHANDLER_HOST_TYPE, roccat_eventhandler_host_interface_init));

enum {
	ACTIVE_WINDOW_CHANGED,
	LAST_SIGNAL,
};

static guint signals[LAST_SIGNAL] = { 0 };

static gboolean plugin_wanted(RoccatEventhandlerPlugin *plugin, guint wanted) {
	guint is;

	is = roccat_eventhandler_plugin_get_talk_device(plugin);

	if (is == 0)
		return FALSE;

	if (wanted == ROCCAT_TALK_DEVICE_ALL)
		return TRUE;

	if (wanted == is) /* exact match */
		return TRUE;

	if (wanted == roccat_talk_device_get_type(is)) /* type match */
		return TRUE;

	return FALSE;
}

static RoccatEventhandlerPlugin *plugin_get_first_wanted(RoccatEventhandlerApplication *application, guint talk_device) {
	RoccatEventhandlerApplicationPrivate *priv = application->priv;
	GSList *iterator;
	RoccatEventhandlerPlugin *plugin;

	for (iterator = priv->plugins; iterator; iterator = g_slist_next(iterator)) {
		plugin = ((PluginListData *)iterator->data)->plugin;
		if (plugin_wanted(plugin, talk_device))
			return plugin;
	}
	return NULL;
}

/*
 * talk callbacks call FIRST plugin matching talk_device
 * talkfx functions call ALL plugins matching talk_device
 */

static void talk_easyshift_cb(RoccatEventhandlerPlugin *source_plugin, guint talk_device, gboolean state, gpointer user_data) {
	RoccatEventhandlerApplication *application = ROCCAT_EVENTHANDLER_APPLICATION(user_data);
	RoccatEventhandlerPlugin *target_plugin;

	target_plugin = plugin_get_first_wanted(application, talk_device);
	if (target_plugin)
		roccat_eventhandler_plugin_talk_easyshift(target_plugin, state);
}

static void talk_easyshift_lock_cb(RoccatEventhandlerPlugin *source_plugin, guint talk_device, gboolean state, gpointer user_data) {
	RoccatEventhandlerApplication *application = ROCCAT_EVENTHANDLER_APPLICATION(user_data);
	RoccatEventhandlerPlugin *target_plugin;

	target_plugin = plugin_get_first_wanted(application, talk_device);
	if (target_plugin)
		roccat_eventhandler_plugin_talk_easyshift_lock(target_plugin, state);
}

static void talk_easyaim_cb(RoccatEventhandlerPlugin *source_plugin, guint talk_device, guchar state, gpointer user_data) {
	RoccatEventhandlerApplication *application = ROCCAT_EVENTHANDLER_APPLICATION(user_data);
	RoccatEventhandlerPlugin *target_plugin;

	target_plugin = plugin_get_first_wanted(application, talk_device);
	if (target_plugin)
		roccat_eventhandler_plugin_talk_easyaim(target_plugin, state);
}

static void talkfx_set_led_rgb_cb(RoccatDBusServer *dbus_server, guint talk_device, guint effect, guint ambient_color, guint event_color, gpointer user_data) {
	RoccatEventhandlerApplication *application = ROCCAT_EVENTHANDLER_APPLICATION(user_data);
	RoccatEventhandlerApplicationPrivate *priv = application->priv;
	GSList *iterator;
	RoccatEventhandlerPlugin *plugin;

	for (iterator = priv->plugins; iterator; iterator = g_slist_next(iterator)) {
		plugin = ((PluginListData *)iterator->data)->plugin;
		if (plugin_wanted(plugin, talk_device))
			roccat_eventhandler_plugin_talkfx_set_led_rgb(plugin, effect, ambient_color, event_color);
	}
}

static void talkfx_restore_led_rgb_cb(RoccatDBusServer *dbus_server, guint talk_device, gpointer user_data) {
	RoccatEventhandlerApplication *application = ROCCAT_EVENTHANDLER_APPLICATION(user_data);
	RoccatEventhandlerApplicationPrivate *priv = application->priv;
	GSList *iterator;
	RoccatEventhandlerPlugin *plugin;

	for (iterator = priv->plugins; iterator; iterator = g_slist_next(iterator)) {
		plugin = ((PluginListData *)iterator->data)->plugin;
		if (plugin_wanted(plugin, talk_device))
			roccat_eventhandler_plugin_talkfx_restore_led_rgb(plugin);
	}
}

static void talkfx_ryos_set_sdk_mode_cb(RoccatDBusServer *dbus_server, guint talk_device, gboolean state, gpointer user_data) {
	RoccatEventhandlerApplication *application = ROCCAT_EVENTHANDLER_APPLICATION(user_data);
	RoccatEventhandlerApplicationPrivate *priv = application->priv;
	GSList *iterator;
	RoccatEventhandlerPlugin *plugin;

	for (iterator = priv->plugins; iterator; iterator = g_slist_next(iterator)) {
		plugin = ((PluginListData *)iterator->data)->plugin;
		if (plugin_wanted(plugin, talk_device))
			roccat_eventhandler_plugin_talkfx_ryos_set_sdk_mode(plugin, state);
	}
}

static void talkfx_ryos_set_all_leds_cb(RoccatDBusServer *dbus_server, guint talk_device, GArray const *data, gpointer user_data) {
	RoccatEventhandlerApplication *application = ROCCAT_EVENTHANDLER_APPLICATION(user_data);
	RoccatEventhandlerApplicationPrivate *priv = application->priv;
	GSList *iterator;
	RoccatEventhandlerPlugin *plugin;

	for (iterator = priv->plugins; iterator; iterator = g_slist_next(iterator)) {
		plugin = ((PluginListData *)iterator->data)->plugin;
		if (plugin_wanted(plugin, talk_device))
			roccat_eventhandler_plugin_talkfx_ryos_set_all_leds(plugin, data);
	}
}

static void talkfx_ryos_turn_on_all_leds_cb(RoccatDBusServer *dbus_server, guint talk_device, gpointer user_data) {
	RoccatEventhandlerApplication *application = ROCCAT_EVENTHANDLER_APPLICATION(user_data);
	RoccatEventhandlerApplicationPrivate *priv = application->priv;
	GSList *iterator;
	RoccatEventhandlerPlugin *plugin;

	for (iterator = priv->plugins; iterator; iterator = g_slist_next(iterator)) {
		plugin = ((PluginListData *)iterator->data)->plugin;
		if (plugin_wanted(plugin, talk_device))
			roccat_eventhandler_plugin_talkfx_ryos_turn_on_all_leds(plugin);
	}
}

static void talkfx_ryos_turn_off_all_leds_cb(RoccatDBusServer *dbus_server, guint talk_device, gpointer user_data) {
	RoccatEventhandlerApplication *application = ROCCAT_EVENTHANDLER_APPLICATION(user_data);
	RoccatEventhandlerApplicationPrivate *priv = application->priv;
	GSList *iterator;
	RoccatEventhandlerPlugin *plugin;

	for (iterator = priv->plugins; iterator; iterator = g_slist_next(iterator)) {
		plugin = ((PluginListData *)iterator->data)->plugin;
		if (plugin_wanted(plugin, talk_device))
			roccat_eventhandler_plugin_talkfx_ryos_turn_off_all_leds(plugin);
	}
}

static void talkfx_ryos_set_led_on_cb(RoccatDBusServer *dbus_server, guint talk_device, guchar index, gpointer user_data) {
	RoccatEventhandlerApplication *application = ROCCAT_EVENTHANDLER_APPLICATION(user_data);
	RoccatEventhandlerApplicationPrivate *priv = application->priv;
	GSList *iterator;
	RoccatEventhandlerPlugin *plugin;

	for (iterator = priv->plugins; iterator; iterator = g_slist_next(iterator)) {
		plugin = ((PluginListData *)iterator->data)->plugin;
		if (plugin_wanted(plugin, talk_device))
			roccat_eventhandler_plugin_talkfx_ryos_set_led_on(plugin, index);
	}
}

static void talkfx_ryos_set_led_off_cb(RoccatDBusServer *dbus_server, guint talk_device, guchar index, gpointer user_data) {
	RoccatEventhandlerApplication *application = ROCCAT_EVENTHANDLER_APPLICATION(user_data);
	RoccatEventhandlerApplicationPrivate *priv = application->priv;
	GSList *iterator;
	RoccatEventhandlerPlugin *plugin;

	for (iterator = priv->plugins; iterator; iterator = g_slist_next(iterator)) {
		plugin = ((PluginListData *)iterator->data)->plugin;
		if (plugin_wanted(plugin, talk_device))
			roccat_eventhandler_plugin_talkfx_ryos_set_led_off(plugin, index);
	}
}

static void talkfx_ryos_all_key_blinking_cb(RoccatDBusServer *dbus_server, guint talk_device, guint interval, guint count, gpointer user_data) {
	RoccatEventhandlerApplication *application = ROCCAT_EVENTHANDLER_APPLICATION(user_data);
	RoccatEventhandlerApplicationPrivate *priv = application->priv;
	GSList *iterator;
	RoccatEventhandlerPlugin *plugin;

	for (iterator = priv->plugins; iterator; iterator = g_slist_next(iterator)) {
		plugin = ((PluginListData *)iterator->data)->plugin;
		if (plugin_wanted(plugin, talk_device))
			roccat_eventhandler_plugin_talkfx_ryos_all_key_blinking(plugin, interval, count);
	}
}

RoccatEventhandlerApplication *roccat_eventhandler_application_new(void) {
	return ROCCAT_EVENTHANDLER_APPLICATION(g_object_new(ROCCAT_EVENTHANDLER_APPLICATION_TYPE, NULL));
}

static PluginListData *plugin_list_data_new(RoccatEventhandlerApplication *application, gchar const *filename) {
	ROCCAT_EVENTHANDLER_PLUGIN_NEW roccat_eventhandler_plugin_new;
	PluginListData *list_data;

	list_data = g_malloc0(sizeof(PluginListData));

	list_data->module = g_module_open(filename, G_MODULE_BIND_LAZY);
	if (!list_data->module)
		goto exit_1;

	if (!g_module_symbol(list_data->module, ROCCAT_EVENTHANDLER_SYMBOL_PLUGIN_NEW, (gpointer *)&roccat_eventhandler_plugin_new))
		goto exit_2;

	list_data->plugin = roccat_eventhandler_plugin_new();
	if (!list_data->plugin)
		goto exit_2;

	return list_data;
exit_2:
	g_module_close(list_data->module);
exit_1:
	g_free(list_data);
	return NULL;
}

static void plugin_list_data_free(gpointer data) {
	PluginListData *list_data = (PluginListData *)data;
	roccat_eventhandler_plugin_stop(list_data->plugin);
	g_object_unref(list_data->plugin);
	g_module_close(list_data->module);
	g_free(list_data);
}

static gboolean append_plugins(RoccatEventhandlerApplication *application) {
	RoccatEventhandlerApplicationPrivate *priv = application->priv;
	GError *error = NULL;
	GDir *dir;
	GPatternSpec *pattern;
	gchar const *name;
	gchar *abs_name;
	PluginListData *list_data;

	if (!g_module_supported())
		return FALSE;

	dir = g_dir_open(EVENTHANDLER_PLUGIN_DIR, 0, &error);
	if (error) {
		g_clear_error(&error);
		return FALSE;
	}

	pattern = g_pattern_spec_new("lib*eventhandler.so");
	while ((name = g_dir_read_name(dir))) {
		if (g_pattern_match_string(pattern, name)) {
			abs_name = g_build_filename(EVENTHANDLER_PLUGIN_DIR, name, NULL);
			list_data = plugin_list_data_new(application, abs_name);
			g_free(abs_name);

			if (list_data) {
				priv->plugins = g_slist_prepend(priv->plugins, list_data);
				g_signal_connect(G_OBJECT(list_data->plugin), "talk-easyshift", G_CALLBACK(talk_easyshift_cb), application);
				g_signal_connect(G_OBJECT(list_data->plugin), "talk-easyshift-lock", G_CALLBACK(talk_easyshift_lock_cb), application);
				g_signal_connect(G_OBJECT(list_data->plugin), "talk-easyaim", G_CALLBACK(talk_easyaim_cb), application);

				roccat_eventhandler_plugin_start(list_data->plugin, ROCCAT_EVENTHANDLER_HOST(application));
			}
		}
	}
	g_pattern_spec_free(pattern);
	g_dir_close(dir);

	return TRUE;
}

static void active_window_change_cb(RoccatWindowMonitor *monitor, gchar const *title, gpointer user_data) {
	RoccatEventhandlerHost *host = ROCCAT_EVENTHANDLER_HOST(user_data);
	g_signal_emit((gpointer)host, signals[ACTIVE_WINDOW_CHANGED], 0, title);
}

static void roccat_eventhandler_application_init(RoccatEventhandlerApplication *application) {
	RoccatEventhandlerApplicationPrivate *priv = ROCCAT_EVENTHANDLER_APPLICATION_GET_PRIVATE(application);
	GError *local_error = NULL;

	application->priv = priv;

	priv->notificator = NULL;
	priv->plugins = NULL;

	if (!gaminggear_input_event_init(USB_VENDOR_ID_ROCCAT, USB_DEVICE_ID_ROCCAT_SOFTWARE, &local_error)) {
		g_warning(_("Could not init input event handling: %s"), local_error->message);
		g_clear_error(&local_error);
	}

	priv->config = roccat_configuration_load();

	priv->dbus_server = roccat_dbus_server_new();
	roccat_dbus_server_connect(priv->dbus_server);
	g_signal_connect(G_OBJECT(priv->dbus_server), "talkfx-set-led-rgb", G_CALLBACK(talkfx_set_led_rgb_cb), application);
	g_signal_connect(G_OBJECT(priv->dbus_server), "talkfx-restore-led-rgb", G_CALLBACK(talkfx_restore_led_rgb_cb), application);
	g_signal_connect(G_OBJECT(priv->dbus_server), "talkfx-ryos-set-sdk-mode", G_CALLBACK(talkfx_ryos_set_sdk_mode_cb), application);
	g_signal_connect(G_OBJECT(priv->dbus_server), "talkfx-ryos-set-all-leds", G_CALLBACK(talkfx_ryos_set_all_leds_cb), application);
	g_signal_connect(G_OBJECT(priv->dbus_server), "talkfx-ryos-turn-on-all-leds", G_CALLBACK(talkfx_ryos_turn_on_all_leds_cb), application);
	g_signal_connect(G_OBJECT(priv->dbus_server), "talkfx-ryos-turn-off-all-leds", G_CALLBACK(talkfx_ryos_turn_off_all_leds_cb), application);
	g_signal_connect(G_OBJECT(priv->dbus_server), "talkfx-ryos-set-led-on", G_CALLBACK(talkfx_ryos_set_led_on_cb), application);
	g_signal_connect(G_OBJECT(priv->dbus_server), "talkfx-ryos-set-led-off", G_CALLBACK(talkfx_ryos_set_led_off_cb), application);
	g_signal_connect(G_OBJECT(priv->dbus_server), "talkfx-ryos-all-key-blinking", G_CALLBACK(talkfx_ryos_all_key_blinking_cb), application);

	priv->window_monitor = roccat_window_monitor_new();
	g_signal_connect(G_OBJECT(priv->window_monitor), "active-window-changed", G_CALLBACK(active_window_change_cb), application);

	roccat_window_monitor_set_interval(priv->window_monitor, roccateventhandler_configuration_get_scan_interval(priv->config));

	append_plugins(application);
}

static void finalize(GObject *object) {
	RoccatEventhandlerApplicationPrivate *priv = ROCCAT_EVENTHANDLER_APPLICATION(object)->priv;
	GError *local_error = NULL;

	g_clear_object(&priv->dbus_server);

	g_slist_free_full(priv->plugins, plugin_list_data_free);
	priv->plugins = NULL;

	g_clear_pointer(&priv->notificator, roccat_notificator_deinit);
	g_clear_pointer(&priv->config, roccat_configuration_free);

	if (!gaminggear_input_event_deinit(&local_error)) {
		g_warning(_("Could not deinit input event handling: %s"), local_error->message);
		g_clear_error(&local_error);
	}

	G_OBJECT_CLASS(roccat_eventhandler_application_parent_class)->finalize(object);
}

static void roccat_eventhandler_application_class_init(RoccatEventhandlerApplicationClass *klass) {
	GObjectClass *gobject_class;

	gobject_class = G_OBJECT_CLASS(klass);

	gobject_class->finalize = finalize;

	g_type_class_add_private(klass, sizeof(RoccatEventhandlerApplicationPrivate));

	signals[ACTIVE_WINDOW_CHANGED] = g_signal_new("active-window-changed", ROCCAT_EVENTHANDLER_HOST_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__STRING, G_TYPE_NONE,
			1, G_TYPE_STRING);
}
