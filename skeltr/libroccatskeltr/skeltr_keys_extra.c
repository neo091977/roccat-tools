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

#include "skeltr_keys_extra.h"
#include "skeltr.h"
#include "roccat_helper.h"
#include "skeltr_device.h"
#include <string.h>

static guint8 const default_keys_extra[sizeof(SkeltrKeysExtra)] = {
		0x0a, 0x08, 0x00, 0xff, 0xf1, 0x00, 0x02, 0x02
};

SkeltrKeysExtra const *skeltr_keys_extra_default(void) {
	return (SkeltrKeysExtra const *)default_keys_extra;
}

static guint16 skeltr_keys_extra_calc_checksum(SkeltrKeysExtra const *keys_extra) {
	return ROCCAT_BYTESUM_PARTIALLY(keys_extra, SkeltrKeysExtra, report_id, checksum);
}

static void skeltr_keys_extra_set_checksum(SkeltrKeysExtra *keys_extra, guint16 new_value) {
	keys_extra->checksum = GUINT16_TO_LE(new_value);
}

static void skeltr_keys_extra_finalize(SkeltrKeysExtra *keys_extra, guint profile_index) {
	keys_extra->report_id = SKELTR_REPORT_ID_KEYS_EXTRA;
	keys_extra->size = sizeof(SkeltrKeysExtra);
	keys_extra->profile_index = profile_index;
	skeltr_keys_extra_set_checksum(keys_extra, skeltr_keys_extra_calc_checksum(keys_extra));
}

gboolean skeltr_keys_extra_write(RoccatDevice *device, guint profile_index, SkeltrKeysExtra *keys_extra, GError **error) {
	g_assert(profile_index < SKELTR_PROFILE_NUM);
	skeltr_keys_extra_finalize(keys_extra, profile_index);
	return skeltr_device_write(device, (gchar const *)keys_extra, sizeof(SkeltrKeysExtra), error);
}

SkeltrKeysExtra *skeltr_keys_extra_read(RoccatDevice *device, guint profile_index, GError **error) {
	SkeltrKeysExtra *keys_extra;

	g_assert(profile_index < SKELTR_PROFILE_NUM);

	gaminggear_device_lock(GAMINGGEAR_DEVICE(device));

	if (!skeltr_select(device, profile_index, SKELTR_CONTROL_REQUEST_KEYS_EXTRA, error)) {
		gaminggear_device_unlock(GAMINGGEAR_DEVICE(device));
		return NULL;
	}

	keys_extra = (SkeltrKeysExtra *)skeltr_device_read(device, SKELTR_REPORT_ID_KEYS_EXTRA, sizeof(SkeltrKeysExtra), error);

	gaminggear_device_unlock(GAMINGGEAR_DEVICE(device));

	return keys_extra;
}

gboolean skeltr_keys_extra_equal(SkeltrKeysExtra const *left, SkeltrKeysExtra const *right) {
	gboolean equal;
	equal = ROCCAT_MEMCMP_PARTIALLY(left, right, SkeltrKeysExtra, capslock, checksum);
	return equal ? FALSE : TRUE;
}

void skeltr_keys_extra_copy(SkeltrKeysExtra *destination, SkeltrKeysExtra const *source) {
	memcpy(destination, source, sizeof(SkeltrKeysExtra));
}
