#ifndef __ROCCAT_NYTH_PROFILE_SETTINGS_H__
#define __ROCCAT_NYTH_PROFILE_SETTINGS_H__

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
#include "nyth.h"

G_BEGIN_DECLS

enum {
	NYTH_PROFILE_SETTINGS_CPI_LEVELS_NUM = 5,
};

typedef struct _NythProfileSettings NythProfileSettings;

struct _NythProfileSettings {
	guint8 report_id; /* NYTH_REPORT_ID_PROFILE_SETTINGS */
	guint8 size; /* always 0x21 */
	guint8 profile_index; /* range 0-4 */
	guint8 advanced_sensitivity; /* RoccatSensitivityAdvanced */
	guint8 sensitivity_x; /* RoccatSensitivity */
	guint8 sensitivity_y; /* RoccatSensitivity */
	guint8 cpi_levels_enabled;
	guint16 cpi_levels[NYTH_PROFILE_SETTINGS_CPI_LEVELS_NUM];
	guint8 cpi_active; /* range 0-4 */
	guint8 polling_rate; /* RoccatPollingRate */
	guint8 lights_enabled;
	guint8 color_effect;
	guint8 light_effect;
	guint8 effect_speed;
	RoccatLight lights[NYTH_LIGHTS_NUM];
	guint16 checksum;
} __attribute__ ((packed));

typedef enum {
	NYTH_PROFILE_SETTINGS_LIGHTS_ENABLED_MASK = 0x03,
	NYTH_PROFILE_SETTINGS_LIGHTS_ENABLED_ON = 3,
	NYTH_PROFILE_SETTINGS_LIGHTS_ENABLED_OFF = 0,
	NYTH_PROFILE_SETTINGS_LIGHTS_ENABLED_BIT_CUSTOM_COLOR_LOGO = 4,
	NYTH_PROFILE_SETTINGS_LIGHTS_ENABLED_BIT_CUSTOM_COLOR_BOTTOM = 5,
} NythProfileSettingsLightsEnabled;

typedef enum {
	NYTH_PROFILE_SETTINGS_COLOR_EFFECT_OFF = 0,
	NYTH_PROFILE_SETTINGS_COLOR_EFFECT_SIMULTANEOUSLY = 1,
	NYTH_PROFILE_SETTINGS_COLOR_EFFECT_UP = 2,
	NYTH_PROFILE_SETTINGS_COLOR_EFFECT_DOWN = 3,
} NythProfileSettingsColorEffect;

typedef enum {
	NYTH_PROFILE_SETTINGS_LIGHT_EFFECT_ALL_OFF = 0, // FIXME unused
	NYTH_PROFILE_SETTINGS_LIGHT_EFFECT_FULLY_LIGHTED = 1,
	NYTH_PROFILE_SETTINGS_LIGHT_EFFECT_BLINKING = 2,
	NYTH_PROFILE_SETTINGS_LIGHT_EFFECT_BREATHING = 3,
	NYTH_PROFILE_SETTINGS_LIGHT_EFFECT_HEARTBEAT = 4,
} NythProfileSettingsLightEffect;

typedef enum {
	NYTH_PROFILE_SETTINGS_EFFECT_SPEED_MIN = 1,
	NYTH_PROFILE_SETTINGS_EFFECT_SPEED_MAX = 3,
} NythProfileSettingsEffectSpeed;

static inline void nyth_profile_settings_set_cpi_level(NythProfileSettings *profile_settings, guint index, guint16 new_value) {
	profile_settings->cpi_levels[index] = GUINT16_TO_LE(new_value);
}

static inline guint16 nyth_profile_settings_get_cpi_level(NythProfileSettings const *profile_settings, guint index) {
	return GUINT16_FROM_LE(profile_settings->cpi_levels[index]);
}

NythProfileSettings const *nyth_profile_settings_default(void);

void nyth_profile_settings_finalize(NythProfileSettings *profile_settings, guint profile_index);
gboolean nyth_profile_settings_write(RoccatDevice *device, guint profile_index, NythProfileSettings *profile_settings, GError **error);
NythProfileSettings *nyth_profile_settings_read(RoccatDevice *device, guint profile_index, GError **error);

gboolean nyth_profile_settings_equal(NythProfileSettings const *left, NythProfileSettings const *right);
void nyth_profile_settings_copy(NythProfileSettings *destination, NythProfileSettings const *source);

G_END_DECLS

#endif
