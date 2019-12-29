#ifndef __ROCCAT_RYOS_STORED_LIGHTS_H__
#define __ROCCAT_RYOS_STORED_LIGHTS_H__

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

#include "ryos.h"

G_BEGIN_DECLS

typedef struct _RyosLightLayer RyosLightLayer;
typedef struct _RyosStoredLights RyosStoredLights;

enum  {
	RYOS_STORED_LIGHTS_LAYER_NUM = 11,
	RYOS_LIGHT_LAYER_KEY_NUM = 123,
	RYOS_LIGHT_LAYER_INDEX_INVALID = 0xff,
};

struct _RyosLightLayer {
	guint8 effect;
	guint8 fade_delay; /* seconds */
	guint8 keys[RYOS_LIGHT_LAYER_KEY_NUM]; /* index as in keys_primary + thumbster keys */
} __attribute__ ((packed));

typedef enum {
	RYOS_LIGHT_LAYER_EFFECT_OFF = 0x0,
	RYOS_LIGHT_LAYER_EFFECT_FADE = 0x1,
	RYOS_LIGHT_LAYER_EFFECT_RIPPLE = 0x2,
} RyosLightLayerEffect;

typedef enum {
	RYOS_LIGHT_LAYER_KEY_BIT_ON = 0,
	/* blink only works when RyosLightLayer.effect == RYOS_LIGHT_LAYER_EFFECT_OFF */
	RYOS_LIGHT_LAYER_KEY_BIT_BLINK = 1,
} RyosLightLayerKey;

void ryos_light_layer_copy(RyosLightLayer *destination, RyosLightLayer const *source);

struct _RyosStoredLights {
	guint8 report_id; /* RYOS_REPORT_ID_STORED_LIGHTS */
	guint16 size; /* 1382 */
	guint8 profile_index;
	guint8 illumination_mode;
	RyosLightLayer layers[RYOS_STORED_LIGHTS_LAYER_NUM];
	guint16 checksum;
} __attribute__ ((packed));

typedef enum {
	RYOS_STORED_LIGHTS_LAYER_NORMAL = 0x0,
	RYOS_STORED_LIGHTS_LAYER_LEFT_SHIFT = 0x1,
	RYOS_STORED_LIGHTS_LAYER_RIGHT_SHIFT = 0x2,
	RYOS_STORED_LIGHTS_LAYER_LEFT_CTRL = 0x3,
	RYOS_STORED_LIGHTS_LAYER_RIGHT_CTRL = 0x4,
	RYOS_STORED_LIGHTS_LAYER_LEFT_ALT = 0x5,
	RYOS_STORED_LIGHTS_LAYER_RIGHT_ALT = 0x6,
	RYOS_STORED_LIGHTS_LAYER_LEFT_WIN = 0x7,
	RYOS_STORED_LIGHTS_LAYER_RIGHT_WIN = 0x8,
	RYOS_STORED_LIGHTS_LAYER_EASY_SHIFT = 0x9,
	RYOS_STORED_LIGHTS_LAYER_FN = 0xa,
} RyosStoredLightsLayers;

gboolean ryos_stored_lights_write(RoccatDevice *ryos, guint profile_index, guint mode, RyosStoredLights *stored_lights, GError **error);
RyosStoredLights *ryos_stored_lights_read_manual(RoccatDevice *ryos, guint profile_index, GError **error);
RyosStoredLights *ryos_stored_lights_read_automatic(RoccatDevice *ryos, guint profile_index, GError **error);
gboolean ryos_stored_lights_equal(RyosStoredLights const *left, RyosStoredLights const *right);
void ryos_stored_lights_copy(RyosStoredLights *destination, RyosStoredLights const *source);
RyosLightLayer *ryos_get_active_light_layer(RoccatDevice *ryos, GError **error);
RyosStoredLights const *ryos_stored_lights_default(void);

G_END_DECLS

#endif
