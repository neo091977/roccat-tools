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

#include "sova_keys_primary.h"
#include "roccat_helper.h"
#include "i18n-lib.h"
#include <string.h>

static guint8 const default_keys_primary[sizeof(SovaKeysPrimary)] = {
	/* 0     1     2     3     4     5     6     7     8     9     a     b     c     d     e     f */
	0x06, 0x95, 0x00, 0x14, 0x08, 0x15, 0x17, 0x09, 0x0A, 0x1D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0 */
	0x00, 0x64, 0x1B, 0x41, 0x3B, 0x25, 0x0C, 0x00, 0x0E, 0x36, 0x06, 0x40, 0x3C, 0x24, 0x18, 0x00, /* 1 */
	0x0D, 0x10, 0x19, 0x3F, 0x00, 0x23, 0x1C, 0x00, 0x0B, 0x11, 0x05, 0x00, 0x43, 0x27, 0x3E, 0x00, /* 2 */
	0x00, 0x8B, 0x22, 0x00, 0x44, 0x3D, 0x00, 0x00, 0x00, 0x8A, 0x21, 0x00, 0x00, 0x00, 0x00, 0x00, /* 3 */
	0x00, 0x88, 0x20, 0x45, 0x2E, 0x2D, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x3A, 0x42, 0x26, 0x12, 0x00, /* 4 */
	0x0F, 0x00, 0x1E, 0x49, 0x4C, 0x4A, 0x13, 0x00, 0x33, 0x37, 0x52, 0x00, 0x00, 0x4B, 0x2F, 0x00, /* 5 */
	0x34, 0x38, 0x51, 0x46, 0x00, 0x2A, 0x30, 0x00, 0x28, 0x87, 0x50, 0x00, 0x00, 0x4E, 0x31, 0x00, /* 6 */
	0x32, 0x4D, 0x4F, 0x00, 0x00, 0x89, 0x00, 0x00, 0x00, 0x00, 0x1A, 0x29, 0x35, 0x00, 0x00, 0x00, /* 7 */
	0x00, 0x2B, 0x04, 0xE1, 0xE3, 0xE0, 0xE2, 0x2C, 0x00, 0x90, 0x16, 0xE5, 0xE7, 0xE4, 0xE6, 0x00, /* 8 */
	0x00, 0x91, 0x07, 0x66, 0x18 /* 9 */
};

SovaKeysPrimary const *sova_keys_primary_default(void) {
	return (SovaKeysPrimary const *)default_keys_primary;
}

static guint16 sova_keys_primary_calc_checksum(SovaKeysPrimary const *keys_primary) {
	return ROCCAT_BYTESUM_PARTIALLY(keys_primary, SovaKeysPrimary, report_id, checksum);
}

static void sova_keys_primary_set_checksum(SovaKeysPrimary *keys_primary, guint16 new_value) {
	keys_primary->checksum = GUINT16_TO_LE(new_value);
}

static void sova_keys_primary_finalize(SovaKeysPrimary *keys_primary, guint profile_index) {
	keys_primary->report_id = SOVA_REPORT_ID_KEYS_PRIMARY;
	keys_primary->size = sizeof(SovaKeysPrimary);
	keys_primary->profile_index = profile_index;
	sova_keys_primary_set_checksum(keys_primary, sova_keys_primary_calc_checksum(keys_primary));
}

gboolean sova_keys_primary_write(RoccatDevice *device, guint profile_index, SovaKeysPrimary *keys_primary, GError **error) {
	g_assert(profile_index < SOVA_PROFILE_NUM);
	sova_keys_primary_finalize(keys_primary, profile_index);
	return sova_device_write(device, (gchar const *)keys_primary, sizeof(SovaKeysPrimary), error);
}

SovaKeysPrimary *sova_keys_primary_read(RoccatDevice *device, guint profile_index, GError **error) {
	SovaKeysPrimary *keys_primary;

	g_assert(profile_index < SOVA_PROFILE_NUM);

	gaminggear_device_lock(GAMINGGEAR_DEVICE(device));

	if (!sova_select(device, profile_index, SOVA_CONTROL_REQUEST_KEYS_PRIMARY, error)) {
		gaminggear_device_unlock(GAMINGGEAR_DEVICE(device));
		return NULL;
	}

	keys_primary = (SovaKeysPrimary *)sova_device_read(device, SOVA_REPORT_ID_KEYS_PRIMARY, sizeof(SovaKeysPrimary), error);

	gaminggear_device_unlock(GAMINGGEAR_DEVICE(device));

	return keys_primary;
}

gboolean sova_keys_primary_equal(SovaKeysPrimary const *left, SovaKeysPrimary const *right) {
	gboolean equal;
	equal = ROCCAT_MEMCMP_PARTIALLY(left, right, SovaKeysPrimary, keys[0], checksum);
	return equal ? FALSE : TRUE;
}

void sova_keys_primary_copy(SovaKeysPrimary *destination, SovaKeysPrimary const *source) {
	memcpy(destination, source, sizeof(SovaKeysPrimary));
}

static gint get_index(SovaKeysPrimary const *data, guint8 hid) {
	guint i;
	for (i = 0; i < SOVA_KEYS_PRIMARY_NUM; ++i)
		if (data->keys[i] == hid)
			return i;
	return -1;
}

guint8 sova_keys_primary_correct_remapped(SovaKeysPrimary const *actual_keys, guint8 hid) {
	gint index;

	index = get_index(actual_keys, hid);
	if (index == -1) {
		g_warning(_("sova_keys_primary_correct_remapped: can't find 0x%02x"), hid);
		return hid;
	}
	return sova_keys_primary_default()->keys[index];
}
