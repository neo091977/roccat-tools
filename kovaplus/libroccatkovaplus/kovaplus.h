#ifndef __ROCCAT_KOVAPLUS_H__
#define __ROCCAT_KOVAPLUS_H__

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
 * \file kovaplus.h
 * \brief definitions for libroccatkovaplus
 * \author Stefan Achatz
 */

#include <gaminggear/macro.h>
#include "roccat_device_scanner.h"
#include "roccat_key_file.h"
#include "roccat_keystroke.h"
#include <glib.h>
#include <stdio.h>
#include <dbus/dbus-glib.h>

G_BEGIN_DECLS

#define USB_DEVICE_ID_ROCCAT_KOVAPLUS 0x2d50

typedef struct _KovaplusActualProfile KovaplusActualProfile;
typedef struct _KovaplusProfileSettings KovaplusProfileSettings;
typedef struct _KovaplusProfileButtons KovaplusProfileButtons;
typedef struct _KovaplusRmp KovaplusRmp;
typedef struct _KovaplusRmpMacroKeyInfo KovaplusRmpMacroKeyInfo;
typedef struct _KovaplusSpecial KovaplusSpecial;
typedef struct _KovaplusInfo KovaplusInfo;
typedef struct _KovaplusDeviceState KovaplusDeviceState;

enum {
	KOVAPLUS_PROFILE_NUM = 5,
	KOVAPLUS_BUTTON_NUM = 18,
	KOVAPLUS_CPI_LEVELS_NUM = 4,
	KOVAPLUS_SENSITIVITY_MIN = 1,
	KOVAPLUS_SENSITIVITY_MAX = 10,
	KOVAPLUS_RMP_MACRO_KEY_INFO_MACROSET_NAME_LENGTH = 50,
	KOVAPLUS_RMP_MACRO_KEY_INFO_MACRO_NAME_LENGTH = 50,
	KOVAPLUS_RMP_MACRO_KEY_INFO_FILENAME_LENGTH = 260,
	KOVAPLUS_RMP_MACRO_KEY_INFO_TIMER_NAME_LENGTH = 100,
	KOVAPLUS_RMP_MACRO_KEY_INFO_KEYSTROKES_NUM = 500,
	KOVAPLUS_COLOR_NUM = 7,
	KOVAPLUS_GAMEFILE_NUM = 3,
};

typedef enum {
	KOVAPLUS_CONTROL_REQUEST_PROFILE_SETTINGS = 0x10,
	KOVAPLUS_CONTROL_REQUEST_PROFILE_BUTTONS = 0x20,
} KovaplusControlRequest;

gboolean kovaplus_select(RoccatDevice *device, guint profile_index, guint request, GError **error);

struct _KovaplusActualProfile {
	guint8 report_id; /* KOVAPLUS_REPORT_ID_ACTUAL_PROFILE */
	guint8 size;
	guint8 profile_index;
} __attribute__ ((packed));

struct _KovaplusInfo {
	guint8 report_id; /* KOVAPLUS_REPORT_ID_INFO */
	guint8 size; /* 6 */
	guint8 firmware_version;
	guint8 unknown[3];
} __attribute__ ((packed));

typedef enum {
	KOVAPLUS_INFO_FUNCTION_RESET = 0x07,
} KovaplusInfoFunction;

struct _KovaplusProfileSettings {
	guint8 report_id; /* KOVAPLUS_REPORT_ID_PROFILE_SETTINGS */
	guint8 size; /* 16 */
	guint8 profile_index; /* range 0-4 */
	guint8 unknown1;
	guint8 sensitivity_x;
	guint8 sensitivity_y;
	guint8 cpi_levels_enabled;
	guint8 cpi_startup_level; /* range 1-4 */
	guint8 polling_rate; /* RoccatPollingRate */
	guint8 light_switch;
	guint8 light_type;
	guint8 color_option; /* range 1-7, 0 = off; in rmp: range 0-6, */
	guint8 color_change_mode;
	guint8 orientation;
	guint16 checksum;
} __attribute__ ((packed));

/* also valid as KovaplusRmpCpiBit */
typedef enum {
	KOVAPLUS_PROFILE_SETTINGS_CPI_LEVEL_BIT_400 = 0,
	KOVAPLUS_PROFILE_SETTINGS_CPI_LEVEL_BIT_800 = 1,
	KOVAPLUS_PROFILE_SETTINGS_CPI_LEVEL_BIT_1600 = 2,
	KOVAPLUS_PROFILE_SETTINGS_CPI_LEVEL_BIT_3200 = 3,
} KovaplusProfileSettingsCpiLevelBit;

/* also valid as KovaplusRmpCpiState */
typedef enum {
	KOVAPLUS_PROFILE_SETTINGS_CPI_LEVEL_BIT_STATE_INACTIVE = 0,
	KOVAPLUS_PROFILE_SETTINGS_CPI_LEVEL_BIT_STATE_ACTIVE = 1,
} KovaplusProfileSettingsCpiLevelBitState;

/* also valid as KovaplusRmpCpiX and KovaplusRmpCpiY */
typedef enum {
	KOVAPLUS_PROFILE_SETTINGS_CPI_LEVEL_400 = 1,
	KOVAPLUS_PROFILE_SETTINGS_CPI_LEVEL_800 = 2,
	KOVAPLUS_PROFILE_SETTINGS_CPI_LEVEL_1600 = 4,
	KOVAPLUS_PROFILE_SETTINGS_CPI_LEVEL_3200 = 7,
} KovaplusProfileSettingsCpiLevel;

/* also valid as KovaplusRmpLightSwitch */
typedef enum {
	KOVAPLUS_PROFILE_SETTINGS_LIGHT_SWITCH_OFF = 0,
	KOVAPLUS_PROFILE_SETTINGS_LIGHT_SWITCH_ON = 1,
} KovaplusProfileSettingsLightSwitch;

/* also valid as KovaplusRmpLightType */
typedef enum {
	KOVAPLUS_PROFILE_SETTINGS_LIGHT_TYPE_FULL_LIGHTENED = 0,
	KOVAPLUS_PROFILE_SETTINGS_LIGHT_TYPE_PULSATING = 1,
} KovaplusProfileSettingsLightType;

typedef enum {
	KOVAPLUS_PROFILE_SETTINGS_COLOR_OPTION_OFF = 0,
	// FIXME 1-7 are colors
} KovaplusProfileSettingsColorOption;

typedef enum {
	KOVAPLUS_PROFILE_SETTINGS_COLOR_CHANGE_MODE_5_SEC = 0,
	KOVAPLUS_PROFILE_SETTINGS_COLOR_CHANGE_MODE_10_SEC = 1,
	KOVAPLUS_PROFILE_SETTINGS_COLOR_CHANGE_MODE_15_SEC = 2,
	KOVAPLUS_PROFILE_SETTINGS_COLOR_CHANGE_MODE_OFF = 3,
} KovaplusProfileSettingsColorChangeMode;

/* also valid as KovaplusRmpOrientation */
typedef enum {
	KOVAPLUS_PROFILE_SETTINGS_ORIENTATION_RIGHT = 0,
	KOVAPLUS_PROFILE_SETTINGS_ORIENTATION_LEFT = 1,
} KovaplusProfileSettingsOrientation;

static inline guint16 kovaplus_profile_settings_get_checksum(KovaplusProfileSettings const *profile_settings) {
	return GUINT16_FROM_LE(profile_settings->checksum);
}

void kovaplus_profile_settings_set_checksum(KovaplusProfileSettings *profile_settings);
void kovaplus_profile_settings_finalize(KovaplusProfileSettings *profile_settings, guint profile_index);
gboolean kovaplus_profile_settings_write(RoccatDevice *kovaplus, guint profile_index, KovaplusProfileSettings *profile_settings, GError **error);
KovaplusProfileSettings *kovaplus_profile_settings_read(RoccatDevice *kovaplus, guint profile_index, GError **error);

struct _KovaplusProfileButtons {
	guint8 report_id; /* KOVAPLUS_REPORT_ID_PROFILE_BUTTONS */
	guint8 size; /* 23 */
	guint8 profile_index; /* range 0-4 */
	guint8 button_type[KOVAPLUS_BUTTON_NUM];
	guint16 checksum;
} __attribute__ ((packed));

/* also valid as KovaplusRmpMacroKeyInfoButtonIndex */
typedef enum {
	KOVAPLUS_PROFILE_BUTTONS_BUTTON_INDEX_LEFT = 0,
	KOVAPLUS_PROFILE_BUTTONS_BUTTON_INDEX_RIGHT = 1,
	KOVAPLUS_PROFILE_BUTTONS_BUTTON_INDEX_MIDDLE = 2,
	KOVAPLUS_PROFILE_BUTTONS_BUTTON_INDEX_FORWARD_LEFT = 3,
	KOVAPLUS_PROFILE_BUTTONS_BUTTON_INDEX_BACKWARD_LEFT = 4,
	KOVAPLUS_PROFILE_BUTTONS_BUTTON_INDEX_FORWARD_RIGHT = 5,
	KOVAPLUS_PROFILE_BUTTONS_BUTTON_INDEX_BACKWARD_RIGHT = 6,
	KOVAPLUS_PROFILE_BUTTONS_BUTTON_INDEX_WHEEL_UP = 7,
	KOVAPLUS_PROFILE_BUTTONS_BUTTON_INDEX_WHEEL_DOWN = 8,
	KOVAPLUS_PROFILE_BUTTONS_BUTTON_INDEX_SHIFT_LEFT = 9,
	KOVAPLUS_PROFILE_BUTTONS_BUTTON_INDEX_SHIFT_RIGHT = 10,
	KOVAPLUS_PROFILE_BUTTONS_BUTTON_INDEX_SHIFT_MIDDLE = 11,
	KOVAPLUS_PROFILE_BUTTONS_BUTTON_INDEX_SHIFT_FORWARD_LEFT = 12,
	KOVAPLUS_PROFILE_BUTTONS_BUTTON_INDEX_SHIFT_BACKWARD_LEFT = 13,
	KOVAPLUS_PROFILE_BUTTONS_BUTTON_INDEX_SHIFT_FORWARD_RIGHT = 14,
	KOVAPLUS_PROFILE_BUTTONS_BUTTON_INDEX_SHIFT_BACKWARD_RIGHT = 15,
	KOVAPLUS_PROFILE_BUTTONS_BUTTON_INDEX_SHIFT_WHEEL_UP = 16,
	KOVAPLUS_PROFILE_BUTTONS_BUTTON_INDEX_SHIFT_WHEEL_DOWN = 17,
} KovaplusProfileButtonsButtonIndex;

/*
 * also valid as
 * KovaplusRmpMacroKeyInfoType
 */
typedef enum {
	KOVAPLUS_PROFILE_BUTTON_TYPE_UNUSED = 0x0, /* button is disabled by system */
	KOVAPLUS_PROFILE_BUTTON_TYPE_CLICK = 0x1,
	KOVAPLUS_PROFILE_BUTTON_TYPE_MENU = 0x2,
	KOVAPLUS_PROFILE_BUTTON_TYPE_UNIVERSAL_SCROLLING = 0x3,
	KOVAPLUS_PROFILE_BUTTON_TYPE_DOUBLE_CLICK = 0x4,
	KOVAPLUS_PROFILE_BUTTON_TYPE_SHORTCUT = 0x5,
	KOVAPLUS_PROFILE_BUTTON_TYPE_DISABLED = 0x6, /* button is disabled by user */
	KOVAPLUS_PROFILE_BUTTON_TYPE_BROWSER_FORWARD = 0x7,
	KOVAPLUS_PROFILE_BUTTON_TYPE_BROWSER_BACKWARD = 0x8,
	KOVAPLUS_PROFILE_BUTTON_TYPE_TILT_LEFT = 0x9,
	KOVAPLUS_PROFILE_BUTTON_TYPE_TILT_RIGHT = 0xa,
	KOVAPLUS_PROFILE_BUTTON_TYPE_SCROLL_UP = 0xd,
	KOVAPLUS_PROFILE_BUTTON_TYPE_SCROLL_DOWN = 0xe,
	KOVAPLUS_PROFILE_BUTTON_TYPE_QUICKLAUNCH = 0xf,
	KOVAPLUS_PROFILE_BUTTON_TYPE_PROFILE_CYCLE = 0x10,
	KOVAPLUS_PROFILE_BUTTON_TYPE_PROFILE_UP = 0x11,
	KOVAPLUS_PROFILE_BUTTON_TYPE_PROFILE_DOWN = 0x12,
	KOVAPLUS_PROFILE_BUTTON_TYPE_CPI_CYCLE = 0x14,
	KOVAPLUS_PROFILE_BUTTON_TYPE_CPI_UP = 0x15,
	KOVAPLUS_PROFILE_BUTTON_TYPE_CPI_DOWN = 0x16,
	KOVAPLUS_PROFILE_BUTTON_TYPE_SENSITIVITY_CYCLE = 0x17,
	KOVAPLUS_PROFILE_BUTTON_TYPE_SENSITIVITY_UP = 0x18,
	KOVAPLUS_PROFILE_BUTTON_TYPE_SENSITIVITY_DOWN = 0x19,
	KOVAPLUS_PROFILE_BUTTON_TYPE_WINDOWS_KEY = 0x1a,
	KOVAPLUS_PROFILE_BUTTON_TYPE_MULTIMEDIA_OPEN_PLAYER = 0x20,
	KOVAPLUS_PROFILE_BUTTON_TYPE_MULTIMEDIA_PREV_TRACK = 0x21,
	KOVAPLUS_PROFILE_BUTTON_TYPE_MULTIMEDIA_NEXT_TRACK = 0x22,
	KOVAPLUS_PROFILE_BUTTON_TYPE_MULTIMEDIA_PLAY_PAUSE = 0x23,
	KOVAPLUS_PROFILE_BUTTON_TYPE_MULTIMEDIA_STOP = 0x24,
	KOVAPLUS_PROFILE_BUTTON_TYPE_MULTIMEDIA_MUTE = 0x25,
	KOVAPLUS_PROFILE_BUTTON_TYPE_MULTIMEDIA_VOLUME_DOWN = 0x26,
	KOVAPLUS_PROFILE_BUTTON_TYPE_MULTIMEDIA_VOLUME_UP = 0x27,
	KOVAPLUS_PROFILE_BUTTON_TYPE_MACRO = 0x30,
	KOVAPLUS_PROFILE_BUTTON_TYPE_TIMER = 0x31, /* timer information is in rmp */
	KOVAPLUS_PROFILE_BUTTON_TYPE_TIMER_STOP = 0x32,
	/* TODO clarify if easywheel and easyshift can only be applied to buttons back-left/right */
	KOVAPLUS_PROFILE_BUTTON_TYPE_EASYSHIFT = 0x41,
	KOVAPLUS_PROFILE_BUTTON_TYPE_EASYWHEEL_SENSITIVITY = 0x42,
	KOVAPLUS_PROFILE_BUTTON_TYPE_EASYWHEEL_PROFILE = 0x43,
	KOVAPLUS_PROFILE_BUTTON_TYPE_EASYWHEEL_CPI = 0x44,
	KOVAPLUS_PROFILE_BUTTON_TYPE_EASYWHEEL_VOLUME = 0x45,
	KOVAPLUS_PROFILE_BUTTON_TYPE_EASYWHEEL_ALT_TAB = 0x46,
	KOVAPLUS_PROFILE_BUTTON_TYPE_EASYWHEEL_AERO_FLIP_3D = 0x47,
} KovaplusProfileButtonType;

static inline guint16 kovaplus_profile_buttons_get_checksum(KovaplusProfileButtons const *profile_buttons) {
	return GUINT16_FROM_LE(profile_buttons->checksum);
}

void kovaplus_profile_buttons_set_checksum(KovaplusProfileButtons *profile_buttons);
void kovaplus_profile_buttons_finalize(KovaplusProfileButtons *profile_buttons, guint index);
gboolean kovaplus_profile_buttons_write(RoccatDevice *kovaplus, guint profile_index, KovaplusProfileButtons *profile_buttons, GError **error);
KovaplusProfileButtons *kovaplus_profile_buttons_read(RoccatDevice *kovaplus, guint profile_index, GError **error);

struct _KovaplusRmpMacroKeyInfo {
	guint8 button_index;
	guint8 type;
	guint8 macroset_name[KOVAPLUS_RMP_MACRO_KEY_INFO_MACROSET_NAME_LENGTH];
	guint8 macro_name[KOVAPLUS_RMP_MACRO_KEY_INFO_MACRO_NAME_LENGTH];
	guint16 unused1;
	guint16 loop;
	guint16 unused2;
	guint16 count;
	RoccatKeystroke keystrokes[KOVAPLUS_RMP_MACRO_KEY_INFO_KEYSTROKES_NUM];
	guint16 unused3;
	guint32 timer_length;
	guint8 timer_name[KOVAPLUS_RMP_MACRO_KEY_INFO_TIMER_NAME_LENGTH];
	guint8 filename[KOVAPLUS_RMP_MACRO_KEY_INFO_FILENAME_LENGTH];
	guint8 checksum;
} __attribute__ ((packed));

static inline guint16 kovaplus_rmp_macro_key_info_get_loop(KovaplusRmpMacroKeyInfo const *info) {
	return GUINT16_FROM_LE(info->loop);
}

static inline void kovaplus_rmp_macro_key_info_set_loop(KovaplusRmpMacroKeyInfo *info, guint16 new_value) {
	info->loop = GUINT16_TO_LE(new_value);
}

static inline guint16 kovaplus_rmp_macro_key_info_get_count(KovaplusRmpMacroKeyInfo const *info) {
	return GUINT16_FROM_LE(info->count);
}

static inline void kovaplus_rmp_macro_key_info_set_count(KovaplusRmpMacroKeyInfo *info, guint16 new_value) {
	info->count = GUINT16_TO_LE(new_value);
}

static inline guint32 kovaplus_rmp_macro_key_info_get_timer_length(KovaplusRmpMacroKeyInfo const *info) {
	return GUINT32_FROM_LE(info->timer_length);
}

static inline void kovaplus_rmp_macro_key_info_set_timer_length(KovaplusRmpMacroKeyInfo *info, guint32 new_value) {
	info->timer_length = GUINT32_TO_LE(new_value);
}

void kovaplus_rmp_macro_key_info_set_checksum(KovaplusRmpMacroKeyInfo *rmp_macro_key_info);
KovaplusRmpMacroKeyInfo *kovaplus_rmp_macro_key_info_new_quicklaunch(gchar const *filename);
KovaplusRmpMacroKeyInfo *kovaplus_rmp_macro_key_info_new_special(KovaplusProfileButtonType type);
KovaplusRmpMacroKeyInfo *kovaplus_rmp_macro_key_info_new_timer(gchar const *name, guint seconds);
KovaplusRmpMacroKeyInfo *kovaplus_rmp_macro_key_info_new_shortcut(guint hid_usage_id, guint8 modifier);
gboolean kovaplus_rmp_macro_key_info_equal(KovaplusRmpMacroKeyInfo const *left, KovaplusRmpMacroKeyInfo const *right);
KovaplusRmpMacroKeyInfo *kovaplus_rmp_macro_key_info_dup(KovaplusRmpMacroKeyInfo const *source);
void kovaplus_rmp_macro_key_info_free(KovaplusRmpMacroKeyInfo *macro_key_info);
KovaplusRmpMacroKeyInfo *gaminggear_macro_to_kovaplus_rmp_macro_key_info(GaminggearMacro const *gaminggear_macro, GError **error);
GaminggearMacro *kovaplus_rmp_macro_key_info_to_gaminggear_macro(KovaplusRmpMacroKeyInfo const *button_info);

typedef enum {
	KOVAPLUS_RMP_COLOR_FLAG_SINGLE_COLOR = 0,
	KOVAPLUS_RMP_COLOR_FLAG_MULTICOLOR_CYCLING = 1,
} KovaplusRmpColorFlag;

typedef enum {
	KOVAPLUS_RMP_COLOR_CHANGE_MODE_5_SEC = 0,
	KOVAPLUS_RMP_COLOR_CHANGE_MODE_10_SEC = 1,
	KOVAPLUS_RMP_COLOR_CHANGE_MODE_15_SEC = 2,
} KovaplusRmpColorChangeMode;

gboolean kovaplus_rmp_get_modified(KovaplusRmp const *rmp);
void kovaplus_rmp_set_modified(KovaplusRmp *rmp);
void kovaplus_rmp_set_unmodified(KovaplusRmp *rmp);
void kovaplus_rmp_free(KovaplusRmp *rmp);
KovaplusRmp *kovaplus_rmp_dup(KovaplusRmp const *src);
KovaplusRmp *kovaplus_rmp_read_with_path(gchar const *path, GError **error);
gboolean kovaplus_rmp_write_with_path(gchar const *path, KovaplusRmp *rmp, GError **error);
KovaplusRmp const *kovaplus_default_rmp(void);

KovaplusRmp *kovaplus_rmp_load(RoccatDevice *kovaplus, guint profile_index, GError **error) ;
gboolean kovaplus_rmp_save(RoccatDevice *kovaplus, KovaplusRmp *rmp, guint profile_index, GError **error);
KovaplusRmp *kovaplus_rmp_load_save_after_reset(RoccatDevice *kovaplus, guint profile_index, GError **error);

KovaplusRmp *kovaplus_rmp_load_actual(guint profile_index);
gboolean kovaplus_rmp_save_actual(KovaplusRmp *rmp, guint profile_index, GError **error);
void kovaplus_rmp_update_from_device(KovaplusRmp *rmp, RoccatDevice *kovaplus, guint profile_index);

void kovaplus_rmp_set_profile_name(KovaplusRmp *rmp, gchar const *string);
gchar *kovaplus_rmp_get_profile_name(KovaplusRmp *rmp);
void kovaplus_rmp_set_sensitivity_x(KovaplusRmp *rmp, guint value);
guint kovaplus_rmp_get_sensitivity_x(KovaplusRmp *rmp);
void kovaplus_rmp_set_sensitivity_y(KovaplusRmp *rmp, guint value);
guint kovaplus_rmp_get_sensitivity_y(KovaplusRmp *rmp);
void kovaplus_rmp_set_cpi(KovaplusRmp *rmp, guint bit, guint value);
void kovaplus_rmp_set_cpi_all(KovaplusRmp *rmp, guint value);
guint kovaplus_rmp_get_cpi(KovaplusRmp *rmp, guint bit);
guint8 kovaplus_rmp_get_cpi_all(KovaplusRmp *rmp);
void kovaplus_rmp_set_orientation(KovaplusRmp *rmp, guint value);
guint kovaplus_rmp_get_orientation(KovaplusRmp *rmp);
void kovaplus_rmp_set_light_switch(KovaplusRmp *rmp, guint value);
guint kovaplus_rmp_get_light_switch(KovaplusRmp *rmp);
void kovaplus_rmp_set_light_type(KovaplusRmp *rmp, guint value);
guint kovaplus_rmp_get_light_type(KovaplusRmp *rmp);
void kovaplus_rmp_set_color_flag(KovaplusRmp *rmp, guint value);
guint kovaplus_rmp_get_color_flag(KovaplusRmp *rmp);
void kovaplus_rmp_set_color_options(KovaplusRmp *rmp, guint value);
guint kovaplus_rmp_get_color_options(KovaplusRmp *rmp);
void kovaplus_rmp_set_color_change_mode(KovaplusRmp *rmp, guint value);
guint kovaplus_rmp_get_color_change_mode(KovaplusRmp *rmp);
void kovaplus_rmp_set_cpi_x(KovaplusRmp *rmp, guint value);
guint kovaplus_rmp_get_cpi_x(KovaplusRmp *rmp);
void kovaplus_rmp_set_cpi_y(KovaplusRmp *rmp, guint value);
guint kovaplus_rmp_get_cpi_y(KovaplusRmp *rmp);
void kovaplus_rmp_set_polling_rate(KovaplusRmp *rmp, guint value);
guint kovaplus_rmp_get_polling_rate(KovaplusRmp *rmp);
void kovaplus_rmp_set_macro_key_info(KovaplusRmp *rmp, guint index, KovaplusRmpMacroKeyInfo *rmp_macro_key_info);
KovaplusRmpMacroKeyInfo *kovaplus_rmp_get_macro_key_info(KovaplusRmp *rmp, guint index);
void kovaplus_rmp_set_game_file_name(KovaplusRmp *rmp, guint index, gchar const *string);
gchar *kovaplus_rmp_get_game_file_name(KovaplusRmp *rmp, guint index);

KovaplusProfileButtons *kovaplus_rmp_to_profile_buttons(KovaplusRmp *rmp);
void kovaplus_rmp_update_with_profile_buttons(KovaplusRmp *rmp, KovaplusProfileButtons const *profile_buttons);
KovaplusProfileSettings *kovaplus_rmp_to_profile_settings(KovaplusRmp *rmp);
void kovaplus_rmp_update_with_profile_settings(KovaplusRmp *rmp, KovaplusProfileSettings const *profile_settings);

guint kovaplus_cpi_level_to_value(guint level);

typedef enum {
	KOVAPLUS_REPORT_ID_MOUSE = 0x1,
	KOVAPLUS_REPORT_ID_SPECIAL = 0x3,
	KOVAPLUS_REPORT_ID_CONTROL = 0x4,
	KOVAPLUS_REPORT_ID_ACTUAL_PROFILE = 0x5,
	KOVAPLUS_REPORT_ID_PROFILE_SETTINGS = 0x6,
	KOVAPLUS_REPORT_ID_PROFILE_BUTTONS = 0x7,
	KOVAPLUS_REPORT_ID_INFO = 0x9,
	KOVAPLUS_REPORT_ID_DEVICE_STATE = 0xa,
} KovaplusReportId;

struct _KovaplusDeviceState {
	guint8 report_id; /* KOVAPLUS_REPORT_ID_DEVICE_STATE */
	guint8 size; /* always 3 */
	guint8 state;
} __attribute__ ((packed));

typedef enum {
	/* Device sends HID multimedia events */
	KOVAPLUS_DEVICE_STATE_STATE_OFF = 0x00,
	/* Driver handles multimedia events */
	KOVAPLUS_DEVICE_STATE_STATE_ON = 0x01,
} KovaplusDeviceStateState;

gboolean kovaplus_device_state_write(RoccatDevice *device, guint state, GError **error);

struct _KovaplusSpecial {
	guint8 report_id; /* KOVAPLUS_REPORT_ID_SPECIAL */
	guint8 unknown1;
	guint8 type;
	guint8 data1;
	guint8 data2;
} __attribute__ ((packed));

typedef enum  {
	KOVAPLUS_SPECIAL_ACTION_PRESS = 0,
	KOVAPLUS_SPECIAL_ACTION_RELEASE = 1,
} KovaplusSpecialAction;

typedef enum {
	KOVAPLUS_SPECIAL_TYPE_TILT = 0x10,
	KOVAPLUS_SPECIAL_TYPE_PROFILE_1 = 0x20, /* data1 = profile_number range 1-5; no release event */
	KOVAPLUS_SPECIAL_TYPE_PROFILE_2 = 0x30, /* data1 = profile_number range 1-5; no release event */
	KOVAPLUS_SPECIAL_TYPE_MACRO = 0x40, /* data1 = button_number range 1-18; data2 = action */
	KOVAPLUS_SPECIAL_TYPE_SHORTCUT = 0x50, /* data1 = button_number range 1-18; data2 = action */
	KOVAPLUS_SPECIAL_TYPE_QUICKLAUNCH = 0x60, /* data1 = button_number range 1-18; data2 = action */
	KOVAPLUS_SPECIAL_TYPE_TIMER = 0x80, /* data1 = button_number range 1-18; data2 = action */
	KOVAPLUS_SPECIAL_TYPE_TIMER_STOP = 0x90, /* data1 = button_number range 1-18; data2 = action */
	KOVAPLUS_SPECIAL_TYPE_CPI = 0xb0, /* data1 = cpi data; no release event */
	KOVAPLUS_SPECIAL_TYPE_SENSITIVITY = 0xc0, /* data1 + data2 = sense range 1-10; no release event */
	KOVAPLUS_SPECIAL_TYPE_MULTIMEDIA = 0xf0, /* data1 = type as in profile_buttons; data2 = action */
} KovaplusSpecialType;

typedef enum {
	KOVAPLUS_INTERFACE_MOUSE = 0,
	KOVAPLUS_INTERFACE_KEYBOARD = 1,
} KovaplusInterface;

RoccatDevice *kovaplus_device_first(void);
RoccatDeviceScanner *kovaplus_device_scanner_new(void);
gchar *kovaplus_device_read(RoccatDevice *roccat_device, guint report_id, gssize length, GError **error);
gboolean kovaplus_device_write(RoccatDevice *roccat_device, gchar const *buffer, gssize length, GError **error);

RoccatKeyFile *kovaplus_configuration_load(void);
void kovaplus_configuration_free(RoccatKeyFile *config);
gboolean kovaplus_configuration_save(RoccatKeyFile *config, GError **error);

gchar *kovaplus_configuration_get_rmp_save_path(RoccatKeyFile *config);
void kovaplus_configuration_set_rmp_save_path(RoccatKeyFile *config, gchar const *path);

gint kovaplus_configuration_get_timer_notification_type(RoccatKeyFile *config);
void kovaplus_configuration_set_timer_notification_type(RoccatKeyFile *config, gint new_value);
gint kovaplus_configuration_get_profile_notification_type(RoccatKeyFile *config);
void kovaplus_configuration_set_profile_notification_type(RoccatKeyFile *config, gint new_value);
gint kovaplus_configuration_get_cpi_notification_type(RoccatKeyFile *config);
void kovaplus_configuration_set_cpi_notification_type(RoccatKeyFile *config, gint new_value);
gint kovaplus_configuration_get_sensitivity_notification_type(RoccatKeyFile *config);
void kovaplus_configuration_set_sensitivity_notification_type(RoccatKeyFile *config, gint new_value);
gdouble kovaplus_configuration_get_notification_volume(RoccatKeyFile *config);
void kovaplus_configuration_set_notification_volume(RoccatKeyFile *config, gdouble new_value);
gint kovaplus_configuration_get_default_profile_number(RoccatKeyFile *config);
void kovaplus_configuration_set_default_profile_number(RoccatKeyFile *config, gint new_value);

KovaplusInfo *kovaplus_info_read(RoccatDevice *kovaplus, GError **error);
gboolean kovaplus_reset(RoccatDevice *kovaplus, GError **error);
guint kovaplus_firmware_version_read(RoccatDevice *kovaplus, GError **error);

guint kovaplus_actual_profile_read(RoccatDevice *kovaplus, GError **error);
gboolean kovaplus_actual_profile_write(RoccatDevice *kovaplus, guint profile_index, GError **error);

gboolean kovaplus_play_shortcut_press(KovaplusRmpMacroKeyInfo const *macro_key_info);
gboolean kovaplus_play_shortcut_release(KovaplusRmpMacroKeyInfo const *macro_key_info);
gboolean kovaplus_play_quicklaunch_forked(KovaplusRmpMacroKeyInfo const *macro_key_info);

#define KOVAPLUS_DBUS_SERVER_PATH "/org/roccat/Kovaplus"
#define KOVAPLUS_DBUS_SERVER_INTERFACE "org.roccat.Kovaplus"

DBusGProxy *kovaplus_dbus_proxy_new(void);
gboolean kovaplus_dbus_emit_profile_data_changed_outside(DBusGProxy *proxy, guint profile_number);
gboolean kovaplus_dbus_emit_profile_data_changed_outside_instant(guint profile_number);
gboolean kovaplus_dbus_emit_profile_changed_outside(DBusGProxy *proxy, guint profile_number);
gboolean kovaplus_dbus_emit_profile_changed_outside_instant(guint profile_number);
gboolean kovaplus_dbus_emit_configuration_changed_outside(DBusGProxy *proxy);

#define KOVAPLUS_DEVICE_NAME "Kova [+]"

G_END_DECLS

#endif
