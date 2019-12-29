#ifndef __ROCCAT_KIRO_PROFILE_H__
#define __ROCCAT_KIRO_PROFILE_H__

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
#include "roccat_button.h"
#include "roccat_light.h"
#include "kiro.h"

G_BEGIN_DECLS

enum {
	KIRO_PROFILE_CPI_LEVELS_NUM = 5,
};

typedef struct _KiroProfile KiroProfile;

/* TODO no breathing with color flow on */
/* Descriptor says it's 65 bytes, Win software reads 66 bytes but writes 65 bytes */
struct _KiroProfile {
	guint8 report_id; /* KIRO_REPORT_ID_PROFILE */
	guint8 size; /* reported 0x34(52) */
	guint8 sensitivity; /* RoccatSensitivity */
	guint8 cpi_levels_enabled; /* bits 0-4 */
	guint8 cpi_levels[KIRO_PROFILE_CPI_LEVELS_NUM];
	guint8 cpi_active; /* range 0-4 */
	guint8 misc; /* KiroProfileMisc, RoccatPollingRate */ 
	guint8 lights_enabled; /* KiroProfileLightsEnabled */
	guint8 color_flow; /* KiroProfileColorFlow */
	guint8 light_effect; /* KiroProfileLightEffect */
	guint8 effect_speed; /* KiroProfileEffectSpeed */
	RoccatLight light;
	guint8 unused1;
	RoccatButton buttons[KIRO_BUTTON_NUM];
	guint16 checksum;
	guint8 unused2[13];
} __attribute__ ((packed));

typedef enum {
	KIRO_PROFILE_LIGHTS_ENABLED_MASK = 0x03,
	KIRO_PROFILE_LIGHTS_ENABLED_ON = 1,
	KIRO_PROFILE_LIGHTS_ENABLED_OFF = 0,
	KIRO_PROFILE_LIGHTS_ENABLED_BIT_CUSTOM_COLOR = 2,
} KiroProfileLightsEnabled;

typedef enum {
	KIRO_PROFILE_MISC_POLLING_RATE_MASK = 0x0f,
	KIRO_PROFILE_MISC_LEFT_SIDE_BUTTONS_BIT = 4,
	KIRO_PROFILE_MISC_RIGHT_SIDE_BUTTONS_BIT = 5,
	KIRO_PROFILE_MISC_LEFT_HANDED_BIT = 6,
} KiroProfileMisc;

typedef enum {
	KIRO_PROFILE_COLOR_FLOW_OFF = 0,
	KIRO_PROFILE_COLOR_FLOW_ON = 1,
} KiroProfileColorFlow;

typedef enum {
	KIRO_PROFILE_LIGHT_EFFECT_FULLY_LIGHTED = 1,
	KIRO_PROFILE_LIGHT_EFFECT_BLINKING = 2,
	KIRO_PROFILE_LIGHT_EFFECT_BREATHING = 3,
} KiroProfileLightEffect;

typedef enum {
	KIRO_PROFILE_EFFECT_SPEED_MIN = 1,
	KIRO_PROFILE_EFFECT_SPEED_MAX = 3,
} KiroProfileEffectSpeed;

typedef enum {
	KIRO_BUTTON_INDEX_LEFT = 0,
	KIRO_BUTTON_INDEX_RIGHT,
	KIRO_BUTTON_INDEX_MIDDLE,
	KIRO_BUTTON_INDEX_FORWARD_LEFT,
	KIRO_BUTTON_INDEX_BACKWARD_LEFT,
	KIRO_BUTTON_INDEX_FORWARD_RIGHT,
	KIRO_BUTTON_INDEX_BACKWARD_RIGHT,
	KIRO_BUTTON_INDEX_TOP,
	KIRO_BUTTON_INDEX_WHEEL_UP,
	KIRO_BUTTON_INDEX_WHEEL_DOWN,
} KiroButtonIndex;

typedef enum {
	KIRO_BUTTON_TYPE_DISABLED = 0x00,
	KIRO_BUTTON_TYPE_CLICK = 0x01,
	KIRO_BUTTON_TYPE_MENU = 0x02,
	KIRO_BUTTON_TYPE_UNIVERSAL_SCROLLING = 0x03,
	KIRO_BUTTON_TYPE_DOUBLE_CLICK = 0x04, /* missing in Windows */
	KIRO_BUTTON_TYPE_SHORTCUT = 0x05,

	KIRO_BUTTON_TYPE_BROWSER_FORWARD = 0x07,
	KIRO_BUTTON_TYPE_BROWSER_BACKWARD = 0x08,
	KIRO_BUTTON_TYPE_TILT_LEFT = 0x09,
	KIRO_BUTTON_TYPE_TILT_RIGHT = 0x0a,

	KIRO_BUTTON_TYPE_SCROLL_UP = 0x0d,
	KIRO_BUTTON_TYPE_SCROLL_DOWN = 0x0e,
	KIRO_BUTTON_TYPE_OPEN_APPLICATION = 0x0f,

	KIRO_BUTTON_TYPE_CPI_CYCLE = 0x14,
	KIRO_BUTTON_TYPE_CPI_UP = 0x15,
	KIRO_BUTTON_TYPE_CPI_DOWN = 0x16,
	KIRO_BUTTON_TYPE_SENSITIVITY_CYCLE = 0x17,
	KIRO_BUTTON_TYPE_SENSITIVITY_UP = 0x18,
	KIRO_BUTTON_TYPE_SENSITIVITY_DOWN = 0x19,

	KIRO_BUTTON_TYPE_OPEN_DRIVER = 0x1b,

	KIRO_BUTTON_TYPE_PREV_TRACK = 0x21,
	KIRO_BUTTON_TYPE_NEXT_TRACK = 0x22,
	KIRO_BUTTON_TYPE_PLAY_PAUSE = 0x23,
	KIRO_BUTTON_TYPE_STOP = 0x24,
	KIRO_BUTTON_TYPE_MUTE_MASTER = 0x25,
	KIRO_BUTTON_TYPE_VOLUME_DOWN = 0x26,
	KIRO_BUTTON_TYPE_VOLUME_UP = 0x27,

	KIRO_BUTTON_TYPE_HOME = 0x62,
	KIRO_BUTTON_TYPE_END = 0x63,
	KIRO_BUTTON_TYPE_PAGE_UP = 0x64,
	KIRO_BUTTON_TYPE_PAGE_DOWN = 0x65,
	KIRO_BUTTON_TYPE_LEFT_SHIFT = 0x66, /* missing in Windows */
	KIRO_BUTTON_TYPE_LEFT_CTRL = 0x67, /* missing in Windows */
	KIRO_BUTTON_TYPE_RIGHT_ALT = 0x68, /* missing in Windows */
	KIRO_BUTTON_TYPE_WIN = 0x69, /* missing in Windows */
} KiroButtonType;

static inline guint kiro_profile_cpi_level_to_cpi(guint8 cpi_level) {
	return cpi_level * 50;
}

static inline guint8 kiro_profile_cpi_to_cpi_level(guint cpi) {
	return cpi / 50;
}

KiroProfile const *kiro_profile_default(void);

KiroProfile *kiro_profile_read(RoccatDevice *device, GError **error);
gboolean kiro_profile_write(RoccatDevice *device, KiroProfile *profile, GError **error);

gboolean kiro_profile_equal(KiroProfile const *left, KiroProfile const *right);
void kiro_profile_copy(KiroProfile *destination, KiroProfile const *source);

G_END_DECLS

#endif
