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

#include "ryos_keys_primary.h"
#include "ryos_default_rkp.h"
#include "roccat_helper.h"
#include "ryos_device.h"
#include "i18n-lib.h"
#include <string.h>

static guint8 const default_keys_primary[sizeof(RyosKeysPrimary)] = {
	0x06, 0x7d, 0x00, 0xaa, 0x29, 0x3a, 0x3c, 0x3e, 0x40, 0x42, 0x43, 0x44, 0x46, 0x53, 0x55, 0x35,
	0x1e, 0x3b, 0x3d, 0x3f, 0x41, 0x27, 0x2e, 0x45, 0x47, 0x54, 0x56, 0xab, 0x1f, 0x20, 0x21, 0x23,
	0x25, 0x2d, 0x2f, 0x89, 0x48, 0x5f, 0x61, 0x2b, 0x14, 0x08, 0x22, 0x24, 0x26, 0x12, 0x30, 0x2a,
	0x49, 0x60, 0x57, 0xac, 0x1a, 0x15, 0x17, 0x1c, 0x0c, 0x13, 0x34, 0x31, 0x4a, 0x5c, 0x5e, 0x39,
	0x04, 0x07, 0x0a, 0x18, 0x0e, 0x0f, 0x32, 0x28, 0x4b, 0x5d, 0x85, 0xad, 0x16, 0x09, 0x0b, 0x0d,
	0x10, 0x33, 0x38, 0xe5, 0x4c, 0x59, 0x5b, 0xe1, 0x64, 0x1b, 0x19, 0x11, 0x36, 0x37, 0x87, 0xe4,
	0x4d, 0x5a, 0x58, 0xae, 0x1d, 0x06, 0x05, 0x8b, 0x8a, 0x90, 0x65, 0x50, 0x4e, 0x62, 0x63, 0xe0,
	0xe3, 0xe2, 0x91, 0x2c, 0x88, 0xe6, 0xf1, 0x51, 0x52, 0x4f, 0xe7, 0x58, 0x24,
};

RyosKeysPrimary const *ryos_keys_primary_default(void) {
	return (RyosKeysPrimary const *)default_keys_primary;
}

static guint16 ryos_keys_primary_calc_checksum(RyosKeysPrimary const *keys_primary) {
	return ROCCAT_BYTESUM_PARTIALLY(keys_primary, RyosKeysPrimary, report_id, checksum);
}

static void ryos_keys_primary_set_checksum(RyosKeysPrimary *keys_primary, guint16 new_value) {
	keys_primary->checksum = GUINT16_TO_LE(new_value);
}

static void ryos_keys_primary_finalize(RyosKeysPrimary *keys_primary, guint profile_index) {
	keys_primary->report_id = RYOS_REPORT_ID_KEYS_PRIMARY;
	keys_primary->size = sizeof(RyosKeysPrimary);
	keys_primary->profile_index = profile_index;
	ryos_keys_primary_set_checksum(keys_primary, ryos_keys_primary_calc_checksum(keys_primary));
}

gboolean ryos_keys_primary_write(RoccatDevice *ryos, guint profile_index, RyosKeysPrimary *keys_primary, GError **error) {
	g_assert(profile_index < RYOS_PROFILE_NUM);
	ryos_keys_primary_finalize(keys_primary, profile_index);
	return ryos_device_write(ryos, (gchar const *)keys_primary, sizeof(RyosKeysPrimary), error);
}

RyosKeysPrimary *ryos_keys_primary_read(RoccatDevice *ryos, guint profile_index, GError **error) {
	RyosKeysPrimary *keys_primary;

	g_assert(profile_index < RYOS_PROFILE_NUM);

	gaminggear_device_lock(GAMINGGEAR_DEVICE(ryos));

	if (!ryos_select(ryos, profile_index, RYOS_CONTROL_REQUEST_KEYS_PRIMARY, error)) {
		gaminggear_device_unlock(GAMINGGEAR_DEVICE(ryos));
		return NULL;
	}

	keys_primary = (RyosKeysPrimary *)ryos_device_read(ryos, RYOS_REPORT_ID_KEYS_PRIMARY, sizeof(RyosKeysPrimary), error);

	gaminggear_device_unlock(GAMINGGEAR_DEVICE(ryos));

	return keys_primary;
}

gboolean ryos_keys_primary_equal(RyosKeysPrimary const *left, RyosKeysPrimary const *right) {
	gboolean equal;
	equal = ROCCAT_MEMCMP_PARTIALLY(left, right, RyosKeysPrimary, keys[0], checksum);
	return equal ? FALSE : TRUE;
}

void ryos_keys_primary_copy(RyosKeysPrimary *destination, RyosKeysPrimary const *source) {
	memcpy(destination, source, sizeof(RyosKeysPrimary));
}

static gint get_index(RyosKeysPrimary const *data, guint8 hid) {
	guint i;
	for (i = 0; i < RYOS_KEYS_PRIMARY_NUM; ++i)
		if (data->keys[i] == hid)
			return i;
	return -1;
}

guint8 ryos_keys_primary_correct_remapped(RyosKeysPrimary const *actual_keys, guint8 hid) {
	gint index;

	index = get_index(actual_keys, hid);
	if (index == -1) {
		g_warning(_("ryos_keys_primary_correct_remapped: can't find 0x%02x"), hid);
		return hid;
	}
	return ryos_rkp_default_static()->keys_primary.keys[index];
}
