#ifndef __ROCCAT_TYON_MACRO_H__
#define __ROCCAT_TYON_MACRO_H__

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
#include "tyon.h"

G_BEGIN_DECLS

enum {
	TYON_MACRO_MACROSET_NAME_LENGTH = 24,
	TYON_MACRO_MACRO_NAME_LENGTH = 24,
	TYON_MACRO_KEYSTROKES_NUM = 480,
};

typedef struct _TyonMacro TyonMacro;

/* This structure is transferred to hardware in 2 parts */
struct _TyonMacro {
	guint8 profile_index;
	guint8 button_index;
	guint8 loop;
	guint8 unused1[24];
	guint8 macroset_name[TYON_MACRO_MACROSET_NAME_LENGTH];
	guint8 macro_name[TYON_MACRO_MACRO_NAME_LENGTH];
	guint16 count;
	RoccatKeystroke keystrokes[TYON_MACRO_KEYSTROKES_NUM];
} __attribute__ ((packed));

static inline guint16 tyon_macro_get_count(TyonMacro const *macro) {
	return GUINT16_FROM_LE(macro->count);
}

static inline void tyon_macro_set_count(TyonMacro *macro, guint16 new_value) {
	macro->count = GUINT16_TO_LE(new_value);
}

void tyon_macro_set_macroset_name(TyonMacro *macro, gchar const *new_name);
void tyon_macro_set_macro_name(TyonMacro *macro, gchar const *new_name);
gboolean tyon_macro_write(RoccatDevice *tyon, guint profile_index, guint button_index, TyonMacro *macro, GError **error);
TyonMacro *tyon_macro_read(RoccatDevice *tyon, guint profile_index, guint button_index, GError **error);
gboolean gaminggear_macro_to_tyon_macro(GaminggearMacro const *gaminggear_macro, TyonMacro *tyon_macro, GError **error);
GaminggearMacro *tyon_macro_to_gaminggear_macro(TyonMacro const *tyon_macro);
gboolean tyon_macro_equal(TyonMacro const *left, TyonMacro const *right);
void tyon_macro_copy(TyonMacro *destination, TyonMacro const *source);

G_END_DECLS

#endif
