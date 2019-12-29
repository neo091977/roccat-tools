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

#include "ryosmkfx_light.h"
#include "ryosmkfx_device.h"
#include "ryosmkfx.h"
#include "roccat_helper.h"

static guint8 const default_light[sizeof(RyosmkfxLight)] = {
	0x0D, 0x20, 0x00, 0x04, 0x01, 0x0F, 0x00, 0x01, 0x00, 0x02, 0x00, 0x00, 0x01, 0x1E, 0x1E, 0x00,
	0x1E, 0x0B, 0x3C, 0x0B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF1, 0x00
};

RyosmkfxLight const *ryosmkfx_light_default(void) {
	return (RyosmkfxLight const *)default_light;
}

static guint16 ryosmkfx_light_calc_checksum(RyosmkfxLight const *light) {
	return ROCCAT_BYTESUM_PARTIALLY(light, RyosmkfxLight, report_id, checksum);
}

static void ryosmkfx_light_set_checksum(RyosmkfxLight *light, guint16 new_value) {
	light->checksum = GUINT16_TO_LE(new_value);
}

static void ryosmkfx_light_finalize(RyosmkfxLight *light, guint profile_index) {
	light->report_id = RYOS_REPORT_ID_LIGHT;
	light->size = sizeof(RyosmkfxLight);
	light->profile_index = profile_index;
	ryosmkfx_light_set_checksum(light, ryosmkfx_light_calc_checksum(light));
}

gboolean ryosmkfx_light_write(RoccatDevice *device, guint profile_index, RyosmkfxLight *light, GError **error) {
	ryosmkfx_light_finalize(light, profile_index);
	return ryos_device_write(device, (gchar const *)light, sizeof(RyosmkfxLight), error);
}

RyosmkfxLight *ryosmkfx_light_read(RoccatDevice *device, guint profile_index, GError **error) {
	RyosmkfxLight *light;

	g_assert(profile_index < RYOS_PROFILE_NUM);

	gaminggear_device_lock(GAMINGGEAR_DEVICE(device));

	if (!ryos_select(device, profile_index, RYOSMKFX_CONTROL_REQUEST_LIGHT, error)) {
		gaminggear_device_unlock(GAMINGGEAR_DEVICE(device));
		return NULL;
	}

	light = (RyosmkfxLight *)ryos_device_read(device, RYOS_REPORT_ID_LIGHT, sizeof(RyosmkfxLight), error);

	gaminggear_device_unlock(GAMINGGEAR_DEVICE(device));

	return light;
}

gboolean ryosmkfx_light_equal(RyosmkfxLight const *left, RyosmkfxLight const *right) {
	gboolean equal;
	equal = ROCCAT_MEMCMP_PARTIALLY(left, right, RyosmkfxLight, brightness, checksum);
	return equal ? FALSE : TRUE;
}

void ryosmkfx_light_copy(RyosmkfxLight *destination, RyosmkfxLight const *source) {
	memcpy(destination, source, sizeof(RyosmkfxLight));
}

void ryosmkfx_light_set_color(RyosmkfxLight *light, guint8 red, guint8 green, guint8 blue) {
	light->red = GUINT16_TO_LE(ryosmkfx_color_to_hardware(red));
	light->green = GUINT16_TO_LE(ryosmkfx_color_to_hardware(green));
	light->blue = GUINT16_TO_LE(ryosmkfx_color_to_hardware(blue));
}

guint8 ryosmkfx_light_get_red(RyosmkfxLight const *light) {
	return ryosmkfx_hardware_to_color(GUINT_FROM_LE(light->red));
}

guint8 ryosmkfx_light_get_green(RyosmkfxLight const *light) {
	return ryosmkfx_hardware_to_color(GUINT_FROM_LE(light->green));
}

guint8 ryosmkfx_light_get_blue(RyosmkfxLight const *light) {
	return ryosmkfx_hardware_to_color(GUINT_FROM_LE(light->blue));
}
