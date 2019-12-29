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

#include "ryosmkfx_stored_lights.h"
#include "ryosmkfx_device.h"
#include "ryosmkfx_light.h"
#include "ryosmkfx_profile.h"
#include "ryosmkfx.h"
#include "roccat_helper.h"

static guint8 const default_stored_lights[sizeof(RyosmkfxStoredLights)] = {
	0x17, 0xEF, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0x11, 0x21, 0x33, 0x44, 0x54, 0x66, 0x11, 0x22, 0x33, 0x43, 0x54, 0x66, 0x11, 0x22,
	0x32, 0x44, 0x54, 0x66, 0x11, 0x22, 0x33, 0x43, 0x54, 0x66, 0x11, 0x22, 0x32, 0x43, 0x54, 0x66,
	0x11, 0x22, 0x33, 0x43, 0x54, 0x66, 0x11, 0x22, 0x33, 0x43, 0x54, 0x66, 0x11, 0x22, 0x33, 0x43,
	0x54, 0x66, 0x11, 0x22, 0x32, 0x43, 0x55, 0x66, 0x11, 0x21, 0x32, 0x43, 0x55, 0x45, 0x11, 0x21,
	0x33, 0x44, 0x54, 0x66, 0x11, 0x22, 0x33, 0x43, 0x54, 0x66, 0x11, 0x22, 0x32, 0x44, 0x54, 0x66,
	0x11, 0x22, 0x33, 0x43, 0x54, 0x66, 0x11, 0x22, 0x32, 0x43, 0x54, 0x66, 0x11, 0x22, 0x33, 0x43,
	0x54, 0x66, 0x11, 0x22, 0x33, 0x43, 0x54, 0x66, 0x11, 0x22, 0x33, 0x43, 0x54, 0x66, 0x11, 0x22,
	0x32, 0x43, 0x55, 0x66, 0x11, 0x21, 0x32, 0x43, 0x55, 0x45, 0x11, 0x21, 0x33, 0x44, 0x54, 0x66,
	0x11, 0x22, 0x33, 0x43, 0x54, 0x66, 0x11, 0x22, 0x32, 0x44, 0x54, 0x66, 0x11, 0x22, 0x33, 0x43,
	0x54, 0x66, 0x11, 0x22, 0x32, 0x43, 0x54, 0x66, 0x11, 0x22, 0x33, 0x43, 0x54, 0x66, 0x11, 0x22,
	0x33, 0x43, 0x54, 0x66, 0x11, 0x22, 0x33, 0x43, 0x54, 0x66, 0x11, 0x22, 0x32, 0x43, 0x55, 0x66,
	0x11, 0x21, 0x32, 0x43, 0x55, 0x45, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x00, 0x1E, 0x1E, 0x1E,
	0x1E, 0x1E, 0x1E, 0x00, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 0xBB,
	0xBB, 0xBB, 0x00, 0xBB, 0xBB, 0xBB, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x99, 0x40
};

RyosmkfxStoredLights const *ryosmkfx_stored_lights_default(void) {
	return (RyosmkfxStoredLights const *)default_stored_lights;
}

static guint16 ryosmkfx_stored_lights_calc_checksum(RyosmkfxStoredLights const *stored_lights) {
	return ROCCAT_BYTESUM_PARTIALLY(stored_lights, RyosmkfxStoredLights, report_id, checksum);
}

static void ryosmkfx_stored_lights_set_checksum(RyosmkfxStoredLights *stored_lights, guint16 new_value) {
	stored_lights->checksum = GUINT16_TO_LE(new_value);
}

static void ryosmkfx_stored_lights_finalize(RyosmkfxStoredLights *stored_lights, guint profile_index) {
	stored_lights->report_id = RYOS_REPORT_ID_STORED_LIGHTS;
	stored_lights->size = sizeof(RyosmkfxStoredLights);
	stored_lights->profile_index = profile_index;
	ryosmkfx_stored_lights_set_checksum(stored_lights, ryosmkfx_stored_lights_calc_checksum(stored_lights));
}

gboolean ryosmkfx_stored_lights_write(RoccatDevice *device, guint profile_index, RyosmkfxStoredLights *stored_lights, GError **error) {
	ryosmkfx_stored_lights_finalize(stored_lights, profile_index);
	return ryos_device_write(device, (gchar const *)stored_lights, sizeof(RyosmkfxStoredLights), error);
}

RyosmkfxStoredLights *ryosmkfx_stored_lights_read(RoccatDevice *device, guint profile_index, GError **error) {
	RyosmkfxStoredLights *stored_lights;

	g_assert(profile_index < RYOS_PROFILE_NUM);

	gaminggear_device_lock(GAMINGGEAR_DEVICE(device));

	if (!ryos_select(device, profile_index, RYOSMKFX_CONTROL_REQUEST_B3, error)) {
		gaminggear_device_unlock(GAMINGGEAR_DEVICE(device));
		return NULL;
	}

	stored_lights = (RyosmkfxStoredLights *)ryos_device_read(device, RYOS_REPORT_ID_STORED_LIGHTS, sizeof(RyosmkfxStoredLights), error);

	gaminggear_device_unlock(GAMINGGEAR_DEVICE(device));

	return stored_lights;
}

gboolean ryosmkfx_stored_lights_equal(RyosmkfxStoredLights const *left, RyosmkfxStoredLights const *right) {
	gboolean equal;
	equal = ROCCAT_MEMCMP_PARTIALLY(left, right, RyosmkfxStoredLights, light_layer, checksum);
	return equal ? FALSE : TRUE;
}

void ryosmkfx_stored_lights_copy(RyosmkfxStoredLights *destination, RyosmkfxStoredLights const *source) {
	memcpy(destination, source, sizeof(RyosmkfxStoredLights));
}

RyosmkfxLightLayer *ryosmkfx_get_active_light_layer(RoccatDevice *device, GError **error) {
	GError *local_error = NULL;
	guint profile_index;
	RyosmkfxLight *light;
	guint mode;
	RyosmkfxStoredLights *stored_lights;
	RyosmkfxLightLayer *result;

	profile_index = ryosmkfx_profile_get_index(ryos_profile_read(device, &local_error));
	if (local_error) {
		g_propagate_error(error, local_error);
		return NULL;
	}

	light = ryosmkfx_light_read(device, profile_index, error);
	if (light == NULL)
		return NULL;
	mode = light->mode;
	g_free(light);

	if (mode == RYOSMKFX_LIGHT_MODE_LAYER) {
		stored_lights = ryosmkfx_stored_lights_read(device, profile_index, error);
		if (stored_lights == NULL)
			return NULL;
		result = (RyosmkfxLightLayer *)g_malloc(sizeof(RyosmkfxLightLayer));
		*result = stored_lights->light_layer;
		g_free(stored_lights);
	} else {
		result = ryosmkfx_light_layer_plain(ryosmkfx_light_get_red(light), ryosmkfx_light_get_green(light), ryosmkfx_light_get_blue(light));
	}

	return result;
}
