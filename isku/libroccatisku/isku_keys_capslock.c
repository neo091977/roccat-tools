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

gboolean isku_key_index_is_keys_capslock(guint key_index) {
	return key_index == ISKU_KEY_INDEX_CAPSLOCK;
}

IskuKeysCapslock *isku_rkp_to_keys_capslock(IskuRkp *rkp) {
	IskuKeysCapslock *keys_capslock;
	IskuRkpMacroKeyInfo *key_info;

	keys_capslock = g_malloc0(sizeof(IskuKeysCapslock));
	key_info = isku_rkp_get_macro_key_info(rkp, ISKU_KEY_INDEX_CAPSLOCK);
	keys_capslock->type = key_info->type;
	g_free(key_info);

	return keys_capslock;
}

void isku_rkp_update_with_keys_capslock(IskuRkp *rkp, IskuKeysCapslock const *capslock) {
	IskuRkpMacroKeyInfo *key_info;

	if (!capslock)
		return;

	if (isku_rkp_macro_key_info_equal_type_actual(rkp, ISKU_KEY_INDEX_CAPSLOCK, capslock->type))
		return;

	switch (capslock->type) {

	case ISKU_KEY_TYPE_SHORTCUT:
	case ISKU_KEY_TYPE_MACRO:
		g_warning(_("Caps lock can not be of type 0x%02x"), capslock->type);
		key_info = isku_rkp_macro_key_info_new_special(ISKU_KEY_TYPE_STANDARD);
		break;

	/* cannot be restored */
	case ISKU_KEY_TYPE_QUICKLAUNCH:
	case ISKU_KEY_TYPE_TIMER:
		key_info = isku_rkp_macro_key_info_new_special(ISKU_KEY_TYPE_STANDARD);
		break;
	default:
		key_info = isku_rkp_macro_key_info_new_special(capslock->type);
	}
	if (key_info) {
		isku_rkp_set_macro_key_info(rkp, ISKU_KEY_INDEX_CAPSLOCK, key_info);
		isku_rkp_macro_key_info_free(key_info);
	}
}

static guint16 isku_keys_capslock_calc_checksum(IskuKeysCapslock const *keys_capslock) {
	return ROCCAT_BYTESUM_PARTIALLY(keys_capslock, IskuKeysCapslock, report_id, checksum);
}

static void isku_keys_capslock_set_checksum(IskuKeysCapslock *keys_capslock) {
	guint16 checksum = isku_keys_capslock_calc_checksum(keys_capslock);
	keys_capslock->checksum = GUINT16_TO_LE(checksum);
}

static void isku_keys_capslock_finalize(IskuKeysCapslock *keys_capslock, guint profile_number) {
	keys_capslock->report_id = ISKU_REPORT_ID_KEYS_CAPSLOCK;
	keys_capslock->size = sizeof(IskuKeysCapslock);
	keys_capslock->profile_number = profile_number;
	isku_keys_capslock_set_checksum(keys_capslock);
}

gboolean isku_keys_capslock_write(RoccatDevice *isku, guint profile_number, IskuKeysCapslock *keys_capslock, GError **error) {
	g_assert(profile_number < ISKU_PROFILE_NUM);
	isku_keys_capslock_finalize(keys_capslock, profile_number);
	return isku_device_write(isku, (gchar const *)keys_capslock, sizeof(IskuKeysCapslock), error);
}

IskuKeysCapslock *isku_keys_capslock_read(RoccatDevice *isku, guint profile_number, GError **error) {
	IskuKeysCapslock *keys_capslock;

	g_assert(profile_number < ISKU_PROFILE_NUM);

	gaminggear_device_lock(GAMINGGEAR_DEVICE(isku));

	if (!isku_select(isku, profile_number, ISKU_CONTROL_REQUEST_KEYS_CAPSLOCK, error)) {
		gaminggear_device_unlock(GAMINGGEAR_DEVICE(isku));
		return NULL;
	}

	keys_capslock = (IskuKeysCapslock *)isku_device_read(isku, ISKU_REPORT_ID_KEYS_CAPSLOCK, sizeof(IskuKeysCapslock), error);

	gaminggear_device_unlock(GAMINGGEAR_DEVICE(isku));

	return keys_capslock;
}

gboolean isku_keys_capslock_equal(IskuKeysCapslock const *left, IskuKeysCapslock const *right) {
	return left->type == right->type;
}
