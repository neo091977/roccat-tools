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
#include "i18n-lib.h"
#include <string.h>

gboolean isku_key_index_is_keys_function(guint key_index) {
	return key_index >= ISKU_KEY_INDEX_F1 && key_index <= ISKU_KEY_INDEX_F12;
}

IskuKeysFunction *isku_rkp_to_keys_function(IskuRkp *rkp) {
	IskuKeysFunction *keys_function;
	IskuRkpMacroKeyInfo *key_info;
	guint i;

	keys_function = g_malloc0(sizeof(IskuKeysFunction));

	for (i = ISKU_KEY_INDEX_F1; i <= ISKU_KEY_INDEX_F12; ++i) {
		key_info = isku_rkp_get_macro_key_info(rkp, i);
		keys_function->types[i - ISKU_KEY_INDEX_F1] = key_info->type;
		keys_function->shortcut_keys[i - ISKU_KEY_INDEX_F1] = key_info->keystrokes[0].key;
		keys_function->shortcut_modifier[i - ISKU_KEY_INDEX_F1] = key_info->keystrokes[0].action;
		isku_rkp_macro_key_info_free(key_info);
	}

	return keys_function;
}

void isku_rkp_update_with_keys_function(IskuRkp *rkp, IskuKeysFunction const *keys) {
	IskuRkpMacroKeyInfo *key_info;
	guint i;

	if (!keys)
		return;

	for (i = 0; i < ISKU_KEYS_FUNCTION_NUM; ++i) {
		if (isku_rkp_macro_key_info_equal_type_actual(rkp, i + ISKU_KEY_INDEX_F1, keys->types[i]))
			continue;

		switch (keys->types[i]) {

		case ISKU_KEY_TYPE_SHORTCUT:
			key_info = isku_rkp_macro_key_info_new_shortcut(keys->shortcut_keys[i], keys->shortcut_modifier[i]);
			break;
		case ISKU_KEY_TYPE_MACRO:
			g_warning(_("Function key 0x%02x can not be of type 0x%02x"), i + ISKU_KEY_INDEX_F1, keys->types[i]);
			key_info = isku_rkp_macro_key_info_new_special(ISKU_KEY_TYPE_STANDARD);
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
			isku_rkp_set_macro_key_info(rkp, i + ISKU_KEY_INDEX_F1, key_info);
			g_free(key_info);
		}
	}
}

static guint16 isku_keys_function_calc_checksum(IskuKeysFunction const *keys_function) {
	return ROCCAT_BYTESUM_PARTIALLY(keys_function, IskuKeysFunction, report_id, checksum);
}

static void isku_keys_function_set_checksum(IskuKeysFunction *keys_function) {
	guint16 checksum = isku_keys_function_calc_checksum(keys_function);
	keys_function->checksum = GUINT16_TO_LE(checksum);
}

static void isku_keys_function_finalize(IskuKeysFunction *keys_function, guint profile_number) {
	keys_function->report_id = ISKU_REPORT_ID_KEYS_FUNCTION;
	keys_function->size = sizeof(IskuKeysFunction);
	keys_function->profile_number = profile_number;
	isku_keys_function_set_checksum(keys_function);
}

gboolean isku_keys_function_write(RoccatDevice *isku, guint profile_number, IskuKeysFunction *keys_function, GError **error) {
	g_assert(profile_number < ISKU_PROFILE_NUM);
	isku_keys_function_finalize(keys_function, profile_number);
	return isku_device_write(isku, (gchar const *)keys_function, sizeof(IskuKeysFunction), error);
}

IskuKeysFunction *isku_keys_function_read(RoccatDevice *isku, guint profile_number, GError **error) {
	IskuKeysFunction *keys_function;

	g_assert(profile_number < ISKU_PROFILE_NUM);

	gaminggear_device_lock(GAMINGGEAR_DEVICE(isku));

	if (!isku_select(isku, profile_number, ISKU_CONTROL_REQUEST_KEYS_FUNCTION, error)) {
		gaminggear_device_unlock(GAMINGGEAR_DEVICE(isku));
		return NULL;
	}

	keys_function = (IskuKeysFunction *)isku_device_read(isku, ISKU_REPORT_ID_KEYS_FUNCTION, sizeof(IskuKeysFunction), error);

	gaminggear_device_unlock(GAMINGGEAR_DEVICE(isku));

	return keys_function;
}

gboolean isku_keys_function_equal(IskuKeysFunction const *left, IskuKeysFunction const *right) {
	gboolean equal;
	equal = ROCCAT_MEMCMP_PARTIALLY(left, right, IskuKeysFunction, types[0], checksum);
	return equal ? FALSE : TRUE;
}
