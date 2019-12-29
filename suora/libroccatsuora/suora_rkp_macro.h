#ifndef __ROCCAT_SUORA_RKP_MACRO_H__
#define __ROCCAT_SUORA_RKP_MACRO_H__

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

#include "suora_macro.h"
#include "roccat_keystroke.h"
#include <gaminggear/macro.h>

G_BEGIN_DECLS

enum {
	SUORA_RKP_MACRO_MACROSET_NAME_LENGTH = 40,
	SUORA_RKP_MACRO_MACRO_NAME_LENGTH = 32,
	SUORA_RKP_MACRO_KEYSTROKES_NUM = 480,
};

typedef struct _SuoraRkpMacro SuoraRkpMacro;

struct _SuoraRkpMacro {
	guint8 report_id; /* RYOS_REPORT_ID_MACRO */
	guint16 size; /* 2002 */
	guint8 profile_index;
	guint8 button_index;
	guint8 loop;
	guint8 macroset_name[SUORA_RKP_MACRO_MACROSET_NAME_LENGTH];
	guint8 macro_name[SUORA_RKP_MACRO_MACRO_NAME_LENGTH];
	guint16 count;
	RoccatKeystroke keystrokes[SUORA_RKP_MACRO_KEYSTROKES_NUM];
	guint8 unused[2];
} __attribute__ ((packed));

static inline guint16 suora_rkp_macro_get_count(SuoraRkpMacro const *macro) {
	return GUINT16_FROM_LE(macro->count);
}

static inline void suora_rkp_macro_set_count(SuoraRkpMacro *macro, guint16 new_value) {
	macro->count = GUINT16_TO_LE(new_value);
}

void suora_rkp_macro_set_macroset_name(SuoraRkpMacro *macro, gchar const *new_name);
void suora_rkp_macro_set_macro_name(SuoraRkpMacro *macro, gchar const *new_name);

void suora_rkp_macro_finalize(SuoraRkpMacro *macro, guint keys_index);

gboolean gaminggear_macro_to_suora_rkp_macro(GaminggearMacro const *gaminggear_macro, SuoraRkpMacro *suora_rkp_macro, GError **error);
gboolean suora_rkp_macro_to_macro(SuoraRkpMacro const *rkp_macro, SuoraMacro *suora_macro, GError **error);
void suora_macro_to_rkp_macro(SuoraMacro const *suora_macro, SuoraRkpMacro *rkp_macro);

gboolean suora_rkp_macro_equal(SuoraRkpMacro const *left, SuoraRkpMacro const *right);
void suora_rkp_macro_copy(SuoraRkpMacro *destination, SuoraRkpMacro const *source);
SuoraRkpMacro *suora_rkp_macro_dup(SuoraRkpMacro const *old);

G_END_DECLS

#endif
