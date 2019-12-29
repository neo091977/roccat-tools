#ifndef __ROCCAT_RYOSMKFX_LED_MACRO_THREAD_H__
#define __ROCCAT_RYOSMKFX_LED_MACRO_THREAD_H__

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

#define RYOSMKFX_LED_MACRO_THREAD_TYPE (ryosmkfx_led_macro_thread_get_type())
#define RYOSMKFX_LED_MACRO_THREAD(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), RYOSMKFX_LED_MACRO_THREAD_TYPE, RyosmkfxLedMacroThread))
#define IS_RYOSMKFX_LED_MACRO_THREAD(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), RYOSMKFX_LED_MACRO_THREAD_TYPE))

typedef struct _RyosmkfxLedMacroThread RyosmkfxLedMacroThread;
typedef struct _RyosmkfxLedMacroThreadPrivate RyosmkfxLedMacroThreadPrivate;

struct _RyosmkfxLedMacroThread {
	GObject parent;
	RyosmkfxLedMacroThreadPrivate *priv;
};

GType ryosmkfx_led_macro_thread_get_type(void);
RyosmkfxLedMacroThread *ryosmkfx_led_macro_thread_new(RoccatDevice *device, RyosLedMacro const *led_macro);

gboolean ryosmkfx_led_macro_thread_get_running(RyosmkfxLedMacroThread *ryosmkfx_led_macro_thread);

void ryosmkfx_led_macro_thread_set_cancelled(RyosmkfxLedMacroThread *ryosmkfx_led_macro_thread);
gboolean ryosmkfx_led_macro_thread_get_cancelled(RyosmkfxLedMacroThread *ryosmkfx_led_macro_thread);

G_END_DECLS

#endif
