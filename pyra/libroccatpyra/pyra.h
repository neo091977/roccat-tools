#ifndef __ROCCAT_PYRA_H__
#define __ROCCAT_PYRA_H__

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
 * \file pyra.h
 * \brief definitions for libroccatpyra
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

#define USB_DEVICE_ID_ROCCAT_PYRA_WIRED 0x2c24
#define USB_DEVICE_ID_ROCCAT_PYRA_WIRELESS 0x2cf6

typedef struct _Pyra2 Pyra2;
typedef struct _Pyra3 Pyra3;
typedef struct _PyraA PyraA;
typedef struct _PyraB PyraB;
typedef struct _PyraControl PyraControl;
typedef struct _PyraSettings PyraSettings;
typedef struct _PyraProfileSettings PyraProfileSettings;
typedef struct _PyraProfileButtons PyraProfileButtons;
typedef struct _PyraInfo PyraInfo;
typedef struct _PyraSpecial PyraSpecial;
typedef struct _PyraRMPMacroKeyInfo PyraRMPMacroKeyInfo;
typedef struct _PyraRMP PyraRMP;

struct _Pyra2 {
	guint8 report_id; /* PYRA_REPORT_ID_2 */
	guint8 unknown[2];
} __attribute__ ((packed));

struct _Pyra3 {
	guint8 report_id; /* PYRA_REPORT_ID_3 */
	guint8 unknown[4];
} __attribute__ ((packed));

struct _PyraA {
	guint8 report_id; /* PYRA_REPORT_ID_A */
	guint8 size; /* always 5 */
	guint8 unknown[3];
} __attribute__ ((packed));

struct _PyraB {
	guint8 report_id; /* PYRA_REPORT_ID_B */
	guint8 size; /* always 3 */
	guint8 unknown; /* 1 */
} __attribute__ ((packed));

struct _PyraControl {
	guint8 report_id; /* PYRA_REPORT_ID_CONTROL */
	/*
	 * value is profile number for request_settings and request_buttons
	 * 1 if status ok for request_status
	 */
	guint8 value; /* Range 0-4 */
	guint8 request;
} __attribute__ ((packed));

typedef enum {
	PYRA_CONTROL_REQUEST_STATUS = 0x00,
	PYRA_CONTROL_REQUEST_PROFILE_SETTINGS = 0x10,
	PYRA_CONTROL_REQUEST_PROFILE_BUTTONS = 0x20,
} PyraControlRequest;

enum {
	PYRA_PROFILE_NUM = 5,
	PYRA_PROFILE_MIN = 0,
	PYRA_PROFILE_MAX = 4,
};

struct _PyraSettings {
	guint8 report_id; /* PYRA_REPORT_ID_SETTINGS */
	guint8 size; /* always 3 */
	guint8 startup_profile; /* Range 0-4! */
} __attribute__ ((packed));

enum {
	PYRA_SENSITIVITY_MIN = 0x1,
	PYRA_SENSITIVITY_MAX = 0xa,
	PYRA_BUTTONS_NUM = 14,
	PYRA_CPI_NUM = 3,
	PYRA_GAMEFILE_NUM = 3,
};

struct _PyraProfileSettings {
	guint8 report_id; /* PYRA_REPORT_ID_PROFILE_SETTINGS */
	guint8 size; /* always 0xd */
	guint8 number; /* Range 0-4 */
	guint8 xy_synchronous;
	guint8 x_sensitivity; /* 0x1-0xa */
	guint8 y_sensitivity;
	guint8 x_cpi;
	guint8 y_cpi;
	guint8 lightswitch; /* 0 = off, 1 = on */
	guint8 lighttype;
	guint8 orientation;
	guint16 checksum; /* byte sum */
} __attribute__ ((packed));

typedef enum {
	PYRA_CPI_400 = 0x1,
	PYRA_CPI_800 = 0x2,
	PYRA_CPI_1600 = 0x4,
} PyraCpi;

typedef enum {
	PYRA_XY_SYNC_OFF = 0,
	PYRA_XY_SYNC_ON = 1,
} PyraXYSync;

typedef enum {
	PYRA_LIGHTSWITCH_OFF = 0,
	PYRA_LIGHTSWITCH_ON = 1,
} PyraLightswitch;

typedef enum {
	PYRA_LIGHT_TYPE_FULL_LIGHTENED = 0,
	PYRA_LIGHT_TYPE_PULSATING = 1,
} PyraLightType;

typedef enum {
	PYRA_ORIENTATION_RIGHT = 0,
	PYRA_ORIENTATION_LEFT = 1,
} PyraOrientation;

struct _PyraProfileButtons {
	guint8 report_id; /* PYRA_REPORT_ID_PROFILE_BUTTONS */
	guint8 size; /* always 0x13 */
	guint8 number; /* Range 0-4 */
	guint8 buttons[PYRA_BUTTONS_NUM];
	guint16 checksum; /* byte sum */
} __attribute__ ((packed));

typedef enum {
	PYRA_PROFILE_BUTTON_INDEX_LEFT = 0,
	PYRA_PROFILE_BUTTON_INDEX_RIGHT = 1,
	PYRA_PROFILE_BUTTON_INDEX_MIDDLE = 2,
	PYRA_PROFILE_BUTTON_INDEX_WHEEL_UP = 3,
	PYRA_PROFILE_BUTTON_INDEX_WHEEL_DOWN = 4,
	PYRA_PROFILE_BUTTON_INDEX_SHIFT_LEFT = 5,
	PYRA_PROFILE_BUTTON_INDEX_SHIFT_RIGHT = 6,
	PYRA_PROFILE_BUTTON_INDEX_SHIFT_MIDDLE = 7,
	PYRA_PROFILE_BUTTON_INDEX_SHIFT_WHEEL_UP = 8,
	PYRA_PROFILE_BUTTON_INDEX_SHIFT_WHEEL_DOWN = 9,
	PYRA_PROFILE_BUTTON_INDEX_LEFT_SIDE = 10,
	PYRA_PROFILE_BUTTON_INDEX_RIGHT_SIDE = 11,
	PYRA_PROFILE_BUTTON_INDEX_SHIFT_LEFT_SIDE = 12,
	PYRA_PROFILE_BUTTON_INDEX_SHIFT_RIGHT_SIDE = 13,
} PyraProfileButtonIndex;

struct _PyraInfo {
	guint8 report_id; /* PYRA_REPORT_ID_INFO */
	guint8 size; /* always 6 */
	guint8 firmware_version;
	guint8 unknown1; /* always 0 */
	guint8 unknown2; /* always 1 */
	guint8 unknown3; /* always 0 */
} __attribute__ ((packed));

typedef enum {
	PYRA_INFO_FUNCTION_RESET = 0x07,
} PyraInfoFunction;

typedef enum {
	PYRA_REPORT_ID_MOUSE = 0x1,
	PYRA_REPORT_ID_2 = 0x2, /* only wireless firmware 1.24 */
	PYRA_REPORT_ID_SPECIAL = 0x3, /* only wireless firmware 1.24 */
	PYRA_REPORT_ID_CONTROL = 0x4,
	PYRA_REPORT_ID_SETTINGS = 0x5,
	PYRA_REPORT_ID_PROFILE_SETTINGS = 0x6,
	PYRA_REPORT_ID_PROFILE_BUTTONS = 0x7,
	PYRA_REPORT_ID_INFO = 0x9,
	PYRA_REPORT_ID_A = 0xa, /* only wireless firmware 1.24 */
	PYRA_REPORT_ID_B = 0xb, /* writes 3 bytes */
} PyraReportId;

typedef enum {
	PYRA_MOUSE_EVENT_HID = 1,
	PYRA_MOUSE_EVENT_2 = 2,
	PYRA_MOUSE_EVENT_BUTTON = 3,
	PYRA_MOUSE_EVENT_PROFILE = 4,
} PyraMouseEvent;

struct _PyraSpecial {
	guint8 report_id;
	guint8 unknown;
	guint8 type;
	guint8 data1;
	guint8 data2;
} __attribute__ ((packed));

enum {
	PYRA_SPECIAL_BUTTON_PRESS = 0,
	PYRA_SPECIAL_BUTTON_RELEASE = 1,
};

typedef enum {
	PYRA_SPECIAL_TYPE_TILT = 0x10,
	PYRA_SPECIAL_TYPE_PROFILE_1 = 0x20,
	PYRA_SPECIAL_TYPE_PROFILE_2 = 0x30,
	PYRA_SPECIAL_TYPE_MACRO = 0x40,
	PYRA_SPECIAL_TYPE_SHORTCUT = 0x50,
	PYRA_SPECIAL_TYPE_QUICKLAUNCH = 0x60,
	PYRA_SPECIAL_TYPE_CPI = 0xb0,
	PYRA_SPECIAL_TYPE_SENSITIVITY = 0xc0,
	PYRA_SPECIAL_TYPE_MULTIMEDIA = 0xf0,
} PyraSpecialType;

enum {
	PYRA_RMP_MACRO_KEY_INFO_MACRO_SET_NAME_LENGTH = 50,
	PYRA_RMP_MACRO_KEY_INFO_MACRO_NAME_LENGTH = 50,
	PYRA_RMP_MACRO_KEY_INFO_KEYSTROKES_NUM = 500,
	PYRA_RMP_MACRO_KEY_INFO_FILENAME_LENGTH = 262,
};

typedef enum {
	PYRA_INTERFACE_MOUSE = 0,
	PYRA_INTERFACE_KEYBOARD = 1,
} PyraInterface;

/* pyra driver uses uninitialized memory so unused values don't have to mean something */
struct _PyraRMPMacroKeyInfo {
	guint8 number;
	guint8 type;
	guint8 macro_set_name[PYRA_RMP_MACRO_KEY_INFO_MACRO_SET_NAME_LENGTH];
	guint8 macro_name[PYRA_RMP_MACRO_KEY_INFO_MACRO_NAME_LENGTH];
	guint8 unused1[2];
	guint8 loop; /* 1-255 if its of type macro */
	guint8 unused2[3];
	guint16 count;
	RoccatKeystroke keystrokes[PYRA_RMP_MACRO_KEY_INFO_KEYSTROKES_NUM];
	guint8 filename[PYRA_RMP_MACRO_KEY_INFO_FILENAME_LENGTH]; /* used by quicklaunch */
	guint8 checksum;
} __attribute__ ((packed));

typedef enum {
	PYRA_RMP_MACRO_KEY_INDEX_LEFT = 0,
	PYRA_RMP_MACRO_KEY_INDEX_RIGHT = 1,
	PYRA_RMP_MACRO_KEY_INDEX_MIDDLE = 2,
	PYRA_RMP_MACRO_KEY_INDEX_LEFT_SIDE = 3,
	PYRA_RMP_MACRO_KEY_INDEX_RIGHT_SIDE = 4,
	PYRA_RMP_MACRO_KEY_INDEX_WHEEL_UP = 5,
	PYRA_RMP_MACRO_KEY_INDEX_WHEEL_DOWN = 6,
	PYRA_RMP_MACRO_KEY_INDEX_SHIFT_LEFT = 7,
	PYRA_RMP_MACRO_KEY_INDEX_SHIFT_RIGHT = 8,
	PYRA_RMP_MACRO_KEY_INDEX_SHIFT_MIDDLE = 9,
	PYRA_RMP_MACRO_KEY_INDEX_SHIFT_LEFT_SIDE = 10,
	PYRA_RMP_MACRO_KEY_INDEX_SHIFT_RIGHT_SIDE = 11,
	PYRA_RMP_MACRO_KEY_INDEX_SHIFT_WHEEL_UP = 12,
	PYRA_RMP_MACRO_KEY_INDEX_SHIFT_WHEEL_DOWN = 13,
} PyraRMPMacroKeyIndex;

typedef enum {
	PYRA_RMP_MACRO_KEY_INFO_TYPE_UNUSED = 0x0, /* button is disabled by system */
	PYRA_RMP_MACRO_KEY_INFO_TYPE_CLICK = 0x1, /* left mouse button */
	PYRA_RMP_MACRO_KEY_INFO_TYPE_MENU = 0x2, /* right mouse button */
	PYRA_RMP_MACRO_KEY_INFO_TYPE_UNIVERSAL_SCROLLING = 0x3, /* wheel button */
	PYRA_RMP_MACRO_KEY_INFO_TYPE_DOUBLE_CLICK = 0x4,
	PYRA_RMP_MACRO_KEY_INFO_TYPE_SHORTCUT = 0x5, /* single button with modifiers */
	PYRA_RMP_MACRO_KEY_INFO_TYPE_DISABLED = 0x6, /* button is disabled by user */
	PYRA_RMP_MACRO_KEY_INFO_TYPE_BROWSER_FORWARD = 0x7, /* mouse button 5 */
	PYRA_RMP_MACRO_KEY_INFO_TYPE_BROWSER_BACKWARD = 0x8,
	PYRA_RMP_MACRO_KEY_INFO_TYPE_TILT_LEFT = 0x9,
	PYRA_RMP_MACRO_KEY_INFO_TYPE_TILT_RIGHT = 0xa,
	PYRA_RMP_MACRO_KEY_INFO_TYPE_SCROLL_UP = 0xd,
	PYRA_RMP_MACRO_KEY_INFO_TYPE_SCROLL_DOWN = 0xe,
	PYRA_RMP_MACRO_KEY_INFO_TYPE_QUICKLAUNCH = 0xf,
	PYRA_RMP_MACRO_KEY_INFO_TYPE_PROFILE_CYCLE = 0x10,
	PYRA_RMP_MACRO_KEY_INFO_TYPE_PROFILE_UP = 0x11,
	PYRA_RMP_MACRO_KEY_INFO_TYPE_PROFILE_DOWN = 0x12,
	PYRA_RMP_MACRO_KEY_INFO_TYPE_CPI_CYCLE = 0x14,
	PYRA_RMP_MACRO_KEY_INFO_TYPE_CPI_UP = 0x15,
	PYRA_RMP_MACRO_KEY_INFO_TYPE_CPI_DOWN = 0x16,
	PYRA_RMP_MACRO_KEY_INFO_TYPE_MULTIMEDIA_OPEN_PLAYER = 0x20,
	PYRA_RMP_MACRO_KEY_INFO_TYPE_MULTIMEDIA_PREV_TRACK = 0x21,
	PYRA_RMP_MACRO_KEY_INFO_TYPE_MULTIMEDIA_NEXT_TRACK = 0x22,
	PYRA_RMP_MACRO_KEY_INFO_TYPE_MULTIMEDIA_PLAY_PAUSE = 0x23,
	PYRA_RMP_MACRO_KEY_INFO_TYPE_MULTIMEDIA_STOP = 0x24,

	/* next 3 are sent as standard hid audio controls */
	PYRA_RMP_MACRO_KEY_INFO_TYPE_MULTIMEDIA_MUTE = 0x25,
	PYRA_RMP_MACRO_KEY_INFO_TYPE_MULTIMEDIA_VOLUME_DOWN = 0x26,
	PYRA_RMP_MACRO_KEY_INFO_TYPE_MULTIMEDIA_VOLUME_UP = 0x27,

	PYRA_RMP_MACRO_KEY_INFO_TYPE_MACRO = 0x30,
	PYRA_RMP_MACRO_KEY_INFO_TYPE_DISABLED2 = 0x40, /* unknown when its used */
	PYRA_RMP_MACRO_KEY_INFO_TYPE_EASYSHIFT = 0x41,
	PYRA_RMP_MACRO_KEY_INFO_TYPE_EASYWHEEL_SENSITIVITY = 0x42,
	PYRA_RMP_MACRO_KEY_INFO_TYPE_EASYWHEEL_PROFILE = 0x43,
	PYRA_RMP_MACRO_KEY_INFO_TYPE_EASYWHEEL_CPI = 0x44,
	PYRA_RMP_MACRO_KEY_INFO_TYPE_EASYWHEEL_VOLUME = 0x45,
} PyraRMPMacroKeyInfoType;

/*!
 * \defgroup rmp RMP handling
 * \defgroup rmp_access RMP accessors
 */


static inline guint16 pyra_rmp_macro_key_info_get_count(PyraRMPMacroKeyInfo const *macro_key_info) {
	return GUINT16_FROM_LE(macro_key_info->count);
}

static inline void pyra_rmp_macro_key_info_set_count(PyraRMPMacroKeyInfo *macro_key_info, guint16 new_value) {
	macro_key_info->count = GUINT16_TO_LE(new_value);
}

guint pyra_cpi_raw_to_cpi(guint cpi_raw);
guint pyra_actual_profile_read(RoccatDevice *pyra, GError **error);
gboolean pyra_actual_profile_write(RoccatDevice *pyra, guint profile_index, GError **error);

PyraInfo *pyra_info_read(RoccatDevice *pyra, GError **error);
gboolean pyra_reset(RoccatDevice *pyra, GError **error);
guint pyra_firmware_version_read(RoccatDevice *pyra, GError **error);

RoccatDevice *pyra_device_first(void);
RoccatDeviceScanner *pyra_device_scanner_new(void);
gchar *pyra_device_read(RoccatDevice *roccat_device, guint report_id, gssize length, GError **error);
gboolean pyra_device_write(RoccatDevice *roccat_device, gchar const *buffer, gssize length, GError **error);

RoccatKeyFile *pyra_configuration_load(void);
void pyra_configuration_free(RoccatKeyFile *config);
gboolean pyra_configuration_save(RoccatKeyFile *config, GError **error);

gchar *pyra_configuration_get_rmp_save_path(RoccatKeyFile *config);
void pyra_configuration_set_rmp_save_path(RoccatKeyFile *config, gchar const *path);

gint pyra_configuration_get_profile_notification_type(RoccatKeyFile *config);
void pyra_configuration_set_profile_notification_type(RoccatKeyFile *config, gint new_value);
gint pyra_configuration_get_cpi_notification_type(RoccatKeyFile *config);
void pyra_configuration_set_cpi_notification_type(RoccatKeyFile *config, gint new_value);
gdouble pyra_configuration_get_notification_volume(RoccatKeyFile *config);
void pyra_configuration_set_notification_volume(RoccatKeyFile *config, gdouble new_value);
gint pyra_configuration_get_default_profile_number(RoccatKeyFile *config);
void pyra_configuration_set_default_profile_number(RoccatKeyFile *config, gint new_value);

/*!
 * \ingroup rmp
 */
PyraRMP *pyra_rmp_read_with_path(gchar const *path, GError **error);

/*!
 * \ingroup rmp
 */
gboolean pyra_rmp_write_with_path(gchar const *path, PyraRMP *rmp, GError **error);

/*!
 * \ingroup rmp
 */
void pyra_rmp_free(PyraRMP *rmp);

/*!
 * \ingroup rmp
 */
gboolean pyra_rmp_get_modified(PyraRMP const *rmp);

/*!
 * \ingroup rmp
 */
void pyra_rmp_set_modified(PyraRMP *rmp);

/*!
 * \ingroup rmp
 */
void pyra_rmp_set_unmodified(PyraRMP *rmp);

/*!
 * \ingroup rmp
 */
PyraRMP const *pyra_default_rmp(void);

PyraRMP *pyra_rmp_dup(PyraRMP const *src);

gboolean pyra_select(RoccatDevice *pyra, guint profile_index, guint request, GError **error);

void pyra_profile_buttons_finalize(PyraProfileButtons *profile_buttons, guint index);
PyraProfileButtons *pyra_profile_buttons_read(RoccatDevice *pyra, guint profile_number, GError **error);
gboolean pyra_profile_buttons_write(RoccatDevice *pyra, guint profile_number, PyraProfileButtons *profile_buttons, GError **error);

void pyra_profile_settings_finalize(PyraProfileSettings *profile_settings, guint index);
gboolean pyra_profile_settings_write(RoccatDevice *pyra, guint profile_number, PyraProfileSettings *profile_settings, GError **error);
PyraProfileSettings *pyra_profile_settings_read(RoccatDevice *pyra, guint profile_number, GError **error);


/*!
 * \ingroup rmp_access
 */
void pyra_rmp_set_macro_key_info(PyraRMP *rmp, guint index, PyraRMPMacroKeyInfo *rmp_macro_key_info);

/*!
 * \ingroup rmp_access
 */
void pyra_rmp_set_profile_name(PyraRMP *rmp, gchar const *string);

/*!
 * \ingroup rmp_access
 */
void pyra_rmp_set_orientation(PyraRMP *rmp, guint value);

/*!
 * \ingroup rmp_access
 * Seems to have no functionality in firmware 1.13
 */
void pyra_rmp_set_xy_synchronous(PyraRMP *rmp, guint value);

/*!
 * \ingroup rmp_access
 */
void pyra_rmp_set_sensitivity_x(PyraRMP *rmp, guint value);

/*!
 * \ingroup rmp_access
 */
void pyra_rmp_set_sensitivity_y(PyraRMP *rmp, guint value);

/*!
 * \ingroup rmp_access
 * \brief sets both cpi values to value
 * Firmware 1.13: different x and y make no sense, y takes precedence
 */
void pyra_rmp_set_cpi(PyraRMP *rmp, guint value);

/*!
 * \ingroup rmp_access
 */
void pyra_rmp_set_cpi_x(PyraRMP *rmp, guint value);

/*!
 * \ingroup rmp_access
 */
void pyra_rmp_set_cpi_y(PyraRMP *rmp, guint value);

/*!
 * \ingroup rmp_access
 */
void pyra_rmp_set_light_switch(PyraRMP *rmp, guint value);

/*!
 * \ingroup rmp_access
 */
void pyra_rmp_set_light_type(PyraRMP *rmp, guint value);

/*!
 * \ingroup rmp_access
 */
PyraRMPMacroKeyInfo *pyra_rmp_get_macro_key_info(PyraRMP *rmp, guint index);

/*!
 * \ingroup rmp_access
 */
gchar *pyra_rmp_get_profile_name(PyraRMP *rmp);

/*!
 * \ingroup rmp_access
 */
guint pyra_rmp_get_orientation(PyraRMP *rmp);

/*!
 * \ingroup rmp_access
 */
guint pyra_rmp_get_xy_synchronous(PyraRMP *rmp);

/*!
 * \ingroup rmp_access
 */
guint pyra_rmp_get_sensitivity_x(PyraRMP *rmp);

/*!
 * \ingroup rmp_access
 */
guint pyra_rmp_get_sensitivity_y(PyraRMP *rmp);

/*!
 * \ingroup rmp_access
 * \brief gets effective cpi value
 */
guint pyra_rmp_get_cpi(PyraRMP *rmp);

/*!
 * \ingroup rmp_access
 */
guint pyra_rmp_get_cpi_x(PyraRMP *rmp);

/*!
 * \ingroup rmp_access
 */
guint pyra_rmp_get_cpi_y(PyraRMP *rmp);

/*!
 * \ingroup rmp_access
 */
guint pyra_rmp_get_light_switch(PyraRMP *rmp);

/*!
 * \ingroup rmp_access
 */
guint pyra_rmp_get_light_type(PyraRMP *rmp);

void pyra_rmp_set_game_file_name(PyraRMP *rmp, guint index, gchar const *string);
gchar *pyra_rmp_get_game_file_name(PyraRMP *rmp, guint index);

guint8 pyra_rmp_macro_key_info_calc_checksum(PyraRMPMacroKeyInfo const *rmp_macro_key_info);
void pyra_rmp_macro_key_info_set_checksum(PyraRMPMacroKeyInfo *rmp_macro_key_info);

PyraRMPMacroKeyInfo *gaminggear_macro_to_pyra_rmp_macro_key_info(GaminggearMacro const *gaminggear_macro, GError **error);
GaminggearMacro *pyra_rmp_macro_key_info_to_gaminggear_macro(PyraRMPMacroKeyInfo const *button_info);

gboolean pyra_rmp_macro_key_info_equal(PyraRMPMacroKeyInfo const *left, PyraRMPMacroKeyInfo const *right);
PyraRMPMacroKeyInfo *pyra_rmp_macro_key_info_dup(PyraRMPMacroKeyInfo const *source);

PyraRMPMacroKeyInfo *pyra_rmp_macro_key_info_new_special(PyraRMPMacroKeyInfoType type);

PyraRMPMacroKeyInfo *pyra_rmp_macro_key_info_new_quicklaunch(gchar const *filename);

PyraRMPMacroKeyInfo *pyra_rmp_macro_key_info_new_shortcut(guint hid_usage_id, gboolean ctrl, gboolean shift, gboolean alt, gboolean win);

/*!
 * \ingroup rmp
 */
PyraProfileButtons *pyra_rmp_to_profile_buttons(PyraRMP *rmp);

/*!
 * \ingroup rmp
 */
PyraProfileSettings *pyra_rmp_to_profile_settings(PyraRMP *rmp);

/*!
 * \ingroup rmp
 */
void pyra_rmp_update_with_profile_buttons(PyraRMP *rmp, PyraProfileButtons const *profile_buttons);

/*!
 * \ingroup rmp
 */
void pyra_rmp_update_with_profile_settings(PyraRMP *rmp, PyraProfileSettings const *profile_settings);

/*!
 * \ingroup rmp
 * Compares the actual rmp with the actual hardware profile.
 * Returns default rmp updated with hardware profile if they differ or if rmp
 * couldn't be read.
 * Returns actual rmp if hardware profile couldn't be read.
 * Returns default rmp if neither could be found.
 */
PyraRMP *pyra_rmp_load(RoccatDevice *pyra, guint profile_index, GError **error);

/*!
 * \ingroup rmp
 */
gboolean pyra_rmp_save(RoccatDevice *pyra, PyraRMP *rmp, guint profile_index, GError **error);
PyraRMP *pyra_rmp_load_save_after_reset(RoccatDevice *pyra, guint profile_index, GError **error);

PyraRMP *pyra_rmp_load_actual(guint profile_index);
gboolean pyra_rmp_save_actual(PyraRMP *rmp, guint profile_index, GError **error);
void pyra_rmp_update_from_device(PyraRMP *rmp, RoccatDevice *pyra, guint profile_index);

gboolean pyra_play_shortcut_press(PyraRMPMacroKeyInfo const *macro_key_info);
gboolean pyra_play_shortcut_release(PyraRMPMacroKeyInfo const *macro_key_info);

/* does a double fork to execute application */
gboolean pyra_play_quicklaunch_forked(PyraRMPMacroKeyInfo const *macro_key_info);

PyraSettings *pyra_settings_read(RoccatDevice *pyra, GError **error);
gboolean pyra_settings_write(RoccatDevice *pyra, PyraSettings *settings, GError **error);

#define PYRA_DBUS_SERVER_PATH "/org/roccat/Pyra"
#define PYRA_DBUS_SERVER_INTERFACE "org.roccat.Pyra"

DBusGProxy *pyra_dbus_proxy_new(void);
gboolean pyra_dbus_emit_profile_data_changed_outside(DBusGProxy *proxy, guint profile_number);
gboolean pyra_dbus_emit_profile_data_changed_outside_instant(guint profile_number);
gboolean pyra_dbus_emit_profile_changed_outside(DBusGProxy *proxy, guint profile_number);
gboolean pyra_dbus_emit_profile_changed_outside_instant(guint profile_number);
gboolean pyra_dbus_emit_configuration_changed_outside(DBusGProxy *proxy);

#define PYRA_DEVICE_NAME "Pyra"
#define PYRA_DEVICE_NAME_WIRED "Pyra Wired"
#define PYRA_DEVICE_NAME_WIRELESS "Pyra Wireless"
#define PYRA_DEVICE_NAME_COMBINED "Pyra Wired/Wireless"

G_END_DECLS

#endif
