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

#include "skeltr_keys_macro.h"
#include "skeltr.h"
#include "roccat_helper.h"
#include <string.h>

static guint8 const default_keys_macro[sizeof(SkeltrKeysMacro)] = {
	0x0D, 0x23, 0x00, 0xD3, 0x00, 0x00, 0xD4, 0x00, 0x00, 0xD5, 0x00, 0x00, 0xD6, 0x00, 0x00, 0xD7,
	0x00, 0x00, 0xD8, 0x00, 0x00, 0xD9, 0x00, 0x00, 0xDA, 0x00, 0x00, 0xDB, 0x00, 0x00, 0xDC, 0x00,
	0x00, 0x9B, 0x08
};

SkeltrKeysMacro const *skeltr_keys_macro_default(void) {
	return (SkeltrKeysMacro const *)default_keys_macro;
}

static guint16 skeltr_keys_macro_calc_checksum(SkeltrKeysMacro const *keys_macro) {
	return ROCCAT_BYTESUM_PARTIALLY(keys_macro, SkeltrKeysMacro, report_id, checksum);
}

static void skeltr_keys_macro_set_checksum(SkeltrKeysMacro *keys_macro, guint16 new_value) {
	keys_macro->checksum = GUINT16_TO_LE(new_value);
}

static void skeltr_keys_macro_finalize(SkeltrKeysMacro *keys_macro, guint profile_index) {
	keys_macro->report_id = SKELTR_REPORT_ID_KEYS_MACRO;
	keys_macro->size = sizeof(SkeltrKeysMacro);
	keys_macro->profile_index = profile_index;
	skeltr_keys_macro_set_checksum(keys_macro, skeltr_keys_macro_calc_checksum(keys_macro));
}

gboolean skeltr_keys_macro_write(RoccatDevice *device, guint profile_index, SkeltrKeysMacro *keys_macro, GError **error) {
	g_assert(profile_index < SKELTR_PROFILE_NUM);
	skeltr_keys_macro_finalize(keys_macro, profile_index);
	return skeltr_device_write(device, (gchar const *)keys_macro, sizeof(SkeltrKeysMacro), error);
}

SkeltrKeysMacro *skeltr_keys_macro_read(RoccatDevice *device, guint profile_index, GError **error) {
	SkeltrKeysMacro *keys_macro;

	g_assert(profile_index < SKELTR_PROFILE_NUM);

	gaminggear_device_lock(GAMINGGEAR_DEVICE(device));

	if (!skeltr_select(device, profile_index, SKELTR_CONTROL_REQUEST_KEYS_MACRO, error)) {
		gaminggear_device_unlock(GAMINGGEAR_DEVICE(device));
		return NULL;
	}

	keys_macro = (SkeltrKeysMacro *)skeltr_device_read(device, SKELTR_REPORT_ID_KEYS_MACRO, sizeof(SkeltrKeysMacro), error);

	gaminggear_device_unlock(GAMINGGEAR_DEVICE(device));

	return keys_macro;
}

gboolean skeltr_keys_macro_equal(SkeltrKeysMacro const *left, SkeltrKeysMacro const *right) {
	gboolean equal;
	equal = ROCCAT_MEMCMP_PARTIALLY(left, right, SkeltrKeysMacro, keys[0], checksum);
	return equal ? FALSE : TRUE;
}

void skeltr_keys_macro_copy(SkeltrKeysMacro *destination, SkeltrKeysMacro const *source) {
	memcpy(destination, source, sizeof(SkeltrKeysMacro));
}

guint skeltr_keys_macro_index_to_macro_index(guint macro_index) {
	return macro_index;
}

guint skeltr_macro_index_to_keys_macro_index(guint macro_index) {
	return macro_index;
}

gboolean skeltr_macro_index_is_keys_macro(guint macro_index) {
	return macro_index < 10;
}
