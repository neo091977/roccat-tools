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

#include "skeltr_keys_easyzone.h"
#include "skeltr.h"
#include "roccat_helper.h"
#include <string.h>

static guint8 const default_keys_easyzone[sizeof(SkeltrKeysEasyzone)] = {
	0x09, 0x41, 0x00, 0x1E, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x20, 0x00, 0x00, 0x21, 0x00, 0x00, 0x22,
	0x00, 0x00, 0x14, 0x00, 0x00, 0x1A, 0x00, 0x00, 0x08, 0x00, 0x00, 0x15, 0x00, 0x00, 0x17, 0x00,
	0x00, 0x04, 0x00, 0x00, 0x16, 0x00, 0x00, 0x07, 0x00, 0x00, 0x09, 0x00, 0x00, 0x0A, 0x00, 0x00,
	0x1D, 0x00, 0x00, 0x1B, 0x00, 0x00, 0x06, 0x00, 0x00, 0x19, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00,
	0x00
};

SkeltrKeysEasyzone const *skeltr_keys_easyzone_default(void) {
	return (SkeltrKeysEasyzone const *)default_keys_easyzone;
}

static guint16 skeltr_keys_easyzone_calc_checksum(SkeltrKeysEasyzone const *keys_easyzone) {
	return ROCCAT_BYTESUM_PARTIALLY(keys_easyzone, SkeltrKeysEasyzone, report_id, checksum);
}

static void skeltr_keys_easyzone_set_checksum(SkeltrKeysEasyzone *keys_easyzone, guint16 new_value) {
	keys_easyzone->checksum = GUINT16_TO_LE(new_value);
}

static void skeltr_keys_easyzone_finalize(SkeltrKeysEasyzone *keys_easyzone, guint profile_index) {
	keys_easyzone->report_id = SKELTR_REPORT_ID_KEYS_EASYZONE;
	keys_easyzone->size = sizeof(SkeltrKeysEasyzone);
	keys_easyzone->profile_index = profile_index;
	skeltr_keys_easyzone_set_checksum(keys_easyzone, skeltr_keys_easyzone_calc_checksum(keys_easyzone));
}

gboolean skeltr_keys_easyzone_write(RoccatDevice *device, guint profile_index, SkeltrKeysEasyzone *keys_easyzone, GError **error) {
	g_assert(profile_index < SKELTR_PROFILE_NUM);
	skeltr_keys_easyzone_finalize(keys_easyzone, profile_index);
	return skeltr_device_write(device, (gchar const *)keys_easyzone, sizeof(SkeltrKeysEasyzone), error);
}

SkeltrKeysEasyzone *skeltr_keys_easyzone_read(RoccatDevice *device, guint profile_index, GError **error) {
	SkeltrKeysEasyzone *keys_easyzone;

	g_assert(profile_index < SKELTR_PROFILE_NUM);

	gaminggear_device_lock(GAMINGGEAR_DEVICE(device));

	if (!skeltr_select(device, profile_index, SKELTR_CONTROL_REQUEST_KEYS_EASYZONE, error)) {
		gaminggear_device_unlock(GAMINGGEAR_DEVICE(device));
		return NULL;
	}

	keys_easyzone = (SkeltrKeysEasyzone *)skeltr_device_read(device, SKELTR_REPORT_ID_KEYS_EASYZONE, sizeof(SkeltrKeysEasyzone), error);

	gaminggear_device_unlock(GAMINGGEAR_DEVICE(device));

	return keys_easyzone;
}

gboolean skeltr_keys_easyzone_equal(SkeltrKeysEasyzone const *left, SkeltrKeysEasyzone const *right) {
	gboolean equal;
	equal = ROCCAT_MEMCMP_PARTIALLY(left, right, SkeltrKeysEasyzone, keys[0], checksum);
	return equal ? FALSE : TRUE;
}

void skeltr_keys_easyzone_copy(SkeltrKeysEasyzone *destination, SkeltrKeysEasyzone const *source) {
	memcpy(destination, source, sizeof(SkeltrKeysEasyzone));
}

guint skeltr_keys_easyzone_index_to_macro_index(guint easyzone_index) {
	return easyzone_index + 16;
}

guint skeltr_macro_index_to_keys_easyzone_index(guint macro_index) {
	return macro_index - 16;
}

gboolean skeltr_macro_index_is_keys_easyzone(guint macro_index) {
	return macro_index >= 16 && macro_index < 36;
}
