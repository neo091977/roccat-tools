#ifndef __ROCCAT_ARVO_H__
#define __ROCCAT_ARVO_H__

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
 * \file arvo.h
 * \brief definitions for libroccatarvo
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

#define USB_DEVICE_ID_ROCCAT_ARVO 0x30d4

typedef struct _ArvoRkp ArvoRkp;
typedef struct _ArvoRkpButtonInfo ArvoRkpButtonInfo;
typedef struct _ArvoButton ArvoButton;
typedef struct _ArvoButtonKey ArvoButtonKey;
typedef struct _ArvoInfo ArvoInfo;
typedef struct _ArvoSpecial ArvoSpecial;
typedef struct _ArvoModeKey ArvoModeKey;
typedef struct _ArvoKeyMask ArvoKeyMask;
typedef struct _ArvoActualProfile ArvoActualProfile;

enum {
	ARVO_PROFILE_NUM = 5,
	ARVO_BUTTON_NUM = 5,
	ARVO_BUTTON_KEYSTROKES_NUM = 10,
	ARVO_KEY_NUM = 5,
	ARVO_RKP_BUTTON_INFO_MACROSET_NAME_LENGTH = 50,
	ARVO_RKP_BUTTON_INFO_MACRO_NAME_LENGTH = 50,
	ARVO_RKP_BUTTON_INFO_KEYSTROKES_NUM = 512,
	ARVO_RKP_BUTTON_INFO_TIMER_NAME_LENGTH = 100,
	ARVO_RKP_BUTTON_INFO_FILENAME_LENGTH_NS = 100,
	ARVO_GAMEFILE_NUM = 3,
};

struct _ArvoModeKey {
	guint8 report_id; /* ARVO_REPORT_ID_MODE_KEY */
	guint8 state;
} __attribute__ ((packed));

/* also valid as ArvoRkpModeKeyState */
typedef enum {
	ARVO_MODE_KEY_STATE_OFF = 0x00,
	ARVO_MODE_KEY_STATE_ON = 0x01, /* on means gaming mode on, mode key light off */
} ArvoModeKeyState;

struct _ArvoKeyMask {
	guint8 report_id; /* ARVO_REPORT_ID_KEY_MASK */
	guint8 key_mask;
} __attribute__ ((packed));

typedef enum {
	ARVO_KEY_MASK_BIT_LEFT_WIN = 0,
	ARVO_KEY_MASK_BIT_RIGHT_WIN = 1,
	ARVO_KEY_MASK_BIT_APP_KEY = 2,
	ARVO_KEY_MASK_BIT_CAPS_LOCK = 3,
	ARVO_KEY_MASK_BIT_TAB = 4,
} ArvoKeyMaskBit;

struct _ArvoActualProfile {
	guint8 report_id; /* ARVO_REPORT_ID_STARTUP_PROFILE */
	guint8 profile_number;
} __attribute__ ((packed));

typedef enum {
	ARVO_REPORT_ID_MODE_KEY = 0x3,
	ARVO_REPORT_ID_BUTTON = 0x4,
	ARVO_REPORT_ID_INFO = 0x5,
	ARVO_REPORT_ID_KEY_MASK = 0x6,
	ARVO_REPORT_ID_STARTUP_PROFILE = 0x7,
} ArvoReportId;

struct _ArvoButtonKey {
	guint8 key;
	guint8 action;
} __attribute__ ((packed));

typedef enum {
	ARVO_BUTTON_KEY_ACTION_RELEASE = 0,
	ARVO_BUTTON_KEY_ACTION_PRESS = 1,
} ArvoButtonKeyAction;

struct _ArvoButton { /* size 24 bytes */
	guint8 report_id; /* ARVO_REPORT_ID_BUTTONS */
	guint8 profile; /* range 1-5 */
	guint8 button; /* range 1-5 */
	ArvoButtonKey keys[ARVO_BUTTON_KEYSTROKES_NUM];
	guint8 unused;
} __attribute__ ((packed));

struct _ArvoInfo { /* size 8 bytes */
	guint8 unknown1[4];
	guint8 firmware_version;
	guint8 unknown2[3];
} __attribute__ ((packed));

typedef enum {
	ARVO_INTERFACE_KEYBOARD = 0,
	ARVO_INTERFACE_MOUSE = 1,
} ArvoInterface;

struct _ArvoSpecial {
	guint8 unknown1; /* always 0x01 */
	guint8 event;
	guint8 unknown2; /* always 0x70 */
} __attribute__ ((packed));

typedef enum {
	ARVO_SPECIAL_EVENT_MASK_ACTION = 0xf0,
	ARVO_SPECIAL_EVENT_MASK_BUTTON = 0x0f,
} ArvoSpecialEventMask;

typedef enum {
	ARVO_SPECIAL_ACTION_RELEASE = 0x0,
	ARVO_SPECIAL_ACTION_PRESS = 0x10,
} ArvoSpecialEventAction;

typedef enum {
	ARVO_SPECIAL_BUTTON_1 = 0x1,
	ARVO_SPECIAL_BUTTON_2 = 0x2,
	ARVO_SPECIAL_BUTTON_3 = 0x3,
	ARVO_SPECIAL_BUTTON_4 = 0x4,
	ARVO_SPECIAL_BUTTON_5 = 0x5,
	ARVO_SPECIAL_BUTTON_MODE_ON = 0x6,
	ARVO_SPECIAL_BUTTON_MODE_OFF = 0x7,
} ArvoSpecialEventButton;

typedef enum {
	ARVO_RKP_BUTTON_INFO_KEYSTROKE_ACTION_PRESS = 0,
	ARVO_RKP_BUTTON_INFO_KEYSTROKE_ACTION_RELEASE = 1,
} ArvoRkpButtonInfoKeystrokeAction;

struct _ArvoRkpButtonInfo { /* size 2269 bytes */
	guint8 number;
	guint8 type;
	guint8 macroset_name[ARVO_RKP_BUTTON_INFO_MACROSET_NAME_LENGTH];
	guint8 macro_name[ARVO_RKP_BUTTON_INFO_MACRO_NAME_LENGTH];
	guint8 unknown[5];
	guint8 ns_loop; /* last element of unknown array */
	guint16 count;
	RoccatKeystroke keystrokes[ARVO_RKP_BUTTON_INFO_KEYSTROKES_NUM];
	guint8 shortcut_key;
	guint8 shortcut_none;
	guint8 shortcut_shift;
	guint8 shortcut_ctrl;
	guint8 shortcut_win;
	guint8 shortcut_alt;
	guint32 timer_length;
	guint8 timer_name[ARVO_RKP_BUTTON_INFO_TIMER_NAME_LENGTH];
	guint8 checksum;
} __attribute__ ((packed));

typedef enum {
	ARVO_RKP_BUTTON_INFO_SHORTCUT_STATE_OFF = 0,
	ARVO_RKP_BUTTON_INFO_SHORTCUT_STATE_ON = 1,
} ArvoRkpButtonInfoShortCutState;

typedef enum {
	ARVO_RKP_BUTTON_INFO_TYPE_SHORTCUT = 0x5,
	ARVO_RKP_BUTTON_INFO_TYPE_MACRO = 0x6,
	ARVO_RKP_BUTTON_INFO_TYPE_OFF = 0x7,
	ARVO_RKP_BUTTON_INFO_TYPE_TIMER_START = 0xa,
	ARVO_RKP_BUTTON_INFO_TYPE_TIMER_STOP = 0xb,
	ARVO_RKP_BUTTON_INFO_TYPE_MULTIMEDIA_OPEN_PLAYER = 0x20,
	ARVO_RKP_BUTTON_INFO_TYPE_MULTIMEDIA_PREV_TRACK = 0x21,
	ARVO_RKP_BUTTON_INFO_TYPE_MULTIMEDIA_NEXT_TRACK = 0x22,
	ARVO_RKP_BUTTON_INFO_TYPE_MULTIMEDIA_PLAY_PAUSE = 0x23,
	ARVO_RKP_BUTTON_INFO_TYPE_MULTIMEDIA_STOP = 0x24,
	ARVO_RKP_BUTTON_INFO_TYPE_MULTIMEDIA_MUTE = 0x25,
	ARVO_RKP_BUTTON_INFO_TYPE_MULTIMEDIA_VOLUME_DOWN = 0x26,
	ARVO_RKP_BUTTON_INFO_TYPE_MULTIMEDIA_VOLUME_UP = 0x27,
	ARVO_RKP_BUTTON_INFO_TYPE_WWW_SEARCH = 0x30,
	ARVO_RKP_BUTTON_INFO_TYPE_WWW_HOME = 0x31,
	ARVO_RKP_BUTTON_INFO_TYPE_WWW_BACK = 0x32,
	ARVO_RKP_BUTTON_INFO_TYPE_WWW_FORWARD = 0x33,
	ARVO_RKP_BUTTON_INFO_TYPE_WWW_STOP = 0x34,
	ARVO_RKP_BUTTON_INFO_TYPE_WWW_REFRESH = 0x35,
	ARVO_RKP_BUTTON_INFO_TYPE_WWW_FAVORITES = 0x36,
	ARVO_RKP_BUTTON_INFO_TYPE_TALK_OTHER_EASYSHIFT_NS = 0xf4,
	ARVO_RKP_BUTTON_INFO_TYPE_TALK_OTHER_EASYSHIFT_LOCK_NS = 0xf5,
	ARVO_RKP_BUTTON_INFO_TYPE_TALK_OTHER_EASYAIM_1_NS = 0xf6,
	ARVO_RKP_BUTTON_INFO_TYPE_TALK_OTHER_EASYAIM_2_NS = 0xf7,
	ARVO_RKP_BUTTON_INFO_TYPE_TALK_OTHER_EASYAIM_3_NS = 0xf8,
	ARVO_RKP_BUTTON_INFO_TYPE_TALK_OTHER_EASYAIM_4_NS = 0xf9,
	ARVO_RKP_BUTTON_INFO_TYPE_TALK_OTHER_EASYAIM_5_NS = 0xfa,
	ARVO_RKP_BUTTON_INFO_TYPE_QUICKLAUNCH_NS = 0xfb,
	ARVO_RKP_BUTTON_INFO_TYPE_OPEN_DRIVER_NS = 0xfc,
	ARVO_RKP_BUTTON_INFO_TYPE_PROFILE_CYCLE_NS = 0xfd,
	ARVO_RKP_BUTTON_INFO_TYPE_PROFILE_UP_NS = 0xfe,
	ARVO_RKP_BUTTON_INFO_TYPE_PROFILE_DOWN_NS = 0xff,
} ArvoRkpButtonInfoType;

typedef enum {
	ARVO_RKP_KEY_STATE_ON = 0,
	ARVO_RKP_KEY_STATE_OFF = 1,
} ArvoRkpKeyState;

static inline guint16 arvo_rkp_button_info_get_count(ArvoRkpButtonInfo const *info) {
	return GUINT16_FROM_LE(info->count);
}

static inline void arvo_rkp_button_info_set_count(ArvoRkpButtonInfo *info, guint16 new_value) {
	info->count = GUINT16_TO_LE(new_value);
}

static inline guint32 arvo_rkp_button_info_get_timer_length(ArvoRkpButtonInfo const *info) {
	return GUINT32_FROM_LE(info->timer_length);
}

static inline void arvo_rkp_button_info_set_timer_length(ArvoRkpButtonInfo *info, guint32 new_value) {
	info->timer_length = GUINT32_TO_LE(new_value);
}

ArvoRkp const *arvo_default_rkp(void);
ArvoRkp *arvo_rkp_read_with_path(gchar const *path, GError **error);
gboolean arvo_rkp_write_with_path(gchar const *path, ArvoRkp *rkp, GError **error);
ArvoRkp *arvo_rkp_dup(ArvoRkp const *src);
void arvo_rkp_free(ArvoRkp *rkp);

RoccatKeyFile *arvo_configuration_load(void);
void arvo_configuration_free(RoccatKeyFile *config);
gboolean arvo_configuration_save(RoccatKeyFile *config, GError **error);

gchar *arvo_configuration_get_rkp_save_path(RoccatKeyFile *config);
void arvo_configuration_set_rkp_save_path(RoccatKeyFile *config, gchar const *path);

gboolean arvo_configuration_get_adjust_game_mode_on_startup(RoccatKeyFile *config);
void arvo_configuration_set_adjust_game_mode_on_startup(RoccatKeyFile *config, gboolean new_value);

gint arvo_configuration_get_timer_notification_type(RoccatKeyFile *config);
void arvo_configuration_set_timer_notification_type(RoccatKeyFile *config, gint new_value);
gint arvo_configuration_get_profile_notification_type(RoccatKeyFile *config);
void arvo_configuration_set_profile_notification_type(RoccatKeyFile *config, gint new_value);
gdouble arvo_configuration_get_notification_volume(RoccatKeyFile *config);
void arvo_configuration_set_notification_volume(RoccatKeyFile *config, gdouble new_value);
gint arvo_configuration_get_default_profile_number(RoccatKeyFile *config);
void arvo_configuration_set_default_profile_number(RoccatKeyFile *config, gint new_value);

guint arvo_actual_profile_read(RoccatDevice *arvo, GError **error);
gboolean arvo_actual_profile_write(RoccatDevice *arvo, guint profile_number, GError **error);
guint arvo_mode_key_read(RoccatDevice *arvo, GError **error);
gboolean arvo_mode_key_write(RoccatDevice *arvo, guint state, GError **error);
guint arvo_key_mask_read(RoccatDevice *arvo, GError **error);
gboolean arvo_key_mask_write(RoccatDevice *arvo, guint mask, GError **error);
gboolean arvo_profile_activate(RoccatDevice *arvo, guint profile_number, ArvoRkp *rkp, GError **error);

gboolean arvo_macros_import(gchar const *filename, GError **error);
gboolean arvo_macros_export(gchar const *filename, GError **error);

ArvoInfo *arvo_info_read(RoccatDevice *arvo, GError **error);
guint arvo_firmware_version_read(RoccatDevice *arvo, GError **error);

ArvoButton *arvo_rkp_button_info_to_button(ArvoRkpButtonInfo *info);
gboolean arvo_button_write(RoccatDevice *arvo, ArvoButton *button,
		guint profile_number, guint button_number, GError **error);

void arvo_rkp_set_modified(ArvoRkp *rkp);
void arvo_rkp_set_unmodified(ArvoRkp *rkp);
gboolean arvo_rkp_get_modified(ArvoRkp const *rkp);
ArvoRkp *arvo_rkp_load(RoccatDevice const *arvo, guint profile_number, GError **error);
gboolean arvo_rkp_save(RoccatDevice *arvo, ArvoRkp *rkp, guint profile_number, GError **error);

ArvoRkp *arvo_rkp_load_actual(guint profile_number);
gboolean arvo_rkp_save_actual(ArvoRkp *rkp, guint profile_number, GError **error);

ArvoRkpButtonInfo *arvo_rkp_get_button_info(ArvoRkp *rkp, guint index);
void arvo_rkp_set_button_info(ArvoRkp *rkp, guint index, ArvoRkpButtonInfo *rkp_button_info);
guint arvo_rkp_get_mode_key(ArvoRkp *rkp);
void arvo_rkp_set_mode_key(ArvoRkp *rkp, guint value);
void arvo_rkp_set_key(ArvoRkp *rkp, guint index, guint value);
guint arvo_rkp_get_key(ArvoRkp *rkp, guint index);
guint8 arvo_rkp_get_key_mask(ArvoRkp *rkp);
void arvo_rkp_set_profile_name(ArvoRkp *rkp, gchar const *string);
gchar *arvo_rkp_get_profile_name(ArvoRkp *rkp);
void arvo_rkp_set_game_file_name(ArvoRkp *rkp, guint index, gchar const *string);
gchar *arvo_rkp_get_game_file_name(ArvoRkp *rkp, guint index);

void arvo_linux_rkp_to_windows(ArvoRkp *rkp);
void arvo_windows_rkp_to_linux(ArvoRkp *rkp);

ArvoRkpButtonInfo *arvo_rkp_button_info_new(void);
ArvoRkpButtonInfo *arvo_rkp_button_info_dup(ArvoRkpButtonInfo const *source);
void arvo_rkp_button_info_free(ArvoRkpButtonInfo *button_info);
ArvoRkpButtonInfo *gaminggear_macro_to_arvo_rkp_button_info(GaminggearMacro const *gaminggear_macro, GError **error);
GaminggearMacro *arvo_rkp_button_info_to_gaminggear_macro(ArvoRkpButtonInfo const *button_info);
ArvoRkpButtonInfo *arvo_rkp_button_info_new_special(ArvoRkpButtonInfoType type);
ArvoRkpButtonInfo *arvo_rkp_button_info_new_timer(gchar const *name, guint seconds);
ArvoRkpButtonInfo *arvo_rkp_button_info_new_quicklaunch_ns(gchar const *filename);
ArvoRkpButtonInfo *arvo_rkp_button_info_new_shortcut(guint hid_usage_id,
		guint shift, guint ctrl, guint win, guint alt);
gboolean arvo_rkp_button_info_equal(ArvoRkpButtonInfo const *left, ArvoRkpButtonInfo const *right);

gboolean arvo_play_quicklaunch_forked_ns(ArvoRkpButtonInfo const *button_info);

#define ARVO_DBUS_SERVER_PATH "/org/roccat/Arvo"
#define ARVO_DBUS_SERVER_INTERFACE "org.roccat.Arvo"

DBusGProxy *arvo_dbus_proxy_new(void);
gboolean arvo_dbus_emit_profile_data_changed_outside(DBusGProxy *proxy, guint profile_number);
gboolean arvo_dbus_emit_profile_data_changed_outside_instant(guint profile_number);
gboolean arvo_dbus_emit_profile_changed_outside(DBusGProxy *proxy, guint profile_number);
gboolean arvo_dbus_emit_profile_changed_outside_instant(guint profile_number);
gboolean arvo_dbus_emit_configuration_changed_outside(DBusGProxy *proxy);

#define ARVO_DEVICE_NAME "Arvo"

RoccatDevice *arvo_device_first(void);
RoccatDeviceScanner *arvo_device_scanner_new(void);
gchar *arvo_device_read(RoccatDevice *roccat_device, guint report_id, gssize length, GError **error);
gboolean arvo_device_write(RoccatDevice *roccat_device, gchar const *buffer, gssize length, GError **error);

G_END_DECLS

#endif
