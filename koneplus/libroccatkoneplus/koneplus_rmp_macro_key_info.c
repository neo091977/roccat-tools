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
#include "roccat.h"
#include "i18n-lib.h"
#include <string.h>

guint8 koneplus_rmp_macro_key_info_calc_checksum(KoneplusRmpMacroKeyInfo const *rmp_macro_key_info) {
	return ROCCAT_BYTESUM_PARTIALLY(rmp_macro_key_info, KoneplusRmpMacroKeyInfo, button_number, checksum);
}

void koneplus_rmp_macro_key_info_set_checksum(KoneplusRmpMacroKeyInfo *rmp_macro_key_info) {
	rmp_macro_key_info->checksum = koneplus_rmp_macro_key_info_calc_checksum(rmp_macro_key_info);
}

static void koneplus_rmp_macro_key_info_set_macroset_name(KoneplusRmpMacroKeyInfo *macro_key_info, gchar const *new_name) {
	g_strlcpy((gchar *)(macro_key_info->macroset_name), new_name, KONEPLUS_RMP_MACRO_KEY_INFO_MACROSET_NAME_LENGTH);
}

static void koneplus_rmp_macro_key_info_set_macro_name(KoneplusRmpMacroKeyInfo *macro_key_info, gchar const *new_name) {
	g_strlcpy((gchar *)(macro_key_info->macro_name), new_name, KONEPLUS_RMP_MACRO_KEY_INFO_MACRO_NAME_LENGTH);
}

static void koneplus_rmp_macro_key_info_set_filename(KoneplusRmpMacroKeyInfo *macro_key_info, gchar const *new_name) {
	g_strlcpy((gchar *)(macro_key_info->filename), new_name, KONEPLUS_RMP_MACRO_KEY_INFO_FILENAME_LENGTH);
}

static void koneplus_rmp_macro_key_info_set_timer_name(KoneplusRmpMacroKeyInfo *macro_key_info, gchar const *new_name) {
	g_strlcpy((gchar *)(macro_key_info->timer_name), new_name, KONEPLUS_RMP_MACRO_KEY_INFO_TIMER_NAME_LENGTH);
}

KoneplusRmpMacroKeyInfo *koneplus_rmp_macro_key_info_new(void) {
	return (KoneplusRmpMacroKeyInfo *)g_malloc0(sizeof(KoneplusRmpMacroKeyInfo));
}

void koneplus_rmp_macro_key_info_free(KoneplusRmpMacroKeyInfo *macro_key_info) {
	g_free(macro_key_info);
}

KoneplusRmpMacroKeyInfo *gaminggear_macro_to_koneplus_rmp_macro_key_info(GaminggearMacro const *gaminggear_macro, GError **error) {
	KoneplusRmpMacroKeyInfo *result;
	guint i, count;

	g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

	count = gaminggear_macro_keystrokes_get_count(&gaminggear_macro->keystrokes);
	if (count > KONEPLUS_RMP_MACRO_KEY_INFO_KEYSTROKES_NUM) {
		g_set_error(error, ROCCAT_ERROR, ROCCAT_ERROR_MACRO_TOO_LONG,
				_("Macro contains %u actions while device only supports %u actions"), count, KONEPLUS_RMP_MACRO_KEY_INFO_KEYSTROKES_NUM);
		return NULL;
	}

	result = koneplus_rmp_macro_key_info_new();
	result->type = KONEPLUS_PROFILE_BUTTON_TYPE_MACRO;
	koneplus_rmp_macro_key_info_set_macroset_name(result, gaminggear_macro->macroset);
	koneplus_rmp_macro_key_info_set_macro_name(result, gaminggear_macro->macro);
	koneplus_rmp_macro_key_info_set_count(result, count);
	result->loop = gaminggear_macro->keystrokes.loop;

	for (i = 0; i < count; ++i) {
		gaminggear_macro_keystroke_to_roccat_keystroke(&gaminggear_macro->keystrokes.keystrokes[i], &result->keystrokes[i]);
	}

	return result;
}

KoneplusRmpMacroKeyInfo *koneplus_rmp_macro_key_info_new_quicklaunch(gchar const *filename) {
	KoneplusRmpMacroKeyInfo *result = koneplus_rmp_macro_key_info_new();
	result->type = KONEPLUS_PROFILE_BUTTON_TYPE_QUICKLAUNCH;
	koneplus_rmp_macro_key_info_set_filename(result, filename);
	return result;
}

KoneplusRmpMacroKeyInfo *koneplus_rmp_macro_key_info_new_special(KoneplusProfileButtonType type) {
	KoneplusRmpMacroKeyInfo *result = koneplus_rmp_macro_key_info_new();
	result->type = type;
	return result;
}

KoneplusRmpMacroKeyInfo *koneplus_rmp_macro_key_info_new_timer(gchar const *name, guint seconds) {
	KoneplusRmpMacroKeyInfo *result = koneplus_rmp_macro_key_info_new();
	result->type = KONEPLUS_PROFILE_BUTTON_TYPE_TIMER;
	koneplus_rmp_macro_key_info_set_timer_name(result, name);
	koneplus_rmp_macro_key_info_set_timer_length(result, seconds);
	return result;
}

KoneplusRmpMacroKeyInfo *koneplus_rmp_macro_key_info_new_talk(KoneplusProfileButtonType type, guint talk_device) {
	KoneplusRmpMacroKeyInfo *result = koneplus_rmp_macro_key_info_new();
	result->type = type;
	koneplus_rmp_macro_key_info_set_talk_device(result, talk_device);
	return result;
}

KoneplusRmpMacroKeyInfo *koneplus_rmp_macro_key_info_new_shortcut(guint hid_usage_id,
		guint8 modifier) {
	KoneplusRmpMacroKeyInfo *result = koneplus_rmp_macro_key_info_new();

	result->type = KONEPLUS_PROFILE_BUTTON_TYPE_SHORTCUT;
	result->keystrokes[0].key = hid_usage_id;
	koneplus_rmp_macro_key_info_set_count(result, 1);
	result->keystrokes[0].action = modifier;

	return result;
}

gboolean koneplus_rmp_macro_key_info_equal(KoneplusRmpMacroKeyInfo const *left, KoneplusRmpMacroKeyInfo const *right) {
	gboolean equal;
	equal = ROCCAT_MEMCMP_PARTIALLY(left, right, KoneplusRmpMacroKeyInfo, type, checksum);
	return equal ? FALSE : TRUE;
}

KoneplusRmpMacroKeyInfo *koneplus_rmp_macro_key_info_dup(KoneplusRmpMacroKeyInfo const *source) {
	return g_memdup((gconstpointer)source, sizeof(KoneplusRmpMacroKeyInfo));
}

KoneplusRmpMacroKeyInfo *koneplus_rmp_macro_key_info_v1_to_koneplus_rmp_macro_key_info(KoneplusRmpMacroKeyInfoV1 const *v1) {
	KoneplusRmpMacroKeyInfo *result;
	result = koneplus_rmp_macro_key_info_new();

	result->button_number = v1->button_number;
	result->type = v1->type;
	koneplus_rmp_macro_key_info_set_talk_device(result, GUINT16_FROM_LE(v1->talk_device));
	koneplus_rmp_macro_key_info_set_macroset_name(result, (gchar const *)v1->macroset_name);
	koneplus_rmp_macro_key_info_set_macro_name(result, (gchar const *)v1->macro_name);
	koneplus_rmp_macro_key_info_set_loop(result, GUINT32_FROM_LE(v1->loop));
	koneplus_rmp_macro_key_info_set_count(result, GUINT16_FROM_LE(v1->count));
	memcpy(&result->keystrokes[0], &v1->keystrokes[0], sizeof(result->keystrokes));
	koneplus_rmp_macro_key_info_set_timer_length(result, GUINT32_FROM_LE(v1->timer_length));
	koneplus_rmp_macro_key_info_set_timer_name(result, (gchar const *)v1->timer_name);
	koneplus_rmp_macro_key_info_set_filename(result, (gchar const *)v1->filename);

	return result;
}
