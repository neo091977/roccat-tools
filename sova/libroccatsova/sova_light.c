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

#include "sova.h"
#include "sova_light.h"
#include "roccat_helper.h"

static guint8 const default_light[sizeof(SovaLight)] = {
	0x0D, 0xA0, 0x00, 0x00, 0x03, 0x01, 0x02, 0x00, 0x02, 0x01, 0x00, 0x03, 0x01, 0x0F, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC9, 0x00
};

SovaLight const *sova_light_default(void) {
	return (SovaLight const *)default_light;
}

static guint16 sova_light_calc_checksum(SovaLight const *light) {
	return ROCCAT_BYTESUM_PARTIALLY(light, SovaLight, report_id, checksum);
}

static void sova_light_set_checksum(SovaLight *light, guint16 new_value) {
	light->checksum = GUINT16_TO_LE(new_value);
}

static void sova_light_finalize(SovaLight *light, guint profile_index) {
	light->report_id = SOVA_REPORT_ID_LIGHT;
	light->size = sizeof(SovaLight);
	light->profile_index = profile_index;
	sova_light_set_checksum(light, sova_light_calc_checksum(light));
}

gboolean sova_light_write(RoccatDevice *device, guint profile_index, SovaLight *light, GError **error) {
	g_assert(profile_index < SOVA_PROFILE_NUM);
	sova_light_finalize(light, profile_index);
	return sova_device_write(device, (gchar const *)light, sizeof(SovaLight), error);
}

SovaLight *sova_light_read(RoccatDevice *device, guint profile_index, GError **error) {
	SovaLight *light;

	g_assert(profile_index < SOVA_PROFILE_NUM);

	gaminggear_device_lock(GAMINGGEAR_DEVICE(device));

	if (!sova_select(device, profile_index, SOVA_CONTROL_REQUEST_LIGHT, error)) {
		gaminggear_device_unlock(GAMINGGEAR_DEVICE(device));
		return NULL;
	}

	light = (SovaLight *)sova_device_read(device, SOVA_REPORT_ID_LIGHT, sizeof(SovaLight), error);

	gaminggear_device_unlock(GAMINGGEAR_DEVICE(device));

	return light;
}

gboolean sova_light_equal(SovaLight const *left, SovaLight const *right) {
	gboolean equal;
	equal = ROCCAT_MEMCMP_PARTIALLY(left, right, SovaLight, unknown1, checksum);
	return equal ? FALSE : TRUE;
}

void sova_light_copy(SovaLight *destination, SovaLight const *source) {
	memcpy(destination, source, sizeof(SovaLight));
}
