#ifndef __ROCCAT_RYOS_LED_MACRO_THREAD_H__
#define __ROCCAT_RYOS_LED_MACRO_THREAD_H__

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

#include "roccat_device.h"
#include "ryos_led_macro.h"
#include <glib-object.h>

G_BEGIN_DECLS

#define RYOS_LED_MACRO_THREAD_TYPE (ryos_led_macro_thread_get_type())
#define RYOS_LED_MACRO_THREAD(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), RYOS_LED_MACRO_THREAD_TYPE, RyosLedMacroThread))
#define IS_RYOS_LED_MACRO_THREAD(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), RYOS_LED_MACRO_THREAD_TYPE))

typedef struct _RyosLedMacroThread RyosLedMacroThread;
typedef struct _RyosLedMacroThreadPrivate RyosLedMacroThreadPrivate;

struct _RyosLedMacroThread {
	GObject parent;
	RyosLedMacroThreadPrivate *priv;
};

GType ryos_led_macro_thread_get_type(void);
RyosLedMacroThread *ryos_led_macro_thread_new(RoccatDevice *device, RyosLedMacro const *led_macro);

gboolean ryos_led_macro_thread_get_running(RyosLedMacroThread *ryos_led_macro_thread);

void ryos_led_macro_thread_set_cancelled(RyosLedMacroThread *ryos_led_macro_thread);
gboolean ryos_led_macro_thread_get_cancelled(RyosLedMacroThread *ryos_led_macro_thread);

G_END_DECLS

#endif
