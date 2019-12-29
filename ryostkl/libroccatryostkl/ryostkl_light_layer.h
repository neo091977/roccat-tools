#ifndef __ROCCAT_RYOSTKL_LIGHT_LAYER_H__
#define __ROCCAT_RYOSTKL_LIGHT_LAYER_H__

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

#include "ryostkl.h"
#include "ryos_custom_lights.h"
#include "roccat_helper.h"

G_BEGIN_DECLS

typedef struct _RyostklLightLayer RyostklLightLayer;

struct _RyostklLightLayer {
	guint8 report_id; /* RYOS_REPORT_ID_STORED_LIGHTS */
	guint8 size; /* 0xfd */
	guint8 profile_index;
	guint8 illumination_mode; /* RyosIlluminationMode */
	guint8 layer; /* RyosStoredLightsLayers */
	guint8 effect; /* RyosLightLayerEffect */
	guint8 effect_delay;
	guint8 effect_fade;
	guint16 keys[RYOS_KEYS_PRIMARY_NUM];
	guint8 thumbs[3];
	guint16 checksum;
} __attribute__ ((packed));

enum {
	RYOSTKL_LIGHT_LAYER_EFFECT_TIME_MIN = 1,
	RYOSTKL_LIGHT_LAYER_EFFECT_TIME_MAX = 30,
	RYOSTKL_LIGHT_LAYER_THUMB_BIT_STATE = 7,
	RYOSTKL_LIGHT_LAYER_KEY_EFFECT_SPEED_MIN = 1,
	RYOSTKL_LIGHT_LAYER_KEY_EFFECT_SPEED_MAX = 5,
};

typedef enum {
	RYOSTKL_LIGHT_LAYER_KEY_EFFECT_OFF = 0,
	RYOSTKL_LIGHT_LAYER_KEY_EFFECT_BLINK = 1,
	RYOSTKL_LIGHT_LAYER_KEY_EFFECT_BREATH = 3,
} RyostklLightLayerKeyEffect;

guint ryostkl_light_layer_key_get_blink_speed(guint16 const *key);
void ryostkl_light_layer_key_set_blink_speed(guint16 *key, guint speed);
guint ryostkl_light_layer_key_get_breath_speed(guint16 const *key);
void ryostkl_light_layer_key_set_breath_speed(guint16 *key, guint speed);
guint ryostkl_light_layer_key_get_effect(guint16 const *key);
void ryostkl_light_layer_key_set_effect(guint16 *key, guint effect);
gboolean ryostkl_light_layer_key_get_state(guint16 const *key);
void ryostkl_light_layer_key_set_state(guint16 *key, gboolean state);

guint16 ryostkl_light_layer_key_create(gboolean state, guint effect, guint blink_speed, guint breath_speed);

static inline gboolean ryostkl_light_layer_thumb_get_state(guint8 const *thumb) {
	return roccat_get_bit8(*thumb, RYOSTKL_LIGHT_LAYER_THUMB_BIT_STATE);
}

static inline void ryostkl_light_layer_thumb_set_state(guint8 *thumb, gboolean state) {
	roccat_set_bit8(thumb, RYOSTKL_LIGHT_LAYER_THUMB_BIT_STATE, state);
}

gboolean ryostkl_light_layer_write(RoccatDevice *device, guint profile_index, RyosIlluminationMode mode, RyosStoredLightsLayers layer, RyostklLightLayer *light_layer, GError **error);
RyostklLightLayer *ryostkl_light_layer_read(RoccatDevice *device, guint profile_index, RyosIlluminationMode mode, RyosStoredLightsLayers layer, GError **error);

gboolean ryostkl_light_layer_equal(RyostklLightLayer const *left, RyostklLightLayer const *right);
void ryostkl_light_layer_copy(RyostklLightLayer *destination, RyostklLightLayer const *source);
RyostklLightLayer *ryostkl_light_layer_dup(RyostklLightLayer const *source);

guint8 ryostkl_light_layer_key_to_thumb(guint16 const *key);
guint16 ryostkl_light_layer_thumb_to_key(guint8 const *thumb);

void ryostkl_light_layer_to_custom_lights(RyostklLightLayer const *light_layer, RyosCustomLights *custom_lights);

RyostklLightLayer const *ryostkl_light_layer_normal_default(void);
RyostklLightLayer const *ryostkl_light_layer_easy_shift_default(void);
RyostklLightLayer const *ryostkl_light_layer_fn_default(void);

G_END_DECLS

#endif
