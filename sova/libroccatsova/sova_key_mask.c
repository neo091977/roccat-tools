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

#include "sova_key_mask.h"
#include "sova.h"
#include "roccat_helper.h"
#include <string.h>

static guchar const default_key_mask[sizeof(SovaKeyMask)] = { 0x0c, 0x06, 0x00, 0x3f, 0x51, 0x00 };

SovaKeyMask const *sova_key_mask_default(void) {
	return (SovaKeyMask const *)default_key_mask;
}

static guint16 sova_key_mask_calc_checksum(SovaKeyMask const *key_mask) {
	return ROCCAT_BYTESUM_PARTIALLY(key_mask, SovaKeyMask, report_id, checksum);
}

static void sova_key_mask_set_checksum(SovaKeyMask *key_mask, guint16 new_value) {
	key_mask->checksum = GUINT16_TO_LE(new_value);
}

static void sova_key_mask_finalize(SovaKeyMask *key_mask, guint profile_index) {
	key_mask->report_id = SOVA_REPORT_ID_KEY_MASK;
	key_mask->size = sizeof(SovaKeyMask);
	key_mask->profile_index = profile_index;
	sova_key_mask_set_checksum(key_mask, sova_key_mask_calc_checksum(key_mask));
}

gboolean sova_key_mask_write(RoccatDevice *device, guint profile_index, SovaKeyMask *key_mask, GError **error) {
	g_assert(profile_index < SOVA_PROFILE_NUM);
	sova_key_mask_finalize(key_mask, profile_index);
	return sova_device_write(device, (gchar const *)key_mask, sizeof(SovaKeyMask), error);
}

SovaKeyMask *sova_key_mask_read(RoccatDevice *device, guint profile_index, GError **error) {
	SovaKeyMask *key_mask;

	g_assert(profile_index < SOVA_PROFILE_NUM);

	gaminggear_device_lock(GAMINGGEAR_DEVICE(device));

	if (!sova_select(device, profile_index, SOVA_CONTROL_REQUEST_KEY_MASK, error)) {
		gaminggear_device_unlock(GAMINGGEAR_DEVICE(device));
		return NULL;
	}

	key_mask = (SovaKeyMask *)sova_device_read(device, SOVA_REPORT_ID_KEY_MASK, sizeof(SovaKeyMask), error);

	gaminggear_device_unlock(GAMINGGEAR_DEVICE(device));

	return key_mask;
}

gboolean sova_key_mask_equal(SovaKeyMask const *left, SovaKeyMask const *right) {
	gboolean equal;
	equal = ROCCAT_MEMCMP_PARTIALLY(left, right, SovaKeyMask, mask, checksum);
	return equal ? FALSE : TRUE;
}

void sova_key_mask_copy(SovaKeyMask *destination, SovaKeyMask const *source) {
	memcpy(destination, source, sizeof(SovaKeyMask));
}
