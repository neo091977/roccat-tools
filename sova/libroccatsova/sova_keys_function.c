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

#include "sova_keys_function.h"
#include "roccat_helper.h"
#include <string.h>

static guint8 const default_keys_function[sizeof(SovaKeysFunction)] = {
	0x07, 0x5F, 0x00, 0x3A, 0x00, 0x00, 0x3B, 0x00, 0x00, 0x3C, 0x00, 0x00, 0x3D, 0x00, 0x00, 0x3E,
	0x00, 0x00, 0x3F, 0x00, 0x00, 0x40, 0x00, 0x00, 0x41, 0x00, 0x00, 0x42, 0x00, 0x00, 0x43, 0x00,
	0x00, 0x44, 0x00, 0x00, 0x45, 0x00, 0x00, 0x46, 0x00, 0x00, 0x49, 0x00, 0x00, 0x4C, 0x00, 0x00,
	0xD2, 0x00, 0x00, 0xD1, 0x00, 0x00, 0xD0, 0x00, 0x00, 0xC1, 0x00, 0x00, 0xCC, 0x00, 0x00, 0xCF,
	0x00, 0x00, 0xCE, 0x00, 0x00, 0xCD, 0x00, 0x00, 0xC2, 0x00, 0x00, 0xC3, 0x00, 0x00, 0xA3, 0x00,
	0x00, 0xA7, 0x00, 0x00, 0x48, 0x00, 0x00, 0x53, 0x00, 0x00, 0x47, 0x00, 0x00, 0x56, 0x0E
};

SovaKeysFunction const *sova_keys_function_default(void) {
	return (SovaKeysFunction const *)default_keys_function;
}

static guint16 sova_keys_function_calc_checksum(SovaKeysFunction const *keys_function) {
	return ROCCAT_BYTESUM_PARTIALLY(keys_function, SovaKeysFunction, report_id, checksum);
}

static void sova_keys_function_set_checksum(SovaKeysFunction *keys_function, guint16 new_value) {
	keys_function->checksum = GUINT16_TO_LE(new_value);
}

static void sova_keys_function_finalize(SovaKeysFunction *keys_function, guint profile_index) {
	keys_function->report_id = SOVA_REPORT_ID_KEYS_FUNCTION;
	keys_function->size = sizeof(SovaKeysFunction);
	keys_function->profile_index = profile_index;
	sova_keys_function_set_checksum(keys_function, sova_keys_function_calc_checksum(keys_function));
}

gboolean sova_keys_function_write(RoccatDevice *device, guint profile_index, SovaKeysFunction *keys_function, GError **error) {
	g_assert(profile_index < SOVA_PROFILE_NUM);
	sova_keys_function_finalize(keys_function, profile_index);
	return sova_device_write(device, (gchar const *)keys_function, sizeof(SovaKeysFunction), error);
}

SovaKeysFunction *sova_keys_function_read(RoccatDevice *device, guint profile_index, GError **error) {
	SovaKeysFunction *keys_function;

	g_assert(profile_index < SOVA_PROFILE_NUM);

	gaminggear_device_lock(GAMINGGEAR_DEVICE(device));

	if (!sova_select(device, profile_index, SOVA_CONTROL_REQUEST_KEYS_FUNCTION, error)) {
		gaminggear_device_unlock(GAMINGGEAR_DEVICE(device));
		return NULL;
	}

	keys_function = (SovaKeysFunction *)sova_device_read(device, SOVA_REPORT_ID_KEYS_FUNCTION, sizeof(SovaKeysFunction), error);

	gaminggear_device_unlock(GAMINGGEAR_DEVICE(device));

	return keys_function;
}

gboolean sova_keys_function_equal(SovaKeysFunction const *left, SovaKeysFunction const *right) {
	gboolean equal;
	equal = ROCCAT_MEMCMP_PARTIALLY(left, right, SovaKeysFunction, keys_normal[0], checksum);
	return equal ? FALSE : TRUE;
}

void sova_keys_function_copy(SovaKeysFunction *destination, SovaKeysFunction const *source) {
	memcpy(destination, source, sizeof(SovaKeysFunction));
}
