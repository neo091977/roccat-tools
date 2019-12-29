/*
 * This file is part of roccat-tools.
 *
 * roccat-tools is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * roccat-tools is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with roccat-tools. If not, see <http://www.gnu.org/licenses/>.
 */

#include "skeltr.h"
#include "skeltr_light.h"
#include "roccat_helper.h"

static guint8 const default_light[sizeof(SkeltrLight)] = {
	0x10, 0x24, 0x00, 0x00, 0x04, 0x01, 0x04, 0x01, 0x01, 0x03, 0x00, 0x14, 0x05, 0x9C, 0xFD, 0x14,
	0x05, 0x9C, 0xFD, 0x14, 0x05, 0x9C, 0xFD, 0x14, 0x05, 0x9C, 0xFD, 0x14, 0x05, 0x9C, 0xFD, 0x04,
	0x01, 0x0F, 0x19, 0x07
};

SkeltrLight const *skeltr_light_default(void) {
	return (SkeltrLight const *)default_light;
}

static guint16 skeltr_light_calc_checksum(SkeltrLight const *light) {
	return ROCCAT_BYTESUM_PARTIALLY(light, SkeltrLight, report_id, checksum);
}

static void skeltr_light_set_checksum(SkeltrLight *light, guint16 new_value) {
	light->checksum = GUINT16_TO_LE(new_value);
}

static void skeltr_light_finalize(SkeltrLight *light, guint profile_index) {
	light->report_id = SKELTR_REPORT_ID_LIGHT;
	light->size = sizeof(SkeltrLight);
	light->profile_index = profile_index;
	skeltr_light_set_checksum(light, skeltr_light_calc_checksum(light));
}

gboolean skeltr_light_write(RoccatDevice *device, guint profile_index, SkeltrLight *light, GError **error) {
	g_assert(profile_index < SKELTR_PROFILE_NUM);
	skeltr_light_finalize(light, profile_index);
	return skeltr_device_write(device, (gchar const *)light, sizeof(SkeltrLight), error);
}

SkeltrLight *skeltr_light_read(RoccatDevice *device, guint profile_index, GError **error) {
	SkeltrLight *light;

	g_assert(profile_index < SKELTR_PROFILE_NUM);

	gaminggear_device_lock(GAMINGGEAR_DEVICE(device));

	if (!skeltr_select(device, profile_index, SKELTR_CONTROL_REQUEST_LIGHT, error)) {
		gaminggear_device_unlock(GAMINGGEAR_DEVICE(device));
		return NULL;
	}

	light = (SkeltrLight *)skeltr_device_read(device, SKELTR_REPORT_ID_LIGHT, sizeof(SkeltrLight), error);

	gaminggear_device_unlock(GAMINGGEAR_DEVICE(device));

	return light;
}

gboolean skeltr_light_equal(SkeltrLight const *left, SkeltrLight const *right) {
	gboolean equal;
	equal = ROCCAT_MEMCMP_PARTIALLY(left, right, SkeltrLight, mode, checksum);
	return equal ? FALSE : TRUE;
}

void skeltr_light_copy(SkeltrLight *destination, SkeltrLight const *source) {
	memcpy(destination, source, sizeof(SkeltrLight));
}
