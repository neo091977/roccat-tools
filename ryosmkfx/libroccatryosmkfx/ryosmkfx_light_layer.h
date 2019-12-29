#ifndef __ROCCAT_RYOSMKFX_LIGHT_LAYER_H__
#define __ROCCAT_RYOSMKFX_LIGHT_LAYER_H__

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

#include <glib.h>

G_BEGIN_DECLS

typedef struct _RyosmkfxLightLayer RyosmkfxLightLayer;
typedef struct _RyosmkfxLightLayerKey RyosmkfxLightLayerKey;
typedef struct _RyosmkfxLightLayerData RyosmkfxLightLayerData;

enum {
	RYOSMKFX_LIGHT_LAYER_COLORS_NUM = 7,
	RYOSMKFX_LIGHT_LAYER_KEYS_NUM = 120,
	RYOSMKFX_LIGHT_LAYER_STATES_NUM = 15,
};

struct _RyosmkfxLightLayer {
	guint8 states[RYOSMKFX_LIGHT_LAYER_STATES_NUM]; // bitwise on/off state as in RyosCustomLights
	guint8 numbers_red[60]; // nibbles determining color number
	guint8 numbers_green[60];
	guint8 numbers_blue[60];
	guint8 colors_red_pwm[RYOSMKFX_LIGHT_LAYER_COLORS_NUM];
	guint8 colors_green_pwm[RYOSMKFX_LIGHT_LAYER_COLORS_NUM];
	guint8 colors_blue_pwm[RYOSMKFX_LIGHT_LAYER_COLORS_NUM];
	guint8 colors_red_brightness[4];
	guint8 colors_green_brightness[4];
	guint8 colors_blue_brightness[4];
} __attribute__ ((packed));

struct _RyosmkfxLightLayerKey {
	guint red; /* 0-255 */
	guint green;
	guint blue;
	gboolean state;
};

struct _RyosmkfxLightLayerData {
	RyosmkfxLightLayerKey keys[RYOSMKFX_LIGHT_LAYER_KEYS_NUM];
};

void ryosmkfx_light_layer_set_data(RyosmkfxLightLayer *light_layer, RyosmkfxLightLayerData const *data);
void ryosmkfx_light_layer_get_data(RyosmkfxLightLayer const *light_layer, RyosmkfxLightLayerData *data);

gboolean ryosmkfx_light_layer_equal(RyosmkfxLightLayer const *left, RyosmkfxLightLayer const *right);
void ryosmkfx_light_layer_copy(RyosmkfxLightLayer *destination, RyosmkfxLightLayer const *source);

RyosmkfxLightLayer *ryosmkfx_light_layer_plain(guint8 red, guint8 green, guint8 blue);

void ryosmkfx_light_layer_data_set_all_off(RyosmkfxLightLayerData *data);
gboolean ryosmkfx_light_layer_data_are_all_off(RyosmkfxLightLayerData const *data);

G_END_DECLS

#endif
