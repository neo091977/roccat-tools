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

gboolean isku_key_index_is_keys_easyzone(guint key_index) {
	return key_index >= ISKU_KEY_INDEX_1 && key_index <= ISKU_KEY_INDEX_B;
}

IskuKeysEasyzone *isku_rkp_to_keys_easyzone(IskuRkp *rkp) {
	IskuKeysEasyzone *keys_easyzone;
	IskuRkpMacroKeyInfo *key_info;
	guint i;

	keys_easyzone = g_malloc0(sizeof(IskuKeysEasyzone));

	for (i = ISKU_KEY_INDEX_1; i <= ISKU_KEY_INDEX_B; ++i) {
		key_info = isku_rkp_get_macro_key_info(rkp, i);
		keys_easyzone->types[i - ISKU_KEY_INDEX_1] = key_info->type;
		keys_easyzone->shortcut_keys[i - ISKU_KEY_INDEX_1] = key_info->keystrokes[0].key;
		keys_easyzone->shortcut_modifier[i - ISKU_KEY_INDEX_1] = key_info->keystrokes[0].action;
		isku_rkp_macro_key_info_free(key_info);
	}

	return keys_easyzone;
}

void isku_rkp_update_with_keys_easyzone(IskuRkp *rkp, IskuKeysEasyzone const *keys) {
	IskuRkpMacroKeyInfo *key_info;
	guint i;

	if (!keys)
		return;

	for (i = 0; i < ISKU_KEYS_EASYZONE_NUM; ++i) {
		if (isku_rkp_macro_key_info_equal_type_actual(rkp, i + ISKU_KEY_INDEX_1, keys->types[i]))
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
			isku_rkp_set_macro_key_info(rkp, i + ISKU_KEY_INDEX_1, key_info);
			g_free(key_info);
		}
	}
}

static guint16 isku_keys_easyzone_calc_checksum(IskuKeysEasyzone const *keys_easyzone) {
	return ROCCAT_BYTESUM_PARTIALLY(keys_easyzone, IskuKeysEasyzone, report_id, checksum);
}

static void isku_keys_easyzone_set_checksum(IskuKeysEasyzone *keys_easyzone) {
	guint16 checksum = isku_keys_easyzone_calc_checksum(keys_easyzone);
	keys_easyzone->checksum = GUINT16_TO_LE(checksum);
}

static void isku_keys_easyzone_finalize(IskuKeysEasyzone *keys_easyzone, guint profile_number) {
	keys_easyzone->report_id = ISKU_REPORT_ID_KEYS_EASYZONE;
	keys_easyzone->size = sizeof(IskuKeysEasyzone);
	keys_easyzone->profile_number = profile_number;
	isku_keys_easyzone_set_checksum(keys_easyzone);
}

gboolean isku_keys_easyzone_write(RoccatDevice *isku, guint profile_number, IskuKeysEasyzone *keys_easyzone, GError **error) {
	g_assert(profile_number < ISKU_PROFILE_NUM);
	isku_keys_easyzone_finalize(keys_easyzone, profile_number);
	return isku_device_write(isku, (gchar const *)keys_easyzone, sizeof(IskuKeysEasyzone), error);
}

IskuKeysEasyzone *isku_keys_easyzone_read(RoccatDevice *isku, guint profile_number, GError **error) {
	IskuKeysEasyzone *keys_easyzone;

	g_assert(profile_number < ISKU_PROFILE_NUM);

	gaminggear_device_lock(GAMINGGEAR_DEVICE(isku));

	if (!isku_select(isku, profile_number, ISKU_CONTROL_REQUEST_KEYS_EASYZONE, error)) {
		gaminggear_device_unlock(GAMINGGEAR_DEVICE(isku));
		return NULL;
	}

	keys_easyzone = (IskuKeysEasyzone *)isku_device_read(isku, ISKU_REPORT_ID_KEYS_EASYZONE, sizeof(IskuKeysEasyzone), error);

	gaminggear_device_unlock(GAMINGGEAR_DEVICE(isku));

	return keys_easyzone;
}

gboolean isku_keys_easyzone_equal(IskuKeysEasyzone const *left, IskuKeysEasyzone const *right) {
	gboolean equal;
	equal = ROCCAT_MEMCMP_PARTIALLY(left, right, IskuKeysEasyzone, types[0], checksum);
	return equal ? FALSE : TRUE;
}
