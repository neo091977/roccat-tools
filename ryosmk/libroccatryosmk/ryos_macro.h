#ifndef __ROCCAT_RYOS_MACRO_H__
#define __ROCCAT_RYOS_MACRO_H__

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

typedef struct _RyosMacro RyosMacro;

/* Given is full sized struct of 2002 bytes that are always sent,
 * but size is dynamic through RYOS_MACRO_KEYSTROKES_NUM
 */
struct _RyosMacro {
	guint8 report_id; /* RYOS_REPORT_ID_MACRO */
	guint16 size; /* dynamic */
	guint8 profile_index;
	guint8 button_index;
	guint8 loop;
	guint8 default_action;
	guint8 unknown1[7];
	guint8 macroset_name[RYOS_MACRO_MACROSET_NAME_LENGTH];
	guint8 unkown2[16];
	guint8 macro_name[RYOS_MACRO_MACRO_NAME_LENGTH];
	guint16 count;
	RoccatKeystroke keystrokes[RYOS_MACRO_KEYSTROKES_NUM];
	guint16 checksum;
} __attribute__ ((packed));

static inline guint16 ryos_macro_get_count(RyosMacro const *macro) {
	return GUINT16_FROM_LE(macro->count);
}

static inline void ryos_macro_set_count(RyosMacro *macro, guint16 new_value) {
	macro->count = GUINT16_TO_LE(new_value);
}

gboolean ryos_macro_write(RoccatDevice *ryos, guint profile_index, guint button_index, RyosMacro *macro, GError **error);
RyosMacro *ryos_macro_read(RoccatDevice *ryos, guint profile_index, guint button_index, GError **error);

gboolean gaminggear_macro_to_ryos_macro(GaminggearMacro const *gaminggear_macro, RyosMacro *ryos_macro, GError **error);

GaminggearMacro *ryos_macro_to_gaminggear_macro(RyosMacro const *ryos_macro);

gboolean ryos_macro_equal(RyosMacro const *left, RyosMacro const *right);
void ryos_macro_copy(RyosMacro *destination, RyosMacro const *source);
RyosMacro *ryos_macro_dup(RyosMacro const *macro);

gboolean ryos_light_macro_write(RoccatDevice *ryos, guint profile_index, RyosMacro *light_macro, GError **error);
RyosMacro *ryos_light_macro_read(RoccatDevice *ryos, guint profile_index, GError **error);

G_END_DECLS

#endif
