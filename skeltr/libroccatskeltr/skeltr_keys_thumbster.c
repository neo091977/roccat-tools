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

#include "skeltr_keys_thumbster.h"
#include "skeltr.h"
#include "roccat_helper.h"
#include <string.h>

static guint8 const default_keys_thumbster[sizeof(SkeltrKeysThumbster)] = {
	0x0B, 0x17, 0x00, 0xB1, 0x00, 0x00, 0xDD, 0x00, 0x00, 0xB0, 0x00, 0x00, 0x39, 0x00, 0x00, 0xCB,
	0x00, 0x00, 0xC0, 0x00, 0x00, 0x24, 0x04
};

SkeltrKeysThumbster const *skeltr_keys_thumbster_default(void) {
	return (SkeltrKeysThumbster const *)default_keys_thumbster;
}

static guint16 skeltr_keys_thumbster_calc_checksum(SkeltrKeysThumbster const *keys_thumbster) {
	return ROCCAT_BYTESUM_PARTIALLY(keys_thumbster, SkeltrKeysThumbster, report_id, checksum);
}

static void skeltr_keys_thumbster_set_checksum(SkeltrKeysThumbster *keys_thumbster, guint16 new_value) {
	keys_thumbster->checksum = GUINT16_TO_LE(new_value);
}

static void skeltr_keys_thumbster_finalize(SkeltrKeysThumbster *keys_thumbster, guint profile_index) {
	keys_thumbster->report_id = SKELTR_REPORT_ID_KEYS_THUMBSTER;
	keys_thumbster->size = sizeof(SkeltrKeysThumbster);
	keys_thumbster->profile_index = profile_index;
	skeltr_keys_thumbster_set_checksum(keys_thumbster, skeltr_keys_thumbster_calc_checksum(keys_thumbster));
}

gboolean skeltr_keys_thumbster_write(RoccatDevice *device, guint profile_index, SkeltrKeysThumbster *keys_thumbster, GError **error) {
	g_assert(profile_index < SKELTR_PROFILE_NUM);
	skeltr_keys_thumbster_finalize(keys_thumbster, profile_index);
	return skeltr_device_write(device, (gchar const *)keys_thumbster, sizeof(SkeltrKeysThumbster), error);
}

SkeltrKeysThumbster *skeltr_keys_thumbster_read(RoccatDevice *device, guint profile_index, GError **error) {
	SkeltrKeysThumbster *keys_thumbster;

	g_assert(profile_index < SKELTR_PROFILE_NUM);

	gaminggear_device_lock(GAMINGGEAR_DEVICE(device));

	if (!skeltr_select(device, profile_index, SKELTR_CONTROL_REQUEST_KEYS_THUMBSTER, error)) {
		gaminggear_device_unlock(GAMINGGEAR_DEVICE(device));
		return NULL;
	}

	keys_thumbster = (SkeltrKeysThumbster *)skeltr_device_read(device, SKELTR_REPORT_ID_KEYS_THUMBSTER, sizeof(SkeltrKeysThumbster), error);

	gaminggear_device_unlock(GAMINGGEAR_DEVICE(device));

	return keys_thumbster;
}

gboolean skeltr_keys_thumbster_equal(SkeltrKeysThumbster const *left, SkeltrKeysThumbster const *right) {
	gboolean equal;
	equal = ROCCAT_MEMCMP_PARTIALLY(left, right, SkeltrKeysThumbster, keys[0], checksum);
	return equal ? FALSE : TRUE;
}

void skeltr_keys_thumbster_copy(SkeltrKeysThumbster *destination, SkeltrKeysThumbster const *source) {
	memcpy(destination, source, sizeof(SkeltrKeysThumbster));
}

guint skeltr_keys_thumbster_index_to_macro_index(guint thumbster_index) {
	return thumbster_index + 10;
}

guint skeltr_macro_index_to_keys_thumbster_index(guint macro_index) {
	return macro_index - 10;
}

gboolean skeltr_macro_index_is_keys_thumbster(guint macro_index) {
	return macro_index >= 10 && macro_index < 16;
}
