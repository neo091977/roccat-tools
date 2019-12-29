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

#include "koneplus_rmp.h"
#include "roccat_helper.h"
#include <string.h>

static void koneplus_rmp_macro_key_info_set_macroset_name(KoneplusRmpMacroKeyInfo *key_info, gchar const *new_name) {
	g_strlcpy((gchar *)(key_info->macroset_name), new_name, KONEPLUS_RMP_MACRO_KEY_INFO_MACROSET_NAME_LENGTH);
}

static void koneplus_rmp_macro_key_info_set_macro_name(KoneplusRmpMacroKeyInfo *key_info, gchar const *new_name) {
	g_strlcpy((gchar *)(key_info->macro_name), new_name, KONEPLUS_RMP_MACRO_KEY_INFO_MACRO_NAME_LENGTH);
}

static void koneplus_macro_set_macroset_name(KoneplusMacro *macro, gchar const *new_name) {
	g_strlcpy((gchar *)(macro->macroset_name), new_name, KONEPLUS_MACRO_MACROSET_NAME_LENGTH);
}

static void koneplus_macro_set_macro_name(KoneplusMacro *macro, gchar const *new_name) {
	g_strlcpy((gchar *)(macro->macro_name), new_name, KONEPLUS_MACRO_MACRO_NAME_LENGTH);
}

static guint16 koneplus_macro_calc_checksum(KoneplusMacro const *macro) {
	return ROCCAT_BYTESUM_PARTIALLY(macro, KoneplusMacro, report_id, checksum);
}

static void koneplus_macro_set_checksum(KoneplusMacro *macro) {
	macro->checksum = GUINT16_TO_LE(koneplus_macro_calc_checksum(macro));
}

static void koneplus_macro_finalize(KoneplusMacro *macro, guint profile_index, guint key_index) {
	g_assert(profile_index < KONEPLUS_PROFILE_NUM);
	macro->report_id = KONEPLUS_REPORT_ID_MACRO;
	koneplus_macro_set_size(macro, sizeof(KoneplusMacro));
	macro->profile_number = profile_index;
	macro->button_number = key_index;
	koneplus_macro_set_checksum(macro);
}

// FIXME return NULL
KoneplusMacro *koneplus_rmp_macro_key_info_to_macro(KoneplusRmpMacroKeyInfo const *macro_key_info) {
	KoneplusMacro *macro;
	guint i;

	if (macro_key_info->type != KONEPLUS_PROFILE_BUTTON_TYPE_MACRO)
		return NULL;

	macro = g_malloc0(sizeof(KoneplusMacro));

	macro->report_id = KONEPLUS_REPORT_ID_MACRO;
	koneplus_macro_set_size(macro, sizeof(KoneplusMacro));
	macro->button_number = macro_key_info->button_number;
	macro->loop = koneplus_rmp_macro_key_info_get_loop(macro_key_info);
	koneplus_macro_set_macroset_name(macro, (gchar const *)macro_key_info->macroset_name);
	koneplus_macro_set_macro_name(macro, (gchar const *)macro_key_info->macro_name);
	koneplus_macro_set_count(macro, koneplus_rmp_macro_key_info_get_count(macro_key_info));

	for (i = 0; i < koneplus_macro_get_count(macro); ++i) {
		macro->keystrokes[i] = macro_key_info->keystrokes[i];
	}

	return macro;
}

KoneplusRmpMacroKeyInfo *koneplus_macro_to_rmp_macro_key_info(KoneplusMacro const *macro) {
	KoneplusRmpMacroKeyInfo *key_info;
	key_info = koneplus_rmp_macro_key_info_new();
	guint i, count;

	count = koneplus_macro_get_count(macro);
	count = MIN(count, KONEPLUS_RMP_MACRO_KEY_INFO_KEYSTROKES_NUM);

	key_info->button_number = macro->button_number;
	key_info->type = KONEPLUS_PROFILE_BUTTON_TYPE_MACRO;
	koneplus_rmp_macro_key_info_set_macroset_name(key_info, (gchar const *)macro->macroset_name);
	koneplus_rmp_macro_key_info_set_macro_name(key_info, (gchar const *)macro->macro_name);
	koneplus_rmp_macro_key_info_set_loop(key_info, macro->loop);
	koneplus_rmp_macro_key_info_set_count(key_info, count);
	for (i = 0; i < count; ++i) {
		key_info->keystrokes[i] = macro->keystrokes[i];
	}

	return key_info;
}

gboolean koneplus_macro_write(RoccatDevice *koneplus, guint profile_index, guint key_index, KoneplusMacro *macro, GError **error) {
	g_assert(profile_index < KONEPLUS_PROFILE_NUM);
	koneplus_macro_finalize(macro, profile_index, key_index);
	return koneplus_device_write(koneplus, (gchar const *)macro, sizeof(KoneplusMacro), error);
}

KoneplusMacro *koneplus_macro_read(RoccatDevice *koneplus, guint profile_index, guint key_index, GError **error) {
	KoneplusMacro *macro;

	gaminggear_device_lock(GAMINGGEAR_DEVICE(koneplus));

	if (!koneplus_select(koneplus, profile_index, key_index, error)) {
		gaminggear_device_unlock(GAMINGGEAR_DEVICE(koneplus));
		return NULL;
	}

	macro = (KoneplusMacro *)koneplus_device_read(koneplus, KONEPLUS_REPORT_ID_MACRO, sizeof(KoneplusMacro), error);

	gaminggear_device_unlock(GAMINGGEAR_DEVICE(koneplus));

	return macro;
}
