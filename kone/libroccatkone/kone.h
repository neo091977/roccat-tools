#ifndef __ROCCAT_KONE_H__
#define __ROCCAT_KONE_H__

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
 * \file kone.h
 * \brief definitions for libroccatkone
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

/*!
 * Valid for \n
 * ROC-11-500 \n
 * ROC-11-501 (sometimes called Kone Max)
 */
#define USB_DEVICE_ID_ROCCAT_KONE 0x2ced

typedef struct _Kone7 Kone7;
typedef struct _Kone8 Kone8;
typedef struct _KoneButtonInfo KoneButtonInfo;
typedef struct _KoneLightInfo KoneLightInfo;
typedef struct _KoneProfile KoneProfile;
typedef struct _KoneSettings KoneSettings;
typedef struct _KoneSpecial KoneSpecial;
typedef struct _KoneRMPButtonInfo KoneRMPButtonInfo;
typedef struct _KoneRMPLightInfo KoneRMPLightInfo;
typedef struct _KoneRMP KoneRMP;

struct _Kone7 {
	guint8 unknown[2];
} __attribute__ ((packed));

struct _Kone8 {
	guint8 unknown[1024];
} __attribute__ ((packed));

typedef enum {
	KONE_KEYSTROKE_BUTTON_1 = 0xf0, /*!< left mouse button */
	KONE_KEYSTROKE_BUTTON_2 = 0xf1, /*!< right mouse button */
	KONE_KEYSTROKE_BUTTON_3 = 0xf2, /*!< wheel */
	KONE_KEYSTROKE_BUTTON_9 = 0xf3, /*!< side button up */
	KONE_KEYSTROKE_BUTTON_8 = 0xf4, /*!< side button down */
} KoneKeystrokeButton;

typedef enum {
	KONE_KEYSTROKE_ACTION_PRESS = 0,
	KONE_KEYSTROKE_ACTION_RELEASE = 1,
	KONE_KEYSTROKE_ACTION_NONE = 3,
	KONE_KEYSTROKE_ACTION_SHIFT = 4,
	KONE_KEYSTROKE_ACTION_CTRL = 5,
	KONE_KEYSTROKE_ACTION_ALT = 6,
	KONE_KEYSTROKE_ACTION_WIN = 7,
} KoneKeystrokeAction;

enum {
	KONE_BUTTON_INFO_MACRO_SET_NAME_LENGTH = 16,
	KONE_BUTTON_INFO_MACRO_NAME_LENGTH = 16,
	KONE_BUTTON_INFO_KEYSTROKES_NUM = 20,
};

/*!
 * \brief button info
 */
struct _KoneButtonInfo {
	guint8 number; /*!< \brief range 1-8 */
	guint8 type; /*!< type of function */
	guint8 macro_type; /*!< 0 = short, 1 = overlong */
	guint8 macro_set_name[KONE_BUTTON_INFO_MACRO_SET_NAME_LENGTH]; /*!< can be max 15 chars long */
	guint8 macro_name[KONE_BUTTON_INFO_MACRO_NAME_LENGTH]; /*!< can be max 15 chars long */
	guint8 count;
	RoccatKeystroke keystrokes[KONE_BUTTON_INFO_KEYSTROKES_NUM];
} __attribute__ ((packed));

typedef enum {
	KONE_BUTTON_INFO_MACRO_TYPE_SHORT = 0,
	KONE_BUTTON_INFO_MACRO_TYPE_LONG = 1,
} KoneButtonInfoMacroType;

typedef enum {
	/* valid button types until firmware 1.32 */
	KONE_BUTTON_INFO_TYPE_BUTTON_1 = 0x1, /*!< click (left mouse button) */
	KONE_BUTTON_INFO_TYPE_BUTTON_2 = 0x2, /*!< menu (right mouse button)*/
	KONE_BUTTON_INFO_TYPE_BUTTON_3 = 0x3, /*!< scroll (wheel) */
	KONE_BUTTON_INFO_TYPE_DOUBLE_CLICK = 0x4,
	KONE_BUTTON_INFO_TYPE_KEY = 0x5,
	KONE_BUTTON_INFO_TYPE_MACRO = 0x6,
	KONE_BUTTON_INFO_TYPE_OFF = 0x7,
	KONE_BUTTON_INFO_TYPE_OSD_XY_PRESCALING = 0x8, /* \todo clarify function and rename */
	KONE_BUTTON_INFO_TYPE_OSD_DPI = 0x9,
	KONE_BUTTON_INFO_TYPE_OSD_PROFILE = 0xa,
	KONE_BUTTON_INFO_TYPE_BUTTON_9 = 0xb, /*!< ie forward */
	KONE_BUTTON_INFO_TYPE_BUTTON_8 = 0xc, /*!< ie backward */
	KONE_BUTTON_INFO_TYPE_DPI_UP = 0xd, /*!< internal */
	KONE_BUTTON_INFO_TYPE_DPI_DOWN = 0xe, /*!< internal */
	KONE_BUTTON_INFO_TYPE_BUTTON_7 = 0xf, /*!< tilt left */
	KONE_BUTTON_INFO_TYPE_BUTTON_6 = 0x10, /*!< tilt right */
	KONE_BUTTON_INFO_TYPE_PROFILE_UP = 0x11, /*!< internal */
	KONE_BUTTON_INFO_TYPE_PROFILE_DOWN = 0x12, /*!< internal */
	/* additional valid button types since firmware 1.38 */
	KONE_BUTTON_INFO_TYPE_SHORTCUT = 0x19,
	KONE_BUTTON_INFO_TYPE_MULTIMEDIA_OPEN_PLAYER = 0x20,
	KONE_BUTTON_INFO_TYPE_MULTIMEDIA_NEXT_TRACK = 0x21,
	KONE_BUTTON_INFO_TYPE_MULTIMEDIA_PREV_TRACK = 0x22,
	KONE_BUTTON_INFO_TYPE_MULTIMEDIA_PLAY_PAUSE = 0x23,
	KONE_BUTTON_INFO_TYPE_MULTIMEDIA_STOP = 0x24,
	KONE_BUTTON_INFO_TYPE_MULTIMEDIA_MUTE = 0x25,
	KONE_BUTTON_INFO_TYPE_MULTIMEDIA_VOLUME_DOWN = 0x26,
	KONE_BUTTON_INFO_TYPE_MULTIMEDIA_VOLUME_UP = 0x27,
} KoneButtonInfoType;

typedef enum {
	KONE_BUTTON_TOP = 1,
	KONE_BUTTON_WHEEL_TILT_LEFT = 2,
	KONE_BUTTON_WHEEL_TILT_RIGHT = 3,
	KONE_BUTTON_FORWARD = 4,
	KONE_BUTTON_BACKWARD = 5,
	KONE_BUTTON_MIDDLE = 6,
	KONE_BUTTON_PLUS = 7,
	KONE_BUTTON_MINUS = 8,
} KoneButtonInfoNumber;

/*!
 * \brief light info
 */
struct _KoneLightInfo {
    guint8 number; /*!< \brief number of light 1-5 */
    guint8 mod;   /*!< \brief 1 = on, 2 = off */
    guint8 red;   /*!< \brief range 0x00-0xff */
    guint8 green; /*!< \brief range 0x00-0xff */
    guint8 blue;  /*!< \brief range 0x00-0xff */
} __attribute__ ((packed));

typedef enum {
	KONE_LIGHT_TOP_LEFT = 1,
	KONE_LIGHT_BOTTOM_LEFT = 2,
	KONE_LIGHT_TOP_RIGHT = 3,
	KONE_LIGHT_BOTTOM_RIGHT = 4,
	KONE_LIGHT_LOGO = 5,
} KoneLightInfoNumber;

typedef enum {
	KONE_LIGHT_INFO_MOD_ON = 1,
	KONE_LIGHT_INFO_MOD_OFF = 2,
} KoneLightInfoMod;

enum {
	KONE_PROFILE_NUM = 5,
	KONE_PROFILE_MIN = 1,
	KONE_PROFILE_MAX = 5,
	KONE_MAIN_SENSITIVITY_MIN = 100,
	KONE_MAIN_SENSITIVITY_MAX = 1000,
	KONE_X_SENSITIVITY_MIN = 100,
	KONE_X_SENSITIVITY_MAX = 1000,
	KONE_Y_SENSITIVITY_MIN = 100,
	KONE_Y_SENSITIVITY_MAX = 1000,
	KONE_LIGHT_EFFECT_SPEED_MIN = 0,
	KONE_LIGHT_EFFECT_SPEED_MAX = 255,
	KONE_LIGHT_INFO_NUM = 5,
	KONE_BUTTON_INFO_NUM = 8,
	KONE_DPI_NUM = 6,
	KONE_GAMEFILE_NUM = 1,
};

/*!
 * \brief mouseprofile
 */
struct _KoneProfile {
	guint16 size; /*!< \brief always 975 */
	guint16 unused; /*!< \brief always 0 */

	/*
	 * \brief range 1-5
	 * Doesn't need to correspond with position in mouse.
	 */
	guint8 profile;
	guint16 main_sensitivity; /*!< \brief range 100-1000 */
	guint8 advanced_sensitivity; /*!< 1 = on, 2 = off */
	guint16 x_sensitivity; /*!< \brief range 100-1000 */
	guint16 y_sensitivity; /*!< \brief range 100-1000 */
	guint8 dpi_enabled; /*!< bit 1 = 800, ... */
	guint8 startup_dpi; /*!< \brief range 1-6 */
	guint8 polling_rate; /*!< \brief 1 = 125Hz, 2 = 500Hz, 3 = 1000Hz */
	guint8 dcu_flag; /*!< \brief there is no dcu, but value is always 2 */
	guint8 light_effect_1; /*!< range 1-3 */
	guint8 light_effect_2; /*!< range 1-5 */
	guint8 light_effect_3; /*!< range 1-4 */
	guint8 light_effect_speed; /*!< range 0-255 */

	KoneLightInfo light_info[KONE_LIGHT_INFO_NUM];
	KoneButtonInfo button_info[KONE_BUTTON_INFO_NUM];

	guint16 checksum; /*!< \brief holds checksum of struct */
} __attribute__ ((packed));

typedef enum {
	KONE_BUTTON_INFO_INDEX_TOP = 0,
	KONE_BUTTON_INFO_INDEX_WHEEL_TILT_LEFT = 1,
	KONE_BUTTON_INFO_INDEX_WHEEL_TILT_RIGHT = 2,
	KONE_BUTTON_INFO_INDEX_FORWARD = 3,
	KONE_BUTTON_INFO_INDEX_BACKWARD = 4,
	KONE_BUTTON_INFO_INDEX_MIDDLE = 5,
	KONE_BUTTON_INFO_INDEX_PLUS = 6,
	KONE_BUTTON_INFO_INDEX_MINUS = 7,
} KoneButtonInfoIndex;

typedef enum {
	KONE_LIGHT_INFO_INDEX_TOP_LEFT = 0,
	KONE_LIGHT_INFO_INDEX_BOTTOM_LEFT = 1,
	KONE_LIGHT_INFO_INDEX_TOP_RIGHT = 2,
	KONE_LIGHT_INFO_INDEX_BOTTOM_RIGHT = 3,
	KONE_LIGHT_INFO_INDEX_LOGO = 4,
} KoneLightInfoIndex;

typedef enum {
	KONE_ADVANCED_SENSITIVITY_ON = 1,
	KONE_ADVANCED_SENSITIVITY_OFF = 2,
} KoneAdvancedSensitivity;

typedef enum {
	KONE_DPI_INACTIVE = 0,
	KONE_DPI_ACTIVE = 1,
} KoneDpi;

typedef enum {
	KONE_DPI_800 = 1,
	KONE_DPI_1200 = 2,
	KONE_DPI_1600 = 3,
	KONE_DPI_2000 = 4,
	KONE_DPI_2400 = 5,
	KONE_DPI_3200 = 6,
} KoneDpiValue;

/*!
 * Should be called 'counts per inch' and not 'dots per inch'. Just being
 * consistent with roccats nomenclature
 */
typedef enum {
	KONE_POLLING_RATE_125 = 1, /*!< 125 Hz */
	KONE_POLLING_RATE_500 = 2, /*!< 500 Hz */
	KONE_POLLING_RATE_1000 = 3, /*!< 1000 Hz */
} KonePollingRate;

typedef enum {
	KONE_DCU_ON = 1,
	KONE_DCU_OFF = 2,
} KoneDcu;

typedef enum {
	/*! All lights have color and state as stated in the light info data. */
	KONE_LIGHT_EFFECT_1_SELECTED_COLOR = 1,

	/*! All lights have the same random color. */
	KONE_LIGHT_EFFECT_1_RANDOM_SINGLE = 2,

	/*! Each light has a unique randomly chosen color. */
	KONE_LIGHT_EFFECT_1_RANDOM_MULTI = 3
} KoneLightEffect1;

typedef enum {
	KONE_LIGHT_EFFECT_2_FIXED_COLOR = 1,
	KONE_LIGHT_EFFECT_2_ROTATE_CLOCKWISE = 2,
	KONE_LIGHT_EFFECT_2_ROTATE_COUNTERCLOCKWISE = 3,
	KONE_LIGHT_EFFECT_2_MOVE_VERTICAL = 4,
	KONE_LIGHT_EFFECT_2_MOVE_HORIZONTAL = 5,
} KoneLightEffect2;

typedef enum {
	KONE_LIGHT_EFFECT_3_FULL_LIGHTENED = 1,
	KONE_LIGHT_EFFECT_3_PULSATING = 2,
	KONE_LIGHT_EFFECT_3_BLINKING = 3,
	KONE_LIGHT_EFFECT_3_HEARTBEAT = 4,
} KoneLightEffect3;

/*!
 * \brief mousesettings
 */
struct _KoneSettings {
	guint16 size; /*!< always 36 */
	guint8  startup_profile; /*!< 1-5 */
	guint8  unknown1;
	guint8  tcu; /*!< 0 = off, 1 = on */
	guint8  unknown2[23];
	guint8  calibration_data[4];
	guint8  unknown3[2];
	guint16 checksum;
} __attribute__ ((packed));

typedef enum {
	KONE_WEIGHT_NONE = 0,
	KONE_WEIGHT_5g = 1,
	KONE_WEIGHT_10g = 2,
	KONE_WEIGHT_15g = 3,
	KONE_WEIGHT_20g = 4,
} KoneWeight;

enum {
	KONE_TCU_INACTIVE = 0,
	KONE_TCU_ACTIVE = 1,
};

struct _KoneSpecial {
	guint8 type;
	guint8 value;
	guint8 key;
} __attribute__ ((packed));

typedef enum {
	KONE_SPECIAL_TYPE_OSD_DPI = 0xa0,
	KONE_SPECIAL_TYPE_OSD_PROFILE = 0xb0,
	KONE_SPECIAL_TYPE_CALIBRATION = 0xc0,
	KONE_SPECIAL_TYPE_OVERLONG_MACRO = 0xe0,
	KONE_SPECIAL_TYPE_MULTIMEDIA = 0xe1,
	KONE_SPECIAL_TYPE_SWITCH_DPI = 0xf0,
	KONE_SPECIAL_TYPE_SWITCH_PROFILE = 0xf1,
} KoneSpecialType;

enum {
	KONE_RMP_BUTTON_INFO_MACRO_SET_NAME_LENGTH = 20,
	KONE_RMP_BUTTON_INFO_MACRO_NAME_LENGTH = 20,
	KONE_RMP_BUTTON_INFO_KEYSTROKES_NUM = 512,
};

/*!
 * \brief extended button info for rmp file
 */
struct _KoneRMPButtonInfo {
	guint8 number;
	guint8 type;
	guint8 ns_loop; /*!< always 0 in Windows */
	guint8 macro_set_name[KONE_RMP_BUTTON_INFO_MACRO_SET_NAME_LENGTH];
	guint8 macro_name[KONE_RMP_BUTTON_INFO_MACRO_NAME_LENGTH];
	guint8 unknown2; /*!< always 0xcc */
	guint16 count;
	RoccatKeystroke keystrokes[KONE_RMP_BUTTON_INFO_KEYSTROKES_NUM];
	guint8 checksum;
} __attribute__ ((packed));

struct _KoneRMPLightInfo {
	guint8 number;
	guint8 mod;
	guint8 red;
	guint8 green;
	guint8 blue;
	guint8 checksum;
} __attribute__ ((packed));

typedef enum {
	KONE_USB_REPORT_ID_PROFILE = 0x5a,
	KONE_USB_REPORT_ID_SETTINGS = 0x15a,
	KONE_USB_REPORT_ID_FIRMWARE_VERSION = 0x25a,
	KONE_USB_REPORT_ID_WEIGHT = 0x45a,
	KONE_USB_REPORT_ID_CALIBRATE = 0x55a,
	KONE_USB_REPORT_ID_CONFIRM_WRITE = 0x65a,
	KONE_USB_REPORT_ID_7 = 0x75a, /* read 2 bytes */
	KONE_USB_REPORT_ID_8 = 0x85a, /* read 1024 bytes */
	KONE_USB_REPORT_ID_FIRMWARE = 0xe5a
} KoneUsbReportId;

typedef enum {
	KONE_INTERFACE_MOUSE = 0,
	KONE_INTERFACE_KEYBOARD = 1,
} KoneInterface;

struct _KoneRMP;

/*!
 * \defgroup kone Kone handling
 * \defgroup rmp RMP handling
 * \defgroup rmp_access RMP accessors
 * \defgroup profile Profile handling
 * \defgroup conversions Conversions
 * \defgroup settings Settings handling
 */

/*!
 * \ingroup profile
 * \brief calculates checksum of \e profile
 * \retval checksum
 */
guint16 kone_profile_calc_checksum(KoneProfile const *profile);

/*!
 * \ingroup profile
 * \brief sets checksum of \e profile
 */
void kone_profile_set_checksum(KoneProfile *profile);

/*!
 * \ingroup settings
 * \brief calculates checksum of \e settings
 * \retval checksum
 */
guint16 kone_settings_calc_checksum(KoneSettings const *settings);

/*!
 * \ingroup settings
 * \brief sets checksum of \e settings
 */
void kone_settings_set_checksum(KoneSettings *settings);

/*!
 * \ingroup rmp
 * \brief calculates checksum of \e rmp_button_info
 * \retval checksum
 */
guint8 kone_rmp_button_info_calc_checksum(KoneRMPButtonInfo const *rmp_button_info);

/*!
 * \ingroup rmp
 * \brief sets checksum of \e rmp_button_info
 */
void kone_rmp_button_info_set_checksum(KoneRMPButtonInfo *rmp_button_info);

/*!
 * \ingroup rmp
 * \brief calculates checksum of \e rmp_light_info
 * \retval checksum
 */
guint8 kone_rmp_light_info_calc_checksum(KoneRMPLightInfo const *rmp_light_info);

/*!
 * \ingroup rmp
 * \brief sets checksum of \e rmp_light_info
 */
void kone_rmp_light_info_set_checksum(KoneRMPLightInfo *rmp_light_info);

/*!
 * \ingroup rmp
 * \brief reads rmp file
 *
 * \retval If successful returns rmp data that has to be freed with \c kone_rmp_free().
 *         On failure returns \c NULL.
 */
KoneRMP *kone_rmp_read_with_path(gchar const *path, GError **error);

/*!
 * \ingroup rmp
 * \brief writes profile as rmp into file
 *
 * \retval If successful returns \c TRUE. On failure, returns \c FALSE with \e error set
 */
gboolean kone_rmp_write_with_path(gchar const *path, KoneRMP *rmp, GError **error);

/*!
 * \ingroup conversions
 * \brief converts rmp data to binary profile
 *
 * Uses default profile which gets updated with rmp data, so you can use rmp files
 * with partial data to just update specific parts of a profile.
 *
 * Doesn't set the profile number and checksum.
 *
 * \retval Returns profile that has to be freed with \c g_free().
 */
KoneProfile *kone_rmp_to_profile(KoneRMP *rmp);

/*!
 * \ingroup conversions
 */
void kone_rmp_update_with_profile(KoneRMP *rmp, KoneProfile const *profile);

/*!
 * \ingroup rmp
 * \brief frees rmp data
 */
void kone_rmp_free(KoneRMP *rmp);

KoneRMP *kone_rmp_dup(KoneRMP const *src);

/*!
 * \ingroup rmp
 * \brief returns rmp with default settings
 * \retval rmp data that has to be freed with \c kone_rmp_free()
 */
KoneRMP const *kone_default_rmp(void);

/*!
 * \ingroup profile
 * \brief reads profile from file
 *
 * sysfs files exist already and should not be created.
 *
 * \retval If successful returns kone_profile that has to be freed with \c g_free()
 *         On failure, returns \c NULL with \e error set
 */
KoneProfile *kone_profile_read(RoccatDevice *kone, guint profile_number, GError **error);


/*!
 * \ingroup profile
 * \brief writes binary profile data to sysfs
 *
 * Sysfs files exist already and should not be created.
 * Profile number and checksum are set in profile.
 *
 * \retval If successful return \c TRUE. On failure, returns \c FALSE with \e error set
 */
gboolean kone_profile_write(RoccatDevice *kone, guint profile_number, KoneProfile *profile, GError **error);

/*!
 * \ingroup settings
 * \brief reads settings from file
 *
 * \retval If successful returns kone_settings that has to be freed with \c g_free()
 *         On failure, returns \c NULL with \e error set
 */
KoneSettings *kone_settings_read(RoccatDevice *kone, GError **error);

/*!
 * \ingroup settings
 * \brief writes settings data to sysfs
 *
 * sysfs files exist already and should not be created.
 *
 * \retval If successful return \c TRUE. On failure, returns \c FALSE with \e error set
 */
gboolean kone_settings_write(RoccatDevice *kone, KoneSettings const *settings, GError **error);

/*!
 * \brief calibrate kone
 *
 * returns after 6 seconds, when calibration is finished.
 *
 * \note mouse changes values in kone_settings, so don't forget to reread before
 *       write
 *
 * \retval If successful return \c TRUE. On failure, returns \c FALSE with \e error set
 */
gboolean kone_calibrate(RoccatDevice *kone, GError **error);

/*!
 * \ingroup settings
 * \brief activate profile
 *
 * Please be aware that this sets the startup profile.
 *
 * \retval If successful return \c TRUE. On failure, returns \c FALSE with \e error set
 */
gboolean kone_profile_activate(RoccatDevice *kone, guint profile_number, GError **error);

/*!
 * \brief reads fast-clip weight
 * \retval If successful returns weight in grams.
 *         On failure \e error is set
 */
guint kone_weight_read(RoccatDevice *kone, GError **error);

/*!
 * \brief reads firmware version
 * \retval If successful returns firmware version as integer.
 *         On failure, returns 0 with \e error set
 */
guint kone_firmware_version_read(RoccatDevice *kone, GError **error);

/*!
 * \brief reads actual dpi value
 * \retval If successful returns actual dpi as integer in range 800 to 3200.
 *         On failure, returns 0 with \e error set
 */
guint kone_actual_dpi_read(RoccatDevice *kone, GError **error);

/*!
 * \ingroup settings
 * \brief reads actual profile number
 * \retval If successful returns actual profile number as integer in range 1 to 5.
 *         On failure, returns 0 with \e error set
 */
guint kone_actual_profile_read(RoccatDevice *kone, GError **error);

/*!
 * \ingroup conversions
 * \brief converts raw firmware value into string
 * \retval String that has to be freed with \c g_free()
 */
gchar *kone_firmware_version_to_string(guint firmware_version);

/*!
 * \ingroup conversions
 * \brief converts raw dpi value into real value
 * \retval real dpi value
 */
guint kone_dpi_raw_to_dpi(guint dpi_raw);

void kone_button_info_to_rmp_button_info(KoneButtonInfo const *from, KoneRMPButtonInfo *to);
void kone_rmp_button_info_to_button_info(KoneRMPButtonInfo const *from, KoneButtonInfo *to);
void kone_rmp_light_info_to_light_info(KoneRMPLightInfo const *from, KoneLightInfo *to);
void kone_light_info_to_rmp_light_info(KoneLightInfo const *light_info, KoneRMPLightInfo *rmp_light_info);

/*!
 * \brief compares KoneRMPButtonInfo
 *
 * Does not take number and checksum into account.
 */
gboolean kone_rmp_button_info_equal(KoneRMPButtonInfo const *left, KoneRMPButtonInfo const *right);

gboolean kone_rmp_light_info_equal(KoneRMPLightInfo const *left, KoneRMPLightInfo const *right);

KoneRMPButtonInfo *kone_rmp_button_info_dup(KoneRMPButtonInfo const *source);

/*!
 *
 */
KoneRMPButtonInfo *kone_rmp_button_info_new_special(KoneButtonInfoType type);

/*!
 * \brief creates KoneRMPButtonInfo which plays single key
 *
 * Does not set number and checksum.
 *
 * \retval KoneRMPButtonInfo that has to be freed with \c g_free().
 */
KoneRMPButtonInfo *kone_rmp_button_info_new_single_key(guint hid_usage_id);

/*!
 * \brief creates KoneRMPButtonInfo which plays shortcut
 *
 * Does not set number and checksum.
 *
 * \retval KoneRMPButtonInfo that has to be freed with \c g_free().
 */
KoneRMPButtonInfo *kone_rmp_button_info_new_shortcut(guint hid_usage_id, guint modifier);

/*!
 * \ingroup conversions
 * \brief converts GaminggearMacro to KoneRMPButtonInfo
 *
 * Does not set number and checksum.
 *
 * \retval KoneRMPButtonInfo that has to be freed with \c g_free().
 */
KoneRMPButtonInfo *gaminggear_macro_to_kone_rmp_button_info(GaminggearMacro const *gaminggear_macro, GError **error);

GaminggearMacro *kone_rmp_button_info_to_gaminggear_macro(KoneRMPButtonInfo const *button_info);

RoccatKeyFile *kone_configuration_load(void);
void kone_configuration_free(RoccatKeyFile *config);
gboolean kone_configuration_save(RoccatKeyFile *config, GError **error);

/*!
 * Doesn't modify the key_file if key is not found. It just returns standard value
 * \retval path that needs to be freed with g_free()
 */
gchar *kone_configuration_get_rmp_save_path(RoccatKeyFile *config);

/*!
 * \brief sets new save_path for rmps
 */
void kone_configuration_set_rmp_save_path(RoccatKeyFile *config, gchar const *path);

gboolean kone_configuration_get_only_notify_on_osd(RoccatKeyFile *config);
void kone_configuration_set_only_notify_on_osd(RoccatKeyFile *config, gboolean new_value);
gint kone_configuration_get_profile_notification_type(RoccatKeyFile *config);
void kone_configuration_set_profile_notification_type(RoccatKeyFile *config, gint new_value);
gint kone_configuration_get_cpi_notification_type(RoccatKeyFile *config);
void kone_configuration_set_cpi_notification_type(RoccatKeyFile *config, gint new_value);
gdouble kone_configuration_get_notification_volume(RoccatKeyFile *config);
void kone_configuration_set_notification_volume(RoccatKeyFile *config, gdouble new_value);
gint kone_configuration_get_default_profile_number(RoccatKeyFile *config);
void kone_configuration_set_default_profile_number(RoccatKeyFile *config, gint new_value);

/*!
 * \ingroup rmp
 */
gboolean kone_rmp_get_modified(KoneRMP const *rmp);

/*!
 * \ingroup rmp
 */
void kone_rmp_set_modified(KoneRMP *rmp);

/*!
 * \ingroup rmp
 */
void kone_rmp_set_unmodified(KoneRMP *rmp);

/*!
 * \ingroup rmp
 * Compares the actual rmp with the actual hardware profile.
 * Returns hardware profile if they differ or rmp couldn't be read.
 * Returns actual rmp if hardware profile couldn't be read.
 * Returns default rmp if neither could be found.
 */
KoneRMP *kone_rmp_load(RoccatDevice *kone, guint profile_number, GError **error);

/*!
 * \ingroup rmp
 */
gboolean kone_rmp_save(RoccatDevice *kone, KoneRMP *rmp_file, guint profile_number, GError **error);

KoneRMP *kone_rmp_load_actual(guint profile_number);
gboolean kone_rmp_save_actual(KoneRMP *rmp, guint profile_nubmer, GError **error);
void kone_rmp_update_from_device(KoneRMP *rmp, RoccatDevice *kone, guint profile_number);

/*!
 * \ingroup profile
 */
void kone_profile_finalize(KoneProfile *profile, guint profile_number);

/*!
 * \ingroup rmp_access
 * \brief sets light info
 *
 * \e rmp_light_info gets finalized before it's written to \e rmp.
 * This means \c number and \c checksum are changed as needed.
 *
 * \e index is in range of 0 to 4.
 */
void kone_rmp_set_rmp_light_info(KoneRMP *rmp, guint index, KoneRMPLightInfo *rmp_light_info);

/*!
 * \ingroup rmp_access
 * \brief sets button info
 *
 * \e rmp_button_info gets finalized before it's written to \e rmp.
 * This means \c number and \c checksum are changed as needed.
 *
 * \e index is in range of 0 to 7.
 */
void kone_rmp_set_rmp_button_info(KoneRMP *rmp, guint index, KoneRMPButtonInfo *rmp_button_info);

/*!
 * \ingroup rmp_access
 * \brief sets main sensitivity
 *
 * \e value can be in range KONE_MAIN_SENSITIVITY_MIN to KONE_MAIN_SENSITIVITY_MAX.
 */
void kone_rmp_set_main_sensitivity(KoneRMP *rmp, guint value);

/*!
 * \ingroup rmp_access
 * \brief sets advanced sensitivity
 *
 * \e value can be of #KoneAdvancedSensitivity.
 */
void kone_rmp_set_advanced_sensitivity(KoneRMP *rmp, guint value);

/*!
 * \ingroup rmp_access
 * \brief sets x sensitivity
 *
 * \e value can be in range \c KONE_X_SENSITIVITY_MIN to \c KONE_X_SENSITIVITY_MAX.
 */
void kone_rmp_set_x_sensitivity(KoneRMP *rmp, guint value);

/*!
 * \ingroup rmp_access
 * \brief sets y sensitivity
 *
 * \e value can be in range \c KONE_Y_SENSITIVITY_MIN to \c KONE_Y_SENSITIVITY_MAX.
 */
void kone_rmp_set_y_sensitivity(KoneRMP *rmp, guint value);

/*!
 * \ingroup rmp_access
 * \brief sets dpi
 *
 * \e bit is counted from 0 to 5.
 * \e value can be either \c KONE_DPI_ACTIVE or \c KONE_DPI_INACTIVE.
 */
void kone_rmp_set_dpi(KoneRMP *rmp, guint bit, guint value);

/*!
 * \ingroup rmp_access
 * \brief sets all dpis from byte
 *
 * \e value is byte like profile->dpi_enabled
 */
void kone_rmp_set_dpi_all(KoneRMP *rmp, guint value);

/*!
 * \ingroup rmp_access
 * \brief sets startup dpi
 *
 * \e value is one of #KoneDpiValue.
 */
void kone_rmp_set_startup_dpi(KoneRMP *rmp, guint value);

/*!
 * \ingroup rmp_access
 * \brief sets polling rate
 *
 * \e value can be one of #KonePollingRate.
 */
void kone_rmp_set_polling_rate(KoneRMP *rmp, guint value);

/*!
 * \ingroup rmp_access
 * \brief sets dcu flag
 *
 * \e value can be one of #KoneDcu.
 *
 * Since the "Distance Control Unit" was replaced by the "Tracking Control Unit"
 * this setting has no effect. Should be set to \c KONE_DCU_OFF.
 */
void kone_rmp_set_dcu_flag(KoneRMP *rmp, guint value);

/*!
 * \ingroup rmp_access
 * \brief sets light effect 1
 *
 * Light effect 1 selects how the base colors are chosen.
 *
 * \e value can be of #KoneLightEffect1.
 */
void kone_rmp_set_light_effect_1(KoneRMP *rmp, guint value);

/*!
 * \ingroup rmp_access
 * \brief sets light effect 2
 *
 * Light effect 2 selects if and how colors move over the 5 lamps.
 *
 * \e value can be of #KoneLightEffect2.
 */
void kone_rmp_set_light_effect_2(KoneRMP *rmp, guint value);

/*!
 * \ingroup rmp_access
 * \brief sets light effect 3
 *
 * Light effect 3 selects if and how lamps are periodically switched on and off.
 *
 * \e value can be of #KoneLightEffect3.
 */
void kone_rmp_set_light_effect_3(KoneRMP *rmp, guint value);

/*!
 * \ingroup rmp_access
 * \brief sets light effect speed
 *
 * Light effect speed selects how fast the various light effects are played.
 *
 * \e value can be in Range 0 to 255.
 */
void kone_rmp_set_light_effect_speed(KoneRMP *rmp, guint value);

/*!
 * \ingroup rmp_access
 */
KoneRMPLightInfo *kone_rmp_get_rmp_light_info(KoneRMP *rmp, guint index);

/*!
 * \ingroup rmp_access
 */
KoneRMPButtonInfo *kone_rmp_get_rmp_button_info(KoneRMP *rmp, guint index);

/*!
 * \ingroup rmp_access
 */
guint kone_rmp_get_main_sensitivity(KoneRMP *rmp);

/*!
 * \ingroup rmp_access
 */
guint kone_rmp_get_advanced_sensitivity(KoneRMP *rmp);

/*!
 * \ingroup rmp_access
 */
guint kone_rmp_get_x_sensitivity(KoneRMP *rmp);

/*!
 * \ingroup rmp_access
 */
guint kone_rmp_get_y_sensitivity(KoneRMP *rmp);

/*!
 * \ingroup rmp_access
 */
guint kone_rmp_get_dpi(KoneRMP *rmp, guint bit);

/*!
 * \ingroup rmp_access
 */
guint8 kone_rmp_get_dpi_all(KoneRMP *rmp);

/*!
 * \ingroup rmp_access
 */
guint kone_rmp_get_startup_dpi(KoneRMP *rmp);

/*!
 * \ingroup rmp_access
 */
guint kone_rmp_get_polling_rate(KoneRMP *rmp);

/*!
 * \ingroup rmp_access
 */
guint kone_rmp_get_dcu_flag(KoneRMP *rmp);

/*!
 * \ingroup rmp_access
 */
guint kone_rmp_get_light_effect_1(KoneRMP *rmp);

/*!
 * \ingroup rmp_access
 */
guint kone_rmp_get_light_effect_2(KoneRMP *rmp);

/*!
 * \ingroup rmp_access
 */
guint kone_rmp_get_light_effect_3(KoneRMP *rmp);

/*!
 * \ingroup rmp_access
 */
guint kone_rmp_get_light_effect_speed(KoneRMP *rmp);

void kone_rmp_set_game_file_name(KoneRMP *rmp, gchar const *string);
gchar *kone_rmp_get_game_file_name(KoneRMP *rmp);

void kone_rmp_remove_profile_name(KoneRMP *rmp);
void kone_rmp_set_profile_name(KoneRMP *rmp, gchar const *string);
gchar *kone_rmp_get_profile_name(KoneRMP *rmp);

void kone_linux_rmp_to_windows(KoneRMP *rmp);
void kone_windows_rmp_to_linux(KoneRMP *rmp);

static inline guint16 kone_profile_get_main_sensitivity(KoneProfile const *profile) {
	return GUINT16_FROM_LE(profile->main_sensitivity);
}

static inline void kone_profile_set_main_sensitivity(KoneProfile *profile, guint16 new_value) {
	profile->main_sensitivity = GUINT16_TO_LE(new_value);
}

static inline guint16 kone_profile_get_x_sensitivity(KoneProfile const *profile) {
	return GUINT16_FROM_LE(profile->x_sensitivity);
}

static inline void kone_profile_set_x_sensitivity(KoneProfile *profile, guint16 new_value) {
	profile->x_sensitivity = GUINT16_TO_LE(new_value);
}

static inline guint16 kone_profile_get_y_sensitivity(KoneProfile const *profile) {
	return GUINT16_FROM_LE(profile->y_sensitivity);
}

static inline void kone_profile_set_y_sensitivity(KoneProfile *profile, guint16 new_value) {
	profile->y_sensitivity = GUINT16_TO_LE(new_value);
}

static inline guint16 kone_rmp_button_info_get_count(KoneRMPButtonInfo const *button_info) {
	return GUINT16_FROM_LE(button_info->count);
}

static inline void kone_rmp_button_info_set_count(KoneRMPButtonInfo *button_info, guint16 new_value) {
	button_info->count = GUINT16_TO_LE(new_value);
}

/*!
 * \brief import macros from windows macro file
 * Windows Kone driver stores macros in %APPDATA%\ROCCAT\Kone\config\macro.ini
 * \retval If successful returns \c TRUE. On failure, returns \c FALSE with \e error set
 */
gboolean kone_macros_import(gchar const *filename, GError **error);

/*!
 * \brief export macros in kone format
 * \retval If successful returns \c TRUE. On failure, returns \c FALSE with \e error set
 */
gboolean kone_macros_export(gchar const *filename, GError **error);

#define KONE_DBUS_SERVER_PATH "/org/roccat/Kone"
#define KONE_DBUS_SERVER_INTERFACE "org.roccat.Kone"

DBusGProxy *kone_dbus_proxy_new(void);
gboolean kone_dbus_emit_profile_data_changed_outside(DBusGProxy *proxy, guint profile_number);
gboolean kone_dbus_emit_profile_data_changed_outside_instant(guint profile_number);
gboolean kone_dbus_emit_profile_changed_outside(DBusGProxy *proxy, guint profile_number);
gboolean kone_dbus_emit_profile_changed_outside_instant(guint profile_number);
gboolean kone_dbus_emit_configuration_changed_outside(DBusGProxy *proxy);

#define KONE_DEVICE_NAME "Kone"

G_END_DECLS

#endif
