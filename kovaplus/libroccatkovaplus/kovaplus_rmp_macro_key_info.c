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

#include "kovaplus_rmp.h"
#include "roccat_helper.h"
#include "roccat.h"
#include "i18n-lib.h"
#include <string.h>

static guint8 kovaplus_rmp_macro_key_info_calc_checksum(KovaplusRmpMacroKeyInfo const *rmp_macro_key_info) {
	return ROCCAT_BYTESUM_PARTIALLY(rmp_macro_key_info, KovaplusRmpMacroKeyInfo, button_index, checksum);
}

void kovaplus_rmp_macro_key_info_set_checksum(KovaplusRmpMacroKeyInfo *rmp_macro_key_info) {
	rmp_macro_key_info->checksum = kovaplus_rmp_macro_key_info_calc_checksum(rmp_macro_key_info);
}

static void kovaplus_rmp_macro_key_info_set_macroset_name(KovaplusRmpMacroKeyInfo *macro_key_info, gchar const *new_name) {
	g_strlcpy((gchar *)(macro_key_info->macroset_name), new_name, KOVAPLUS_RMP_MACRO_KEY_INFO_MACROSET_NAME_LENGTH);
}

static void kovaplus_rmp_macro_key_info_set_macro_name(KovaplusRmpMacroKeyInfo *macro_key_info, gchar const *new_name) {
	g_strlcpy((gchar *)(macro_key_info->macro_name), new_name, KOVAPLUS_RMP_MACRO_KEY_INFO_MACRO_NAME_LENGTH);
}

static void kovaplus_rmp_macro_key_info_set_filename(KovaplusRmpMacroKeyInfo *macro_key_info, gchar const *new_name) {
	g_strlcpy((gchar *)(macro_key_info->filename), new_name, KOVAPLUS_RMP_MACRO_KEY_INFO_FILENAME_LENGTH);
}

static void kovaplus_rmp_macro_key_info_set_timer_name(KovaplusRmpMacroKeyInfo *macro_key_info, gchar const *new_name) {
	g_strlcpy((gchar *)(macro_key_info->timer_name), new_name, KOVAPLUS_RMP_MACRO_KEY_INFO_TIMER_NAME_LENGTH);
}

static KovaplusRmpMacroKeyInfo *kovaplus_rmp_macro_key_info_new(void) {
	return (KovaplusRmpMacroKeyInfo *)g_malloc0(sizeof(KovaplusRmpMacroKeyInfo));
}

void kovaplus_rmp_macro_key_info_free(KovaplusRmpMacroKeyInfo *macro_key_info) {
	g_free(macro_key_info);
}

KovaplusRmpMacroKeyInfo *gaminggear_macro_to_kovaplus_rmp_macro_key_info(GaminggearMacro const *gaminggear_macro, GError **error) {
	KovaplusRmpMacroKeyInfo *result;
	guint i, count;

	g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

	count = gaminggear_macro_keystrokes_get_count(&gaminggear_macro->keystrokes);
	if (count > KOVAPLUS_RMP_MACRO_KEY_INFO_KEYSTROKES_NUM) {
		g_set_error(error, ROCCAT_ERROR, ROCCAT_ERROR_MACRO_TOO_LONG,
				_("Macro contains %u actions while device only supports %u actions"), count, KOVAPLUS_RMP_MACRO_KEY_INFO_KEYSTROKES_NUM);
		return NULL;
	}

	result = kovaplus_rmp_macro_key_info_new();
	result->type = KOVAPLUS_PROFILE_BUTTON_TYPE_MACRO;
	kovaplus_rmp_macro_key_info_set_macroset_name(result, gaminggear_macro->macroset);
	kovaplus_rmp_macro_key_info_set_macro_name(result, gaminggear_macro->macro);
	kovaplus_rmp_macro_key_info_set_count(result, count);
	result->loop = gaminggear_macro->keystrokes.loop;

	for (i = 0; i < count; ++i) {
		gaminggear_macro_keystroke_to_roccat_keystroke(&gaminggear_macro->keystrokes.keystrokes[i], &result->keystrokes[i]);
	}

	return result;
}

GaminggearMacro *kovaplus_rmp_macro_key_info_to_gaminggear_macro(KovaplusRmpMacroKeyInfo const *button_info) {
	GaminggearMacro *gaminggear_macro;
	GaminggearMacroKeystroke keystroke;
	guint i;
	
	if (button_info->type != KOVAPLUS_PROFILE_BUTTON_TYPE_MACRO)
		return NULL;
		
	gaminggear_macro = gaminggear_macro_new((gchar const *)button_info->macroset_name, (gchar const *)button_info->macro_name, NULL);
	
	for (i = 0; i < kovaplus_rmp_macro_key_info_get_count(button_info); ++i) {
		roccat_keystroke_to_gaminggear_macro_keystroke(&button_info->keystrokes[i], &keystroke);
		gaminggear_macro_add_keystroke(gaminggear_macro, &keystroke);
	}
	
	return gaminggear_macro;
}

KovaplusRmpMacroKeyInfo *kovaplus_rmp_macro_key_info_new_quicklaunch(gchar const *filename) {
	KovaplusRmpMacroKeyInfo *result = kovaplus_rmp_macro_key_info_new();
	result->type = KOVAPLUS_PROFILE_BUTTON_TYPE_QUICKLAUNCH;
	kovaplus_rmp_macro_key_info_set_filename(result, filename);
	return result;
}

KovaplusRmpMacroKeyInfo *kovaplus_rmp_macro_key_info_new_special(KovaplusProfileButtonType type) {
	KovaplusRmpMacroKeyInfo *result = kovaplus_rmp_macro_key_info_new();
	result->type = type;
	return result;
}

KovaplusRmpMacroKeyInfo *kovaplus_rmp_macro_key_info_new_timer(gchar const *name, guint seconds) {
	KovaplusRmpMacroKeyInfo *result = kovaplus_rmp_macro_key_info_new();
	result->type = KOVAPLUS_PROFILE_BUTTON_TYPE_TIMER;
	kovaplus_rmp_macro_key_info_set_timer_name(result, name);
	kovaplus_rmp_macro_key_info_set_timer_length(result, seconds);
	return result;
}

KovaplusRmpMacroKeyInfo *kovaplus_rmp_macro_key_info_new_shortcut(guint hid_usage_id,
		guint8 modifier) {
	KovaplusRmpMacroKeyInfo *result = kovaplus_rmp_macro_key_info_new();

	result->type = KOVAPLUS_PROFILE_BUTTON_TYPE_SHORTCUT;
	kovaplus_rmp_macro_key_info_set_count(result, 1);
	result->keystrokes[0].key = hid_usage_id;
	result->keystrokes[0].action = modifier;

	return result;
}

gboolean kovaplus_rmp_macro_key_info_equal(KovaplusRmpMacroKeyInfo const *left, KovaplusRmpMacroKeyInfo const *right) {
	gboolean equal;
	equal = ROCCAT_MEMCMP_PARTIALLY(left, right, KovaplusRmpMacroKeyInfo, type, checksum);
	return equal ? FALSE : TRUE;
}

KovaplusRmpMacroKeyInfo *kovaplus_rmp_macro_key_info_dup(KovaplusRmpMacroKeyInfo const *source) {
	return g_memdup((gconstpointer)source, sizeof(KovaplusRmpMacroKeyInfo));
}
