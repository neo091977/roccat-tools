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

gboolean isku_key_index_is_keys_thumbster(guint key_index) {
	return key_index >= ISKU_KEY_INDEX_T1 && key_index <= ISKU_KEY_INDEX_SHIFT_T3;
}

IskuKeysThumbster *isku_rkp_to_keys_thumbster(IskuRkp *rkp) {
	IskuKeysThumbster *keys_thumbster;
	IskuRkpMacroKeyInfo *key_info;
	guint i;

	keys_thumbster = g_malloc0(sizeof(IskuKeysThumbster));

	for (i = ISKU_KEY_INDEX_T1; i <= ISKU_KEY_INDEX_SHIFT_T3; ++i) {
		key_info = isku_rkp_get_macro_key_info(rkp, i);
		keys_thumbster->types[i - ISKU_KEY_INDEX_T1] = key_info->type;
		keys_thumbster->shortcut_keys[i - ISKU_KEY_INDEX_T1] = key_info->keystrokes[0].key;
		keys_thumbster->shortcut_modifier[i - ISKU_KEY_INDEX_T1] = key_info->keystrokes[0].action;
		isku_rkp_macro_key_info_free(key_info);
	}

	return keys_thumbster;
}

void isku_rkp_update_with_keys_thumbster(IskuRkp *rkp, IskuKeysThumbster const *keys) {
	IskuRkpMacroKeyInfo *key_info;
	guint i;

	if (!keys)
		return;

	for (i = 0; i < ISKU_KEYS_THUMBSTER_NUM; ++i) {
		if (isku_rkp_macro_key_info_equal_type_actual(rkp, i + ISKU_KEY_INDEX_T1, keys->types[i]))
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
			if (isku_rkp_macro_key_info_equal_type_actual(rkp, i + ISKU_KEY_INDEX_T1, keys->types[i]))
				key_info = NULL;
			else
				key_info = isku_rkp_macro_key_info_new_special(ISKU_KEY_TYPE_STANDARD);
			break;
		default:
			key_info = isku_rkp_macro_key_info_new_special(keys->types[i]);
		}
		if (key_info) {
			isku_rkp_set_macro_key_info(rkp, i + ISKU_KEY_INDEX_T1, key_info);
			g_free(key_info);
		}
	}
}

static guint16 isku_keys_thumbster_calc_checksum(IskuKeysThumbster const *keys_thumbster) {
	return ROCCAT_BYTESUM_PARTIALLY(keys_thumbster, IskuKeysThumbster, report_id, checksum);
}

static void isku_keys_thumbster_set_checksum(IskuKeysThumbster *keys_thumbster) {
	guint16 checksum = isku_keys_thumbster_calc_checksum(keys_thumbster);
	keys_thumbster->checksum = GUINT16_TO_LE(checksum);
}

static void isku_keys_thumbster_finalize(IskuKeysThumbster *keys_thumbster, guint profile_number) {
	keys_thumbster->report_id = ISKU_REPORT_ID_KEYS_THUMBSTER;
	keys_thumbster->size = sizeof(IskuKeysThumbster);
	keys_thumbster->profile_number = profile_number;
	isku_keys_thumbster_set_checksum(keys_thumbster);
}

gboolean isku_keys_thumbster_write(RoccatDevice *isku, guint profile_number, IskuKeysThumbster *keys_thumbster, GError **error) {
	g_assert(profile_number < ISKU_PROFILE_NUM);
	isku_keys_thumbster_finalize(keys_thumbster, profile_number);
	return isku_device_write(isku, (gchar const *)keys_thumbster, sizeof(IskuKeysThumbster), error);
}

IskuKeysThumbster *isku_keys_thumbster_read(RoccatDevice *isku, guint profile_number, GError **error) {
	IskuKeysThumbster *keys_thumbster;

	g_assert(profile_number < ISKU_PROFILE_NUM);

	gaminggear_device_lock(GAMINGGEAR_DEVICE(isku));

	if (!isku_select(isku, profile_number, ISKU_CONTROL_REQUEST_KEYS_THUMBSTER, error)) {
		gaminggear_device_unlock(GAMINGGEAR_DEVICE(isku));
		return NULL;
	}

	keys_thumbster = (IskuKeysThumbster *)isku_device_read(isku, ISKU_REPORT_ID_KEYS_THUMBSTER, sizeof(IskuKeysThumbster), error);

	gaminggear_device_unlock(GAMINGGEAR_DEVICE(isku));

	return keys_thumbster;
}

gboolean isku_keys_thumbster_equal(IskuKeysThumbster const *left, IskuKeysThumbster const *right) {
	gboolean equal;
	equal = ROCCAT_MEMCMP_PARTIALLY(left, right, IskuKeysThumbster, types[0], checksum);
	return equal ? FALSE : TRUE;
}
