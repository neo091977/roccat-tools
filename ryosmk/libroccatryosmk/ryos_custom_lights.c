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

#include "ryos_custom_lights.h"
#include "ryos_device.h"
#include "roccat_helper.h"

static guint16 ryos_custom_lights_calc_checksum(RyosCustomLights const *custom_lights) {
	return ROCCAT_BYTESUM_PARTIALLY(custom_lights, RyosCustomLights, report_id, checksum);
}

static void ryos_custom_lights_set_checksum(RyosCustomLights *custom_lights, guint16 new_value) {
	custom_lights->checksum = GUINT16_TO_LE(new_value);
}

static void ryos_custom_lights_finalize(RyosCustomLights *custom_lights) {
	custom_lights->report_id = RYOS_REPORT_ID_CUSTOM_LIGHTS;
	custom_lights->size = sizeof(RyosCustomLights);
	ryos_custom_lights_set_checksum(custom_lights, ryos_custom_lights_calc_checksum(custom_lights));
}

gboolean ryos_custom_lights_write(RoccatDevice *ryos, RyosCustomLights *custom_lights, GError **error) {
	ryos_custom_lights_finalize(custom_lights);
	return ryos_device_write_with_custom_wait(ryos, (gchar const *)custom_lights, sizeof(RyosCustomLights), 10, error);
}

gboolean ryos_custom_lights_empty(RyosCustomLights const *custom_lights) {
	guint i;
	for (i = 0; i < RYOS_CUSTOM_LIGHTS_DATA_LENGTH; ++i)
		if (custom_lights->data[i])
			return FALSE;
	return TRUE;
}

void ryos_light_layer_to_custom_lights(RyosLightLayer const *light_layer, RyosCustomLights *custom_lights, gboolean blink_state) {
	guint i;
	guint key;
	gboolean on;
	gboolean blink;
	gboolean state;

	for (i = 0; i < RYOS_LIGHT_LAYER_KEY_NUM; ++i) {
		key = light_layer->keys[i];
		on = roccat_get_bit8(key, RYOS_LIGHT_LAYER_KEY_BIT_ON);
		blink = roccat_get_bit8(key, RYOS_LIGHT_LAYER_KEY_BIT_BLINK);
		state = (blink && !blink_state) ? FALSE : on;
		roccat_bitfield_set_bit(&custom_lights->data[0], i, state);
	}
}
