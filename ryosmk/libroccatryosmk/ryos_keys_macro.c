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

#include "ryos_keys_macro.h"
#include "roccat_helper.h"
#include "ryos_device.h"
#include <string.h>

static guint8 const default_keys_macro[sizeof(RyosKeysMacro)] = {
	0x08, 0x23, 0x00, 0xD3, 0x00, 0x00, 0xD4, 0x00, 0x00, 0xD5, 0x00, 0x00, 0xD6, 0x00, 0x00, 0xD7,
	0x00, 0x00, 0xD8, 0x00, 0x00, 0xD9, 0x00, 0x00, 0xDA, 0x00, 0x00, 0xDB, 0x00, 0x00, 0xDC, 0x00,
	0x00, 0x96, 0x08,
};

RyosKeysMacro const *ryos_keys_macro_default(void) {
	return (RyosKeysMacro const *)default_keys_macro;
}

static guint16 ryos_keys_macro_calc_checksum(RyosKeysMacro const *keys_macro) {
	return ROCCAT_BYTESUM_PARTIALLY(keys_macro, RyosKeysMacro, report_id, checksum);
}

static void ryos_keys_macro_set_checksum(RyosKeysMacro *keys_macro, guint16 new_value) {
	keys_macro->checksum = GUINT16_TO_LE(new_value);
}

static void ryos_keys_macro_finalize(RyosKeysMacro *keys_macro, guint profile_index) {
	keys_macro->report_id = RYOS_REPORT_ID_KEYS_MACRO;
	keys_macro->size = sizeof(RyosKeysMacro);
	keys_macro->profile_index = profile_index;
	ryos_keys_macro_set_checksum(keys_macro, ryos_keys_macro_calc_checksum(keys_macro));
}

gboolean ryos_keys_macro_write(RoccatDevice *ryos, guint profile_index, RyosKeysMacro *keys_macro, GError **error) {
	g_assert(profile_index < RYOS_PROFILE_NUM);
	ryos_keys_macro_finalize(keys_macro, profile_index);
	return ryos_device_write(ryos, (gchar const *)keys_macro, sizeof(RyosKeysMacro), error);
}

RyosKeysMacro *ryos_keys_macro_read(RoccatDevice *ryos, guint profile_index, GError **error) {
	RyosKeysMacro *keys_macro;

	g_assert(profile_index < RYOS_PROFILE_NUM);

	gaminggear_device_lock(GAMINGGEAR_DEVICE(ryos));

	if (!ryos_select(ryos, profile_index, RYOS_CONTROL_REQUEST_KEYS_MACRO, error)) {
		gaminggear_device_unlock(GAMINGGEAR_DEVICE(ryos));
		return NULL;
	}

	keys_macro = (RyosKeysMacro *)ryos_device_read(ryos, RYOS_REPORT_ID_KEYS_MACRO, sizeof(RyosKeysMacro), error);

	gaminggear_device_unlock(GAMINGGEAR_DEVICE(ryos));

	return keys_macro;
}

gboolean ryos_keys_macro_equal(RyosKeysMacro const *left, RyosKeysMacro const *right) {
	gboolean equal;
	equal = ROCCAT_MEMCMP_PARTIALLY(left, right, RyosKeysMacro, keys[0], checksum);
	return equal ? FALSE : TRUE;
}

void ryos_keys_macro_copy(RyosKeysMacro *destination, RyosKeysMacro const *source) {
	memcpy(destination, source, sizeof(RyosKeysMacro));
}

guint ryos_keys_macro_index_to_macro_index(guint macro_index) {
	return macro_index;
}

guint ryos_macro_index_to_keys_macro_index(guint macro_index) {
	return macro_index;
}

gboolean ryos_macro_index_is_keys_macro(guint macro_index) {
	return macro_index <= 0x09;
}
