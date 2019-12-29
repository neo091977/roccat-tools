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

gboolean isku_key_index_is_keys_media(guint key_index) {
	return key_index >= ISKU_KEY_INDEX_MEDIA_MUTE && key_index <= ISKU_KEY_INDEX_MEDIA_COMPUTER;
}

IskuKeysMedia *isku_rkp_to_keys_media(IskuRkp *rkp) {
	IskuKeysMedia *keys_media;
	IskuRkpMacroKeyInfo *key_info;
	guint i;

	keys_media = g_malloc0(sizeof(IskuKeysMedia));

	for (i = ISKU_KEY_INDEX_MEDIA_MUTE; i <= ISKU_KEY_INDEX_MEDIA_COMPUTER; ++i) {
		key_info = isku_rkp_get_macro_key_info(rkp, i);
		keys_media->types[i - ISKU_KEY_INDEX_MEDIA_MUTE] = key_info->type;
		keys_media->shortcut_keys[i - ISKU_KEY_INDEX_MEDIA_MUTE] = key_info->keystrokes[0].key;
		keys_media->shortcut_modifier[i - ISKU_KEY_INDEX_MEDIA_MUTE] = key_info->keystrokes[0].action;
		isku_rkp_macro_key_info_free(key_info);
	}

	return keys_media;
}

void isku_rkp_update_with_keys_media(IskuRkp *rkp, IskuKeysMedia const *keys) {
	IskuRkpMacroKeyInfo *key_info;
	guint i;

	if (!keys)
		return;

	for (i = 0; i < ISKU_KEYS_MEDIA_NUM; ++i) {
		if (isku_rkp_macro_key_info_equal_type_actual(rkp, i + ISKU_KEY_INDEX_MEDIA_MUTE, keys->types[i]))
			continue;

		switch (keys->types[i]) {

		case ISKU_KEY_TYPE_SHORTCUT:
			key_info = isku_rkp_macro_key_info_new_shortcut(keys->shortcut_keys[i], keys->shortcut_modifier[i]);
			break;
		case ISKU_KEY_TYPE_MACRO:
			g_warning(_("Media key 0x%02x can not be of type 0x%02x"), i + ISKU_KEY_INDEX_MEDIA_MUTE, keys->types[i]);
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
			isku_rkp_set_macro_key_info(rkp, i + ISKU_KEY_INDEX_MEDIA_MUTE, key_info);
			g_free(key_info);
		}
	}
}

static guint16 isku_keys_media_calc_checksum(IskuKeysMedia const *keys_media) {
	return ROCCAT_BYTESUM_PARTIALLY(keys_media, IskuKeysMedia, report_id, checksum);
}

static void isku_keys_media_set_checksum(IskuKeysMedia *keys_media) {
	guint16 checksum = isku_keys_media_calc_checksum(keys_media);
	keys_media->checksum = GUINT16_TO_LE(checksum);
}

static void isku_keys_media_finalize(IskuKeysMedia *keys_media, guint profile_number) {
	keys_media->report_id = ISKU_REPORT_ID_KEYS_MEDIA;
	keys_media->size = sizeof(IskuKeysMedia);
	keys_media->profile_number = profile_number;
	isku_keys_media_set_checksum(keys_media);
}

gboolean isku_keys_media_write(RoccatDevice *isku, guint profile_number, IskuKeysMedia *keys_media, GError **error) {
	g_assert(profile_number < ISKU_PROFILE_NUM);
	isku_keys_media_finalize(keys_media, profile_number);
	return isku_device_write(isku, (gchar const *)keys_media, sizeof(IskuKeysMedia), error);
}

IskuKeysMedia *isku_keys_media_read(RoccatDevice *isku, guint profile_number, GError **error) {
	IskuKeysMedia *keys_media;

	g_assert(profile_number < ISKU_PROFILE_NUM);

	gaminggear_device_lock(GAMINGGEAR_DEVICE(isku));

	if (!isku_select(isku, profile_number, ISKU_CONTROL_REQUEST_KEYS_MEDIA, error)) {
		gaminggear_device_unlock(GAMINGGEAR_DEVICE(isku));
		return NULL;
	}

	keys_media = (IskuKeysMedia *)isku_device_read(isku, ISKU_REPORT_ID_KEYS_MEDIA, sizeof(IskuKeysMedia), error);

	gaminggear_device_unlock(GAMINGGEAR_DEVICE(isku));

	return keys_media;
}

gboolean isku_keys_media_equal(IskuKeysMedia const *left, IskuKeysMedia const *right) {
	gboolean equal;
	equal = ROCCAT_MEMCMP_PARTIALLY(left, right, IskuKeysMedia, types[0], checksum);
	return equal ? FALSE : TRUE;
}
