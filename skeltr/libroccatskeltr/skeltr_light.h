#ifndef __ROCCAT_SKELTR_LIGHT_H__
#define __ROCCAT_SKELTR_LIGHT_H__

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

#include "skeltr_device.h"
#include "roccat_light.h"

G_BEGIN_DECLS

typedef struct _SkeltrLight SkeltrLight;

enum {
	SKELTR_LIGHTS_NUM = 5,
};

struct _SkeltrLight {
	guint8 report_id; /* SKELTR_REPORT_ID_LIGHT */
	guint8 size; /* 0x24 */
	guint8 profile_index;
	guint8 mode; /* SkeltrLightMode */
	guint8 preset; /* SkeltrLightPreset */
	guint8 effect_speed; /* SkeltrLightEffectSpeed */
	guint8 start_effect; /* SkeltrLightStartEffect */
	guint8 sleep_effect; /* SkeltrLightSleepEffect */
	guint8 wakeup_effect; /* SkeltrLightWakeupEffect */
	guint8 profile_change_effect; /* SkeltrLightProfileChangeEffect */
	guint8 color_flow; /* SkeltrLightColorFlow */
	RoccatLight colors[SKELTR_LIGHTS_NUM]; /* SkeltrLightColorIndex */
	guint8 brightness; /* SkeltrLightBrightness */
	guint8 unknown; /* 0x01 */
	guint8 sleep_after; /* minutes */
	guint16 checksum;
} __attribute__ ((packed));

typedef enum {
	SKELTR_LIGHT_COLOR_INDEX_KEYS = 0,
	SKELTR_LIGHT_COLOR_INDEX_TOP_LEFT = 1,
	SKELTR_LIGHT_COLOR_INDEX_BOTTOM_LEFT = 2,
	SKELTR_LIGHT_COLOR_INDEX_TOP_RIGHT = 3,
	SKELTR_LIGHT_COLOR_INDEX_BOTTOM_RIGHT = 4,
} SkeltrLightColorIndex;

typedef enum {
	SKELTR_LIGHT_COLOR_FLOW_OFF = 0,
	SKELTR_LIGHT_COLOR_FLOW_ALL_LIGHTS_SIMULTANEOUSLY = 1,
	SKELTR_LIGHT_COLOR_FLOW_UP = 2,
	SKELTR_LIGHT_COLOR_FLOW_DOWN = 3,
	SKELTR_LIGHT_COLOR_FLOW_LEFT = 4,
	SKELTR_LIGHT_COLOR_FLOW_RIGHT = 5,
} SkeltrLightColorFlow;

typedef enum {
	SKELTR_LIGHT_MODE_PRESET = 0,
	SKELTR_LIGHT_MODE_CUSTOM = 1,
} SkeltrLightMode;

typedef enum {
	SKELTR_LIGHT_EFFECT_SPEED_MIN = 0,
	SKELTR_LIGHT_EFFECT_SPEED_MAX = 2,
} SkeltrLightEffectSpeed;

typedef enum {
	SKELTR_LIGHT_BRIGHTNESS_MIN = 0,
	SKELTR_LIGHT_BRIGHTNESS_MAX = 5,
} SkeltrLightBrightness;

typedef enum {
	SKELTR_LIGHT_PRESET_OFF = 0,
	SKELTR_LIGHT_PRESET_FULLY_LIT = 1,
	SKELTR_LIGHT_PRESET_BLINKING = 2,
	// 3?
	SKELTR_LIGHT_PRESET_BREATHING = 4,
	SKELTR_LIGHT_PRESET_PULSING = 5,
	SKELTR_LIGHT_PRESET_HEARTBEAT = 6,
	SKELTR_LIGHT_PRESET_FADE_FX = 7,
	SKELTR_LIGHT_PRESET_HEATMAP = 8,
} SkeltrLightPreset;

typedef enum {
	SKELTR_LIGHT_START_EFFECT_OFF = 0,
	SKELTR_LIGHT_START_EFFECT_BREATHING = 1,
	SKELTR_LIGHT_START_EFFECT_BLINKING = 2,
	// 3?
	SKELTR_LIGHT_START_EFFECT_FLICKERING = 4,
} SkeltrLightStartEffect;

typedef enum {
	SKELTR_LIGHT_SLEEP_EFFECT_OFF = 0,
	SKELTR_LIGHT_SLEEP_EFFECT_BREATHING = 1,
	SKELTR_LIGHT_SLEEP_EFFECT_BLINKING = 2,
} SkeltrLightSleepEffect;

typedef enum {
	SKELTR_LIGHT_WAKEUP_EFFECT_OFF = 0,
	SKELTR_LIGHT_WAKEUP_EFFECT_PULSING = 1,
	SKELTR_LIGHT_WAKEUP_EFFECT_FLICKERING = 2,
	SKELTR_LIGHT_WAKEUP_EFFECT_HEARTBEAT = 3,
} SkeltrLightWakeupEffect;

typedef enum {
	SKELTR_LIGHT_PROFILE_CHANGE_EFFECT_OFF = 0,
	SKELTR_LIGHT_PROFILE_CHANGE_EFFECT_PULSING = 1,
	SKELTR_LIGHT_PROFILE_CHANGE_EFFECT_FLICKERING = 2,
	SKELTR_LIGHT_PROFILE_CHANGE_EFFECT_HEARTBEAT = 3,
} SkeltrLightProfileChangeEffect;

gboolean skeltr_light_write(RoccatDevice *device, guint profile_index, SkeltrLight *light, GError **error);
SkeltrLight *skeltr_light_read(RoccatDevice *device, guint profile_index, GError **error);
gboolean skeltr_light_equal(SkeltrLight const *left, SkeltrLight const *right);
void skeltr_light_copy(SkeltrLight *destination, SkeltrLight const *source);
SkeltrLight const *skeltr_light_default(void);

G_END_DECLS

#endif
