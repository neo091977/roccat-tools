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

#include "skeltr_keys_function.h"
#include "skeltr.h"
#include "roccat_helper.h"
#include <string.h>

static guint8 const default_keys_function[sizeof(SkeltrKeysFunction)] = {
	0x08, 0x5F, 0x00, 0x3A, 0x00, 0x00, 0x3B, 0x00, 0x00, 0x3C, 0x00, 0x00, 0x3D, 0x00, 0x00, 0x3E,
	0x00, 0x00, 0x3F, 0x00, 0x00, 0x40, 0x00, 0x00, 0x41, 0x00, 0x00, 0x42, 0x00, 0x00, 0x43, 0x00,
	0x00, 0x44, 0x00, 0x00, 0x45, 0x00, 0x00, 0x46, 0x00, 0x00, 0x47, 0x00, 0x00, 0x48, 0x00, 0x00,
	0xD2, 0x00, 0x00, 0xD1, 0x00, 0x00, 0xD0, 0x00, 0x00, 0xC1, 0x00, 0x00, 0xCC, 0x00, 0x00, 0xCF,
	0x00, 0x00, 0xCE, 0x00, 0x00, 0xCD, 0x00, 0x00, 0xC2, 0x00, 0x00, 0xC3, 0x00, 0x00, 0xA3, 0x00,
	0x00, 0xA7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x82, 0x00, 0x00, 0xF1, 0x0D
};

SkeltrKeysFunction const *skeltr_keys_function_default(void) {
	return (SkeltrKeysFunction const *)default_keys_function;
}

static guint16 skeltr_keys_function_calc_checksum(SkeltrKeysFunction const *keys_function) {
	return ROCCAT_BYTESUM_PARTIALLY(keys_function, SkeltrKeysFunction, report_id, checksum);
}

static void skeltr_keys_function_set_checksum(SkeltrKeysFunction *keys_function, guint16 new_value) {
	keys_function->checksum = GUINT16_TO_LE(new_value);
}

static void skeltr_keys_function_finalize(SkeltrKeysFunction *keys_function, guint profile_index) {
	keys_function->report_id = SKELTR_REPORT_ID_KEYS_FUNCTION;
	keys_function->size = sizeof(SkeltrKeysFunction);
	keys_function->profile_index = profile_index;
	skeltr_keys_function_set_checksum(keys_function, skeltr_keys_function_calc_checksum(keys_function));
}

gboolean skeltr_keys_function_write(RoccatDevice *device, guint profile_index, SkeltrKeysFunction *keys_function, GError **error) {
	g_assert(profile_index < SKELTR_PROFILE_NUM);
	skeltr_keys_function_finalize(keys_function, profile_index);
	return skeltr_device_write(device, (gchar const *)keys_function, sizeof(SkeltrKeysFunction), error);
}

SkeltrKeysFunction *skeltr_keys_function_read(RoccatDevice *device, guint profile_index, GError **error) {
	SkeltrKeysFunction *keys_function;

	g_assert(profile_index < SKELTR_PROFILE_NUM);

	gaminggear_device_lock(GAMINGGEAR_DEVICE(device));

	if (!skeltr_select(device, profile_index, SKELTR_CONTROL_REQUEST_KEYS_FUNCTION, error)) {
		gaminggear_device_unlock(GAMINGGEAR_DEVICE(device));
		return NULL;
	}

	keys_function = (SkeltrKeysFunction *)skeltr_device_read(device, SKELTR_REPORT_ID_KEYS_FUNCTION, sizeof(SkeltrKeysFunction), error);

	gaminggear_device_unlock(GAMINGGEAR_DEVICE(device));

	return keys_function;
}

gboolean skeltr_keys_function_equal(SkeltrKeysFunction const *left, SkeltrKeysFunction const *right) {
	gboolean equal;
	equal = ROCCAT_MEMCMP_PARTIALLY(left, right, SkeltrKeysFunction, keys[0], checksum);
	return equal ? FALSE : TRUE;
}

void skeltr_keys_function_copy(SkeltrKeysFunction *destination, SkeltrKeysFunction const *source) {
	memcpy(destination, source, sizeof(SkeltrKeysFunction));
}
