#ifndef __ROCCAT_RYOSMKFX_LIGHT_H__
#define __ROCCAT_RYOSMKFX_LIGHT_H__

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

#include "roccat_device.h"

G_BEGIN_DECLS

typedef struct _RyosmkfxLight RyosmkfxLight;

struct _RyosmkfxLight {
	guint8 report_id; /* RYOS_REPORT_ID_LIGHT */
	guint8 size; /* 0x20 */
	guint8 profile_index; /* 0-5 */
	guint8 brightness; /* RYOS_LIGHT_BRIGHTNESS_MIN-RYOS_LIGHT_BRIGHTNESS_MAX */
	guint8 dimness; /* RYOS_LIGHT_BRIGHTNESS_MIN-RYOS_LIGHT_BRIGHTNESS_MAX */
	guint8 timeout; /* minutes */
	guint8 mode; /* RyosmkfxLightMode */
	guint8 effect; /* RyosmkfxLightEffect */
	guint8 unknown4; // 0x00
	guint8 effect_speed; /* RyosmkfxLightEffectSpeed */
	guint8 unknown5; // 0x00
	guint8 led_feedback; /* RyosmkfxLightLedFeedback */
	guint8 dimness_type; /* RyosmkfxLightDimnessType */
	guint8 unknown8; // 0x1e
	guint16 red;
	guint16 green;
	guint16 blue;
	guint8 unused[10];
	guint16 checksum;
} __attribute__ ((packed));

typedef enum {
	RYOSMKFX_LIGHT_EFFECT_SPEED_MIN = 1,
	RYOSMKFX_LIGHT_EFFECT_SPEED_MAX = 3,
} RyosmkfxLightEffectSpeed;

typedef enum {
	RYOSMKFX_LIGHT_LED_FEEDBACK_OFF = 0x00,
	RYOSMKFX_LIGHT_LED_FEEDBACK_MACRO_EXECUTION = 0x01,
} RyosmkfxLightLedFeedback;

typedef enum {
	RYOSMKFX_LIGHT_DIMNESS_TYPE_OFF = 0x00,
	RYOSMKFX_LIGHT_DIMNESS_TYPE_STARLIT_SKY = 0x01,
	// TODO 2?
	RYOSMKFX_LIGHT_DIMNESS_TYPE_FALL_ASLEEP = 0x03,
} RyosmkfxLightDimnessType;

typedef enum {
	RYOSMKFX_LIGHT_EFFECT_OFF = 0x00,
	RYOSMKFX_LIGHT_EFFECT_FULLY_LIT = 0x01,
	RYOSMKFX_LIGHT_EFFECT_BLINKING = 0x02,
	RYOSMKFX_LIGHT_EFFECT_BREATHING = 0x03,
	RYOSMKFX_LIGHT_EFFECT_HEARTBEAT = 0x04,
	RYOSMKFX_LIGHT_EFFECT_EQUALIZER = 0x05, /* implemented on client side */
	RYOSMKFX_LIGHT_EFFECT_RIPPLE_FX = 0x06, /* implemented on client side */
	RYOSMKFX_LIGHT_EFFECT_WAVE = 0x07, /* implemented on client side */
	RYOSMKFX_LIGHT_EFFECT_HEATMAP = 0x08, /* implemented on client side */
	RYOSMKFX_LIGHT_EFFECT_GAME_PRESET = 0x09, /* implemented on client side */
	RYOSMKFX_LIGHT_EFFECT_FADE_FX = 0x0a,
} RyosmkfxLightEffect;

typedef enum {
	RYOSMKFX_LIGHT_MODE_PLAIN = 0x00,
	RYOSMKFX_LIGHT_MODE_LAYER = 0x01,
} RyosmkfxLightMode;

gboolean ryosmkfx_light_write(RoccatDevice *device, guint profile_index, RyosmkfxLight *_light, GError **error);
RyosmkfxLight *ryosmkfx_light_read(RoccatDevice *device, guint profile_index, GError **error);
gboolean ryosmkfx_light_equal(RyosmkfxLight const *left, RyosmkfxLight const *right);
void ryosmkfx_light_copy(RyosmkfxLight *destination, RyosmkfxLight const *source);
RyosmkfxLight const *ryosmkfx_light_default(void);

void ryosmkfx_light_set_color(RyosmkfxLight *light, guint8 red, guint8 green, guint8 blue);
guint8 ryosmkfx_light_get_red(RyosmkfxLight const *light);
guint8 ryosmkfx_light_get_green(RyosmkfxLight const *light);
guint8 ryosmkfx_light_get_blue(RyosmkfxLight const *light);

G_END_DECLS

#endif
