#ifndef __ROCCAT_RYOSTKL_LED_MACRO_THREAD_H__
#define __ROCCAT_RYOSTKL_LED_MACRO_THREAD_H__

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

#include "ryostkl_light_layer.h"
#include "ryos_led_macro.h"
#include "roccat_device.h"
#include <glib-object.h>

G_BEGIN_DECLS

#define RYOSTKL_LED_MACRO_THREAD_TYPE (ryostkl_led_macro_thread_get_type())
#define RYOSTKL_LED_MACRO_THREAD(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), RYOSTKL_LED_MACRO_THREAD_TYPE, RyostklLedMacroThread))
#define IS_RYOSTKL_LED_MACRO_THREAD(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), RYOSTKL_LED_MACRO_THREAD_TYPE))

typedef struct _RyostklLedMacroThread RyostklLedMacroThread;
typedef struct _RyostklLedMacroThreadPrivate RyostklLedMacroThreadPrivate;

struct _RyostklLedMacroThread {
	GObject parent;
	RyostklLedMacroThreadPrivate *priv;
};

GType ryostkl_led_macro_thread_get_type(void);
RyostklLedMacroThread *ryostkl_led_macro_thread_new(RoccatDevice *device, RyostklLightLayer const *light_layer, RyosLedMacro const *led_macro);

gboolean ryostkl_led_macro_thread_get_running(RyostklLedMacroThread *ryostkl_led_macro_thread);

void ryostkl_led_macro_thread_set_cancelled(RyostklLedMacroThread *ryostkl_led_macro_thread);
gboolean ryostkl_led_macro_thread_get_cancelled(RyostklLedMacroThread *ryostkl_led_macro_thread);

G_END_DECLS

#endif
