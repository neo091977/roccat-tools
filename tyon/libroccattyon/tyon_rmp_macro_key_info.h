#ifndef __ROCCAT_TYON_RMP_MACRO_KEY_INFO_H__
#define __ROCCAT_TYON_RMP_MACRO_KEY_INFO_H__

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
#include "tyon_profile_buttons.h"
#include "tyon_macro.h"

G_BEGIN_DECLS

typedef struct _TyonRmpMacroKeyInfo TyonRmpMacroKeyInfo;

enum {
	TYON_RMP_MACRO_KEY_INFO_MACROSET_NAME_LENGTH = 50,
	TYON_RMP_MACRO_KEY_INFO_MACRO_NAME_LENGTH = 50,
	TYON_RMP_MACRO_KEY_INFO_KEYSTROKES_NUM = TYON_MACRO_KEYSTROKES_NUM,
	TYON_RMP_MACRO_KEY_INFO_TIMER_NAME_LENGTH = 100,
	TYON_RMP_MACRO_KEY_INFO_FILENAME_LENGTH = 260,
};

struct _TyonRmpMacroKeyInfo {
	guint8 button_index;
	guint8 type;
	guint16 unused1;
	guint16 talk_device;
	guint8 unused2[52];
	guint8 macroset_name[TYON_RMP_MACRO_KEY_INFO_MACROSET_NAME_LENGTH];
	guint8 macro_name[TYON_RMP_MACRO_KEY_INFO_MACRO_NAME_LENGTH];
	guint16 unused3;
	guint32 loop;
	guint16 count;
	RoccatKeystroke keystrokes[TYON_RMP_MACRO_KEY_INFO_KEYSTROKES_NUM];
	guint8 unused4[82];
	guint32 timer_length;
	guint8 timer_name[TYON_RMP_MACRO_KEY_INFO_TIMER_NAME_LENGTH];
	guint8 filename[TYON_RMP_MACRO_KEY_INFO_FILENAME_LENGTH];
	guint8 checksum;
} __attribute__ ((packed));

static inline guint16 tyon_rmp_macro_key_info_get_talk_device(TyonRmpMacroKeyInfo const *info) {
	return GUINT16_FROM_LE(info->talk_device);
}

static inline void tyon_rmp_macro_key_info_set_talk_device(TyonRmpMacroKeyInfo *info, guint16 new_value) {
	info->talk_device = GUINT16_TO_LE(new_value);
}

static inline guint32 tyon_rmp_macro_key_info_get_loop(TyonRmpMacroKeyInfo const *info) {
	return GUINT32_FROM_LE(info->loop);
}

static inline void tyon_rmp_macro_key_info_set_loop(TyonRmpMacroKeyInfo *info, guint32 new_value) {
	info->loop = GUINT32_TO_LE(new_value);
}

static inline guint16 tyon_rmp_macro_key_info_get_count(TyonRmpMacroKeyInfo const *info) {
	return GUINT16_FROM_LE(info->count);
}

static inline void tyon_rmp_macro_key_info_set_count(TyonRmpMacroKeyInfo *info, guint16 new_value) {
	info->count = GUINT16_TO_LE(new_value);
}

static inline guint32 tyon_rmp_macro_key_info_get_timer_length(TyonRmpMacroKeyInfo const *info) {
	return GUINT32_FROM_LE(info->timer_length);
}

static inline void tyon_rmp_macro_key_info_set_timer_length(TyonRmpMacroKeyInfo *info, guint32 new_value) {
	info->timer_length = GUINT32_TO_LE(new_value);
}

guint8 tyon_rmp_macro_key_info_calc_checksum(TyonRmpMacroKeyInfo const *rmp_macro_key_info);
void tyon_rmp_macro_key_info_set_checksum(TyonRmpMacroKeyInfo *rmp_macro_key_info);
TyonRmpMacroKeyInfo *tyon_rmp_macro_key_info_new(void);
void tyon_rmp_macro_key_info_free(TyonRmpMacroKeyInfo *macro_key_info);
TyonRmpMacroKeyInfo *gaminggear_macro_to_tyon_rmp_macro_key_info(GaminggearMacro const *gaminggear_macro, GError **error);
TyonRmpMacroKeyInfo *tyon_rmp_macro_key_info_new_quicklaunch(gchar const *filename);
TyonRmpMacroKeyInfo *tyon_rmp_macro_key_info_new_special(TyonButtonType type);
TyonRmpMacroKeyInfo *tyon_rmp_macro_key_info_new_timer(gchar const *name, guint seconds);
TyonRmpMacroKeyInfo *tyon_rmp_macro_key_info_new_talk(TyonButtonType type, guint talk_device);
TyonRmpMacroKeyInfo *tyon_rmp_macro_key_info_new_shortcut(guint hid_usage_id, guint8 modifier);
gboolean tyon_rmp_macro_key_info_equal(TyonRmpMacroKeyInfo const *left, TyonRmpMacroKeyInfo const *right);
TyonRmpMacroKeyInfo *tyon_rmp_macro_key_info_dup(TyonRmpMacroKeyInfo const *source);
TyonMacro *tyon_rmp_macro_key_info_to_macro(TyonRmpMacroKeyInfo const *macro_key_info);
TyonRmpMacroKeyInfo *tyon_macro_to_rmp_macro_key_info(TyonMacro const *macro);

G_END_DECLS

#endif
