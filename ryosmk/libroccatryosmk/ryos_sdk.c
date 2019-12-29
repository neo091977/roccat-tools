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

#include "ryos_sdk.h"
#include "ryos_custom_lights.h"
#include "ryos_key_relations.h"
#include "ryos_light.h"
#include "ryos_profile.h"
#include "ryos_light_control.h"
#include "roccat_helper.h"
#include <string.h>

static RyosCustomLights sdk_custom_lights = { 0 };

static gboolean sdk_write(RoccatDevice *ryos, GError **error) {
	return ryos_custom_lights_write(ryos, &sdk_custom_lights, error);
}

gboolean ryos_sdk_set_kb_sdk_mode(RoccatDevice *ryos, gboolean state, GError **error) {
	gboolean retval;
	RyosLightLayer *light_layer;
	
	retval = ryos_light_control_custom(ryos, state, error);
	if (!retval)
		return retval;

	if (!state)
		return retval;

	light_layer = ryos_get_active_light_layer(ryos, error);
	if (!light_layer)
		return FALSE;
	
	ryos_light_layer_to_custom_lights(light_layer, &sdk_custom_lights, TRUE);
	g_free(light_layer);
	
	return sdk_write(ryos, error);
}

gboolean ryos_sdk_set_all_leds(RoccatDevice *ryos, GArray const *data, GError **error) {
	guint i;
	gboolean value;

	for (i = 0; i < RYOS_SDK_KEY_NUM; ++i) {
		if (i < data->len)
			value = g_array_index(data, guchar, i) ? TRUE : FALSE;
		else
			value = FALSE;
		roccat_bitfield_set_bit(&sdk_custom_lights.data[0], ryos_sdk_index_to_light_index(i), value);
	}

	return sdk_write(ryos, error);
}

gboolean ryos_sdk_turn_on_all_leds(RoccatDevice *ryos, GError **error) {
	memset(&sdk_custom_lights.data[0], 0xff, RYOS_CUSTOM_LIGHTS_DATA_LENGTH);
	return sdk_write(ryos, error);
}

gboolean ryos_sdk_turn_off_all_leds(RoccatDevice *ryos, GError **error) {
	memset(&sdk_custom_lights.data[0], 0x00, RYOS_CUSTOM_LIGHTS_DATA_LENGTH);
	return sdk_write(ryos, error);
}

gboolean ryos_sdk_set_led_on(RoccatDevice *ryos, guint8 index, GError **error) {
	roccat_bitfield_set_bit(&sdk_custom_lights.data[0], ryos_sdk_index_to_light_index(index), TRUE);
	return sdk_write(ryos, error);
}

gboolean ryos_sdk_set_led_off(RoccatDevice *ryos, guint8 index, GError **error) {
	roccat_bitfield_set_bit(&sdk_custom_lights.data[0], ryos_sdk_index_to_light_index(index), FALSE);
	return sdk_write(ryos, error);
}

// TODO threaded?
gboolean ryos_sdk_all_key_blinking(RoccatDevice *ryos, guint interval, guint count, GError **error) {
	guint i;

	for (i = 0; i < count; ++i) {
		if (!ryos_sdk_turn_on_all_leds(ryos, error))
			return FALSE;
		g_usleep(interval * 1000);
		if (!ryos_sdk_turn_off_all_leds(ryos, error))
			return FALSE;
		g_usleep(interval * 1000);
	}

	return TRUE;
}
