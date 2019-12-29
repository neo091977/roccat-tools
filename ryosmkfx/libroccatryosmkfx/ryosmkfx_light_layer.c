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

#include "ryosmkfx_light_layer.h"
#include "ryosmkfx.h"
#include "ryos_stored_lights.h"
#include "roccat_helper.h"
#include <string.h>
#include <stdlib.h>

static guint8 ryosmkfx_light_layer_get_red(RyosmkfxLightLayer const *light_layer, guint color_number) {
	guint16 hardware = 0;

	if (color_number == 0)
		return 0;

	hardware = ryosmkfx_hardware_color_build(
			light_layer->colors_red_pwm[color_number - 1],
			roccat_nibblefield_get_nibble(light_layer->colors_red_brightness, color_number - 1));
	return ryosmkfx_hardware_to_color(hardware);
}

static void ryosmkfx_light_layer_set_red(RyosmkfxLightLayer *light_layer, guint color_number, guint8 color) {
	guint16 hardware = ryosmkfx_color_to_hardware(color);
	light_layer->colors_red_pwm[color_number - 1] = ryosmkfx_hardware_color_get_pwm(hardware);
	roccat_nibblefield_set_nibble(light_layer->colors_red_brightness, color_number - 1, ryosmkfx_hardware_color_get_brightness(hardware));
}

static guint8 ryosmkfx_light_layer_get_green(RyosmkfxLightLayer const *light_layer, guint color_number) {
	guint16 hardware = 0;

	if (color_number == 0)
		return 0;

	hardware = ryosmkfx_hardware_color_build(
			light_layer->colors_green_pwm[color_number - 1],
			roccat_nibblefield_get_nibble(light_layer->colors_green_brightness, color_number - 1));
	return ryosmkfx_hardware_to_color(hardware);
}

static void ryosmkfx_light_layer_set_green(RyosmkfxLightLayer *light_layer, guint color_number, guint8 color) {
	guint16 hardware = ryosmkfx_color_to_hardware(color);
	light_layer->colors_green_pwm[color_number - 1] = ryosmkfx_hardware_color_get_pwm(hardware);
	roccat_nibblefield_set_nibble(light_layer->colors_green_brightness, color_number - 1, ryosmkfx_hardware_color_get_brightness(hardware));
}

static guint8 ryosmkfx_light_layer_get_blue(RyosmkfxLightLayer const *light_layer, guint color_number) {
	guint16 hardware = 0;

	if (color_number == 0)
		return 0;

	hardware = ryosmkfx_hardware_color_build(
			light_layer->colors_blue_pwm[color_number - 1],
			roccat_nibblefield_get_nibble(light_layer->colors_blue_brightness, color_number - 1));
	return ryosmkfx_hardware_to_color(hardware);
}

static void ryosmkfx_light_layer_set_blue(RyosmkfxLightLayer *light_layer, guint color_number, guint8 color) {
	guint16 hardware = ryosmkfx_color_to_hardware(color);
	light_layer->colors_blue_pwm[color_number - 1] = ryosmkfx_hardware_color_get_pwm(hardware);
	roccat_nibblefield_set_nibble(light_layer->colors_blue_brightness, color_number - 1, ryosmkfx_hardware_color_get_brightness(hardware));
}

gboolean ryosmkfx_light_layer_equal(RyosmkfxLightLayer const *left, RyosmkfxLightLayer const *right) {
	return memcmp(left, right, sizeof(RyosmkfxLightLayer)) ? FALSE : TRUE;
}

void ryosmkfx_light_layer_copy(RyosmkfxLightLayer *destination, RyosmkfxLightLayer const *source) {
	memcpy(destination, source, sizeof(RyosmkfxLightLayer));
}

static GList *get_unique_values(guint8 const values[RYOSMKFX_LIGHT_LAYER_KEYS_NUM],
		RyosmkfxLightLayerData const *data) {
	guint key_index;
	GHashTable *hash = g_hash_table_new(NULL, NULL);
	GList *unique_values;

	for (key_index = 0; key_index < RYOSMKFX_LIGHT_LAYER_KEYS_NUM; ++key_index) {
		if (data->keys[key_index].state)
			g_hash_table_insert(hash, GUINT_TO_POINTER(values[key_index]), GUINT_TO_POINTER(TRUE));
	}
	unique_values = g_hash_table_get_keys(hash);
	g_hash_table_destroy(hash);

	return unique_values;
}

static void init_means(guint8 const values[RYOSMKFX_LIGHT_LAYER_KEYS_NUM],
		guint8 means[RYOSMKFX_LIGHT_LAYER_COLORS_NUM],
		RyosmkfxLightLayerData const *data) {
	guint color_index;
	GList *unique_values;
	guint length;

	unique_values = get_unique_values(values, data);
	length = g_list_length(unique_values);

	for (color_index = 0; color_index < MIN(length, RYOSMKFX_LIGHT_LAYER_COLORS_NUM); ++color_index) {
		means[color_index] = GPOINTER_TO_UINT(g_list_nth_data(unique_values, color_index));
	}
	for (color_index = MIN(length, RYOSMKFX_LIGHT_LAYER_COLORS_NUM); color_index < RYOSMKFX_LIGHT_LAYER_COLORS_NUM; ++color_index) {
		means[color_index] = 0;
	}

	g_list_free(unique_values);
}

static gboolean set_cluster(guint8 const values[RYOSMKFX_LIGHT_LAYER_KEYS_NUM],
		guint8 const means[RYOSMKFX_LIGHT_LAYER_COLORS_NUM],
		guint8 cluster[RYOSMKFX_LIGHT_LAYER_KEYS_NUM],
		RyosmkfxLightLayerData const *data) {
	guint8 color_index;
	guint key_index;
	guint smallest_error;
	guint8 smallest_cluster;
	guint error;
	gboolean changed = FALSE;

	for (key_index = 0; key_index < RYOSMKFX_LIGHT_LAYER_KEYS_NUM; ++key_index) {
		if (!data->keys[key_index].state)
			continue;

		smallest_error = G_MAXUINT32;
		smallest_cluster = 0;
		for (color_index = 0; color_index < RYOSMKFX_LIGHT_LAYER_COLORS_NUM; ++color_index) {
			error = abs((gint)values[key_index] - (gint)means[color_index]);
			if (error < smallest_error) {
				smallest_error = error;
				smallest_cluster = color_index;
			}
		}
		if (cluster[key_index] != smallest_cluster) {
			cluster[key_index] = smallest_cluster;
			changed = TRUE;
		}
	}

	return changed;
}

static void update_means(guint8 const values[RYOSMKFX_LIGHT_LAYER_KEYS_NUM],
		guint8 means[RYOSMKFX_LIGHT_LAYER_COLORS_NUM],
		guint8 const cluster[RYOSMKFX_LIGHT_LAYER_KEYS_NUM],
		RyosmkfxLightLayerData const *data) {
	guint mean_sums[RYOSMKFX_LIGHT_LAYER_COLORS_NUM] = { 0 };
	guint counts[RYOSMKFX_LIGHT_LAYER_COLORS_NUM] = { 0 };
	guint color_index;
	guint key_index;

	for (key_index = 0; key_index < RYOSMKFX_LIGHT_LAYER_KEYS_NUM; ++key_index) {
		if (!data->keys[key_index].state)
			continue;

		mean_sums[cluster[key_index]] += values[key_index];
		++counts[cluster[key_index]];
	}

	for (color_index = 0; color_index < RYOSMKFX_LIGHT_LAYER_COLORS_NUM; ++color_index) {
		if (counts[color_index] > 0) {
			means[color_index] = mean_sums[color_index] / counts[color_index];
		}
	}
}

void ryosmkfx_light_layer_set_data(RyosmkfxLightLayer *light_layer, RyosmkfxLightLayerData const *data) {
	guint8 values_red[RYOSMKFX_LIGHT_LAYER_KEYS_NUM];
	guint8 values_green[RYOSMKFX_LIGHT_LAYER_KEYS_NUM];
	guint8 values_blue[RYOSMKFX_LIGHT_LAYER_KEYS_NUM];
	guint8 means_red[RYOSMKFX_LIGHT_LAYER_COLORS_NUM];
	guint8 means_green[RYOSMKFX_LIGHT_LAYER_COLORS_NUM];
	guint8 means_blue[RYOSMKFX_LIGHT_LAYER_COLORS_NUM];
	guint8 cluster_red[RYOSMKFX_LIGHT_LAYER_KEYS_NUM];
	guint8 cluster_green[RYOSMKFX_LIGHT_LAYER_KEYS_NUM];
	guint8 cluster_blue[RYOSMKFX_LIGHT_LAYER_KEYS_NUM];
	guint color_index;
	guint key_index;

	for (key_index = 0; key_index < RYOSMKFX_LIGHT_LAYER_KEYS_NUM; ++key_index) {
		values_red[key_index] = data->keys[key_index].red;
		values_green[key_index] = data->keys[key_index].green;
		values_blue[key_index] = data->keys[key_index].blue;

		cluster_red[key_index] = G_MAXUINT8;
		cluster_green[key_index] = G_MAXUINT8;
		cluster_blue[key_index] = G_MAXUINT8;
	}

//#pragma omp parallel
	{
//#pragma omp sections
		{
//#pragma omp section
			{
				init_means(values_red, means_red, data);
				while (set_cluster(values_red, means_red, cluster_red, data)) {
					update_means(values_red, means_red, cluster_red, data);
				}
			}
//#pragma omp section
			{
				init_means(values_green, means_green, data);
				while (set_cluster(values_green, means_green, cluster_green, data)) {
					update_means(values_green, means_green, cluster_green, data);
				}
			}
//#pragma omp section
			{
				init_means(values_blue, means_blue, data);
				while (set_cluster(values_blue, means_blue, cluster_blue, data)) {
					update_means(values_blue, means_blue, cluster_blue, data);
				}
			}
		}
	}

	for (color_index = 0; color_index < RYOSMKFX_LIGHT_LAYER_COLORS_NUM; ++color_index) {
		ryosmkfx_light_layer_set_red(light_layer, color_index + 1, means_red[color_index]);
		ryosmkfx_light_layer_set_green(light_layer, color_index + 1, means_green[color_index]);
		ryosmkfx_light_layer_set_blue(light_layer, color_index + 1, means_blue[color_index]);
	}

	for (key_index = 0; key_index < RYOSMKFX_LIGHT_LAYER_KEYS_NUM; ++key_index) {
		roccat_nibblefield_set_nibble(light_layer->numbers_red, key_index, cluster_red[key_index] + 1);
		roccat_nibblefield_set_nibble(light_layer->numbers_green, key_index, cluster_green[key_index] + 1);
		roccat_nibblefield_set_nibble(light_layer->numbers_blue, key_index, cluster_blue[key_index] + 1);
		roccat_bitfield_set_bit(light_layer->states, key_index, data->keys[key_index].state);
	}
}

void ryosmkfx_light_layer_get_data(RyosmkfxLightLayer const *light_layer, RyosmkfxLightLayerData *data) {
	guint number;
	guint i;

	for (i = 0; i < RYOSMKFX_LIGHT_LAYER_KEYS_NUM; ++i) {
		data->keys[i].state = roccat_bitfield_get_bit(light_layer->states, i);

		number = roccat_nibblefield_get_nibble(light_layer->numbers_red, i);
		data->keys[i].red = ryosmkfx_light_layer_get_red(light_layer, number);

		number = roccat_nibblefield_get_nibble(light_layer->numbers_green, i);
		data->keys[i].green = ryosmkfx_light_layer_get_green(light_layer, number);

		number = roccat_nibblefield_get_nibble(light_layer->numbers_blue, i);
		data->keys[i].blue = ryosmkfx_light_layer_get_blue(light_layer, number);
	}
}

RyosmkfxLightLayer *ryosmkfx_light_layer_plain(guint8 red, guint8 green, guint8 blue) {
	RyosmkfxLightLayer *light_layer;
	guint key_index;

	light_layer = (RyosmkfxLightLayer *)g_malloc(sizeof(RyosmkfxLightLayer));

	ryosmkfx_light_layer_set_red(light_layer, 1, red);
	ryosmkfx_light_layer_set_green(light_layer, 1, green);
	ryosmkfx_light_layer_set_blue(light_layer, 1, blue);

	for (key_index = 0; key_index < RYOSMKFX_LIGHT_LAYER_KEYS_NUM; ++key_index) {
		roccat_nibblefield_set_nibble(light_layer->numbers_red, key_index, 1);
		roccat_nibblefield_set_nibble(light_layer->numbers_green, key_index, 1);
		roccat_nibblefield_set_nibble(light_layer->numbers_blue, key_index, 1);
		roccat_bitfield_set_bit(light_layer->states, key_index, TRUE);
	}

	return light_layer;
}

void ryosmkfx_light_layer_data_set_all_off(RyosmkfxLightLayerData *data) {
	guint key_index;
	for (key_index = 0; key_index < RYOSMKFX_LIGHT_LAYER_KEYS_NUM; ++key_index)
		data->keys[key_index].state = FALSE;
}

gboolean ryosmkfx_light_layer_data_are_all_off(RyosmkfxLightLayerData const *data) {
	guint key_index;
	for (key_index = 0; key_index < RYOSMKFX_LIGHT_LAYER_KEYS_NUM; ++key_index)
		if (data->keys[key_index].state)
			return FALSE;
	return TRUE;
}
