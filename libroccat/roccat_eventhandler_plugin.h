#ifndef __ROCCAT_EVENTHANDLER_PLUGIN_H__
#define __ROCCAT_EVENTHANDLER_PLUGIN_H__

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

#include "roccat_eventhandler_host.h"
#include <glib-object.h>

G_BEGIN_DECLS

#define ROCCAT_EVENTHANDLER_PLUGIN_API __attribute__((visibility("default")))

#define ROCCAT_EVENTHANDLER_PLUGIN_TYPE (roccat_eventhandler_plugin_get_type())
#define ROCCAT_EVENTHANDLER_PLUGIN(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), ROCCAT_EVENTHANDLER_PLUGIN_TYPE, RoccatEventhandlerPlugin))
#define IS_ROCCAT_EVENTHANDLER_PLUGIN(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), ROCCAT_EVENTHANDLER_PLUGIN_TYPE))
#define ROCCAT_EVENTHANDLER_PLUGIN_GET_INTERFACE(inst) (G_TYPE_INSTANCE_GET_INTERFACE((inst), ROCCAT_EVENTHANDLER_PLUGIN_TYPE, RoccatEventhandlerPluginInterface))

typedef struct _RoccatEventhandlerPlugin RoccatEventhandlerPlugin;
typedef struct _RoccatEventhandlerPluginInterface RoccatEventhandlerPluginInterface;

struct _RoccatEventhandlerPluginInterface {
	GTypeInterface parent;

	void (*start)(RoccatEventhandlerPlugin *self, RoccatEventhandlerHost *host);
	void (*stop)(RoccatEventhandlerPlugin *self);

	guint (*get_talk_device)(RoccatEventhandlerPlugin *self);
	void (*talk_easyshift)(RoccatEventhandlerPlugin *self, gboolean state);
	void (*talk_easyshift_lock)(RoccatEventhandlerPlugin *self, gboolean state);
	void (*talk_easyaim)(RoccatEventhandlerPlugin *self, guchar state);

	void (*talkfx_set_led_rgb)(RoccatEventhandlerPlugin *self, guint32 effect, guint32 ambient_color, guint32 event_color);
	void (*talkfx_restore_led_rgb)(RoccatEventhandlerPlugin *self);

	void (*talkfx_ryos_set_sdk_mode)(RoccatEventhandlerPlugin *self, gboolean state);
	void (*talkfx_ryos_set_all_leds)(RoccatEventhandlerPlugin *self, GArray const *data);
	void (*talkfx_ryos_turn_on_all_leds)(RoccatEventhandlerPlugin *self);
	void (*talkfx_ryos_turn_off_all_leds)(RoccatEventhandlerPlugin *self);
	void (*talkfx_ryos_set_led_on)(RoccatEventhandlerPlugin *self, guchar index);
	void (*talkfx_ryos_set_led_off)(RoccatEventhandlerPlugin *self, guchar index);
	void (*talkfx_ryos_all_key_blinking)(RoccatEventhandlerPlugin *self, guint interval, guint count);
};

GType roccat_eventhandler_plugin_get_type(void);

void roccat_eventhandler_plugin_start(RoccatEventhandlerPlugin *self, RoccatEventhandlerHost *host);
void roccat_eventhandler_plugin_stop(RoccatEventhandlerPlugin *self);

guint roccat_eventhandler_plugin_get_talk_device(RoccatEventhandlerPlugin *self);

void roccat_eventhandler_plugin_talk_easyshift(RoccatEventhandlerPlugin *self, gboolean state);
void roccat_eventhandler_plugin_talk_easyshift_lock(RoccatEventhandlerPlugin *self, gboolean state);
void roccat_eventhandler_plugin_talk_easyaim(RoccatEventhandlerPlugin *self, guchar state);

void roccat_eventhandler_plugin_talkfx_set_led_rgb(RoccatEventhandlerPlugin *self, guint32 effect, guint32 ambient_color, guint32 event_color);
void roccat_eventhandler_plugin_talkfx_restore_led_rgb(RoccatEventhandlerPlugin *self);

void roccat_eventhandler_plugin_talkfx_ryos_set_sdk_mode(RoccatEventhandlerPlugin *self, gboolean state);
void roccat_eventhandler_plugin_talkfx_ryos_set_all_leds(RoccatEventhandlerPlugin *self, GArray const *data);
void roccat_eventhandler_plugin_talkfx_ryos_turn_on_all_leds(RoccatEventhandlerPlugin *self);
void roccat_eventhandler_plugin_talkfx_ryos_turn_off_all_leds(RoccatEventhandlerPlugin *self);
void roccat_eventhandler_plugin_talkfx_ryos_set_led_on(RoccatEventhandlerPlugin *self, guchar index);
void roccat_eventhandler_plugin_talkfx_ryos_set_led_off(RoccatEventhandlerPlugin *self, guchar index);
void roccat_eventhandler_plugin_talkfx_ryos_all_key_blinking(RoccatEventhandlerPlugin *self, guint interval, guint count);

void roccat_eventhandler_plugin_emit_talk_easyshift(RoccatEventhandlerPlugin *self, guint talk_device, gboolean state);
void roccat_eventhandler_plugin_emit_talk_easyshift_lock(RoccatEventhandlerPlugin *self, guint talk_device, gboolean state);
void roccat_eventhandler_plugin_emit_talk_easyaim(RoccatEventhandlerPlugin *self, guint talk_device, guchar state);
void roccat_eventhandler_plugin_emit_device_added(RoccatEventhandlerPlugin *self, guint product_id);
void roccat_eventhandler_plugin_emit_device_removed(RoccatEventhandlerPlugin *self, guint product_id);

RoccatEventhandlerPlugin *roccat_eventhandler_plugin_new(void);

#define ROCCAT_EVENTHANDLER_SYMBOL_PLUGIN_NEW "roccat_eventhandler_plugin_new"
typedef RoccatEventhandlerPlugin *(*ROCCAT_EVENTHANDLER_PLUGIN_NEW)(void);

G_END_DECLS

#endif
