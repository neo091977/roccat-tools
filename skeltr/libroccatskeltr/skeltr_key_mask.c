/*
 * This file is part of roccat-tools.
 *
 * roccat-tools is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Macro Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * roccat-tools is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Macro Public License for more details.
 *
 * You should have received a copy of the GNU Macro Public License
 * along with roccat-tools. If not, see <http://www.gnu.org/licenses/>.
 */

#include "skeltr_key_mask.h"
#include "skeltr.h"
#include "roccat_helper.h"
#include <string.h>

static guchar const default_key_mask[sizeof(SkeltrKeyMask)] = { 0x07, 0x06, 0x00, 0x3f, 0x4c, 0x00 };

SkeltrKeyMask const *skeltr_key_mask_default(void) {
	return (SkeltrKeyMask const *)default_key_mask;
}

static guint16 skeltr_key_mask_calc_checksum(SkeltrKeyMask const *key_mask) {
	return ROCCAT_BYTESUM_PARTIALLY(key_mask, SkeltrKeyMask, report_id, checksum);
}

static void skeltr_key_mask_set_checksum(SkeltrKeyMask *key_mask, guint16 new_value) {
	key_mask->checksum = GUINT16_TO_LE(new_value);
}

static void skeltr_key_mask_finalize(SkeltrKeyMask *key_mask, guint profile_index) {
	key_mask->report_id = SKELTR_REPORT_ID_KEY_MASK;
	key_mask->size = sizeof(SkeltrKeyMask);
	key_mask->profile_index = profile_index;
	skeltr_key_mask_set_checksum(key_mask, skeltr_key_mask_calc_checksum(key_mask));
}

gboolean skeltr_key_mask_write(RoccatDevice *device, guint profile_index, SkeltrKeyMask *key_mask, GError **error) {
	g_assert(profile_index < SKELTR_PROFILE_NUM);
	skeltr_key_mask_finalize(key_mask, profile_index);
	return skeltr_device_write(device, (gchar const *)key_mask, sizeof(SkeltrKeyMask), error);
}

SkeltrKeyMask *skeltr_key_mask_read(RoccatDevice *device, guint profile_index, GError **error) {
	SkeltrKeyMask *key_mask;

	g_assert(profile_index < SKELTR_PROFILE_NUM);

	gaminggear_device_lock(GAMINGGEAR_DEVICE(device));

	if (!skeltr_select(device, profile_index, SKELTR_CONTROL_REQUEST_KEY_MASK, error)) {
		gaminggear_device_unlock(GAMINGGEAR_DEVICE(device));
		return NULL;
	}

	key_mask = (SkeltrKeyMask *)skeltr_device_read(device, SKELTR_REPORT_ID_KEY_MASK, sizeof(SkeltrKeyMask), error);

	gaminggear_device_unlock(GAMINGGEAR_DEVICE(device));

	return key_mask;
}

gboolean skeltr_key_mask_equal(SkeltrKeyMask const *left, SkeltrKeyMask const *right) {
	gboolean equal;
	equal = ROCCAT_MEMCMP_PARTIALLY(left, right, SkeltrKeyMask, mask, checksum);
	return equal ? FALSE : TRUE;
}

void skeltr_key_mask_copy(SkeltrKeyMask *destination, SkeltrKeyMask const *source) {
	memcpy(destination, source, sizeof(SkeltrKeyMask));
}
