#ifndef __ROCCAT_KONEPLUS_H__
#define __ROCCAT_KONEPLUS_H__

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

/*!
 * \file koneplus.h
 * \brief definitions for libroccatkoneplus
 * \author Stefan Achatz
 */

#include <gaminggear/macro.h>
#include "roccat_device_scanner.h"
#include "roccat_key_file.h"
#include "roccat_keystroke.h"
#include "roccat_button.h"
#include "roccat_light.h"
#include <glib.h>
#include <stdio.h>
#include <dbus/dbus-glib.h>

G_BEGIN_DECLS

#define USB_DEVICE_ID_ROCCAT_KONEPLUS 0x2d51

typedef struct _KoneplusActualProfile KoneplusActualProfile;
typedef struct _KoneplusA KoneplusA;
typedef struct _KoneplusTalk KoneplusTalk;
typedef struct _KoneplusControl KoneplusControl;
typedef struct _KoneplusDeviceState KoneplusDeviceState;
typedef struct _KoneplusInfo KoneplusInfo;
typedef struct _KoneplusSensor KoneplusSensor;
typedef struct _KoneplusSensorImage KoneplusSensorImage;
typedef struct _KoneplusControlUnit KoneplusControlUnit;
typedef struct _KoneplusProfileSettings KoneplusProfileSettings;
typedef struct _KoneplusProfileButtons KoneplusProfileButtons;
typedef struct _KoneplusMacro KoneplusMacro;
typedef struct _KoneplusSpecial KoneplusSpecial;
typedef struct _KoneplusRmp KoneplusRmp;
typedef struct _KoneplusRmpMacroKeyInfoV1 KoneplusRmpMacroKeyInfoV1;
typedef struct _KoneplusRmpMacroKeyInfo KoneplusRmpMacroKeyInfo;
typedef struct _KoneplusRmpLightInfo KoneplusRmpLightInfo;

enum {
	KONEPLUS_PROFILE_NUM = 5,
	KONEPLUS_PROFILE_SETTINGS_CPI_INACTIVE = 0,
	KONEPLUS_PROFILE_SETTINGS_CPI_ACTIVE = 1,
	KONEPLUS_PROFILE_SETTINGS_LIGHT_EFFECT_SPEED_MIN = 1,
	KONEPLUS_PROFILE_SETTINGS_LIGHT_EFFECT_SPEED_MAX = 3,
	KONEPLUS_TCU_IMAGE_SIZE = 30,
	KONEPLUS_PROFILE_BUTTON_NUM = 24,
	KONEPLUS_MACRO_MACROSET_NAME_LENGTH = 24,
	KONEPLUS_MACRO_MACRO_NAME_LENGTH = 24,
	KONEPLUS_MACRO_KEYSTROKES_NUM = 500,
	KONEPLUS_PROFILE_SETTING_LIGHTS_NUM = 4,
	KONEPLUS_PROFILE_SETTING_CPI_LEVELS_NUM = 5,
	KONEPLUS_PROFILE_SETTING_CPI_MIN = 1,
	KONEPLUS_PROFILE_SETTING_CPI_MAX = 60,
	KONEPLUS_RMP_MACRO_KEY_INFO_MACROSET_NAME_LENGTH = 50,
	KONEPLUS_RMP_MACRO_KEY_INFO_MACRO_NAME_LENGTH = 50,
	KONEPLUS_RMP_MACRO_KEY_INFO_KEYSTROKES_NUM = 500,
	KONEPLUS_RMP_MACRO_KEY_INFO_TIMER_NAME_LENGTH = 100,
	KONEPLUS_RMP_MACRO_KEY_INFO_FILENAME_LENGTH = 260,
	KONEPLUS_RMP_LIGHT_INFO_COLORS_NUM = 33,
	KONEPLUS_LIGHT_INFO_INDEX_CUSTOM = 0xff,
	KONEPLUS_FIRMWARE_SIZE = 49140,
	KONEPLUS_GAMEFILE_NUM = 3,
};

struct _KoneplusActualProfile {
	guint8 report_id; /* KONEPLUS_REPORT_ID_PROFILE */
	guint8 size; /* 0x3 */
	guint8 profile_index; /* range 0-4 */
} __attribute__ ((packed));

guint koneplus_actual_profile_read(RoccatDevice *device, GError **error);
gboolean koneplus_actual_profile_write(RoccatDevice *device, guint profile_index, GError **error);

struct _KoneplusA {
	guint8 report_id; /* KONEPLUS_REPORT_ID_A */
	guint8 size; /* always 8 */
	guint8 unknown[6];
} __attribute__ ((packed));

struct _KoneplusTalk {
	guint8 report_id; /* KONEPLUS_REPORT_ID_TALK */
	guint8 size; /* always 16 */
	guint8 easyshift;
	guint8 easyshift_lock;
	guint8 easyaim;
	guint8 fx_status;
	guint8 zone;
	guint8 unused;
	guint8 effect;
	guint8 speed;
	guint8 ambient_red;
	guint8 ambient_green;
	guint8 ambient_blue;
	guint8 event_red;
	guint8 event_green;
	guint8 event_blue;
} __attribute__ ((packed));

/* also valid as KoneplusTalkEasyshiftLock */
typedef enum {
	KONEPLUS_TALK_EASYSHIFT_OFF = 0,
	KONEPLUS_TALK_EASYSHIFT_ON = 1,
	KONEPLUS_TALK_EASYSHIFT_UNUSED = 0xff,
} KoneplusTalkEasyshift;

typedef enum {
	KONEPLUS_TALK_EASYAIM_OFF = 0,
	KONEPLUS_TALK_EASYAIM_1 = 1,
	KONEPLUS_TALK_EASYAIM_2 = 2,
	KONEPLUS_TALK_EASYAIM_3 = 3,
	KONEPLUS_TALK_EASYAIM_4 = 4,
	KONEPLUS_TALK_EASYAIM_5 = 5,
	KONEPLUS_TALK_EASYAIM_UNUSED = 0xff,
} KoneplusTalkEasyaim;

typedef enum {
	KONEPLUS_TALKFX_ZONE_EVENT = 4,
	KONEPLUS_TALKFX_ZONE_AMBIENT = 3,
} KoneplusTalkfxZone;

typedef enum {
	KONEPLUS_TALKFX_STATE_UNUSED = 0xff,
} KoneplusTalkfxState;

gboolean koneplus_talk_easyshift(RoccatDevice *koneplus, guint8 state, GError **error);
gboolean koneplus_talk_easyshift_lock(RoccatDevice *koneplus, guint8 state, GError **error);
gboolean koneplus_talk_easyaim(RoccatDevice *koneplus, guint8 state, GError **error);

gboolean koneplus_talkfx_write(RoccatDevice *koneplus, KoneplusTalk *talk, GError **error);
KoneplusTalk *koneplus_talkfx_read(RoccatDevice *koneplus, GError **error);

/* color uses 0x00rrggbb scheme */
gboolean koneplus_talkfx(RoccatDevice *koneplus, guint32 effect, guint32 ambient_color, guint32 event_color, GError **error);
gboolean koneplus_talkfx_off(RoccatDevice *koneplus, GError **error);

struct _KoneplusControl {
	guint8 report_id; /* KONEPLUS_REPORT_ID_CONTROL */
	guint8 value;
	guint8 request;
} __attribute__ ((packed));

/* also valid as KoneplusFirmwareWriteControl */
typedef enum  {
	KONEPLUS_CONTROL_REQUEST_STATUS_CRITICAL = 0,
	KONEPLUS_CONTROL_REQUEST_STATUS_OK = 1,
	KONEPLUS_CONTROL_REQUEST_STATUS_INVALID = 2,
	KONEPLUS_CONTROL_REQUEST_STATUS_BUSY = 3,
} KoneplusControlValue;

typedef enum {
	KONEPLUS_CONTROL_REQUEST_STATUS = 0x00,
	KONEPLUS_CONTROL_REQUEST_PROFILE_SETTINGS = 0x80,
	KONEPLUS_CONTROL_REQUEST_PROFILE_BUTTONS = 0x90,
} KoneplusControlRequest;

struct _KoneplusDeviceState {
	guint8 report_id; /* KONEPLUS_REPORT_ID_DEVICE_STATE */
	guint8 size; /* always 3 */
	guint8 state;
} __attribute__ ((packed));

typedef enum {
	/* Device sends HID multimedia events */
	KONEPLUS_DEVICE_STATE_STATE_OFF = 0x00,
	/* Driver handles multimedia events */
	KONEPLUS_DEVICE_STATE_STATE_ON = 0x01,
} KoneplusDeviceStateState;

gboolean koneplus_device_state_write(RoccatDevice *device, guint state, GError **error);

struct _KoneplusInfo {
	guint8 report_id; /* KONEPLUS_REPORT_ID_INFO */
	guint8 size; /* always 6 */
	guint8 firmware_version;
	guint8 dfu_version;
	guint8 unknown[2];
} __attribute__ ((packed));

typedef enum {
	KONEPLUS_INFO_FUNCTION_RESET = 0x0b,
} KoneplusInfoFunction;

struct _KoneplusSensor {
	guint8 report_id; /* KONEPLUS_REPORT_ID_SENSOR */
	guint8 action;
	guint8 reg;
	guint8 value;
} __attribute__ ((packed));

typedef enum {
	KONEPLUS_SENSOR_ACTION_WRITE = 1,
	KONEPLUS_SENSOR_ACTION_READ = 2,
	KONEPLUS_SENSOR_ACTION_FRAME_CAPTURE = 3,
} KoneplusSensorAction;

struct _KoneplusSensorImage {
	guint8 report_id; /* KONEPLUS_REPORT_ID_SENSOR */
	guint8 action; /* 3 */
	guint8 reg; /* 0 */
	guint8 data[KONEPLUS_TCU_IMAGE_SIZE * KONEPLUS_TCU_IMAGE_SIZE];
	guint8 filler[123]; /* all bytes 0x11 */
	guint16 checksum;
} __attribute__ ((packed));

struct _KoneplusControlUnit {
	guint8 report_id; /* KONEPLUS_REPORT_ID_CONTROL_UNIT */
	guint8 size; /* always 6 */
	guint8 dcu;
	guint8 tcu;
	guint8 median;
	guint8 action;
} __attribute__ ((packed));

typedef enum {
	KONEPLUS_CONTROL_UNIT_ACTION_CANCEL = 0x00,
	KONEPLUS_CONTROL_UNIT_ACTION_ACCEPT = 0x01,
	KONEPLUS_CONTROL_UNIT_ACTION_OFF = 0x02,
	KONEPLUS_CONTROL_UNIT_ACTION_UNDEFINED = 0xff,
} KoneplusControlUnitAction;

typedef enum {
	KONEPLUS_TRACKING_CONTROL_UNIT_OFF = 0,
	KONEPLUS_TRACKING_CONTROL_UNIT_ON = 1,
} KoneplusControlUnitTcu;

typedef enum {
	KONEPLUS_DISTANCE_CONTROL_UNIT_OFF = 0,
	KONEPLUS_DISTANCE_CONTROL_UNIT_EXTRA_LOW = 1,
	KONEPLUS_DISTANCE_CONTROL_UNIT_LOW = 2,
	KONEPLUS_DISTANCE_CONTROL_UNIT_NORMAL = 3,
} KoneplusControlUnitDcu;

struct _KoneplusProfileSettings {
	guint8 report_id; /* KONEPLUS_REPORT_ID_PROFILE_SETTINGS */
	guint8 size; /* always 43 */
	guint8 profile_index; /* range 0-4 */
	guint8 xy_sync; /* RoccatSensitivityAdvanced */
	guint8 sensitivity_x; /* RoccatSensitivity */
	guint8 sensitivity_y; /* RoccatSensitivity */
	guint8 cpi_levels_enabled; /* bits 1-5 */

	 /* range 1-60 means 100-6000 cpi */
	guint8 cpi_levels_x[KONEPLUS_PROFILE_SETTING_CPI_LEVELS_NUM];
	guint8 cpi_startup_level; /* range 0-4 */

	/*
	 * These values can be set differently to cpi_levels_x and are reported
	 * by cpi change events, but are not used.
	 */
	guint8 cpi_levels_y[KONEPLUS_PROFILE_SETTING_CPI_LEVELS_NUM];
	guint8 talkfx; /* only valid for KoneXTD */
	guint8 polling_rate; /* RoccatPollingRate */
	guint8 lights_enabled; /* bits 1-4 */
	guint8 light_effect_mode; /* does nothing */
	guint8 color_flow_effect;
	guint8 light_effect_type;
	guint8 light_effect_speed; /* range 1-3 */
	RoccatLight lights[KONEPLUS_PROFILE_SETTING_LIGHTS_NUM];
	guint16 checksum;
} __attribute__ ((packed));

typedef enum {
	KONEPLUS_PROFILE_SETTINGS_TALKFX_OFF = 0,
	KONEPLUS_PROFILE_SETTINGS_TALKFX_ON = 1,
} KoneplusProfileSettingsTalkFx;

/*
 * also valid as
 * KoneplusRmpLightInfoIndex;
 */
typedef enum {
	KONEPLUS_PROFILE_SETTINGS_LIGHT_INDEX_TOP_LEFT = 0,
	KONEPLUS_PROFILE_SETTINGS_LIGHT_INDEX_BOTTOM_LEFT = 1,
	KONEPLUS_PROFILE_SETTINGS_LIGHT_INDEX_TOP_RIGHT = 2,
	KONEPLUS_PROFILE_SETTINGS_LIGHT_INDEX_BOTTOM_RIGHT = 3,
} KoneplusProfileSettingsLightIndex;

typedef enum {
	KONEPLUS_PROFILE_SETTINGS_COLOR_FLOW_EFFECT_OFF = 0,
	KONEPLUS_PROFILE_SETTINGS_COLOR_FLOW_EFFECT_SIMULTANEOUSLY = 1,
	KONEPLUS_PROFILE_SETTINGS_COLOR_FLOW_EFFECT_UP = 2,
	KONEPLUS_PROFILE_SETTINGS_COLOR_FLOW_EFFECT_DOWN = 3,
	KONEPLUS_PROFILE_SETTINGS_COLOR_FLOW_EFFECT_LEFT = 4,
	KONEPLUS_PROFILE_SETTINGS_COLOR_FLOW_EFFECT_RIGHT = 5,
} KoneplusProfileSettingsColorFlowEffect;

typedef enum {
	KONEPLUS_PROFILE_SETTINGS_LIGHT_EFFECT_TYPE_ALL_OFF = 0,
	KONEPLUS_PROFILE_SETTINGS_LIGHT_EFFECT_TYPE_FULLY_LIGHTED = 1,
	KONEPLUS_PROFILE_SETTINGS_LIGHT_EFFECT_TYPE_BLINKING = 2,
	KONEPLUS_PROFILE_SETTINGS_LIGHT_EFFECT_TYPE_BREATHING = 3,
	KONEPLUS_PROFILE_SETTINGS_LIGHT_EFFECT_TYPE_ALL_HEARTBEAT = 4,
} KoneplusProfileSettingsLightEffectType;

/*
 * also valid as
 * KoneplusRmpMacroKeyInfoType
 */
typedef enum {
	KONEPLUS_PROFILE_BUTTON_TYPE_UNUSED = 0x0, /* button is disabled by system */
	KONEPLUS_PROFILE_BUTTON_TYPE_CLICK = 0x1,
	KONEPLUS_PROFILE_BUTTON_TYPE_MENU = 0x2,
	KONEPLUS_PROFILE_BUTTON_TYPE_UNIVERSAL_SCROLLING = 0x3,
	KONEPLUS_PROFILE_BUTTON_TYPE_DOUBLE_CLICK = 0x4,
	KONEPLUS_PROFILE_BUTTON_TYPE_SHORTCUT = 0x5,
	KONEPLUS_PROFILE_BUTTON_TYPE_DISABLED = 0x6, /* button is disabled by user */
	KONEPLUS_PROFILE_BUTTON_TYPE_BROWSER_FORWARD = 0x7,
	KONEPLUS_PROFILE_BUTTON_TYPE_BROWSER_BACKWARD = 0x8,
	KONEPLUS_PROFILE_BUTTON_TYPE_TILT_LEFT = 0x9,
	KONEPLUS_PROFILE_BUTTON_TYPE_TILT_RIGHT = 0xa,
	KONEPLUS_PROFILE_BUTTON_TYPE_SCROLL_UP = 0xd,
	KONEPLUS_PROFILE_BUTTON_TYPE_SCROLL_DOWN = 0xe,
	KONEPLUS_PROFILE_BUTTON_TYPE_QUICKLAUNCH = 0xf, /* info what to start is in rmp */
	KONEPLUS_PROFILE_BUTTON_TYPE_PROFILE_CYCLE = 0x10,
	KONEPLUS_PROFILE_BUTTON_TYPE_PROFILE_UP = 0x11,
	KONEPLUS_PROFILE_BUTTON_TYPE_PROFILE_DOWN = 0x12,
	KONEPLUS_PROFILE_BUTTON_TYPE_CPI_CYCLE = 0x14,
	KONEPLUS_PROFILE_BUTTON_TYPE_CPI_UP = 0x15,
	KONEPLUS_PROFILE_BUTTON_TYPE_CPI_DOWN = 0x16,
	KONEPLUS_PROFILE_BUTTON_TYPE_SENSITIVITY_CYCLE = 0x17,
	KONEPLUS_PROFILE_BUTTON_TYPE_SENSITIVITY_UP = 0x18,
	KONEPLUS_PROFILE_BUTTON_TYPE_SENSITIVITY_DOWN = 0x19,
	KONEPLUS_PROFILE_BUTTON_TYPE_WINDOWS_KEY = 0x1a,
	KONEPLUS_PROFILE_BUTTON_TYPE_OPEN_DRIVER = 0x1b,
	KONEPLUS_PROFILE_BUTTON_TYPE_MULTIMEDIA_OPEN_PLAYER = 0x20,
	KONEPLUS_PROFILE_BUTTON_TYPE_MULTIMEDIA_PREV_TRACK = 0x21,
	KONEPLUS_PROFILE_BUTTON_TYPE_MULTIMEDIA_NEXT_TRACK = 0x22,
	KONEPLUS_PROFILE_BUTTON_TYPE_MULTIMEDIA_PLAY_PAUSE = 0x23,
	KONEPLUS_PROFILE_BUTTON_TYPE_MULTIMEDIA_STOP = 0x24,
	KONEPLUS_PROFILE_BUTTON_TYPE_MULTIMEDIA_MUTE = 0x25,
	KONEPLUS_PROFILE_BUTTON_TYPE_MULTIMEDIA_VOLUME_DOWN = 0x26,
	KONEPLUS_PROFILE_BUTTON_TYPE_MULTIMEDIA_VOLUME_UP = 0x27,
	KONEPLUS_PROFILE_BUTTON_TYPE_MACRO = 0x30,
	KONEPLUS_PROFILE_BUTTON_TYPE_TIMER = 0x31, /* timer information is in rmp */
	KONEPLUS_PROFILE_BUTTON_TYPE_TIMER_STOP = 0x32,
	KONEPLUS_PROFILE_BUTTON_TYPE_EASYAIM_1 = 0x33,
	KONEPLUS_PROFILE_BUTTON_TYPE_EASYAIM_2 = 0x34,
	KONEPLUS_PROFILE_BUTTON_TYPE_EASYAIM_3 = 0x35,
	KONEPLUS_PROFILE_BUTTON_TYPE_EASYAIM_4 = 0x36,
	KONEPLUS_PROFILE_BUTTON_TYPE_EASYAIM_5 = 0x37,
	/* TODO clarify if easywheel and easyshift can only be applied to buttons 4+5 */
	KONEPLUS_PROFILE_BUTTON_TYPE_EASYSHIFT = 0x41, /* only device */
	KONEPLUS_PROFILE_BUTTON_TYPE_EASYWHEEL_SENSITIVITY = 0x42,
	KONEPLUS_PROFILE_BUTTON_TYPE_EASYWHEEL_PROFILE = 0x43,
	KONEPLUS_PROFILE_BUTTON_TYPE_EASYWHEEL_CPI = 0x44,
	KONEPLUS_PROFILE_BUTTON_TYPE_EASYWHEEL_VOLUME = 0x45,
	KONEPLUS_PROFILE_BUTTON_TYPE_EASYWHEEL_ALT_TAB = 0x46,
	KONEPLUS_PROFILE_BUTTON_TYPE_EASYWHEEL_AERO_FLIP_3D = 0x47,
	KONEPLUS_PROFILE_BUTTON_TYPE_TALK_OTHER_EASYSHIFT = 0x51, /* only other device */
	KONEPLUS_PROFILE_BUTTON_TYPE_TALK_OTHER_EASYSHIFT_LOCK = 0x52,
	KONEPLUS_PROFILE_BUTTON_TYPE_TALK_BOTH_EASYSHIFT = 0x53, /* device and other device */
} KoneplusProfileButtonType;

struct _KoneplusProfileButtons {
	guint8 report_id; /* KONEPLUS_REPORT_ID_PROFILE_BUTTONS */
	guint8 size; /* always 77 */
	guint8 profile_index; /* range 0-4 */
	RoccatButton buttons[KONEPLUS_PROFILE_BUTTON_NUM];
	guint16 checksum;
} __attribute__ ((packed));

/*
 * also valid as
 * KoneplusMacroButtonNumber
 * KoneplusRmpMacroKeyIndex
 * KoneplusRmpMacroKeyInfoButtonNumber
 */
typedef enum {
	KONEPLUS_PROFILE_BUTTONS_BUTTON_INDEX_LEFT = 0,
	KONEPLUS_PROFILE_BUTTONS_BUTTON_INDEX_RIGHT = 1,
	KONEPLUS_PROFILE_BUTTONS_BUTTON_INDEX_MIDDLE = 2,
	KONEPLUS_PROFILE_BUTTONS_BUTTON_INDEX_FORWARD = 3,
	KONEPLUS_PROFILE_BUTTONS_BUTTON_INDEX_BACKWARD = 4,
	KONEPLUS_PROFILE_BUTTONS_BUTTON_INDEX_TILT_LEFT = 5,
	KONEPLUS_PROFILE_BUTTONS_BUTTON_INDEX_TILT_RIGHT = 6,
	KONEPLUS_PROFILE_BUTTONS_BUTTON_INDEX_WHEEL_UP = 7,
	KONEPLUS_PROFILE_BUTTONS_BUTTON_INDEX_WHEEL_DOWN = 8,
	KONEPLUS_PROFILE_BUTTONS_BUTTON_INDEX_PLUS = 9,
	KONEPLUS_PROFILE_BUTTONS_BUTTON_INDEX_MINUS = 10,
	KONEPLUS_PROFILE_BUTTONS_BUTTON_INDEX_TOP = 11,
	KONEPLUS_PROFILE_BUTTONS_BUTTON_INDEX_SHIFT_LEFT = 12,
	KONEPLUS_PROFILE_BUTTONS_BUTTON_INDEX_SHIFT_RIGHT = 13,
	KONEPLUS_PROFILE_BUTTONS_BUTTON_INDEX_SHIFT_MIDDLE = 14,
	KONEPLUS_PROFILE_BUTTONS_BUTTON_INDEX_SHIFT_FORWARD = 15,
	KONEPLUS_PROFILE_BUTTONS_BUTTON_INDEX_SHIFT_BACKWARD = 16,
	KONEPLUS_PROFILE_BUTTONS_BUTTON_INDEX_SHIFT_TILT_LEFT = 17,
	KONEPLUS_PROFILE_BUTTONS_BUTTON_INDEX_SHIFT_TILT_RIGHT = 18,
	KONEPLUS_PROFILE_BUTTONS_BUTTON_INDEX_SHIFT_WHEEL_UP = 19,
	KONEPLUS_PROFILE_BUTTONS_BUTTON_INDEX_SHIFT_WHEEL_DOWN = 20,
	KONEPLUS_PROFILE_BUTTONS_BUTTON_INDEX_SHIFT_PLUS = 21,
	KONEPLUS_PROFILE_BUTTONS_BUTTON_INDEX_SHIFT_MINUS = 22,
	KONEPLUS_PROFILE_BUTTONS_BUTTON_INDEX_SHIFT_TOP = 23,
} KoneplusProfileButtonsButtonIndex;

struct _KoneplusMacro {
	guint8 report_id; /* KONEPLUS_REPORT_ID_MACRO */
	guint16 size; /* always 2082 */
	guint8 profile_number; /* range 0-4 */
	guint8 button_number; /* range 0-23 */
	guint8 loop;
	guint8 unknown[24];
	guint8 macroset_name[KONEPLUS_MACRO_MACROSET_NAME_LENGTH];
	guint8 macro_name[KONEPLUS_MACRO_MACRO_NAME_LENGTH];
	guint16 count;
	RoccatKeystroke keystrokes[KONEPLUS_MACRO_KEYSTROKES_NUM];
	guint16 checksum;
} __attribute__ ((packed));

typedef enum {
	KONEPLUS_REPORT_ID_MOUSE = 0x01,
	KONEPLUS_REPORT_ID_SPECIAL = 0x03,
	KONEPLUS_REPORT_ID_CONTROL = 0x04,
	KONEPLUS_REPORT_ID_ACTUAL_PROFILE = 0x05,
	KONEPLUS_REPORT_ID_PROFILE_SETTINGS = 0x06,
	KONEPLUS_REPORT_ID_PROFILE_BUTTONS = 0x07,
	KONEPLUS_REPORT_ID_MACRO = 0x08,
	KONEPLUS_REPORT_ID_INFO = 0x09,
	KONEPLUS_REPORT_ID_A = 0x0a,
	KONEPLUS_REPORT_ID_SENSOR = 0x0c,
	KONEPLUS_REPORT_ID_DEVICE_STATE = 0x0e,
	KONEPLUS_REPORT_ID_CONTROL_UNIT = 0x0f,
	KONEPLUS_REPORT_ID_TALK = 0x10,
	KONEPLUS_REPORT_ID_FIRMWARE_WRITE = 0x1b,
	KONEPLUS_REPORT_ID_FIRMWARE_WRITE_CONTROL = 0x1c,
} KoneplusReportId;

struct _KoneplusSpecial {
	guint8 report_id; /* KONEPLUS_REPORT_ID_SPECIAL */
	guint8 zero1;
	guint8 type;
	guint8 data1;
	guint8 data2;
	/* Kone[+], KoneXTD, KonePure, KonePureOptical deliver 8 bytes but last 3
	 * bytes are just remnants of last mouse event.
	 * KonePureMilitary and KoneXTDOptical only deliver 5 bytes. */
	// guint8 rubbish[3];
} __attribute__ ((packed));

typedef enum {
	KONEPLUS_INTERFACE_MOUSE = 0,
	KONEPLUS_INTERFACE_KEYBOARD = 1,
} KoneplusInterface;

typedef enum {
	KONEPLUS_SPECIAL_TYPE_TILT = 0x10,
	KONEPLUS_SPECIAL_TYPE_SCROLL = 0x1b,
	KONEPLUS_SPECIAL_TYPE_PROFILE = 0x20,
	KONEPLUS_SPECIAL_TYPE_QUICKLAUNCH = 0x60,
	KONEPLUS_SPECIAL_TYPE_TIMER = 0x80,
	KONEPLUS_SPECIAL_TYPE_TIMER_STOP = 0x90,
	KONEPLUS_SPECIAL_TYPE_OPEN_DRIVER = 0xa0,
	KONEPLUS_SPECIAL_TYPE_CPI = 0xb0,
	KONEPLUS_SPECIAL_TYPE_SENSITIVITY = 0xc0,
	KONEPLUS_SPECIAL_TYPE_MULTIMEDIA = 0xf0,

	/* data1 = KoneplusProfileButtonsButtonIndex + 1
	 * data2 = KoneplusSpecialAction
	 */
	KONEPLUS_SPECIAL_TYPE_TALK = 0xff,
} KoneplusSpecialType;

typedef enum  {
	KONEPLUS_SPECIAL_ACTION_PRESS = 0,
	KONEPLUS_SPECIAL_ACTION_RELEASE = 1,
} KoneplusSpecialAction;

/* old versions use a slightly smaller version */
struct _KoneplusRmpMacroKeyInfoV1 {
	guint8 button_number;
	guint8 type;
	guint16 unused1;
	guint16 talk_device;
	guint8 unused2[46];
	guint8 macroset_name[KONEPLUS_RMP_MACRO_KEY_INFO_MACROSET_NAME_LENGTH];
	guint8 macro_name[KONEPLUS_RMP_MACRO_KEY_INFO_MACRO_NAME_LENGTH];
	guint32 loop;
	guint16 count;
	RoccatKeystroke keystrokes[KONEPLUS_RMP_MACRO_KEY_INFO_KEYSTROKES_NUM];
	guint16 unused4;
	guint32 timer_length;
	guint8 timer_name[KONEPLUS_RMP_MACRO_KEY_INFO_TIMER_NAME_LENGTH];
	guint8 filename[KONEPLUS_RMP_MACRO_KEY_INFO_FILENAME_LENGTH];
	guint8 checksum;
} __attribute__ ((packed));

struct _KoneplusRmpMacroKeyInfo {
	guint8 button_number;
	guint8 type;
	guint16 unused1;
	guint16 talk_device;
	guint8 unused2[52];
	guint8 macroset_name[KONEPLUS_RMP_MACRO_KEY_INFO_MACROSET_NAME_LENGTH];
	guint8 macro_name[KONEPLUS_RMP_MACRO_KEY_INFO_MACRO_NAME_LENGTH];
	guint16 unused3;
	guint32 loop;
	guint16 count;
	RoccatKeystroke keystrokes[KONEPLUS_RMP_MACRO_KEY_INFO_KEYSTROKES_NUM];
	guint16 unused4;
	guint32 timer_length;
	guint8 timer_name[KONEPLUS_RMP_MACRO_KEY_INFO_TIMER_NAME_LENGTH];
	guint8 filename[KONEPLUS_RMP_MACRO_KEY_INFO_FILENAME_LENGTH];
	guint8 checksum;
} __attribute__ ((packed));

struct _KoneplusRmpLightInfo {
	guint8 index; /* means a color index of 33 colors */
	guint8 state;
	guint8 red;
	guint8 green;
	guint8 blue;
	guint8 checksum;
} __attribute__ ((packed));

typedef enum {
	KONEPLUS_RMP_LIGHT_INFO_STATE_ON = 1,
	KONEPLUS_RMP_LIGHT_INFO_STATE_OFF = 2,
} KoneplusRmpLightInfoState;

struct _KoneplusTimer {
	guint8 length; /* in sec */
	guint8 unknown1[3];
	guint8 name[24];
	guint8 unknown2[76];
	guint8 checksum;
} __attribute__ ((packed));

static inline guint16 koneplus_macro_get_size(KoneplusMacro const *macro) {
	return GUINT16_FROM_LE(macro->size);
}

static inline void koneplus_macro_set_size(KoneplusMacro *macro, guint16 new_value) {
	macro->size = GUINT16_TO_LE(new_value);
}

static inline guint16 koneplus_macro_get_count(KoneplusMacro const *macro) {
	return GUINT16_FROM_LE(macro->count);
}

static inline void koneplus_macro_set_count(KoneplusMacro *macro, guint16 new_value) {
	macro->count = GUINT16_TO_LE(new_value);
}

static inline guint16 koneplus_rmp_macro_key_info_get_talk_device(KoneplusRmpMacroKeyInfo const *info) {
	return GUINT16_FROM_LE(info->talk_device);
}

static inline void koneplus_rmp_macro_key_info_set_talk_device(KoneplusRmpMacroKeyInfo *info, guint16 new_value) {
	info->talk_device = GUINT16_TO_LE(new_value);
}

static inline guint32 koneplus_rmp_macro_key_info_get_loop(KoneplusRmpMacroKeyInfo const *info) {
	return GUINT32_FROM_LE(info->loop);
}

static inline void koneplus_rmp_macro_key_info_set_loop(KoneplusRmpMacroKeyInfo *info, guint32 new_value) {
	info->loop = GUINT32_TO_LE(new_value);
}

static inline guint16 koneplus_rmp_macro_key_info_get_count(KoneplusRmpMacroKeyInfo const *info) {
	return GUINT16_FROM_LE(info->count);
}

static inline void koneplus_rmp_macro_key_info_set_count(KoneplusRmpMacroKeyInfo *info, guint16 new_value) {
	info->count = GUINT16_TO_LE(new_value);
}

static inline guint32 koneplus_rmp_macro_key_info_get_timer_length(KoneplusRmpMacroKeyInfo const *info) {
	return GUINT32_FROM_LE(info->timer_length);
}

static inline void koneplus_rmp_macro_key_info_set_timer_length(KoneplusRmpMacroKeyInfo *info, guint32 new_value) {
	info->timer_length = GUINT32_TO_LE(new_value);
}

RoccatDevice *koneplus_device_first(void);
RoccatDeviceScanner *koneplus_device_scanner_new(void);
gchar *koneplus_device_read(RoccatDevice *roccat_device, guint report_id, gssize length, GError **error);
gboolean koneplus_device_write(RoccatDevice *roccat_device, gchar const *buffer, gssize length, GError **error);

KoneplusInfo *koneplus_info_read(RoccatDevice *koneplus, GError **error);
gboolean koneplus_reset(RoccatDevice *koneplus, GError **error);
guint koneplus_firmware_version_read(RoccatDevice *koneplus, GError **error);


KoneplusControlUnit *koneplus_control_unit_read(RoccatDevice *koneplus, GError **error);
gboolean koneplus_tracking_control_unit_test(RoccatDevice *koneplus, guint dcu, guint median, GError **error);
gboolean koneplus_tracking_control_unit_cancel(RoccatDevice *koneplus, guint dcu, GError **error);
gboolean koneplus_tracking_control_unit_accept(RoccatDevice *koneplus, guint dcu, guint median, GError **error);
gboolean koneplus_tracking_control_unit_off(RoccatDevice *koneplus, guint dcu, GError **error);
guint koneplus_distance_control_unit_get(RoccatDevice *koneplus, GError **error);
gboolean koneplus_distance_control_unit_try(RoccatDevice *koneplus, guint new_dcu, GError **error);
gboolean koneplus_distance_control_unit_cancel(RoccatDevice *koneplus, guint old_dcu, GError **error);
gboolean koneplus_distance_control_unit_accept(RoccatDevice *koneplus, guint new_dcu, GError **error);

KoneplusRmp *koneplus_rmp_read_with_path(gchar const *path, GKeyFile *default_key_file, GError **error);
gboolean koneplus_rmp_write_with_path(gchar const *path, KoneplusRmp *rmp, GError **error);
KoneplusRmp *koneplus_default_rmp(void);
GKeyFile *koneplus_rmp_defaults(void);
void koneplus_rmp_free(KoneplusRmp *rmp);
KoneplusRmp *koneplus_rmp_dup(KoneplusRmp const *src);

gboolean koneplus_sensor_calibrate_step(RoccatDevice *koneplus, GError **error);
KoneplusSensorImage *koneplus_sensor_image_read(RoccatDevice *koneplus, GError **error);
guint koneplus_sensor_image_get_median(KoneplusSensorImage const *image);
gboolean koneplus_sensor_write_register(RoccatDevice *koneplus, guint8 reg, guint8 value, GError **error);
guint8 koneplus_sensor_read_register(RoccatDevice *koneplus, guint8 reg, GError **error);

RoccatKeyFile *koneplus_configuration_load(void);
void koneplus_configuration_free(RoccatKeyFile *config);
gboolean koneplus_configuration_save(RoccatKeyFile *config, GError **error);

gchar *koneplus_configuration_get_rmp_save_path(RoccatKeyFile *config);
void koneplus_configuration_set_rmp_save_path(RoccatKeyFile *config, gchar const *path);

gint koneplus_configuration_get_timer_notification_type(RoccatKeyFile *config);
void koneplus_configuration_set_timer_notification_type(RoccatKeyFile *config, gint new_value);
gint koneplus_configuration_get_profile_notification_type(RoccatKeyFile *config);
void koneplus_configuration_set_profile_notification_type(RoccatKeyFile *config, gint new_value);
gint koneplus_configuration_get_cpi_notification_type(RoccatKeyFile *config);
void koneplus_configuration_set_cpi_notification_type(RoccatKeyFile *config, gint new_value);
gint koneplus_configuration_get_sensitivity_notification_type(RoccatKeyFile *config);
void koneplus_configuration_set_sensitivity_notification_type(RoccatKeyFile *config, gint new_value);
gdouble koneplus_configuration_get_notification_volume(RoccatKeyFile *config);
void koneplus_configuration_set_notification_volume(RoccatKeyFile *config, gdouble new_value);
gint koneplus_configuration_get_default_profile_number(RoccatKeyFile *config);
void koneplus_configuration_set_default_profile_number(RoccatKeyFile *config, gint new_value);

gboolean koneplus_select(RoccatDevice *device, guint profile_index, guint request, GError **error);

gboolean koneplus_profile_buttons_write(RoccatDevice *koneplus, guint profile_index, KoneplusProfileButtons *profile_buttons, GError **error);
KoneplusProfileButtons *koneplus_profile_buttons_read(RoccatDevice *koneplus, guint profile_index, GError **error);
gboolean koneplus_profile_buttons_equal(KoneplusProfileButtons const *left, KoneplusProfileButtons const *right);

void koneplus_profile_settings_finalize(KoneplusProfileSettings *profile_settings, guint profile_index);
gboolean koneplus_profile_settings_write(RoccatDevice *koneplus, guint profile_index, KoneplusProfileSettings *profile_settings, GError **error);
KoneplusProfileSettings *koneplus_profile_settings_read(RoccatDevice *koneplus, guint profile_index, GError **error);

/* returns NULL if no macro should be sent - thats no error */
KoneplusMacro *koneplus_rmp_macro_key_info_to_macro(KoneplusRmpMacroKeyInfo const *macro_key_info);
KoneplusRmpMacroKeyInfo *koneplus_macro_to_rmp_macro_key_info(KoneplusMacro const *macro);

gboolean koneplus_macro_write(RoccatDevice *koneplus, guint profile_index, guint key_index, KoneplusMacro *macro, GError **error);
KoneplusMacro *koneplus_macro_read(RoccatDevice *koneplus, guint profile_index, guint key_index, GError **error);

KoneplusRmp *koneplus_rmp_load(RoccatDevice *koneplus, guint profile_index, GError **error);
gboolean koneplus_rmp_save(RoccatDevice *koneplus, KoneplusRmp *rmp, guint profile_index, GError **error);
KoneplusRmp *koneplus_rmp_load_save_after_reset(RoccatDevice *koneplus, guint profile_index, GError **error);

KoneplusRmp *koneplus_rmp_load_actual(guint profile_index);
gboolean koneplus_rmp_save_actual(KoneplusRmp *rmp, guint profile_index, GError **error);
void koneplus_rmp_update_from_device(KoneplusRmp *rmp, RoccatDevice *koneplus, guint profile_index);

gboolean koneplus_rmp_get_modified(KoneplusRmp const *rmp);
void koneplus_rmp_set_modified(KoneplusRmp *rmp);
void koneplus_rmp_set_unmodified(KoneplusRmp *rmp);

guint8 koneplus_rmp_macro_key_info_calc_checksum(KoneplusRmpMacroKeyInfo const *rmp_macro_key_info);
void koneplus_rmp_macro_key_info_set_checksum(KoneplusRmpMacroKeyInfo *rmp_macro_key_info);

guint8 koneplus_rmp_light_info_calc_checksum(KoneplusRmpLightInfo const *rmp_light_info);
void koneplus_rmp_light_info_set_checksum(KoneplusRmpLightInfo *rmp_light_info);
void koneplus_rmp_light_info_to_light_info(KoneplusRmpLightInfo const *from, RoccatLight *to, gboolean custom);

void koneplus_light_info_to_rmp_light_info(RoccatLight const *light_info, KoneplusRmpLightInfo *rmp_light_info, gboolean state);
void koneplus_light_info_to_custom_rmp_light_info(RoccatLight const *light_info, KoneplusRmpLightInfo *rmp_light_info);

KoneplusRmpLightInfo const *koneplus_rmp_light_info_get_standard(guint index);
gboolean koneplus_rmp_light_info_equal(KoneplusRmpLightInfo const *left, KoneplusRmpLightInfo const *right);

KoneplusProfileButtons *koneplus_rmp_to_profile_buttons(KoneplusRmp *rmp);
KoneplusProfileSettings *koneplus_rmp_to_profile_settings(KoneplusRmp *rmp);
void koneplus_rmp_update_with_profile_buttons(KoneplusRmp *rmp, KoneplusProfileButtons const *profile_buttons);
void koneplus_rmp_update_with_profile_settings(KoneplusRmp *rmp, KoneplusProfileSettings const *profile_settings);
void koneplus_rmp_update_with_macro(KoneplusRmp *rmp, guint key_index, KoneplusMacro const *macro);

void koneplus_rmp_set_macro_key_info(KoneplusRmp *rmp, guint index, KoneplusRmpMacroKeyInfo *rmp_macro_key_info);
void koneplus_rmp_set_profile_name(KoneplusRmp *rmp, gchar const *string);
void koneplus_rmp_set_xy_synchronous(KoneplusRmp *rmp, guint value);
void koneplus_rmp_set_sensitivity_x(KoneplusRmp *rmp, guint value);
void koneplus_rmp_set_sensitivity_y(KoneplusRmp *rmp, guint value);
void koneplus_rmp_set_sensitivity(KoneplusRmp *rmp, guint value);
void koneplus_rmp_set_cpi(KoneplusRmp *rmp, guint bit, guint value);
void koneplus_rmp_set_cpi_all(KoneplusRmp *rmp, guint value);
void koneplus_rmp_set_rmp_light_info(KoneplusRmp *rmp, guint index, KoneplusRmpLightInfo *rmp_light_info);
void koneplus_rmp_set_polling_rate(KoneplusRmp *rmp, guint value);
void koneplus_rmp_set_light_effect_type(KoneplusRmp *rmp, guint value);
void koneplus_rmp_set_light_effect_speed(KoneplusRmp *rmp, guint value);
void koneplus_rmp_set_light_effect_mode(KoneplusRmp *rmp, guint value);
void koneplus_rmp_set_light_color_flow(KoneplusRmp *rmp, guint value);
void koneplus_rmp_set_cpi_level_x(KoneplusRmp *rmp, guint index, guint value);
void koneplus_rmp_set_cpi_level_y(KoneplusRmp *rmp, guint index, guint value);
void koneplus_rmp_set_cpi_x(KoneplusRmp *rmp, guint value);
void koneplus_rmp_set_cpi_y(KoneplusRmp *rmp, guint value);
void koneplus_rmp_set_game_file_name(KoneplusRmp *rmp, guint index, gchar const *string);
void koneplus_rmp_set_talkfx(KoneplusRmp *rmp, guint value);

KoneplusRmpMacroKeyInfo *koneplus_rmp_get_macro_key_info(KoneplusRmp *rmp, guint index);
gchar *koneplus_rmp_get_profile_name(KoneplusRmp *rmp);
guint koneplus_rmp_get_xy_synchronous(KoneplusRmp *rmp);
guint koneplus_rmp_get_sensitivity_x(KoneplusRmp *rmp);
guint koneplus_rmp_get_sensitivity_y(KoneplusRmp *rmp);
guint koneplus_rmp_get_sensitivity(KoneplusRmp *rmp);
guint koneplus_rmp_get_cpi(KoneplusRmp *rmp, guint bit);
guint8 koneplus_rmp_get_cpi_all(KoneplusRmp *rmp);
KoneplusRmpLightInfo *koneplus_rmp_get_rmp_light_info(KoneplusRmp *rmp, guint index);
guint koneplus_rmp_get_polling_rate(KoneplusRmp *rmp);
guint koneplus_rmp_get_light_effect_type(KoneplusRmp *rmp);
guint koneplus_rmp_get_light_effect_speed(KoneplusRmp *rmp);
guint koneplus_rmp_get_light_effect_mode(KoneplusRmp *rmp);
guint koneplus_rmp_get_light_color_flow(KoneplusRmp *rmp);
guint koneplus_rmp_get_cpi_level_x(KoneplusRmp *rmp, guint index);
guint koneplus_rmp_get_cpi_level_y(KoneplusRmp *rmp, guint index);
guint koneplus_rmp_get_cpi_x(KoneplusRmp *rmp);
guint koneplus_rmp_get_cpi_y(KoneplusRmp *rmp);
gchar *koneplus_rmp_get_game_file_name(KoneplusRmp *rmp, guint index);
guint koneplus_rmp_get_talkfx(KoneplusRmp *rmp);

gboolean koneplus_play_quicklaunch_forked(KoneplusRmpMacroKeyInfo const *macro_key_info);

KoneplusRmpMacroKeyInfo *koneplus_rmp_macro_key_info_new(void);
void koneplus_rmp_macro_key_info_free(KoneplusRmpMacroKeyInfo *macro_key_info);
KoneplusRmpMacroKeyInfo *gaminggear_macro_to_koneplus_rmp_macro_key_info(GaminggearMacro const *gaminggear_macro, GError **error);
KoneplusRmpMacroKeyInfo *koneplus_rmp_macro_key_info_new_quicklaunch(gchar const *filename);
KoneplusRmpMacroKeyInfo *koneplus_rmp_macro_key_info_new_special(KoneplusProfileButtonType type);
KoneplusRmpMacroKeyInfo *koneplus_rmp_macro_key_info_new_timer(gchar const *name, guint seconds);
KoneplusRmpMacroKeyInfo *koneplus_rmp_macro_key_info_new_talk(KoneplusProfileButtonType type, guint talk_device);
KoneplusRmpMacroKeyInfo *koneplus_rmp_macro_key_info_new_shortcut(guint hid_usage_id, guint8 modifier);
gboolean koneplus_rmp_macro_key_info_equal(KoneplusRmpMacroKeyInfo const *left, KoneplusRmpMacroKeyInfo const *right);
KoneplusRmpMacroKeyInfo *koneplus_rmp_macro_key_info_dup(KoneplusRmpMacroKeyInfo const *source);
KoneplusRmpMacroKeyInfo *koneplus_rmp_macro_key_info_v1_to_koneplus_rmp_macro_key_info(KoneplusRmpMacroKeyInfoV1 const *v1);

#define KONEPLUS_DBUS_SERVER_PATH "/org/roccat/Koneplus"
#define KONEPLUS_DBUS_SERVER_INTERFACE "org.roccat.Koneplus"

DBusGProxy *koneplus_dbus_proxy_new(void);
gboolean koneplus_dbus_emit_profile_data_changed_outside(DBusGProxy *proxy, guint profile_number);
gboolean koneplus_dbus_emit_profile_data_changed_outside_instant(guint profile_number);
gboolean koneplus_dbus_emit_profile_changed_outside(DBusGProxy *proxy, guint profile_number);
gboolean koneplus_dbus_emit_profile_changed_outside_instant(guint profile_number);
gboolean koneplus_dbus_emit_configuration_changed_outside(DBusGProxy *proxy);

#define KONEPLUS_DEVICE_NAME "Kone[+]"

G_END_DECLS

#endif
