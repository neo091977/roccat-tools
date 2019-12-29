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

#include "skeltr_keys_primary.h"
#include "skeltr.h"
#include "roccat_helper.h"
#include "i18n-lib.h"
#include <string.h>

static guint8 const default_keys_primary[sizeof(SkeltrKeysPrimary)] = {
	0x06, 0x95, 0x00, 0x90, 0x62, 0xF1, 0x26, 0x00, 0x00, 0xA8, 0x91, 0x00, 0x5D, 0xE5, 0xA2, 0xA1,
	0xAE, 0x00, 0x24, 0x47, 0x00, 0x00, 0x59, 0xAC, 0xE6, 0x42, 0x1F, 0x57, 0x5C, 0x58, 0x00, 0x55,
	0x23, 0x4A, 0x4D, 0x61, 0xE3, 0x32, 0x63, 0xAD, 0x56, 0x4B, 0x22, 0x12, 0x40, 0x07, 0x34, 0x37,
	0x65, 0x41, 0x00, 0x60, 0x44, 0xE1, 0x00, 0x54, 0x21, 0x49, 0x00, 0x18, 0x1C, 0x04, 0x0B, 0x10,
	0x11, 0x50, 0xA0, 0x31, 0x2A, 0x89, 0x5A, 0x28, 0x45, 0x00, 0x20, 0x1A, 0x00, 0x0E, 0x64, 0x1B,
	0x8A, 0x3A, 0x46, 0x13, 0x2F, 0x33, 0xAB, 0x5B, 0x38, 0x2D, 0x27, 0xA9, 0x5E, 0xA5, 0xA4, 0xA6,
	0x00, 0x25, 0x00, 0x0C, 0x30, 0x16, 0x3F, 0x36, 0x87, 0x2E, 0x52, 0x15, 0x17, 0x09, 0x0A, 0x19,
	0x05, 0x4E, 0x4F, 0x08, 0x3C, 0x0F, 0x3D, 0x06, 0x88, 0x3B, 0x43, 0x14, 0x2B, 0x0D, 0x29, 0x1D,
	0x8B, 0x35, 0x3E, 0x5F, 0x85, 0xE2, 0x2C, 0x53, 0x51, 0x4C, 0x00, 0x48, 0x00, 0xAA, 0x00, 0xE0,
	0x00, 0xE4, 0x1E, 0x73, 0x28
};

SkeltrKeysPrimary const *skeltr_keys_primary_default(void) {
	return (SkeltrKeysPrimary const *)default_keys_primary;
}

static guint16 skeltr_keys_primary_calc_checksum(SkeltrKeysPrimary const *keys_primary) {
	return ROCCAT_BYTESUM_PARTIALLY(keys_primary, SkeltrKeysPrimary, report_id, checksum);
}

static void skeltr_keys_primary_set_checksum(SkeltrKeysPrimary *keys_primary, guint16 new_value) {
	keys_primary->checksum = GUINT16_TO_LE(new_value);
}

static void skeltr_keys_primary_finalize(SkeltrKeysPrimary *keys_primary, guint profile_index) {
	keys_primary->report_id = SKELTR_REPORT_ID_KEYS_PRIMARY;
	keys_primary->size = sizeof(SkeltrKeysPrimary);
	keys_primary->profile_index = profile_index;
	skeltr_keys_primary_set_checksum(keys_primary, skeltr_keys_primary_calc_checksum(keys_primary));
}

gboolean skeltr_keys_primary_write(RoccatDevice *device, guint profile_index, SkeltrKeysPrimary *keys_primary, GError **error) {
	g_assert(profile_index < SKELTR_PROFILE_NUM);
	skeltr_keys_primary_finalize(keys_primary, profile_index);
	return skeltr_device_write(device, (gchar const *)keys_primary, sizeof(SkeltrKeysPrimary), error);
}

SkeltrKeysPrimary *skeltr_keys_primary_read(RoccatDevice *device, guint profile_index, GError **error) {
	SkeltrKeysPrimary *keys_primary;

	g_assert(profile_index < SKELTR_PROFILE_NUM);

	gaminggear_device_lock(GAMINGGEAR_DEVICE(device));

	if (!skeltr_select(device, profile_index, SKELTR_CONTROL_REQUEST_KEYS_PRIMARY, error)) {
		gaminggear_device_unlock(GAMINGGEAR_DEVICE(device));
		return NULL;
	}

	keys_primary = (SkeltrKeysPrimary *)skeltr_device_read(device, SKELTR_REPORT_ID_KEYS_PRIMARY, sizeof(SkeltrKeysPrimary), error);

	gaminggear_device_unlock(GAMINGGEAR_DEVICE(device));

	return keys_primary;
}

gboolean skeltr_keys_primary_equal(SkeltrKeysPrimary const *left, SkeltrKeysPrimary const *right) {
	gboolean equal;
	equal = ROCCAT_MEMCMP_PARTIALLY(left, right, SkeltrKeysPrimary, keys[0], checksum);
	return equal ? FALSE : TRUE;
}

void skeltr_keys_primary_copy(SkeltrKeysPrimary *destination, SkeltrKeysPrimary const *source) {
	memcpy(destination, source, sizeof(SkeltrKeysPrimary));
}

static gint get_index(SkeltrKeysPrimary const *data, guint8 hid) {
	guint i;
	for (i = 0; i < SKELTR_KEYS_PRIMARY_NUM; ++i)
		if (data->keys[i] == hid)
			return i;
	return -1;
}

guint8 skeltr_keys_primary_correct_remapped(SkeltrKeysPrimary const *actual_keys, guint8 hid) {
	gint index;

	index = get_index(actual_keys, hid);
	if (index == -1) {
		g_warning(_("skeltr_keys_primary_correct_remapped: can't find 0x%02x"), hid);
		return hid;
	}
	return skeltr_keys_primary_default()->keys[index];
}
