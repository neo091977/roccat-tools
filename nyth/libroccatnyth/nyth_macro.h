#ifndef __ROCCAT_NYTH_MACRO_H__
#define __ROCCAT_NYTH_MACRO_H__

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
#include "nyth.h"

G_BEGIN_DECLS

enum {
	NYTH_MACRO_MACROSET_NAME_LENGTH = 40,
	NYTH_MACRO_MACRO_NAME_LENGTH = 32,
	NYTH_MACRO_KEYSTROKES_NUM = 480,
};

typedef struct _NythMacro NythMacro;

/* This structure is transferred to hardware in 2 parts */
struct _NythMacro {
	guint8 profile_index;
	guint8 button_index;
	guint8 loop;
	guint8 macroset_name[NYTH_MACRO_MACROSET_NAME_LENGTH];
	guint8 macro_name[NYTH_MACRO_MACRO_NAME_LENGTH];
	guint16 count;
	RoccatKeystroke keystrokes[NYTH_MACRO_KEYSTROKES_NUM];
} __attribute__ ((packed));

static inline guint16 nyth_macro_get_count(NythMacro const *macro) {
	return GUINT16_FROM_LE(macro->count);
}

static inline void nyth_macro_set_count(NythMacro *macro, guint16 new_value) {
	macro->count = GUINT16_TO_LE(new_value);
}

void nyth_macro_set_macroset_name(NythMacro *macro, gchar const *new_name);
void nyth_macro_set_macro_name(NythMacro *macro, gchar const *new_name);
gboolean nyth_macro_write(RoccatDevice *nyth, guint profile_index, guint button_index, NythMacro *macro, GError **error);
NythMacro *nyth_macro_read(RoccatDevice *nyth, guint profile_index, guint button_index, GError **error);
gboolean gaminggear_macro_to_nyth_macro(GaminggearMacro const *gaminggear_macro, NythMacro *nyth_macro, GError **error);
GaminggearMacro *nyth_macro_to_gaminggear_macro(NythMacro const *nyth_macro);
gboolean nyth_macro_equal(NythMacro const *left, NythMacro const *right);
void nyth_macro_copy(NythMacro *destination, NythMacro const *source);

G_END_DECLS

#endif
