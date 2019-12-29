#ifndef __ROCCAT_SAVU_H__
#define __ROCCAT_SAVU_H__

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

#include <gaminggear/macro.h>
#include "roccat_device_scanner.h"
#include "roccat_timer.h"
#include "roccat_key_file.h"
#include "roccat_keystroke.h"
#include "roccat_button.h"
#include "roccat_light.h"
#include <glib.h>
#include <stdio.h>
#include <dbus/dbus-glib.h>

G_BEGIN_DECLS

#define USB_DEVICE_ID_ROCCAT_SAVU 0x2d5a

typedef struct _SavuControl SavuControl;
typedef struct _SavuProfile SavuProfile;
typedef struct _SavuGeneral SavuGeneral;
typedef struct _SavuButtons SavuButtons;
typedef struct _SavuMacro SavuMacro;
typedef struct _SavuInfo SavuInfo;
typedef struct _SavuDeviceState SavuDeviceState;
typedef struct _SavuSensor SavuSensor;
typedef struct _SavuRmp SavuRmp;
typedef struct _SavuRmpTimer SavuRmpTimer;
typedef struct _SavuRmpMacroKeyInfo SavuRmpMacroKeyInfo;
typedef struct _SavuSpecial SavuSpecial;

enum {
	SAVU_PROFILE_NUM = 5,
	SAVU_PHYSICAL_BUTTON_NUM = 7,
	SAVU_BUTTON_NUM = 2 * SAVU_PHYSICAL_BUTTON_NUM,
	SAVU_CPI_NUM = 4,
	SAVU_GENERAL_CPI_LEVELS_NUM = 4,
	SAVU_MACRO_MACROSET_NAME_LENGTH = 24,
	SAVU_MACRO_MACRO_NAME_LENGTH = 24,
	SAVU_MACRO_KEYSTROKES_NUM = 500,
	SAVU_RMP_MACRO_KEY_INFO_NAME_LENGTH = 52,
	SAVU_RMP_TIMER_NAME_LENGTH = 24,
	SAVU_RMP_GAME_FILE_NUM = 3,
	SAVU_SINGLE_COLOR_NUM = 16,
	SAVU_GENERAL_COLOR_INDEX_CUSTOM = 0x10,
	SAVU_SENSOR_FRAME_WIDTH = 30,
	SAVU_SENSOR_FRAME_HEIGHT = 30,
	SAVU_SENSOR_FRAME_SIZE = SAVU_SENSOR_FRAME_WIDTH * SAVU_SENSOR_FRAME_HEIGHT,
	SAVU_GAMEFILE_NUM = 3,
};

struct _SavuControl {
	guint8 report_id; /* SAVU_REPORT_ID_CONTROL */
	guint8 value;
	guint8 request;
} __attribute__ ((packed));

gboolean savu_select(RoccatDevice *device, guint profile_index, guint request, GError **error);

typedef enum {
	SAVU_CONTROL_VALUE_CRITICAL = 0,
	SAVU_CONTROL_VALUE_OK = 1,
	SAVU_CONTROL_VALUE_INVALID = 2,
	SAVU_CONTROL_VALUE_BUSY = 3,
} SavuControlValue;

typedef enum {
	SAVU_CONTROL_REQUEST_WRITE_CHECK = 0x00, /* SavuControl.value = SavuControlValue */

	/*
	 * To request macro readout:
	 * SavuControl.value = profile_index
	 * SavuControl.request = button_index in range 2-13
	 */

	SAVU_CONTROL_REQUEST_GENERAL = 0x80, /* SavuControl.value = profile_index */
	SAVU_CONTROL_REQUEST_BUTTONS = 0x90, /* SavuControl.value = profile_index */
} SavuControlRequest;

struct _SavuProfile {
	guint8 report_id; /* SAVU_REPORT_ID_PROFILE */
	guint8 size; /* 0x3 */
	guint8 profile_index; /* range 0-4 */
} __attribute__ ((packed));

guint savu_profile_read(RoccatDevice *savu, GError **error);
gboolean savu_profile_write(RoccatDevice *savu, guint profile_index, GError **error);

struct _SavuGeneral {
	guint8 report_id; /* SAVU_REPORT_ID_GENERAL */
	guint8 size; /* 0x10 */
	guint8 profile_index; /* range 0-4 */
	guint8 sensitivity_sync; /* RoccatSensitivityAdvanced */
	guint8 sensitivity_x; /* RoccatSensitivity */
	guint8 sensitivity_y; /* RoccatSensitivity */
	guint8 cpi_mask;
	guint8 cpi;
	guint8 polling_rate; /* RoccatPollingRate */
	guint8 lighting_mask;
	RoccatLight light; /* range 0x0 - 0xf, 0x10 is custom color */
	guint16 checksum;
} __attribute__ ((packed));

SavuGeneral *savu_general_read(RoccatDevice *savu, guint profile_index, GError **error);
gboolean savu_general_write(RoccatDevice *savu, guint profile_index, SavuGeneral *general, GError **error);
guint16 savu_general_calc_checksum(SavuGeneral const *general);

static inline guint16 savu_general_get_checksum(SavuGeneral const *general) {
	return GUINT16_FROM_LE(general->checksum);
}

static inline void savu_general_set_checksum(SavuGeneral *general, guint16 new_value) {
	general->checksum = GUINT16_TO_LE(new_value);
}

typedef enum {
	SAVU_GENERAL_CPI_MASK_BIT_400 = 0,
	SAVU_GENERAL_CPI_MASK_BIT_800 = 1,
	SAVU_GENERAL_CPI_MASK_BIT_1600 = 2,
	SAVU_GENERAL_CPI_MASK_BIT_4000 = 3,
} SavuGeneralCpiMaskBit;

typedef enum {
	SAVU_GENERAL_CPI_400 = 0,
	SAVU_GENERAL_CPI_800 = 1,
	SAVU_GENERAL_CPI_1600 = 2,
	SAVU_GENERAL_CPI_4000 = 3,
} SavuGeneralCpi;

typedef enum {
	SAVU_GENERAL_LIGHTING_MASK_BIT_LIGHT = 0,
	SAVU_GENERAL_LIGHTING_MASK_BIT_BREATHING = 1,
	SAVU_GENERAL_LIGHTING_MASK_BIT_COLOR_FLOW = 2,
} SavuGeneralLightingMask;

typedef enum {
	SAVU_BUTTON_TYPE_CLICK = 0x01,
	SAVU_BUTTON_TYPE_MENU = 0x02,
	SAVU_BUTTON_TYPE_UNIVERSAL_SCROLLING = 0x03,
	SAVU_BUTTON_TYPE_DOUBLE_CLICK = 0x04,
	SAVU_BUTTON_TYPE_SHORTCUT = 0x05,
	SAVU_BUTTON_TYPE_DISABLED = 0x06,
	SAVU_BUTTON_TYPE_BROWSER_FORWARD = 0x07,
	SAVU_BUTTON_TYPE_BROWSER_BACKWARD = 0x08,
	SAVU_BUTTON_TYPE_TILT_LEFT = 0x09,
	SAVU_BUTTON_TYPE_TILT_RIGHT = 0x0a,
	SAVU_BUTTON_TYPE_SCROLL_UP = 0x0d,
	SAVU_BUTTON_TYPE_SCROLL_DOWN = 0x0e,
	SAVU_BUTTON_TYPE_QUICKLAUNCH = 0x0f,
	SAVU_BUTTON_TYPE_PROFILE_CYCLE = 0x10,
	SAVU_BUTTON_TYPE_PROFILE_UP = 0x11,
	SAVU_BUTTON_TYPE_PROFILE_DOWN = 0x12,
	SAVU_BUTTON_TYPE_CPI_CYCLE = 0x14,
	SAVU_BUTTON_TYPE_CPI_UP = 0x15,
	SAVU_BUTTON_TYPE_CPI_DOWN = 0x16,
	SAVU_BUTTON_TYPE_SENSITIVITY_CYCLE = 0x17,
	SAVU_BUTTON_TYPE_SENSITIVITY_UP = 0x18,
	SAVU_BUTTON_TYPE_SENSITIVITY_DOWN = 0x19,
	SAVU_BUTTON_TYPE_WINDOWS_KEY = 0x1a,
	SAVU_BUTTON_TYPE_OPEN_DRIVER = 0x1b,
	SAVU_BUTTON_TYPE_MULTIMEDIA_OPEN_PLAYER = 0x20,
	SAVU_BUTTON_TYPE_MULTIMEDIA_PREV_TRACK = 0x21,
	SAVU_BUTTON_TYPE_MULTIMEDIA_NEXT_TRACK = 0x22,
	SAVU_BUTTON_TYPE_MULTIMEDIA_PLAY_PAUSE = 0x23,
	SAVU_BUTTON_TYPE_MULTIMEDIA_STOP = 0x24,
	SAVU_BUTTON_TYPE_MULTIMEDIA_MUTE = 0x25,
	SAVU_BUTTON_TYPE_MULTIMEDIA_VOLUME_DOWN = 0x26,
	SAVU_BUTTON_TYPE_MULTIMEDIA_VOLUME_UP = 0x27,
	SAVU_BUTTON_TYPE_MACRO = 0x30,
	SAVU_BUTTON_TYPE_TIMER = 0x31,
	SAVU_BUTTON_TYPE_TIMER_STOP = 0x32,
	SAVU_BUTTON_TYPE_EASYAIM_400 = 0x33,
	SAVU_BUTTON_TYPE_EASYAIM_800 = 0x34,
	SAVU_BUTTON_TYPE_EASYAIM_1600 = 0x35,
	SAVU_BUTTON_TYPE_EASYAIM_4000 = 0x36,
	SAVU_BUTTON_TYPE_EASYSHIFT = 0x41,
	SAVU_BUTTON_TYPE_EASYWHEEL_SENSITIVITY = 0x42,
	SAVU_BUTTON_TYPE_EASYWHEEL_PROFILE = 0x43,
	SAVU_BUTTON_TYPE_EASYWHEEL_CPI = 0x44,
	SAVU_BUTTON_TYPE_EASYWHEEL_VOLUME = 0x45,
	SAVU_BUTTON_TYPE_EASYWHEEL_ALT_TAG = 0x46,
} SavuButtonType;

struct _SavuButtons {
	guint8 report_id; /* SAVU_REPORT_ID_BUTTONS */
	guint8 size; /* 0x2f */
	guint8 profile_index; /* range 0-4 */
	RoccatButton buttons[SAVU_BUTTON_NUM];
	guint16 checksum;
} __attribute__ ((packed));

SavuButtons *savu_buttons_read(RoccatDevice *savu, guint profile_index, GError **error);
gboolean savu_buttons_write(RoccatDevice *savu, guint profile_index, SavuButtons *buttons, GError **error);
guint16 savu_buttons_calc_checksum(SavuButtons const *buttons);

static inline guint16 savu_buttons_get_checksum(SavuButtons const *buttons) {
	return GUINT16_FROM_LE(buttons->checksum);
}

static inline void savu_buttons_set_checksum(SavuButtons *buttons, guint16 new_value) {
	buttons->checksum = GUINT16_TO_LE(new_value);
}

struct _SavuMacro {
	guint8 report_id; /* SAVU_REPORT_ID_MACRO */
	guint16 size; /* 0x0823 */
	guint8 profile_index; /* range 0-4 */
	guint8 button_index; /* range 0-13 */
	guint8 unused1; /* 0 */
	guint8 loop;
	guint8 unused2[24];
	guint8 macroset_name[SAVU_MACRO_MACROSET_NAME_LENGTH];
	guint8 macro_name[SAVU_MACRO_MACRO_NAME_LENGTH];
	guint16 count;
	RoccatKeystroke keystrokes[SAVU_MACRO_KEYSTROKES_NUM];
	guint16 checksum;
} __attribute__ ((packed));

SavuMacro *savu_macro_read(RoccatDevice *savu, guint profile_index, guint button_index, GError **error);
gboolean savu_macro_write(RoccatDevice *savu, guint profile_index, guint button_index, SavuMacro *macro, GError **error);
guint16 savu_macro_calc_checksum(SavuMacro const *macro);

static inline guint16 savu_macro_get_size(SavuMacro const *macro) {
	return GUINT16_FROM_LE(macro->size);
}

static inline void savu_macro_set_size(SavuMacro *macro, guint16 new_value) {
	macro->size = GUINT16_TO_LE(new_value);
}

static inline guint16 savu_macro_get_count(SavuMacro const *macro) {
	return GUINT16_FROM_LE(macro->count);
}

static inline void savu_macro_set_count(SavuMacro *macro, guint16 new_value) {
	macro->count = GUINT16_TO_LE(new_value);
}

static inline guint16 savu_macro_get_checksum(SavuMacro const *macro) {
	return GUINT16_FROM_LE(macro->checksum);
}

static inline void savu_macro_set_checksum(SavuMacro *macro, guint16 new_value) {
	macro->checksum = GUINT16_TO_LE(new_value);
}

/* also valid as SavuMacroButtonIndex */
typedef enum {
	SAVU_BUTTON_INDEX_LEFT = 0,
	SAVU_BUTTON_INDEX_RIGHT = 1,
	SAVU_BUTTON_INDEX_MIDDLE = 2,
	SAVU_BUTTON_INDEX_FORWARD = 3,
	SAVU_BUTTON_INDEX_BACKWARD = 4,
	SAVU_BUTTON_INDEX_WHEEL_UP = 5,
	SAVU_BUTTON_INDEX_WHEEL_DOWN = 6,
	SAVU_BUTTON_INDEX_SHIFT_LEFT = 7,
	SAVU_BUTTON_INDEX_SHIFT_RIGHT = 8,
	SAVU_BUTTON_INDEX_SHIFT_MIDDLE = 9,
	SAVU_BUTTON_INDEX_SHIFT_FORWARD = 10,
	SAVU_BUTTON_INDEX_SHIFT_BACKWARD = 11,
	SAVU_BUTTON_INDEX_SHIFT_WHEEL_UP = 12,
	SAVU_BUTTON_INDEX_SHIFT_WHEEL_DOWN = 13,
} SavuButtonIndex;

struct _SavuInfo {
	guint8 report_id; /* SAVU_REPORT_ID_INFO */
	guint8 size; /* 0x08 */
	guint8 firmware_version;
	guint8 unknown1;
	guint8 unknown2;
	guint8 unknown3;
	guint8 sensor_srom_id;
	guint8 unknown5;
} __attribute__ ((packed));

typedef enum {
	SAVU_INFO_FUNCTION_RESET = 0x6,
} SavuInfoFunction;

SavuInfo *savu_info_read(RoccatDevice *savu, GError **error);
guint savu_firmware_version_read(RoccatDevice *savu, GError **error);
gboolean savu_reset(RoccatDevice *savu, GError **error);

struct _SavuDeviceState {
	guint8 report_id; /* SAVU_REPORT_ID_DEVICE_STATE */
	guint8 size; /* 0x3 */
	guint8 state;
} __attribute__ ((packed));

typedef enum {
	/* Device sends HID multimedia events */
	SAVU_DEVICE_STATE_STATE_OFF = 0x00,
	/* Driver handles multimedia events */
	SAVU_DEVICE_STATE_STATE_ON = 0x01,
} SavuDeviceStateState;

gboolean savu_device_state_write(RoccatDevice *savu, guint state, GError **error);

struct _SavuSensor {
	guint8 report_id; /* SAVU_REPORT_ID_SENSOR */
	guint8 action;
	guint8 reg;
	guint8 value;
} __attribute__ ((packed));

typedef enum {
	SAVU_SENSOR_ACTION_WRITE = 1,
	SAVU_SENSOR_ACTION_READ = 2,
	SAVU_SENSOR_ACTION_FRAME_CAPTURE = 3,
} SavuSensorAction;

gboolean savu_sensor_write_value(RoccatDevice *savu, guint8 reg, guint8 value, GError **error);
guint8 savu_sensor_read_value(RoccatDevice *savu, guint8 reg, GError **error);

typedef enum {
	SAVU_REPORT_ID_SPECIAL = 0x03,
	SAVU_REPORT_ID_CONTROL = 0x04,
	SAVU_REPORT_ID_PROFILE = 0x05,
	SAVU_REPORT_ID_GENERAL = 0x06,
	SAVU_REPORT_ID_BUTTONS = 0x07,
	SAVU_REPORT_ID_MACRO = 0x08,
	SAVU_REPORT_ID_INFO = 0x09,
	SAVU_REPORT_ID_DEVICE_STATE = 0x0a, /* wo */
	SAVU_REPORT_ID_SENSOR = 0x0c,
	SAVU_REPORT_ID_SROM_WRITE = 0x0d,
	SAVU_REPORT_ID_FIRMWARE = 0x1b,
	SAVU_REPORT_ID_FIRMWARE_CONTROL = 0x1c,
} SavuReportId;

struct _SavuRmpTimer {
	guint32 seconds;
	guint8 name[SAVU_RMP_TIMER_NAME_LENGTH];
} __attribute__ ((packed));

RoccatTimer *savu_rmp_timer_to_roccat_timer(SavuRmpTimer const *rmp_timer);
SavuRmpTimer *savu_roccat_timer_to_rmp_timer(RoccatTimer const *timer);

static inline guint32 savu_rmp_timer_get_seconds(SavuRmpTimer const *timer) {
	return GUINT32_FROM_LE(timer->seconds);
}

static inline void savu_rmp_timer_set_seconds(SavuRmpTimer *timer, guint32 new_value) {
	timer->seconds = GUINT32_TO_LE(new_value);
}

struct _SavuRmpMacroKeyInfo {
	guint8 name[SAVU_RMP_MACRO_KEY_INFO_NAME_LENGTH];
	guint32 loop;
	guint32 count;
	RoccatKeystroke keystrokes[SAVU_MACRO_KEYSTROKES_NUM];
	guint8 checksum;
} __attribute__ ((packed));

SavuRmpMacroKeyInfo *savu_rmp_macro_key_info_new(void);
SavuRmpMacroKeyInfo *savu_rmp_macro_key_info_dup(SavuRmpMacroKeyInfo const *source);
void savu_rmp_macro_key_info_free(SavuRmpMacroKeyInfo *macro_key_info);
SavuMacro *savu_rmp_macro_key_info_to_macro(SavuRmpMacroKeyInfo const *macro_key_info);
GaminggearMacro *savu_rmp_macro_key_info_to_gaminggear_macro(SavuRmpMacroKeyInfo const *macro);
SavuRmpMacroKeyInfo *savu_gaminggear_macro_to_rmp_macro_key_info(GaminggearMacro const *gaminggear_macro, GError **error);

static inline guint32 savu_rmp_macro_key_info_get_loop(SavuRmpMacroKeyInfo const *macro_key_info) {
	return GUINT32_FROM_LE(macro_key_info->loop);
}

static inline void savu_rmp_macro_key_info_set_loop(SavuRmpMacroKeyInfo *macro_key_info, guint32 new_value) {
	macro_key_info->loop = GUINT32_TO_LE(new_value);
}

static inline guint32 savu_rmp_macro_key_info_get_count(SavuRmpMacroKeyInfo const *macro_key_info) {
	return GUINT32_FROM_LE(macro_key_info->count);
}

static inline void savu_rmp_macro_key_info_set_count(SavuRmpMacroKeyInfo *macro_key_info, guint32 new_value) {
	macro_key_info->count = GUINT32_TO_LE(new_value);
}

struct _SavuSpecial {
	guint8 report_id; /* SAVU_REPORT_ID_SPECIAL */
	guint8 zero;
	guint8 type;
	guint8 data[2];
} __attribute__ ((packed));

typedef enum {
	SAVU_SPECIAL_ACTION_PRESS = 0,
	SAVU_SPECIAL_ACTION_RELEASE = 1,
} SavuSpecialAction;

typedef enum {
	SAVU_SPECIAL_TYPE_TILT = 0x10,
	SAVU_SPECIAL_TYPE_PROFILE = 0x20,

	/* data[0] = count, usually 0x0a */
	SAVU_SPECIAL_TYPE_RAD_LEFT = 0x30,
	SAVU_SPECIAL_TYPE_RAD_RIGHT = 0x31,
	SAVU_SPECIAL_TYPE_RAD_MIDDLE = 0x32,
	SAVU_SPECIAL_TYPE_RAD_THUMB_1 = 0x33,
	SAVU_SPECIAL_TYPE_RAD_THUMB_2 = 0x34,
	SAVU_SPECIAL_TYPE_RAD_SCROLL_UP = 0x35,
	SAVU_SPECIAL_TYPE_RAD_SCROLL_DOWN = 0x36,
	SAVU_SPECIAL_TYPE_RAD_EASYSHIFT = 0x37,
	SAVU_SPECIAL_TYPE_RAD_EASYAIM = 0x38,
	SAVU_SPECIAL_TYPE_RAD_DISTANCE = 0x39, // TOTO unit

	SAVU_SPECIAL_TYPE_QUICKLAUNCH = 0x60,
	SAVU_SPECIAL_TYPE_TIMER = 0x80,
	SAVU_SPECIAL_TYPE_TIMER_STOP = 0x90,
	SAVU_SPECIAL_TYPE_OPEN_DRIVER = 0xa0,
	SAVU_SPECIAL_TYPE_CPI = 0xb0,
	SAVU_SPECIAL_TYPE_SENSITIVITY = 0xc0,
	SAVU_SPECIAL_TYPE_MULTIMEDIA = 0xf0,
} SavuSpecialType;

typedef enum {
	SAVU_INTERFACE_MOUSE = 0,
	SAVU_INTERFACE_KEYBOARD = 1,
} SavuInterface;

enum {
	// SAVU_FIRMWARE_UPDATE_WAIT_INIT = 2330,
	SAVU_FIRMWARE_UPDATE_WAIT_PREPARE = 320,
	SAVU_FIRMWARE_UPDATE_WAIT_INIT_DATA = 2330,
	SAVU_FIRMWARE_UPDATE_WAIT_DATA = 330,
	SAVU_FIRMWARE_UPDATE_FINAL_WRITE_CHECK = TRUE,
	SAVU_FIRMWARE_SIZE = 24076,
};

RoccatDevice *savu_device_first(void);
RoccatDeviceScanner *savu_device_scanner_new(void);
gchar *savu_device_read(RoccatDevice *roccat_device, guint report_id, gssize length, GError **error);
gboolean savu_device_write(RoccatDevice *roccat_device, gchar const *buffer, gssize length, GError **error);

SavuRmp *savu_rmp_load(RoccatDevice *savu, guint profile_index, GError **error);
gboolean savu_rmp_save(RoccatDevice *savu, SavuRmp *rmp, guint profile_index, GError **error);
SavuRmp *savu_rmp_load_save_after_reset(RoccatDevice *savu, guint profile_index, GError **error);
SavuRmp *savu_rmp_load_actual(guint profile_index);
gboolean savu_rmp_save_actual(SavuRmp *rmp, guint profile_index, GError **error);
void savu_rmp_update_from_device(RoccatDevice *savu, SavuRmp *rmp, guint profile_index);
gboolean savu_rmp_get_modified(SavuRmp const *rmp);
void savu_rmp_set_modified(SavuRmp *rmp);
void savu_rmp_set_unmodified(SavuRmp *rmp);
void savu_rmp_free(SavuRmp *rmp);
SavuRmp *savu_rmp_dup(SavuRmp const *src);
SavuRmp *savu_rmp_read_with_path(gchar const *path, GError **error);
gboolean savu_rmp_write_with_path(gchar const *path, SavuRmp *rmp, GError **error);
SavuRmp const *savu_default_rmp(void);

/* returns NULL if button is not a macro */
SavuMacro *savu_rmp_to_macro(SavuRmp *rmp, guint index);

SavuButtons *savu_rmp_to_buttons(SavuRmp *rmp);
SavuGeneral *savu_rmp_to_general(SavuRmp *rmp);

void savu_rmp_update_with_roccat_timer(SavuRmp *rmp, guint index, RoccatTimer const *timer);
void savu_rmp_update_with_macro(SavuRmp *rmp, guint index, SavuMacro const *macro);
void savu_rmp_update_with_general(SavuRmp *rmp, SavuGeneral *general);
void savu_rmp_update_with_buttons(SavuRmp *rmp, SavuButtons *buttons);

gchar *savu_rmp_get_profile_name(SavuRmp *rmp);
void savu_rmp_set_profile_name(SavuRmp *rmp, gchar const *string);

/* Deceptive: A value of 1 means async (Advanced on)
 * Windows driver stores a value of 6 for x and y if this value is 0 (Advanced off)
 */
void savu_rmp_set_xy_synchronous(SavuRmp *rmp, guint value);
guint savu_rmp_get_xy_synchronous(SavuRmp *rmp);

void savu_rmp_set_sensitivity_x(SavuRmp *rmp, guint value);
guint savu_rmp_get_sensitivity_x(SavuRmp *rmp);
void savu_rmp_set_sensitivity_y(SavuRmp *rmp, guint value);
guint savu_rmp_get_sensitivity_y(SavuRmp *rmp);
void savu_rmp_set_color_red(SavuRmp *rmp, guint value);
guint savu_rmp_get_color_red(SavuRmp *rmp);
void savu_rmp_set_color_green(SavuRmp *rmp, guint value);
guint savu_rmp_get_color_green(SavuRmp *rmp);
void savu_rmp_set_color_blue(SavuRmp *rmp, guint value);
guint savu_rmp_get_color_blue(SavuRmp *rmp);
void savu_rmp_set_polling_rate(SavuRmp *rmp, guint value);
guint savu_rmp_get_polling_rate(SavuRmp *rmp);
void savu_rmp_set_color_database(SavuRmp *rmp, guint value);
guint savu_rmp_get_color_database(SavuRmp *rmp);
void savu_rmp_set_cpi_step(SavuRmp *rmp, guint value);
guint savu_rmp_get_cpi_step(SavuRmp *rmp);
void savu_rmp_set_cpi(SavuRmp *rmp, guint index, guint value);
guint savu_rmp_get_cpi(SavuRmp *rmp, guint index);
void savu_rmp_set_light_switch(SavuRmp *rmp, guint value);
guint savu_rmp_get_light_switch(SavuRmp *rmp);
void savu_rmp_set_breath_on(SavuRmp *rmp, guint value);
guint savu_rmp_get_breath_on(SavuRmp *rmp);
void savu_rmp_set_color_flow(SavuRmp *rmp, guint value);
guint savu_rmp_get_color_flow(SavuRmp *rmp);
SavuRmpMacroKeyInfo *savu_rmp_get_macro_key_info(SavuRmp *rmp, guint index);
void savu_rmp_set_macro_key_info(SavuRmp *rmp, guint index, SavuRmpMacroKeyInfo const *key_info);
SavuRmpTimer *savu_rmp_get_timer(SavuRmp *rmp, guint index);
void savu_rmp_set_timer(SavuRmp *rmp, guint index, SavuRmpTimer const *timer);
RoccatButton *savu_rmp_get_button(SavuRmp *rmp, guint index);
void savu_rmp_set_button(SavuRmp *rmp, guint index, RoccatButton const *button);
gchar *savu_rmp_get_game_file(SavuRmp *rmp, guint index);
void savu_rmp_set_game_file(SavuRmp *rmp, guint index, gchar const *string);
gchar *savu_rmp_get_launchpath(SavuRmp *rmp, guint index);
void savu_rmp_set_launchpath(SavuRmp *rmp, guint index, gchar const *string);

RoccatKeyFile *savu_configuration_load(void);
void savu_configuration_free(RoccatKeyFile *config);
gboolean savu_configuration_save(RoccatKeyFile *config, GError **error);

gchar *savu_configuration_get_rmp_path(RoccatKeyFile *config);
void savu_configuration_set_rmp_path(RoccatKeyFile *config, gchar const *path);

gchar *savu_configuration_get_macro_path(RoccatKeyFile *config);
void savu_configuration_set_macro_path(RoccatKeyFile *config, gchar const *path);

gchar *savu_configuration_get_timer_path(RoccatKeyFile *config);
void savu_configuration_set_timer_path(RoccatKeyFile *config, gchar const *path);

gint savu_configuration_get_timer_notification_type(RoccatKeyFile *config);
void savu_configuration_set_timer_notification_type(RoccatKeyFile *config, gint new_value);

gint savu_configuration_get_profile_notification_type(RoccatKeyFile *config);
void savu_configuration_set_profile_notification_type(RoccatKeyFile *config, gint new_value);

gint savu_configuration_get_sensitivity_notification_type(RoccatKeyFile *config);
void savu_configuration_set_sensitivity_notification_type(RoccatKeyFile *config, gint new_value);

gint savu_configuration_get_cpi_notification_type(RoccatKeyFile *config);
void savu_configuration_set_cpi_notification_type(RoccatKeyFile *config, gint new_value);

gdouble savu_configuration_get_notification_volume(RoccatKeyFile *config);
void savu_configuration_set_notification_volume(RoccatKeyFile *config, gdouble new_value);

gint savu_configuration_get_default_profile_number(RoccatKeyFile *config);
void savu_configuration_set_default_profile_number(RoccatKeyFile *config, gint new_value);

#define SAVU_DEVICE_NAME "Savu"

#define SAVU_DBUS_SERVER_PATH "/org/roccat/Savu"
#define SAVU_DBUS_SERVER_INTERFACE "org.roccat.Savu"

DBusGProxy *savu_dbus_proxy_new(void);
gboolean savu_dbus_emit_profile_data_changed_outside(DBusGProxy *proxy, guint profile_number);
gboolean savu_dbus_emit_profile_data_changed_outside_instant(guint profile_number);
gboolean savu_dbus_emit_profile_changed_outside(DBusGProxy *proxy, guint profile_number);
gboolean savu_dbus_emit_profile_changed_outside_instant(guint profile_number);
gboolean savu_dbus_emit_configuration_changed_outside(DBusGProxy *proxy);

gboolean savu_play_quicklaunch_forked(SavuRmp *rmp, guint index);

G_END_DECLS

#endif
