#ifndef __ROCCAT_SOVA_LIGHT_H__
#define __ROCCAT_SOVA_LIGHT_H__

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

#include "sova_device.h"

G_BEGIN_DECLS

typedef struct _SovaLight SovaLight;

struct _SovaLight {
	guint8 report_id; /* SOVA_REPORT_ID_LIGHT */
	guint8 size; /* 0xa0 */
	guint8 profile_index;
	guint8 unknown1;
	guint8 preset; /* SovaLightPreset */
	guint8 effect_speed; /* SovaLightEffectSpeed */
	guint8 start_effect; /* SovaLightStartEffect */
	guint8 sleep_effect; /* SovaLightSleepEffect */
	guint8 wakeup_effect; /* SovaLightWakeupEffect */
	guint8 profile_change_effect; /* SovaLightProfileChangeEffect */
	guint8 unknown2;
	guint8 brightness; /* SovaLightBrightness */
	guint8 unknown3; /* 0x01 */
	guint8 sleep_after; /* minutes */
	guint8 unknown4[144];
	guint16 checksum;
} __attribute__ ((packed));

typedef enum {
	SOVA_LIGHT_EFFECT_SPEED_MIN = 0,
	SOVA_LIGHT_EFFECT_SPEED_MAX = 2,
} SovaLightEffectSpeed;

typedef enum {
	SOVA_LIGHT_BRIGHTNESS_MIN = 0,
	SOVA_LIGHT_BRIGHTNESS_MAX = 5,
} SovaLightBrightness;

typedef enum {
	SOVA_LIGHT_PRESET_OFF = 0,
	SOVA_LIGHT_PRESET_FULLY_LIT = 1,
	SOVA_LIGHT_PRESET_BLINKING = 2,
	SOVA_LIGHT_PRESET_BREATHING = 3,
	SOVA_LIGHT_PRESET_HEARTBEAT = 4,
	SOVA_LIGHT_PRESET_IMPULSE = 5,
	SOVA_LIGHT_PRESET_HEATMAP = 6,
} SovaLightPreset;

typedef enum {
	SOVA_LIGHT_START_EFFECT_OFF = 0,
	SOVA_LIGHT_START_EFFECT_BREATHING = 1,
	SOVA_LIGHT_START_EFFECT_BLINKING = 2,
	//SOVA_LIGHT_START_EFFECT_ = 3,
	SOVA_LIGHT_START_EFFECT_FLICKERING = 4,
} SovaLightStartEffect;

typedef enum {
	SOVA_LIGHT_SLEEP_EFFECT_OFF = 0,
	SOVA_LIGHT_SLEEP_EFFECT_BREATHING = 1,
	SOVA_LIGHT_SLEEP_EFFECT_BLINKING = 2,
} SovaLightSleepEffect;

typedef enum {
	SOVA_LIGHT_WAKEUP_EFFECT_OFF = 0,
	SOVA_LIGHT_WAKEUP_EFFECT_IMPULSE = 1,
	SOVA_LIGHT_WAKEUP_EFFECT_FLICKERING = 2,
	SOVA_LIGHT_WAKEUP_EFFECT_HEARTBEAT = 3,
} SovaLightWakeupEffect;

typedef enum {
	SOVA_LIGHT_PROFILE_CHANGE_EFFECT_OFF = 0,
	SOVA_LIGHT_PROFILE_CHANGE_EFFECT_IMPULSE = 1,
	SOVA_LIGHT_PROFILE_CHANGE_EFFECT_FLICKERING = 2,
	SOVA_LIGHT_PROFILE_CHANGE_EFFECT_HEARTBEAT = 3,
} SovaLightProfileChangeEffect;

gboolean sova_light_write(RoccatDevice *device, guint profile_index, SovaLight *light, GError **error);
SovaLight *sova_light_read(RoccatDevice *device, guint profile_index, GError **error);
gboolean sova_light_equal(SovaLight const *left, SovaLight const *right);
void sova_light_copy(SovaLight *destination, SovaLight const *source);
SovaLight const *sova_light_default(void);

G_END_DECLS

#endif
