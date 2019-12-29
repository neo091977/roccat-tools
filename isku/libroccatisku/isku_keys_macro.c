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

#include "isku_rkp.h"
#include "roccat_helper.h"
#include <string.h>

gboolean isku_key_index_is_keys_macro(guint key_index) {
	return key_index >= ISKU_KEY_INDEX_M1 && key_index <= ISKU_KEY_INDEX_SHIFT_M5;
}

IskuKeysMacro *isku_rkp_to_keys_macro(IskuRkp *rkp) {
	IskuKeysMacro *keys_macro;
	IskuRkpMacroKeyInfo *key_info;
	guint i;

	keys_macro = g_malloc0(sizeof(IskuKeysMacro));

	for (i = ISKU_KEY_INDEX_M1; i <= ISKU_KEY_INDEX_SHIFT_M5; ++i) {
		key_info = isku_rkp_get_macro_key_info(rkp, i);
		keys_macro->types[i - ISKU_KEY_INDEX_M1] = key_info->type;
		keys_macro->shortcut_keys[i - ISKU_KEY_INDEX_M1] = key_info->keystrokes[0].key;
		keys_macro->shortcut_modifier[i - ISKU_KEY_INDEX_M1] = key_info->keystrokes[0].action;
		g_free(key_info);
	}

	return keys_macro;
}

void isku_rkp_update_with_keys_macro(IskuRkp *rkp, IskuKeysMacro const *keys) {
	IskuRkpMacroKeyInfo *key_info;
	guint i;

	if (!keys)
		return;

	for (i = 0; i < ISKU_KEYS_MACRO_NUM; ++i) {
		if (isku_rkp_macro_key_info_equal_type_actual(rkp, i + ISKU_KEY_INDEX_M1, keys->types[i]))
			continue;

		switch (keys->types[i]) {

		case ISKU_KEY_TYPE_SHORTCUT:
			key_info = isku_rkp_macro_key_info_new_shortcut(keys->shortcut_keys[i], keys->shortcut_modifier[i]);
			break;
		/* will be restored by isku_rkp_update_with_macro() */
		case ISKU_KEY_TYPE_MACRO:
			key_info = NULL;
			break;

		/* cannot be restored */
		case ISKU_KEY_TYPE_QUICKLAUNCH:
		case ISKU_KEY_TYPE_TIMER:
			key_info = isku_rkp_macro_key_info_new_special(ISKU_KEY_TYPE_STANDARD);
			break;
		default:
			key_info = isku_rkp_macro_key_info_new_special(keys->types[i]);
		}
		if (key_info) {
			isku_rkp_set_macro_key_info(rkp, i + ISKU_KEY_INDEX_M1, key_info);
			isku_rkp_macro_key_info_free(key_info);
		}
	}
}

static guint16 isku_keys_macro_calc_checksum(IskuKeysMacro const *keys_macro) {
	return ROCCAT_BYTESUM_PARTIALLY(keys_macro, IskuKeysMacro, report_id, checksum);
}

static void isku_keys_macro_set_checksum(IskuKeysMacro *keys_macro) {
	guint16 checksum = isku_keys_macro_calc_checksum(keys_macro);
	keys_macro->checksum = GUINT16_TO_LE(checksum);
}

static void isku_keys_macro_finalize(IskuKeysMacro *keys_macro, guint profile_number) {
	keys_macro->report_id = ISKU_REPORT_ID_KEYS_MACRO;
	keys_macro->size = sizeof(IskuKeysMacro);
	keys_macro->profile_number = profile_number;
	isku_keys_macro_set_checksum(keys_macro);
}

gboolean isku_keys_macro_write(RoccatDevice *isku, guint profile_number, IskuKeysMacro *keys_macro, GError **error) {
	g_assert(profile_number < ISKU_PROFILE_NUM);
	isku_keys_macro_finalize(keys_macro, profile_number);
	return isku_device_write(isku, (gchar const *)keys_macro, sizeof(IskuKeysMacro), error);
}

IskuKeysMacro *isku_keys_macro_read(RoccatDevice *isku, guint profile_number, GError **error) {
	IskuKeysMacro *keys_macro;

	g_assert(profile_number < ISKU_PROFILE_NUM);

	gaminggear_device_lock(GAMINGGEAR_DEVICE(isku));

	if (!isku_select(isku, profile_number, ISKU_CONTROL_REQUEST_KEYS_MACRO, error)) {
		gaminggear_device_unlock(GAMINGGEAR_DEVICE(isku));
		return NULL;
	}

	keys_macro = (IskuKeysMacro *)isku_device_read(isku, ISKU_REPORT_ID_KEYS_MACRO, sizeof(IskuKeysMacro), error);

	gaminggear_device_unlock(GAMINGGEAR_DEVICE(isku));

	return keys_macro;
}

gboolean isku_keys_macro_equal(IskuKeysMacro const *left, IskuKeysMacro const *right) {
	gboolean equal;
	equal = ROCCAT_MEMCMP_PARTIALLY(left, right, IskuKeysMacro, types[0], checksum);
	return equal ? FALSE : TRUE;
}
