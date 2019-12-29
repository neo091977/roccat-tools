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

#include "tyon_rmp_macro_key_info.h"
#include "tyon_device.h"
#include "roccat_helper.h"
#include "roccat.h"
#include "i18n-lib.h"
#include <string.h>

guint8 tyon_rmp_macro_key_info_calc_checksum(TyonRmpMacroKeyInfo const *rmp_macro_key_info) {
	return ROCCAT_BYTESUM_PARTIALLY(rmp_macro_key_info, TyonRmpMacroKeyInfo, button_index, checksum);
}

void tyon_rmp_macro_key_info_set_checksum(TyonRmpMacroKeyInfo *rmp_macro_key_info) {
	rmp_macro_key_info->checksum = tyon_rmp_macro_key_info_calc_checksum(rmp_macro_key_info);
}

static void tyon_rmp_macro_key_info_set_macroset_name(TyonRmpMacroKeyInfo *macro_key_info, gchar const *new_name) {
	g_strlcpy((gchar *)(macro_key_info->macroset_name), new_name, TYON_RMP_MACRO_KEY_INFO_MACROSET_NAME_LENGTH);
}

static void tyon_rmp_macro_key_info_set_macro_name(TyonRmpMacroKeyInfo *macro_key_info, gchar const *new_name) {
	g_strlcpy((gchar *)(macro_key_info->macro_name), new_name, TYON_RMP_MACRO_KEY_INFO_MACRO_NAME_LENGTH);
}

static void tyon_rmp_macro_key_info_set_filename(TyonRmpMacroKeyInfo *macro_key_info, gchar const *new_name) {
	g_strlcpy((gchar *)(macro_key_info->filename), new_name, TYON_RMP_MACRO_KEY_INFO_FILENAME_LENGTH);
}

static void tyon_rmp_macro_key_info_set_timer_name(TyonRmpMacroKeyInfo *macro_key_info, gchar const *new_name) {
	g_strlcpy((gchar *)(macro_key_info->timer_name), new_name, TYON_RMP_MACRO_KEY_INFO_TIMER_NAME_LENGTH);
}

TyonRmpMacroKeyInfo *tyon_rmp_macro_key_info_new(void) {
	return (TyonRmpMacroKeyInfo *)g_malloc0(sizeof(TyonRmpMacroKeyInfo));
}

void tyon_rmp_macro_key_info_free(TyonRmpMacroKeyInfo *macro_key_info) {
	g_free(macro_key_info);
}

TyonRmpMacroKeyInfo *gaminggear_macro_to_tyon_rmp_macro_key_info(GaminggearMacro const *gaminggear_macro, GError **error) {
	TyonRmpMacroKeyInfo *result;
	guint i, count;

	g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

	count = gaminggear_macro_keystrokes_get_count(&gaminggear_macro->keystrokes);
	if (count > TYON_RMP_MACRO_KEY_INFO_KEYSTROKES_NUM) {
		g_set_error(error, ROCCAT_ERROR, ROCCAT_ERROR_MACRO_TOO_LONG,
				_("Macro contains %u actions while device only supports %u actions"), count, TYON_RMP_MACRO_KEY_INFO_KEYSTROKES_NUM);
		return NULL;
	}

	result = tyon_rmp_macro_key_info_new();
	result->type = TYON_BUTTON_TYPE_MACRO;
	tyon_rmp_macro_key_info_set_macroset_name(result, gaminggear_macro->macroset);
	tyon_rmp_macro_key_info_set_macro_name(result, gaminggear_macro->macro);
	tyon_rmp_macro_key_info_set_count(result, count);
	result->loop = gaminggear_macro->keystrokes.loop;

	for (i = 0; i < count; ++i) {
		gaminggear_macro_keystroke_to_roccat_keystroke(&gaminggear_macro->keystrokes.keystrokes[i], &result->keystrokes[i]);
	}

	return result;
}

TyonRmpMacroKeyInfo *tyon_rmp_macro_key_info_new_quicklaunch(gchar const *filename) {
	TyonRmpMacroKeyInfo *result = tyon_rmp_macro_key_info_new();
	result->type = TYON_BUTTON_TYPE_QUICKLAUNCH;
	tyon_rmp_macro_key_info_set_filename(result, filename);
	return result;
}

TyonRmpMacroKeyInfo *tyon_rmp_macro_key_info_new_special(TyonButtonType type) {
	TyonRmpMacroKeyInfo *result = tyon_rmp_macro_key_info_new();
	result->type = type;
	return result;
}

TyonRmpMacroKeyInfo *tyon_rmp_macro_key_info_new_timer(gchar const *name, guint seconds) {
	TyonRmpMacroKeyInfo *result = tyon_rmp_macro_key_info_new();
	result->type = TYON_BUTTON_TYPE_TIMER;
	tyon_rmp_macro_key_info_set_timer_name(result, name);
	tyon_rmp_macro_key_info_set_timer_length(result, seconds);
	return result;
}

TyonRmpMacroKeyInfo *tyon_rmp_macro_key_info_new_talk(TyonButtonType type, guint talk_device) {
	TyonRmpMacroKeyInfo *result = tyon_rmp_macro_key_info_new();
	result->type = type;
	tyon_rmp_macro_key_info_set_talk_device(result, talk_device);
	return result;
}

TyonRmpMacroKeyInfo *tyon_rmp_macro_key_info_new_shortcut(guint hid_usage_id,
		guint8 modifier) {
	TyonRmpMacroKeyInfo *result = tyon_rmp_macro_key_info_new();

	result->type = TYON_BUTTON_TYPE_SHORTCUT;
	result->keystrokes[0].key = hid_usage_id;
	tyon_rmp_macro_key_info_set_count(result, 1);
	result->keystrokes[0].action = modifier;

	return result;
}

gboolean tyon_rmp_macro_key_info_equal(TyonRmpMacroKeyInfo const *left, TyonRmpMacroKeyInfo const *right) {
	gboolean equal;
	equal = ROCCAT_MEMCMP_PARTIALLY(left, right, TyonRmpMacroKeyInfo, type, checksum);
	return equal ? FALSE : TRUE;
}

TyonRmpMacroKeyInfo *tyon_rmp_macro_key_info_dup(TyonRmpMacroKeyInfo const *source) {
	return g_memdup((gconstpointer)source, sizeof(TyonRmpMacroKeyInfo));
}

TyonMacro *tyon_rmp_macro_key_info_to_macro(TyonRmpMacroKeyInfo const *macro_key_info) {
	TyonMacro *macro;
	guint i;

	if (macro_key_info->type != TYON_BUTTON_TYPE_MACRO)
		return NULL;

	macro = g_malloc0(sizeof(TyonMacro));

	macro->button_index = macro_key_info->button_index;
	macro->loop = tyon_rmp_macro_key_info_get_loop(macro_key_info);
	tyon_macro_set_macroset_name(macro, (gchar const *)macro_key_info->macroset_name);
	tyon_macro_set_macro_name(macro, (gchar const *)macro_key_info->macro_name);
	tyon_macro_set_count(macro, tyon_rmp_macro_key_info_get_count(macro_key_info));

	for (i = 0; i < tyon_macro_get_count(macro); ++i)
		macro->keystrokes[i] = macro_key_info->keystrokes[i];

	return macro;
}

TyonRmpMacroKeyInfo *tyon_macro_to_rmp_macro_key_info(TyonMacro const *macro) {
	TyonRmpMacroKeyInfo *key_info;
	key_info = tyon_rmp_macro_key_info_new();
	guint i, count;

	count = tyon_macro_get_count(macro);
	count = MIN(count, TYON_RMP_MACRO_KEY_INFO_KEYSTROKES_NUM);
	count = MIN(count, TYON_MACRO_KEYSTROKES_NUM);

	key_info->button_index = macro->button_index;
	key_info->type = TYON_BUTTON_TYPE_MACRO;
	tyon_rmp_macro_key_info_set_macroset_name(key_info, (gchar const *)macro->macroset_name);
	tyon_rmp_macro_key_info_set_macro_name(key_info, (gchar const *)macro->macro_name);
	tyon_rmp_macro_key_info_set_loop(key_info, macro->loop);
	tyon_rmp_macro_key_info_set_count(key_info, count);
	for (i = 0; i < count; ++i)
		key_info->keystrokes[i] = macro->keystrokes[i];

	return key_info;
}
