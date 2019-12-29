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

#include "ryos_keys_extra.h"
#include "roccat_helper.h"
#include "ryos_device.h"
#include <string.h>

static guint8 const default_keys_extra[sizeof(RyosKeysExtra)] = { 0x0a, 0x08, 0x00, 0xff, 0xf1, 0x00, 0x02, 0x02, };

RyosKeysExtra const *ryos_keys_extra_default(void) {
	return (RyosKeysExtra const *)default_keys_extra;
}

static guint16 ryos_keys_extra_calc_checksum(RyosKeysExtra const *keys_extra) {
	return ROCCAT_BYTESUM_PARTIALLY(keys_extra, RyosKeysExtra, report_id, checksum);
}

static void ryos_keys_extra_set_checksum(RyosKeysExtra *keys_extra, guint16 new_value) {
	keys_extra->checksum = GUINT16_TO_LE(new_value);
}

static void ryos_keys_extra_finalize(RyosKeysExtra *keys_extra, guint profile_index) {
	keys_extra->report_id = RYOS_REPORT_ID_KEYS_EXTRA;
	keys_extra->size = sizeof(RyosKeysExtra);
	keys_extra->profile_index = profile_index;
	ryos_keys_extra_set_checksum(keys_extra, ryos_keys_extra_calc_checksum(keys_extra));
}

gboolean ryos_keys_extra_write(RoccatDevice *ryos, guint profile_index, RyosKeysExtra *keys_extra, GError **error) {
	g_assert(profile_index < RYOS_PROFILE_NUM);
	ryos_keys_extra_finalize(keys_extra, profile_index);
	return ryos_device_write(ryos, (gchar const *)keys_extra, sizeof(RyosKeysExtra), error);
}

RyosKeysExtra *ryos_keys_extra_read(RoccatDevice *ryos, guint profile_index, GError **error) {
	RyosKeysExtra *keys_extra;

	g_assert(profile_index < RYOS_PROFILE_NUM);

	gaminggear_device_lock(GAMINGGEAR_DEVICE(ryos));

	if (!ryos_select(ryos, profile_index, RYOS_CONTROL_REQUEST_KEYS_EXTRA, error)) {
		gaminggear_device_unlock(GAMINGGEAR_DEVICE(ryos));
		return NULL;
	}

	keys_extra = (RyosKeysExtra *)ryos_device_read(ryos, RYOS_REPORT_ID_KEYS_EXTRA, sizeof(RyosKeysExtra), error);

	gaminggear_device_unlock(GAMINGGEAR_DEVICE(ryos));

	return keys_extra;
}

gboolean ryos_keys_extra_equal(RyosKeysExtra const *left, RyosKeysExtra const *right) {
	gboolean equal;
	equal = ROCCAT_MEMCMP_PARTIALLY(left, right, RyosKeysExtra, capslock, checksum);
	return equal ? FALSE : TRUE;
}

void ryos_keys_extra_copy(RyosKeysExtra *destination, RyosKeysExtra const *source) {
	memcpy(destination, source, sizeof(RyosKeysExtra));
}
