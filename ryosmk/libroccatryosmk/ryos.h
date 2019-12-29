#ifndef __ROCCAT_RYOS_H__
#define __ROCCAT_RYOS_H__

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
#include <dbus/dbus-glib.h>

G_BEGIN_DECLS

#define USB_DEVICE_ID_ROCCAT_RYOS_MK 0x3138
#define USB_DEVICE_ID_ROCCAT_RYOS_MK_GLOW 0x31ce
#define USB_DEVICE_ID_ROCCAT_RYOS_MK_PRO 0x3232

#define RYOS_DEVICE_NAME "Ryos MK"
#define RYOS_DEVICE_NAME_GLOW "Ryos MK Glow"
#define RYOS_DEVICE_NAME_PRO "Ryos MK Pro"
#define RYOS_DEVICE_NAME_COMBINED "Ryos MK/Glow/Pro"

typedef struct _RyosSpecial RyosSpecial;
typedef struct _RyosReset RyosReset;
typedef struct _Ryos12 Ryos12;
typedef struct _Ryos14 Ryos14;

enum {
	RYOS_PROFILE_NUM = 5,
	RYOS_KEYS_EASYZONE_NUM = 96,
	RYOS_KEYS_MACRO_NUM = 5,
	RYOS_KEYS_THUMBSTER_NUM = 3,
	RYOS_KEYS_FUNCTION_NUM = 15,
	RYOS_KEYS_PRIMARY_NUM = 120,
	RYOS_LIGHT_BRIGHTNESS_MIN = 0,
	RYOS_LIGHT_BRIGHTNESS_MAX = 5,
	RYOS_MACRO_MACROSET_NAME_LENGTH = 24,
	RYOS_MACRO_MACRO_NAME_LENGTH = 24,
	RYOS_MACRO_KEYSTROKES_NUM = 480,
	RYOS_RKP_PROFILE_NAME_LENGTH = 50, /* utf16 */
	RYOS_RKP_PROFILE_GAMEFILE_LENGTH = 256,
	RYOS_RKP_PROFILE_GAMEFILE_NUM = 3,
	RYOS_RKP_KEYS_NUM = 0x70,
	RYOS_RKP_TIMER_NAME_LENGTH = 24,
	RYOS_RKP_QUICKLAUNCH_PATH_LENGTH = 256, /* utf16 */
	RYOS_RKP_QUICKLAUNCH_NAME_LENGTH = 24, /* utf16 */
	RYOS_RKP_TALK_NAME_LENGTH = 40, /* utf16 */
	RYOS_RKP_TALK_NUM = 9,
	RYOS_FIRMWARE_SIZE = 56844,
	RYOS_EFFECT_SCRIPT_NAME_LENGTH = 256,
};

/*
 * valid as:
 * RyosLightIlluminationMode
 * RyosStoredLightsIlluminationMode
 */
typedef enum {
	RYOS_ILLUMINATION_MODE_AUTOMATIC = 0,
	RYOS_ILLUMINATION_MODE_MANUAL = 1,
} RyosIlluminationMode;

typedef enum {
	RYOS_KEY_TYPE_DISABLED = 0x00,
	RYOS_KEY_TYPE_SYSTEM_SHUTDOWN = 0x81,
	RYOS_KEY_TYPE_SYSTEM_SLEEP = 0x82,
	RYOS_KEY_TYPE_SYSTEM_WAKE = 0x83,
	RYOS_KEY_TYPE_TALK_EASYSHIFT_ALL = 0x8c,

	/* Only work with fn and only on f11/f12 */
	RYOS_KEY_TYPE_LIGHT = 0xa3,
	RYOS_KEY_TYPE_RECORD = 0xa7,

	RYOS_KEY_TYPE_PROFILE_UP = 0xb0,
	RYOS_KEY_TYPE_PROFILE_DOWN = 0xb1,
	RYOS_KEY_TYPE_PROFILE_CYCLE = 0xb2, /* called "toggle" in RyosMKFX */
	RYOS_KEY_TYPE_PROFILE_1 = 0xb3,
	RYOS_KEY_TYPE_PROFILE_2 = 0xb4,
	RYOS_KEY_TYPE_PROFILE_3 = 0xb5,
	RYOS_KEY_TYPE_PROFILE_4 = 0xb6,
	RYOS_KEY_TYPE_PROFILE_5 = 0xb7,
	RYOS_KEY_TYPE_SHORTCUT = 0xb8,
	RYOS_KEY_TYPE_QUICKLAUNCH = 0xb9, /* called "open application" in RyosMKFX */
	RYOS_KEY_TYPE_MACRO_PLAY_PAUSE = 0xba,
	RYOS_KEY_TYPE_TIMER = 0xbc,
	RYOS_KEY_TYPE_TIMER_STOP = 0xbd,
	RYOS_KEY_TYPE_MACRO = 0xbe,
	RYOS_KEY_TYPE_LED_MACRO = 0xbf,
	RYOS_KEY_TYPE_APP_EMAIL = 0xc0,
	RYOS_KEY_TYPE_APP_CALCULATOR = 0xc1,
	RYOS_KEY_TYPE_APP_MY_COMPUTER = 0xc2,
	RYOS_KEY_TYPE_APP_BROWSER = 0xc3,
	RYOS_KEY_TYPE_WWW_SEARCH = 0xc4,
	RYOS_KEY_TYPE_WWW_HOME = 0xc5,
	RYOS_KEY_TYPE_WWW_BACK = 0xc6,
	RYOS_KEY_TYPE_WWW_FORWARD = 0xc7,
	RYOS_KEY_TYPE_WWW_STOP = 0xc8,
	RYOS_KEY_TYPE_WWW_REFRESH = 0xc9,
	RYOS_KEY_TYPE_WWW_FAVORITES = 0xca,
	RYOS_KEY_TYPE_MULTIMEDIA_OPEN_PLAYER = 0xcb, /* called "media select in RyosMKFX */
	RYOS_KEY_TYPE_MULTIMEDIA_PLAY_PAUSE = 0xcc,
	RYOS_KEY_TYPE_MULTIMEDIA_NEXT_TRACK = 0xcd,
	RYOS_KEY_TYPE_MULTIMEDIA_PREV_TRACK = 0xce,
	RYOS_KEY_TYPE_MULTIMEDIA_STOP = 0xcf,
	RYOS_KEY_TYPE_MULTIMEDIA_VOLUME_UP = 0xd0,
	RYOS_KEY_TYPE_MULTIMEDIA_VOLUME_DOWN = 0xd1,
	RYOS_KEY_TYPE_MULTIMEDIA_MUTE = 0xd2, /* called "mute master volume" in RyosMKFX */
	RYOS_KEY_TYPE_STANDARD_SHIFT_1 = 0xd3,
	RYOS_KEY_TYPE_STANDARD_SHIFT_2 = 0xd4,
	RYOS_KEY_TYPE_STANDARD_SHIFT_3 = 0xd5,
	RYOS_KEY_TYPE_STANDARD_SHIFT_4 = 0xd6,
	RYOS_KEY_TYPE_STANDARD_SHIFT_5 = 0xd7,
	RYOS_KEY_TYPE_STANDARD_CTRL_1 = 0xd8,
	RYOS_KEY_TYPE_STANDARD_CTRL_2 = 0xd9,
	RYOS_KEY_TYPE_STANDARD_CTRL_3 = 0xda,
	RYOS_KEY_TYPE_STANDARD_CTRL_4 = 0xdb,
	RYOS_KEY_TYPE_STANDARD_CTRL_5 = 0xdc,
	RYOS_KEY_TYPE_OPEN_DRIVER = 0xdd,
	RYOS_KEY_TYPE_FN = 0xf1,
	RYOS_KEY_TYPE_TALK_EASYSHIFT = 0xf5,
	RYOS_KEY_TYPE_TALK_EASYSHIFT_LOCK = 0xf6,
	RYOS_KEY_TYPE_TALK_EASYAIM_1 = 0xf7,
	RYOS_KEY_TYPE_TALK_EASYAIM_2 = 0xf8,
	RYOS_KEY_TYPE_TALK_EASYAIM_3 = 0xf9,
	RYOS_KEY_TYPE_TALK_EASYAIM_4 = 0xfa,
	RYOS_KEY_TYPE_TALK_EASYAIM_5 = 0xfb,
	RYOS_KEY_TYPE_EASYSHIFT = 0xff,
} RyosKeyType;

typedef enum {
	RYOS_KEY_MODIFIER_BIT_CTRL = 0,
	RYOS_KEY_MODIFIER_BIT_SHIFT = 1,
	RYOS_KEY_MODIFIER_BIT_ALT = 2,
	RYOS_KEY_MODIFIER_BIT_WIN = 3,
} RyosKeyModifier;

guint8 ryos_key_build_shortcut_modifier(gboolean ctrl, gboolean shift, gboolean alt, gboolean win);

struct _Ryos12 {
	guint8 report_id; /* RYOS_REPORT_ID_12 */
	guint8 size; /* always 0x0e */
	guint8 profile_index; // TODO ?
	guint8 unknown[9];
	guint16 checksum;
} __attribute__ ((packed));

struct _Ryos14 {
	guint8 unknown[20]; // all 0xff on plain
} __attribute__ ((packed));

typedef enum {
	RYOS_REPORT_ID_SPECIAL = 0x03,
	RYOS_REPORT_ID_CONTROL = 0x04, /* rw */
	RYOS_REPORT_ID_PROFILE = 0x05, /* rw */
	RYOS_REPORT_ID_KEYS_PRIMARY = 0x06, /* rw */
	RYOS_REPORT_ID_KEYS_FUNCTION = 0x07, /* rw */
	RYOS_REPORT_ID_KEYS_MACRO = 0x08, /* rw */
	RYOS_REPORT_ID_KEYS_THUMBSTER = 0x09, /* rw */
	RYOS_REPORT_ID_KEYS_EXTRA = 0x0a, /* rw */
	RYOS_REPORT_ID_KEYS_EASYZONE = 0x0b, /* rw */
	RYOS_REPORT_ID_KEY_MASK = 0x0c, /* rw */
	RYOS_REPORT_ID_LIGHT = 0x0d, /* rw */
	RYOS_REPORT_ID_MACRO = 0x0e, /* rw */
	RYOS_REPORT_ID_INFO = 0x0f, /* ro */
	RYOS_REPORT_ID_RESET = 0x11, /* wo */
	RYOS_REPORT_ID_12 = 0x12, /* rw */
	RYOS_REPORT_ID_LIGHT_CONTROL = 0x13, /* wo */
	RYOS_REPORT_ID_14 = 0x14, /* rw */
	RYOS_REPORT_ID_DEVICE_STATE = 0x15, /* wo */
	RYOS_REPORT_ID_TALK = 0x16, /* rw */
	RYOS_REPORT_ID_STORED_LIGHTS = 0x17, /* rw */
	RYOS_REPORT_ID_CUSTOM_LIGHTS = 0x18, /* wo */
	RYOS_REPORT_ID_LIGHT_MACRO = 0x19, /* rw */
	RYOS_REPORT_ID_FIRMWARE_WRITE = 0x1b, /* wo */
	RYOS_REPORT_ID_FIRMWARE_WRITE_CONTROL = 0x1c, /* ro */
	RYOS_REPORT_ID_1d = 0x1d, /* wo */
} RyosReportId;

struct _RyosSpecial {
	guint8 report_id; /* RYOS_REPORT_ID_SPECIAL */
	guint8 subtype;
	guint8 type;
	guint8 data;
	guint8 action;
} __attribute__ ((packed));

typedef enum {
	RYOS_SPECIAL_SUBTYPE_NONE = 0x00,
} RyosSpecialSubtype;
	
typedef enum {
	RYOS_SPECIAL_SUBTYPE_RAD_W = 0x01,
	RYOS_SPECIAL_SUBTYPE_RAD_A = 0x02,
	RYOS_SPECIAL_SUBTYPE_RAD_S = 0x03,
	RYOS_SPECIAL_SUBTYPE_RAD_D = 0x04,
	RYOS_SPECIAL_SUBTYPE_RAD_THUMBSTER1 = 0x05,
	RYOS_SPECIAL_SUBTYPE_RAD_THUMBSTER2 = 0x06,
	RYOS_SPECIAL_SUBTYPE_RAD_THUMBSTER3 = 0x07,
	RYOS_SPECIAL_SUBTYPE_RAD_EASYSHIFT = 0x08,
	RYOS_SPECIAL_SUBTYPE_RAD_MULTIMEDIA = 0x09,
	RYOS_SPECIAL_SUBTYPE_RAD_M1 = 0x0a,
	RYOS_SPECIAL_SUBTYPE_RAD_M2 = 0x0b,
	RYOS_SPECIAL_SUBTYPE_RAD_M3 = 0x0c,
	RYOS_SPECIAL_SUBTYPE_RAD_M4 = 0x0d,
	RYOS_SPECIAL_SUBTYPE_RAD_M5 = 0x0e,
	RYOS_SPECIAL_SUBTYPE_RAD_MACRO_SHORTCUT = 0x0f,
	RYOS_SPECIAL_SUBTYPE_RAD_TALK = 0x10,
	RYOS_SPECIAL_SUBTYPE_RAD_MACRO_LIFE_REC = 0x11,
	RYOS_SPECIAL_SUBTYPE_RAD_BACKLIGHT = 0x12,
	RYOS_SPECIAL_SUBTYPE_RAD_TOTAL = 0x13,
} RyosSpecialSubtypeRad;

typedef enum {
	RYOS_SPECIAL_ACTION_PRESS = 0,
	RYOS_SPECIAL_ACTION_RELEASE = 1,
} RyosSpecialAction;

/* applicable on
 * event->action for RYOS_SPECIAL_TYPE_RIPPLE
 * event->data for RYOS_SPECIAL_TYPE_OPEN_DRIVER
 */
typedef enum {
	RYOS_SPECIAL_FUNCTION_ACTIVATE = 1,
	RYOS_SPECIAL_FUNCTION_DEACTIVATE = 0,
} RyosSpecialFunction;

typedef enum {
	/* data = profile index
	 */
	RYOS_SPECIAL_TYPE_PROFILE_START = 0x01,
	RYOS_SPECIAL_TYPE_PROFILE = 0x02,

	/* data = key index
	 * action = TODO
	 */
	RYOS_SPECIAL_TYPE_MACRO = 0x03,

	/* data = see RyosSpecialLiveRecordingAction
	 * action = RyosSpecialLiveRecordingAction
	 */
	RYOS_SPECIAL_TYPE_LIVE_RECORDING = 0x04,

	/* data = key index
	 * action = RyosSpecialAction
	 */
	RYOS_SPECIAL_TYPE_QUICKLAUNCH = 0x07,

	/* data = 0xff
	 * action = RyosSpecialAction
	 */
	RYOS_SPECIAL_TYPE_EASYSHIFT = 0xa, // FIXME confirm

	RYOS_SPECIAL_TYPE_MULTIMEDIA = 0x0b,

	/* data = 0-5
	 */
	RYOS_SPECIAL_TYPE_BACKLIGHT = 0x0c,

	/* data = key index
	 * action = RyosSpecialAction
	 */
	RYOS_SPECIAL_TYPE_TIMER_START = 0x0d,
	RYOS_SPECIAL_TYPE_TIMER_STOP = 0x0e,

	/* data = RyosSpecialFunction
	 */
	RYOS_SPECIAL_TYPE_OPEN_DRIVER = 0x10,

	/* data = key index
	 * action = RyosSpecialAction
	 */
	RYOS_SPECIAL_TYPE_LED_MACRO = 0xbf,

	/* subtype = RyosSpecialSubtypeRad
	 * data = 0x01
	 * action = 0x00
	 */
	RYOS_SPECIAL_TYPE_RAD = 0xfa,

	/* data = RyosMKPro, RyosTKL: hid or RyosMKFX: light_index + 16
	 * action = RyosSpecialFunction
	 */
	RYOS_SPECIAL_TYPE_EFFECT = 0xfb,

	/* data = RyosStoredLightsLayers
	 */
	RYOS_SPECIAL_TYPE_LAYER = 0xfc,

	/* data = RyosSpecialFunction
	 */
	RYOS_SPECIAL_TYPE_EASYSHIFT_SELF = 0xfd,

	/* data = hid
	 * action = RyosSpecialAction
	 */
	RYOS_SPECIAL_TYPE_TALK = 0xff,
} RyosSpecialType;

typedef enum {
	RYOS_SPECIAL_LIVE_RECORDING_ACTION_START = 1, /* data = 0x00 */
	RYOS_SPECIAL_LIVE_RECORDING_ACTION_MACRO_KEY_SELECTED = 2, /* data = key_index */
	RYOS_SPECIAL_LIVE_RECORDING_ACTION_END_SUCCESS = 3, /* data = key_index (driver reads macro) (update of key settings seems to be done by device and driver individually) */
	RYOS_SPECIAL_LIVE_RECORDING_ACTION_END_ABORT = 4, /* data = 0x00 */
	RYOS_SPECIAL_LIVE_RECORDING_ACTION_INVALID_KEY = 5, /* data = 0x00 */
} RyosSpecialLiveRecordingAction;

G_END_DECLS

#endif
