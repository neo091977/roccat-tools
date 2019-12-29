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

#include "roccat_eventhandler_plugin.h"
#include "g_cclosure_roccat_marshaller.h"

enum {
	TALK_EASYSHIFT,
	TALK_EASYSHIFT_LOCK,
	TALK_EASYAIM,
	DEVICE_ADDED,
	DEVICE_REMOVED,
	LAST_SIGNAL,
};

static guint signals[LAST_SIGNAL] = { 0 };

void roccat_eventhandler_plugin_start(RoccatEventhandlerPlugin *self, RoccatEventhandlerHost *host) {
	RoccatEventhandlerPluginInterface *interface = ROCCAT_EVENTHANDLER_PLUGIN_GET_INTERFACE(self);
	if (interface->start) interface->start(self, host);
}

void roccat_eventhandler_plugin_stop(RoccatEventhandlerPlugin *self) {
	RoccatEventhandlerPluginInterface *interface = ROCCAT_EVENTHANDLER_PLUGIN_GET_INTERFACE(self);
	if (interface->stop) interface->stop(self);
}

guint roccat_eventhandler_plugin_get_talk_device(RoccatEventhandlerPlugin *self) {
	RoccatEventhandlerPluginInterface *interface = ROCCAT_EVENTHANDLER_PLUGIN_GET_INTERFACE(self);
	return interface->get_talk_device ? interface->get_talk_device(self) : 0;
}

void roccat_eventhandler_plugin_talk_easyshift(RoccatEventhandlerPlugin *self, gboolean state) {
	RoccatEventhandlerPluginInterface *interface = ROCCAT_EVENTHANDLER_PLUGIN_GET_INTERFACE(self);
	if (interface->talk_easyshift) interface->talk_easyshift(self, state);
}

void roccat_eventhandler_plugin_talk_easyshift_lock(RoccatEventhandlerPlugin *self, gboolean state) {
	RoccatEventhandlerPluginInterface *interface = ROCCAT_EVENTHANDLER_PLUGIN_GET_INTERFACE(self);
	if (interface->talk_easyshift_lock) interface->talk_easyshift_lock(self, state);
}

void roccat_eventhandler_plugin_talk_easyaim(RoccatEventhandlerPlugin *self, guchar state) {
	RoccatEventhandlerPluginInterface *interface = ROCCAT_EVENTHANDLER_PLUGIN_GET_INTERFACE(self);
	if (interface->talk_easyaim) interface->talk_easyaim(self, state);
}

void roccat_eventhandler_plugin_talkfx_set_led_rgb(RoccatEventhandlerPlugin *self, guint32 effect, guint32 ambient_color, guint32 event_color) {
	RoccatEventhandlerPluginInterface *interface = ROCCAT_EVENTHANDLER_PLUGIN_GET_INTERFACE(self);
	if (interface->talkfx_set_led_rgb) interface->talkfx_set_led_rgb(self, effect, ambient_color, event_color);
}

void roccat_eventhandler_plugin_talkfx_ryos_set_sdk_mode(RoccatEventhandlerPlugin *self, gboolean state) {
	RoccatEventhandlerPluginInterface *interface = ROCCAT_EVENTHANDLER_PLUGIN_GET_INTERFACE(self);
	if (interface->talkfx_ryos_set_sdk_mode) interface->talkfx_ryos_set_sdk_mode(self, state);
}

void roccat_eventhandler_plugin_talkfx_ryos_set_all_leds(RoccatEventhandlerPlugin *self, GArray const *data) {
	RoccatEventhandlerPluginInterface *interface = ROCCAT_EVENTHANDLER_PLUGIN_GET_INTERFACE(self);
	if (interface->talkfx_ryos_set_all_leds) interface->talkfx_ryos_set_all_leds(self, data);
}

void roccat_eventhandler_plugin_talkfx_ryos_turn_on_all_leds(RoccatEventhandlerPlugin *self) {
	RoccatEventhandlerPluginInterface *interface = ROCCAT_EVENTHANDLER_PLUGIN_GET_INTERFACE(self);
	if (interface->talkfx_ryos_turn_on_all_leds) interface->talkfx_ryos_turn_on_all_leds(self);
}

void roccat_eventhandler_plugin_talkfx_ryos_turn_off_all_leds(RoccatEventhandlerPlugin *self) {
	RoccatEventhandlerPluginInterface *interface = ROCCAT_EVENTHANDLER_PLUGIN_GET_INTERFACE(self);
	if (interface->talkfx_ryos_turn_off_all_leds) interface->talkfx_ryos_turn_off_all_leds(self);
}

void roccat_eventhandler_plugin_talkfx_ryos_set_led_on(RoccatEventhandlerPlugin *self, guchar index) {
	RoccatEventhandlerPluginInterface *interface = ROCCAT_EVENTHANDLER_PLUGIN_GET_INTERFACE(self);
	if (interface->talkfx_ryos_set_led_on) interface->talkfx_ryos_set_led_on(self, index);
}

void roccat_eventhandler_plugin_talkfx_ryos_set_led_off(RoccatEventhandlerPlugin *self, guchar index) {
	RoccatEventhandlerPluginInterface *interface = ROCCAT_EVENTHANDLER_PLUGIN_GET_INTERFACE(self);
	if (interface->talkfx_ryos_set_led_off) interface->talkfx_ryos_set_led_off(self, index);
}

void roccat_eventhandler_plugin_talkfx_ryos_all_key_blinking(RoccatEventhandlerPlugin *self, guint interval, guint count) {
	RoccatEventhandlerPluginInterface *interface = ROCCAT_EVENTHANDLER_PLUGIN_GET_INTERFACE(self);
	if (interface->talkfx_ryos_all_key_blinking) interface->talkfx_ryos_all_key_blinking(self, interval, count);
}

void roccat_eventhandler_plugin_talkfx_restore_led_rgb(RoccatEventhandlerPlugin *self) {
	RoccatEventhandlerPluginInterface *interface = ROCCAT_EVENTHANDLER_PLUGIN_GET_INTERFACE(self);
	if (interface->talkfx_restore_led_rgb) interface->talkfx_restore_led_rgb(self);
}

void roccat_eventhandler_plugin_emit_talk_easyshift(RoccatEventhandlerPlugin *self, guint talk_device, gboolean state) {
	g_signal_emit((gpointer)self, signals[TALK_EASYSHIFT], 0, talk_device, state);
}

void roccat_eventhandler_plugin_emit_talk_easyshift_lock(RoccatEventhandlerPlugin *self, guint talk_device, gboolean state) {
	g_signal_emit((gpointer)self, signals[TALK_EASYSHIFT_LOCK], 0, talk_device, state);
}

void roccat_eventhandler_plugin_emit_talk_easyaim(RoccatEventhandlerPlugin *self, guint talk_device, guchar state) {
	g_signal_emit((gpointer)self, signals[TALK_EASYAIM], 0, talk_device, state);
}

void roccat_eventhandler_plugin_emit_device_added(RoccatEventhandlerPlugin *self, guint product_id) {
	g_signal_emit((gpointer)self, signals[DEVICE_ADDED], 0, product_id);
}

void roccat_eventhandler_plugin_emit_device_removed(RoccatEventhandlerPlugin *self, guint product_id) {
	g_signal_emit((gpointer)self, signals[DEVICE_REMOVED], 0, product_id);
}

G_DEFINE_INTERFACE(RoccatEventhandlerPlugin, roccat_eventhandler_plugin, G_TYPE_OBJECT);

static void roccat_eventhandler_plugin_default_init(RoccatEventhandlerPluginInterface *iface) {
	signals[TALK_EASYSHIFT] = g_signal_new("talk-easyshift", ROCCAT_EVENTHANDLER_PLUGIN_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_roccat_marshal_VOID__UINT_BOOLEAN, G_TYPE_NONE, 2, G_TYPE_UINT, G_TYPE_BOOLEAN);

	signals[TALK_EASYSHIFT_LOCK] = g_signal_new("talk-easyshift-lock", ROCCAT_EVENTHANDLER_PLUGIN_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_roccat_marshal_VOID__UINT_BOOLEAN, G_TYPE_NONE, 2, G_TYPE_UINT, G_TYPE_BOOLEAN);

	signals[TALK_EASYAIM] = g_signal_new("talk-easyaim", ROCCAT_EVENTHANDLER_PLUGIN_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_roccat_marshal_VOID__UINT_UCHAR, G_TYPE_NONE, 2, G_TYPE_UINT, G_TYPE_UCHAR);

	signals[DEVICE_ADDED] = g_signal_new("device-added", ROCCAT_EVENTHANDLER_PLUGIN_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UINT, G_TYPE_NONE, 1, G_TYPE_UINT);

	signals[DEVICE_REMOVED] = g_signal_new("device-removed", ROCCAT_EVENTHANDLER_PLUGIN_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UINT, G_TYPE_NONE, 1, G_TYPE_UINT);
}
