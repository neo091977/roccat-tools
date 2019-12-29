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

#include "lua_dbus_server.h"
#include "roccat_eventhandler_plugin.h"
#include "roccat_process_helper.h"
#include "lua.h"
#include "config.h"
#include "i18n-lib.h"
#include "g_roccat_helper.h"
#include <glib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

GType lua_eventhandler_get_type(void);

#define LUA_EVENTHANDLER_TYPE (lua_eventhandler_get_type())
#define LUA_EVENTHANDLER(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), LUA_EVENTHANDLER_TYPE, LuaEventhandler))
#define IS_LUA_EVENTHANDLER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), LUA_EVENTHANDLER_TYPE))
#define LUA_EVENTHANDLER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), LUA_EVENTHANDLER_TYPE, LuaEventhandlerClass))
#define IS_LUA_EVENTHANDLER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), LUA_EVENTHANDLER_TYPE))
#define LUA_EVENTHANDLER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), LUA_EVENTHANDLER_TYPE, LuaEventhandlerPrivate))

typedef struct _LuaEventhandler LuaEventhandler;
typedef struct _LuaEventhandlerClass LuaEventhandlerClass;
typedef struct _LuaEventhandlerPrivate LuaEventhandlerPrivate;

struct _LuaEventhandler {
	GObject parent;
	LuaEventhandlerPrivate *priv;
};

struct _LuaEventhandlerClass {
	GObjectClass parent_class;
};

struct _LuaEventhandlerPrivate {
	RoccatEventhandlerHost *host;
	LuaDBusServer *dbus_server;
	RoccatDeviceScanner *device_scanner;
	RoccatDevice *device;
	gboolean device_set_up;
	RoccatKeyFile *config;
};

static guint lua_eventhandler_get_talk_device(RoccatEventhandlerPlugin *self) {
	return 0;
}

static void open_gui_cb(gpointer source, gpointer user_data) {
	gchar *path;
	path = g_build_path("/", INSTALL_PREFIX, "bin", "roccatluaconfig", (gchar const *)NULL);
	roccat_double_fork(path);
	g_free(path);
}

static void configuration_reload(LuaEventhandler *eventhandler) {
	LuaEventhandlerPrivate *priv = eventhandler->priv;
	if (priv->config)
		lua_configuration_free(priv->config);
	priv->config = lua_configuration_load();
}

static void device_add_cb(RoccatDeviceScannerInterface *interface, RoccatDevice *device, gpointer user_data) {
	LuaEventhandler *eventhandler = LUA_EVENTHANDLER(user_data);
	LuaEventhandlerPrivate *priv = eventhandler->priv;

	if (priv->device) {
		g_critical(_("Multiple devices present."));
		return;
	}

	priv->device = device;
	g_object_ref(G_OBJECT(device));
	roccat_device_debug(device);

	configuration_reload(eventhandler);

	// FIXME create in init and just disconnect/reconnect?
	priv->dbus_server = lua_dbus_server_new();
	g_signal_connect(G_OBJECT(priv->dbus_server), "open-gui", G_CALLBACK(open_gui_cb), eventhandler);
	lua_dbus_server_connect(priv->dbus_server);

	priv->device_set_up = TRUE;

	roccat_eventhandler_plugin_emit_device_added(ROCCAT_EVENTHANDLER_PLUGIN(eventhandler), gaminggear_device_get_product_id(GAMINGGEAR_DEVICE(priv->device)));
}

static void stop(LuaEventhandler *eventhandler) {
	LuaEventhandlerPrivate *priv = eventhandler->priv;

	roccat_eventhandler_plugin_emit_device_removed(ROCCAT_EVENTHANDLER_PLUGIN(eventhandler), gaminggear_device_get_product_id(GAMINGGEAR_DEVICE(priv->device)));

	priv->device_set_up = FALSE;

	g_clear_object(&priv->dbus_server);
	g_clear_object(&priv->device);
	g_clear_pointer(&priv->config, lua_configuration_free);
}

static void device_remove_cb(RoccatDeviceScanner *scanner, RoccatDevice *device, gpointer user_data) {
	LuaEventhandler *eventhandler = LUA_EVENTHANDLER(user_data);
	LuaEventhandlerPrivate *priv = eventhandler->priv;
	if (priv->device && gaminggear_device_equal(GAMINGGEAR_DEVICE(device), GAMINGGEAR_DEVICE(priv->device)))
		stop(eventhandler);
}

static void lua_eventhandler_start(RoccatEventhandlerPlugin *self, RoccatEventhandlerHost *host) {
	LuaEventhandler *eventhandler = LUA_EVENTHANDLER(self);
	LuaEventhandlerPrivate *priv = eventhandler->priv;

	priv->host = host;
	g_object_ref(priv->host);

	priv->device_scanner = lua_device_scanner_new();
	g_signal_connect(G_OBJECT(priv->device_scanner), "device-added", G_CALLBACK(device_add_cb), eventhandler);
	g_signal_connect(G_OBJECT(priv->device_scanner), "device-removed", G_CALLBACK(device_remove_cb), eventhandler);
	roccat_device_scanner_interface_start(ROCCAT_DEVICE_SCANNER_INTERFACE(priv->device_scanner));
}

static void lua_eventhandler_stop(RoccatEventhandlerPlugin *self) {
	LuaEventhandler *eventhandler = LUA_EVENTHANDLER(self);
	LuaEventhandlerPrivate *priv = eventhandler->priv;

	stop(eventhandler);

	g_clear_object(&priv->device_scanner);
	g_clear_object(&priv->host);
}

static void lua_roccat_eventhandler_interface_init(RoccatEventhandlerPluginInterface *iface) {
	iface->start = lua_eventhandler_start;
	iface->stop = lua_eventhandler_stop;
	iface->get_talk_device = lua_eventhandler_get_talk_device;
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

G_DEFINE_TYPE_WITH_CODE(LuaEventhandler, lua_eventhandler, G_TYPE_OBJECT,
		G_IMPLEMENT_INTERFACE(ROCCAT_EVENTHANDLER_PLUGIN_TYPE, lua_roccat_eventhandler_interface_init));

static void lua_eventhandler_init(LuaEventhandler *eventhandler) {
	eventhandler->priv = LUA_EVENTHANDLER_GET_PRIVATE(eventhandler);
}

static void dispose(GObject *object) {
	lua_eventhandler_stop(ROCCAT_EVENTHANDLER_PLUGIN(object));

	G_OBJECT_CLASS(lua_eventhandler_parent_class)->dispose(object);
}

static void lua_eventhandler_class_init(LuaEventhandlerClass *klass) {
	GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
	gobject_class->dispose = dispose;

	g_type_class_add_private(klass, sizeof(LuaEventhandlerPrivate));
}

ROCCAT_EVENTHANDLER_PLUGIN_API RoccatEventhandlerPlugin *roccat_eventhandler_plugin_new(void) {
	return ROCCAT_EVENTHANDLER_PLUGIN(g_object_new(LUA_EVENTHANDLER_TYPE, NULL));
}
