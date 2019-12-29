#ifndef __ROCCAT_KOVA2016_MACRO_H__
#define __ROCCAT_KOVA2016_MACRO_H__

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
#include "kova2016.h"

G_BEGIN_DECLS

enum {
	KOVA2016_MACRO_MACROSET_NAME_LENGTH = 40,
	KOVA2016_MACRO_MACRO_NAME_LENGTH = 32,
	KOVA2016_MACRO_KEYSTROKES_NUM = 480,
};

typedef struct _Kova2016Macro Kova2016Macro;

/* This structure is transferred to hardware in 2 parts */
struct _Kova2016Macro {
	guint8 profile_index;
	guint8 button_index;
	guint8 loop;
	guint8 macroset_name[KOVA2016_MACRO_MACROSET_NAME_LENGTH];
	guint8 macro_name[KOVA2016_MACRO_MACRO_NAME_LENGTH];
	guint16 count;
	RoccatKeystroke keystrokes[KOVA2016_MACRO_KEYSTROKES_NUM];
} __attribute__ ((packed));

static inline guint16 kova2016_macro_get_count(Kova2016Macro const *macro) {
	return GUINT16_FROM_LE(macro->count);
}

static inline void kova2016_macro_set_count(Kova2016Macro *macro, guint16 new_value) {
	macro->count = GUINT16_TO_LE(new_value);
}

void kova2016_macro_set_macroset_name(Kova2016Macro *macro, gchar const *new_name);
void kova2016_macro_set_macro_name(Kova2016Macro *macro, gchar const *new_name);
gboolean kova2016_macro_write(RoccatDevice *kova2016, guint profile_index, guint button_index, Kova2016Macro *macro, GError **error);
Kova2016Macro *kova2016_macro_read(RoccatDevice *kova2016, guint profile_index, guint button_index, GError **error);
gboolean gaminggear_macro_to_kova2016_macro(GaminggearMacro const *gaminggear_macro, Kova2016Macro *kova2016_macro, GError **error);
GaminggearMacro *kova2016_macro_to_gaminggear_macro(Kova2016Macro const *kova2016_macro);
gboolean kova2016_macro_equal(Kova2016Macro const *left, Kova2016Macro const *right);
void kova2016_macro_copy(Kova2016Macro *destination, Kova2016Macro const *source);

G_END_DECLS

#endif
