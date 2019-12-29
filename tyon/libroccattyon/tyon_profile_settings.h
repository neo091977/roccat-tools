#ifndef __ROCCAT_TYON_PROFILE_SETTINGS_H__
#define __ROCCAT_TYON_PROFILE_SETTINGS_H__

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
#include "roccat_helper.h"
#include "tyon.h"

G_BEGIN_DECLS

enum {
	TYON_PROFILE_SETTINGS_CPI_LEVELS_NUM = 5,
};

typedef struct _TyonProfileSettings TyonProfileSettings;
typedef struct _TyonLight TyonLight;

struct _TyonLight {
	guint8 index;
	guint8 red;
	guint8 green;
	guint8 blue;
	guint8 unused;
} __attribute__ ((packed));

struct _TyonProfileSettings {
	guint8 report_id; /* TYON_REPORT_ID_PROFILE_SETTINGS */
	guint8 size; /* always 30 */
	guint8 profile_index; /* range 0-4 */
	guint8 advanced_sensitivity; /* RoccatSensitivityAdvanced */
	guint8 sensitivity_x; /* RoccatSensitivity */
	guint8 sensitivity_y; /* RoccatSensitivity */
	guint8 cpi_levels_enabled;
	guint8 cpi_levels[TYON_PROFILE_SETTINGS_CPI_LEVELS_NUM];
	guint8 cpi_active; /* range 0-4 */
	guint8 talkfx_polling_rate; /* TyonProfileSettingsTalkfx + RoccatPollingRate */
	guint8 lights_enabled;
	guint8 color_flow;
	guint8 light_effect;
	guint8 effect_speed;
	TyonLight lights[TYON_LIGHTS_NUM];
	guint16 checksum;
} __attribute__ ((packed));

static inline guint8 tyon_profile_settings_get_polling_rate(TyonProfileSettings const *profile_settings) {
	return roccat_get_nibble8(profile_settings->talkfx_polling_rate, ROCCAT_NIBBLE_LOW);
}

static inline guint8 tyon_profile_settings_get_talkfx(TyonProfileSettings const *profile_settings) {
	return roccat_get_nibble8(profile_settings->talkfx_polling_rate, ROCCAT_NIBBLE_HIGH);
}

static inline void tyon_profile_settings_set_polling_rate(TyonProfileSettings *profile_settings, guint8 new_value) {
	roccat_set_nibble8(&profile_settings->talkfx_polling_rate, ROCCAT_NIBBLE_LOW, new_value);
}

static inline void tyon_profile_settings_set_talkfx(TyonProfileSettings *profile_settings, guint8 new_value) {
	roccat_set_nibble8(&profile_settings->talkfx_polling_rate, ROCCAT_NIBBLE_HIGH, new_value);
}

typedef enum {
	TYON_PROFILE_SETTINGS_TALKFX_ON = 0,
	TYON_PROFILE_SETTINGS_TALKFX_OFF = 1,
} TyonProfileSettingsTalkfx;

typedef enum {
	TYON_PROFILE_SETTINGS_LIGHTS_ENABLED_BIT_WHEEL = 0,
	TYON_PROFILE_SETTINGS_LIGHTS_ENABLED_BIT_BOTTOM = 1,
	TYON_PROFILE_SETTINGS_LIGHTS_ENABLED_BIT_CUSTOM_COLOR = 4,
} TyonProfileSettingsLightsEnabled;

typedef enum {
	TYON_PROFILE_SETTINGS_COLOR_FLOW_OFF = 0,
	TYON_PROFILE_SETTINGS_COLOR_FLOW_SIMULTANEOUSLY = 1,
	TYON_PROFILE_SETTINGS_COLOR_FLOW_UP = 2,
	TYON_PROFILE_SETTINGS_COLOR_FLOW_DOWN = 3,
} TyonProfileSettingsColorFlow;

typedef enum {
	TYON_PROFILE_SETTINGS_LIGHT_EFFECT_ALL_OFF = 0,
	TYON_PROFILE_SETTINGS_LIGHT_EFFECT_FULLY_LIGHTED = 1,
	TYON_PROFILE_SETTINGS_LIGHT_EFFECT_BLINKING = 2,
	TYON_PROFILE_SETTINGS_LIGHT_EFFECT_BREATHING = 3,
	TYON_PROFILE_SETTINGS_LIGHT_EFFECT_HEARTBEAT = 4,
} TyonProfileSettingsLightEffect;

typedef enum {
	TYON_PROFILE_SETTINGS_EFFECT_SPEED_MIN = 1,
	TYON_PROFILE_SETTINGS_EFFECT_SPEED_MAX = 3,
} TyonProfileSettingsEffectSpeed;

void tyon_profile_settings_finalize(TyonProfileSettings *profile_settings, guint profile_index);
gboolean tyon_profile_settings_write(RoccatDevice *device, guint profile_index, TyonProfileSettings *profile_settings, GError **error);
TyonProfileSettings *tyon_profile_settings_read(RoccatDevice *device, guint profile_index, GError **error);
gboolean tyon_profile_settings_equal(TyonProfileSettings const *left, TyonProfileSettings const *right);

G_END_DECLS

#endif
