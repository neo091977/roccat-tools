#ifndef __ROCCAT_KOVA2016_PROFILE_SETTINGS_H__
#define __ROCCAT_KOVA2016_PROFILE_SETTINGS_H__

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
#include "roccat_light.h"
#include "kova2016.h"

G_BEGIN_DECLS

enum {
	KOVA2016_PROFILE_SETTINGS_CPI_LEVELS_NUM = 5,
};

typedef struct _Kova2016ProfileSettings Kova2016ProfileSettings;

struct _Kova2016ProfileSettings {
	guint8 report_id; /* KOVA2016_REPORT_ID_PROFILE_SETTINGS */
	guint8 size; /* always 0x1c */
	guint8 profile_index; /* range 0-4 */
	guint8 unknown1;
	guint8 unknown2;
	guint8 handedness;
	guint8 cpi_levels_enabled;
	guint8 cpi_levels[KOVA2016_PROFILE_SETTINGS_CPI_LEVELS_NUM];
	guint8 cpi_active; /* range 0-4 */
	guint8 polling_rate; /* RoccatPollingRate */
	guint8 lights_enabled;
	guint8 color_flow;
	guint8 light_effect;
	guint8 effect_speed;
	RoccatLight lights[KOVA2016_LIGHTS_NUM];
	guint16 checksum;
} __attribute__ ((packed));

typedef enum {
	KOVA2016_PROFILE_SETTINGS_HANDEDNESS_LEFT = 0xfe,
	KOVA2016_PROFILE_SETTINGS_HANDEDNESS_RIGHT = 0xff,
} Kova2016ProfileSettingsHandedness;

typedef enum { // FIXME
	KOVA2016_PROFILE_SETTINGS_LIGHTS_ENABLED_MASK = 0x03,
	KOVA2016_PROFILE_SETTINGS_LIGHTS_ENABLED_ON = 3,
	KOVA2016_PROFILE_SETTINGS_LIGHTS_ENABLED_OFF = 0,
	KOVA2016_PROFILE_SETTINGS_LIGHTS_ENABLED_BIT_CUSTOM_COLOR_PIPE = 4,
	KOVA2016_PROFILE_SETTINGS_LIGHTS_ENABLED_BIT_CUSTOM_COLOR_WHEEL = 5,
} Kova2016ProfileSettingsLightsEnabled;

typedef enum {
	KOVA2016_PROFILE_SETTINGS_COLOR_FLOW_OFF = 0,
	KOVA2016_PROFILE_SETTINGS_COLOR_FLOW_ON = 1,
} Kova2016ProfileSettingsColorFlow;

typedef enum {
	KOVA2016_PROFILE_SETTINGS_LIGHT_EFFECT_FULLY_LIGHTED = 1,
	KOVA2016_PROFILE_SETTINGS_LIGHT_EFFECT_BLINKING = 2,
	KOVA2016_PROFILE_SETTINGS_LIGHT_EFFECT_BREATHING = 3,
} Kova2016ProfileSettingsLightEffect;

typedef enum {
	KOVA2016_PROFILE_SETTINGS_EFFECT_SPEED_MIN = 1,
	KOVA2016_PROFILE_SETTINGS_EFFECT_SPEED_MAX = 3,
} Kova2016ProfileSettingsEffectSpeed;

static inline guint kova2016_profile_settings_cpi_level_to_cpi(guint8 cpi_level) {
	return cpi_level * 50;
}

static inline guint8 kova2016_profile_settings_cpi_to_cpi_level(guint cpi) {
	return cpi / 50;
}

Kova2016ProfileSettings const *kova2016_profile_settings_default(void);

void kova2016_profile_settings_finalize(Kova2016ProfileSettings *profile_settings, guint profile_index);
gboolean kova2016_profile_settings_write(RoccatDevice *device, guint profile_index, Kova2016ProfileSettings *profile_settings, GError **error);
Kova2016ProfileSettings *kova2016_profile_settings_read(RoccatDevice *device, guint profile_index, GError **error);

gboolean kova2016_profile_settings_equal(Kova2016ProfileSettings const *left, Kova2016ProfileSettings const *right);
void kova2016_profile_settings_copy(Kova2016ProfileSettings *destination, Kova2016ProfileSettings const *source);

G_END_DECLS

#endif
