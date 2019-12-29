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

#include "ryos_light.h"
#include "roccat_helper.h"
#include "ryos_device.h"
#include <string.h>

static guint8 const default_light[sizeof(RyosLight)] = { 0x0d, 0x10, 0x00, 0x03, 0x01, 0x0f, 0xf4, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x26, 0x01, };

RyosLight const *ryos_light_default(void) {
	return (RyosLight const *)default_light;
}

static guint16 ryos_light_calc_checksum(RyosLight const *light) {
	return ROCCAT_BYTESUM_PARTIALLY(light, RyosLight, report_id, checksum);
}

static void ryos_light_set_checksum(RyosLight *light, guint16 new_value) {
	light->checksum = GUINT16_TO_LE(new_value);
}

static void ryos_light_finalize(RyosLight *light, guint profile_index) {
	light->report_id = RYOS_REPORT_ID_LIGHT;
	light->size = sizeof(RyosLight);
	light->profile_index = profile_index;
	ryos_light_set_checksum(light, ryos_light_calc_checksum(light));
}

gboolean ryos_light_write(RoccatDevice *ryos, guint profile_index, RyosLight *light, GError **error) {
	g_assert(profile_index < RYOS_PROFILE_NUM);
	ryos_light_finalize(light, profile_index);
	return ryos_device_write(ryos, (gchar const *)light, sizeof(RyosLight), error);
}

RyosLight *ryos_light_read(RoccatDevice *ryos, guint profile_index, GError **error) {
	RyosLight *light;

	g_assert(profile_index < RYOS_PROFILE_NUM);

	gaminggear_device_lock(GAMINGGEAR_DEVICE(ryos));

	if (!ryos_select(ryos, profile_index, RYOS_CONTROL_REQUEST_LIGHT, error)) {
		gaminggear_device_unlock(GAMINGGEAR_DEVICE(ryos));
		return NULL;
	}

	light = (RyosLight *)ryos_device_read(ryos, RYOS_REPORT_ID_LIGHT, sizeof(RyosLight), error);

	gaminggear_device_unlock(GAMINGGEAR_DEVICE(ryos));

	return light;
}

gboolean ryos_light_equal(RyosLight const *left, RyosLight const *right) {
	gboolean equal;
	equal = ROCCAT_MEMCMP_PARTIALLY(left, right, RyosLight, brightness, checksum);
	return equal ? FALSE : TRUE;
}

void ryos_light_copy(RyosLight *destination, RyosLight const *source) {
	memcpy(destination, source, sizeof(RyosLight));
}
