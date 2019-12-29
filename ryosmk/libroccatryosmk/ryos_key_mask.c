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

#include "ryos_key_mask.h"
#include "roccat_helper.h"
#include "ryos_device.h"
#include <string.h>

static guchar const default_key_mask[sizeof(RyosKeyMask)] = { 0x0c, 0x06, 0x00, 0x3f, 0x51, 0x00 };

RyosKeyMask const *ryos_key_mask_default(void) {
	return (RyosKeyMask const *)default_key_mask;
}

static guint16 ryos_key_mask_calc_checksum(RyosKeyMask const *key_mask) {
	return ROCCAT_BYTESUM_PARTIALLY(key_mask, RyosKeyMask, report_id, checksum);
}

static void ryos_key_mask_set_checksum(RyosKeyMask *key_mask, guint16 new_value) {
	key_mask->checksum = GUINT16_TO_LE(new_value);
}

static void ryos_key_mask_finalize(RyosKeyMask *key_mask, guint profile_index) {
	key_mask->report_id = RYOS_REPORT_ID_KEY_MASK;
	key_mask->size = sizeof(RyosKeyMask);
	key_mask->profile_index = profile_index;
	ryos_key_mask_set_checksum(key_mask, ryos_key_mask_calc_checksum(key_mask));
}

gboolean ryos_key_mask_write(RoccatDevice *ryos, guint profile_index, RyosKeyMask *key_mask, GError **error) {
	g_assert(profile_index < RYOS_PROFILE_NUM);
	ryos_key_mask_finalize(key_mask, profile_index);
	return ryos_device_write(ryos, (gchar const *)key_mask, sizeof(RyosKeyMask), error);
}

RyosKeyMask *ryos_key_mask_read(RoccatDevice *ryos, guint profile_index, GError **error) {
	RyosKeyMask *key_mask;

	g_assert(profile_index < RYOS_PROFILE_NUM);

	gaminggear_device_lock(GAMINGGEAR_DEVICE(ryos));

	if (!ryos_select(ryos, profile_index, RYOS_CONTROL_REQUEST_KEY_MASK, error)) {
		gaminggear_device_unlock(GAMINGGEAR_DEVICE(ryos));
		return NULL;
	}

	key_mask = (RyosKeyMask *)ryos_device_read(ryos, RYOS_REPORT_ID_KEY_MASK, sizeof(RyosKeyMask), error);

	gaminggear_device_unlock(GAMINGGEAR_DEVICE(ryos));

	return key_mask;
}

gboolean ryos_key_mask_equal(RyosKeyMask const *left, RyosKeyMask const *right) {
	gboolean equal;
	equal = ROCCAT_MEMCMP_PARTIALLY(left, right, RyosKeyMask, mask, checksum);
	return equal ? FALSE : TRUE;
}

void ryos_key_mask_copy(RyosKeyMask *destination, RyosKeyMask const *source) {
	memcpy(destination, source, sizeof(RyosKeyMask));
}
