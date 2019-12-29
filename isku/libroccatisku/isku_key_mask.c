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

IskuKeyMask *isku_rkp_to_key_mask(IskuRkp *rkp) {
	IskuKeyMask *key_mask;

	key_mask = g_malloc0(sizeof(IskuKeyMask));
	key_mask->mask = isku_rkp_get_key_mask(rkp);

	return key_mask;
}

void isku_rkp_update_with_key_mask(IskuRkp *rkp, IskuKeyMask const *key_mask) {
	if (key_mask)
		isku_rkp_set_key_mask(rkp, key_mask->mask);
}

static guint16 isku_key_mask_calc_checksum(IskuKeyMask const *key_mask) {
	return ROCCAT_BYTESUM_PARTIALLY(key_mask, IskuKeyMask, report_id, checksum);
}

static void isku_key_mask_set_checksum(IskuKeyMask *key_mask) {
	guint16 checksum = isku_key_mask_calc_checksum(key_mask);
	key_mask->checksum = GUINT16_TO_LE(checksum);
}

static void isku_key_mask_finalize(IskuKeyMask *key_mask, guint profile_number) {
	key_mask->report_id = ISKU_REPORT_ID_KEY_MASK;
	key_mask->size = sizeof(IskuKeyMask);
	key_mask->profile_number = profile_number;
	isku_key_mask_set_checksum(key_mask);
}

void isku_rkp_set_key_mask(IskuRkp *rkp, guint8 mask) {
	guint i;

	for (i = 0; i < ISKU_KEY_MASK_BIT_NUM; ++i)
		isku_rkp_set_modify_key(rkp, i, roccat_get_bit8(mask, i));
}

guint8 isku_rkp_get_key_mask(IskuRkp *rkp) {
	guint8 result = 0;
	guint i;

	for (i = 0; i < ISKU_KEY_MASK_BIT_NUM; ++i)
		roccat_set_bit8(&result, i, isku_rkp_get_modify_key(rkp, i));
	return result;
}

gboolean isku_key_mask_write(RoccatDevice *isku, guint profile_number, IskuKeyMask *key_mask, GError **error) {
	g_assert(profile_number < ISKU_PROFILE_NUM);
	isku_key_mask_finalize(key_mask, profile_number);
	return isku_device_write(isku, (gchar const *)key_mask, sizeof(IskuKeyMask), error);
}

IskuKeyMask *isku_key_mask_read(RoccatDevice *isku, guint profile_number, GError **error) {
	IskuKeyMask *key_mask;

	g_assert(profile_number < ISKU_PROFILE_NUM);

	gaminggear_device_lock(GAMINGGEAR_DEVICE(isku));

	if (!isku_select(isku, profile_number, ISKU_CONTROL_REQUEST_KEY_MASK, error)) {
		gaminggear_device_unlock(GAMINGGEAR_DEVICE(isku));
		return NULL;
	}

	key_mask = (IskuKeyMask *)isku_device_read(isku, ISKU_REPORT_ID_KEY_MASK, sizeof(IskuKeyMask), error);

	gaminggear_device_unlock(GAMINGGEAR_DEVICE(isku));

	return key_mask;
}

gboolean isku_key_mask_equal(IskuKeyMask const *left, IskuKeyMask const *right) {
	return left->mask == right->mask;
}
