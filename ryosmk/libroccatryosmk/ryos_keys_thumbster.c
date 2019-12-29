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

#include "ryos_keys_thumbster.h"
#include "roccat_helper.h"
#include "ryos_device.h"
#include <string.h>

static guint8 const default_keys_thumbster[sizeof(RyosKeysThumbster)] = {
	0x09, 0x17, 0x00, 0xb1, 0x00, 0x00, 0xdd, 0x00, 0x00, 0xb0, 0x00, 0x00, 0x39, 0x00, 0x00, 0xcb,
	0x00, 0x00, 0xc0, 0x00, 0x00, 0x22, 0x04,
};

RyosKeysThumbster const *ryos_keys_thumbster_default(void) {
	return (RyosKeysThumbster const *)default_keys_thumbster;
}

static guint16 ryos_keys_thumbster_calc_checksum(RyosKeysThumbster const *keys_thumbster) {
	return ROCCAT_BYTESUM_PARTIALLY(keys_thumbster, RyosKeysThumbster, report_id, checksum);
}

static void ryos_keys_thumbster_set_checksum(RyosKeysThumbster *keys_thumbster, guint16 new_value) {
	keys_thumbster->checksum = GUINT16_TO_LE(new_value);
}

static void ryos_keys_thumbster_finalize(RyosKeysThumbster *keys_thumbster, guint profile_index) {
	keys_thumbster->report_id = RYOS_REPORT_ID_KEYS_THUMBSTER;
	keys_thumbster->size = sizeof(RyosKeysThumbster);
	keys_thumbster->profile_index = profile_index;
	ryos_keys_thumbster_set_checksum(keys_thumbster, ryos_keys_thumbster_calc_checksum(keys_thumbster));
}

gboolean ryos_keys_thumbster_write(RoccatDevice *ryos, guint profile_index, RyosKeysThumbster *keys_thumbster, GError **error) {
	g_assert(profile_index < RYOS_PROFILE_NUM);
	ryos_keys_thumbster_finalize(keys_thumbster, profile_index);
	return ryos_device_write(ryos, (gchar const *)keys_thumbster, sizeof(RyosKeysThumbster), error);
}

RyosKeysThumbster *ryos_keys_thumbster_read(RoccatDevice *ryos, guint profile_index, GError **error) {
	RyosKeysThumbster *keys_thumbster;

	g_assert(profile_index < RYOS_PROFILE_NUM);

	gaminggear_device_lock(GAMINGGEAR_DEVICE(ryos));

	if (!ryos_select(ryos, profile_index, RYOS_CONTROL_REQUEST_KEYS_THUMBSTER, error)) {
		gaminggear_device_unlock(GAMINGGEAR_DEVICE(ryos));
		return NULL;
	}

	keys_thumbster = (RyosKeysThumbster *)ryos_device_read(ryos, RYOS_REPORT_ID_KEYS_THUMBSTER, sizeof(RyosKeysThumbster), error);

	gaminggear_device_unlock(GAMINGGEAR_DEVICE(ryos));

	return keys_thumbster;
}

gboolean ryos_keys_thumbster_equal(RyosKeysThumbster const *left, RyosKeysThumbster const *right) {
	gboolean equal;
	equal = ROCCAT_MEMCMP_PARTIALLY(left, right, RyosKeysThumbster, keys[0], checksum);
	return equal ? FALSE : TRUE;
}

void ryos_keys_thumbster_copy(RyosKeysThumbster *destination, RyosKeysThumbster const *source) {
	memcpy(destination, source, sizeof(RyosKeysThumbster));
}

guint ryos_keys_thumbster_index_to_macro_index(guint thumbster_index) {
	return thumbster_index + 0x0a;
}

guint ryos_macro_index_to_keys_thumbster_index(guint macro_index) {
	return macro_index - 0x0a;
}

gboolean ryos_macro_index_is_keys_thumbster(guint macro_index) {
	return macro_index >= 0x0a && macro_index <= 0x0f;
}
