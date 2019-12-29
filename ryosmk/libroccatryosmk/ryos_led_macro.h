#ifndef __ROCCAT_RYOS_LED_MACRO_H__
#define __ROCCAT_RYOS_LED_MACRO_H__

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

#include <gaminggear/macro.h>
#include "roccat_keystroke.h"
#include "ryos.h"

G_BEGIN_DECLS

typedef struct _RyosLedMacro RyosLedMacro;

/* Format of IlluminationButtonSet.dat entries
 * Limiting size of fields to be compatible with RyosMacro for ease of use.
 */
struct _RyosLedMacro {
	guint8 index;
	guint8 loop;
	guint8 default_action;
	guint8 unknown1[7];
	guint8 macroset_name[RYOS_MACRO_MACROSET_NAME_LENGTH];
	guint8 unknown2[16];
	guint8 macro_name[RYOS_MACRO_MACRO_NAME_LENGTH];
	guint16 count;
	RoccatKeystroke keystrokes[RYOS_MACRO_KEYSTROKES_NUM];
	guint16 unused;
} __attribute__ ((packed));

static inline guint16 ryos_led_macro_get_count(RyosLedMacro const *led_macro) {
	return GUINT16_FROM_LE(led_macro->count);
}

static inline void ryos_led_macro_set_count(RyosLedMacro *led_macro, guint16 new_value) {
	led_macro->count = GUINT16_TO_LE(new_value);
}

gboolean gaminggear_macro_to_ryos_led_macro(GaminggearMacro const *gaminggear_macro, RyosLedMacro *ryos_led_macro, GError **error);
gboolean ryos_led_macro_equal(RyosLedMacro const *left, RyosLedMacro const *right);
void ryos_led_macro_copy(RyosLedMacro *destination, RyosLedMacro const *source);
RyosLedMacro *ryos_led_macro_dup(RyosLedMacro const *led_macro);

G_END_DECLS

#endif
