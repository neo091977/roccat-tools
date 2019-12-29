#ifndef __ROCCAT_RYOSMKFX_MACRO_H__
#define __ROCCAT_RYOSMKFX_MACRO_H__

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
#include "roccat_device.h"
#include "roccat_keystroke.h"

G_BEGIN_DECLS

enum {
	RYOSMKFX_MACRO_MACROSET_NAME_LENGTH = 40,
	RYOSMKFX_MACRO_MACRO_NAME_LENGTH = 32,
	RYOSMKFX_MACRO_KEYSTROKES_NUM = 480,
};

typedef struct _RyosmkfxMacro RyosmkfxMacro;

/* This structure is transferred to hardware in 2 parts */
struct _RyosmkfxMacro {
	guint8 profile_index;
	guint8 button_index;
	guint8 loop;
	guint8 macroset_name[RYOSMKFX_MACRO_MACROSET_NAME_LENGTH];
	guint8 macro_name[RYOSMKFX_MACRO_MACRO_NAME_LENGTH];
	guint16 count;
	RoccatKeystroke keystrokes[RYOSMKFX_MACRO_KEYSTROKES_NUM];
} __attribute__ ((packed));

static inline guint16 ryosmkfx_macro_get_count(RyosmkfxMacro const *macro) {
	return GUINT16_FROM_LE(macro->count);
}

static inline void ryosmkfx_macro_set_count(RyosmkfxMacro *macro, guint16 new_value) {
	macro->count = GUINT16_TO_LE(new_value);
}

void ryosmkfx_macro_set_macroset_name(RyosmkfxMacro *macro, gchar const *new_name);
void ryosmkfx_macro_set_macro_name(RyosmkfxMacro *macro, gchar const *new_name);
gboolean ryosmkfx_macro_write(RoccatDevice *ryosmkfx, guint profile_index, guint button_index, RyosmkfxMacro *macro, GError **error);
RyosmkfxMacro *ryosmkfx_macro_read(RoccatDevice *ryosmkfx, guint profile_index, guint button_index, GError **error);
gboolean gaminggear_macro_to_ryosmkfx_macro(GaminggearMacro const *gaminggear_macro, RyosmkfxMacro *ryosmkfx_macro, GError **error);
GaminggearMacro *ryosmkfx_macro_to_gaminggear_macro(RyosmkfxMacro const *ryosmkfx_macro);
gboolean ryosmkfx_macro_equal(RyosmkfxMacro const *left, RyosmkfxMacro const *right);
void ryosmkfx_macro_copy(RyosmkfxMacro *destination, RyosmkfxMacro const *source);

G_END_DECLS

#endif
