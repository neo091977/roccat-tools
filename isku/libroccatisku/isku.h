#ifndef __ROCCAT_ISKU_H__
#define __ROCCAT_ISKU_H__

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
 * \file isku.h
 * \brief definitions for libroccatisku
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

#define USB_DEVICE_ID_ROCCAT_ISKU 0x319c

typedef struct _IskuActualProfile IskuActualProfile;
typedef struct _IskuKeyMask IskuKeyMask;
typedef struct _IskuKeysFunction IskuKeysFunction;
typedef struct _IskuKeysEasyzone IskuKeysEasyzone;
typedef struct _IskuKeysMedia IskuKeysMedia;
typedef struct _IskuKeysThumbster IskuKeysThumbster;
typedef struct _IskuKeysMacro IskuKeysMacro;
typedef struct _IskuMacro IskuMacro;
typedef struct _IskuLight IskuLight;
typedef struct _IskuKeysCapslock IskuKeysCapslock;
typedef struct _IskuLastSet IskuLastSet;
typedef struct _IskuDeviceState IskuDeviceState;
typedef struct _IskuTalk IskuTalk;
typedef struct _IskuInfo IskuInfo;
typedef struct _IskuRkp IskuRkp;
typedef struct _IskuRkpMacroKeyInfo IskuRkpMacroKeyInfo;
typedef struct _IskuReset IskuReset;
typedef struct _IskuSpecial IskuSpecial;

enum {
	ISKU_PROFILE_NUM = 5,
	ISKU_FIRMWARE_SIZE = 56844,
	ISKU_KEYS_MEDIA_NUM = 8,
	ISKU_KEYS_EASYZONE_NUM = 20,
	ISKU_KEYS_MACRO_NUM = 10, /* +Easyshift */
	ISKU_KEYS_FUNCTION_NUM = 12,
	ISKU_KEYS_THUMBSTER_NUM = 6, /* +Easyshift */
	ISKU_KEY_INDEX_NUM = 57,
	ISKU_RKP_MACRO_KEY_INFO_TALK_DEVICE_NAME_LENGTH = 50,
	ISKU_RKP_MACRO_KEY_INFO_MACROSET_NAME_LENGTH = 50,
	ISKU_RKP_MACRO_KEY_INFO_MACRO_NAME_LENGTH = 50,
	ISKU_RKP_MACRO_KEY_INFO_KEYSTROKES_NUM = 500,
	ISKU_RKP_MACRO_KEY_INFO_TIMER_NAME_LENGTH = 100,
	ISKU_RKP_MACRO_KEY_INFO_FILENAME_LENGTH = 260,
	ISKU_MACRO_MACROSET_NAME_LENGTH = 24,
	ISKU_MACRO_MACRO_NAME_LENGTH = 24,
	ISKU_MACRO_KEYSTROKES_NUM = 500,
	ISKU_LIGHT_BRIGHTNESS_MIN = 0,
	ISKU_LIGHT_BRIGHTNESS_MAX = 5,
	ISKU_KEY_MASK_BIT_NUM = 6,
	ISKU_GAMEFILE_NUM = 3,
};

struct _IskuActualProfile {
	guint8 report_id; /* ISKU_REPORT_ID_ACTUAL_PROFILE */
	guint8 size; /* always 3 */
	guint8 actual_profile;
} __attribute__ ((packed));

guint isku_actual_profile_read(RoccatDevice *isku, GError **error);
gboolean isku_actual_profile_write(RoccatDevice *isku, guint profile_index, GError **error);

/* read data may not be valid on a new device (TODO or after firmware update?) */
struct _IskuLastSet {
	guint8 report_id; /* ISKU_REPORT_ID_LAST_SET */
	guint8 size; /* always 0x14 */
	guint32 real_time; /* integer secs since epoch */
	guint8 unknown2[12];
	guint16 checksum;
} __attribute__ ((packed));

static inline guint32 isku_last_set_get_real_time(IskuLastSet const *ls) {
	return GUINT32_FROM_LE(ls->real_time);
}

static inline void isku_last_set_set_real_time(IskuLastSet *ls, guint32 new_value) {
	ls->real_time = GUINT32_TO_LE(new_value);
}

guint32 isku_last_set_read(RoccatDevice *isku, GError **error);
gboolean isku_last_set_write(RoccatDevice *isku, GError **error);

struct _Isku15 {
	guint8 report_id; /* ISKU_REPORT_ID_15 */
	guint8 size; /* always 3 */
	guint8 unknown;
} __attribute__ ((packed));

struct _IskuReset {
	guint8 report_id; /* ISKU_REPORT_ID_RESET */
	guint8 size; /* always 3 */
	guint8 function; /* always 4 */
};

typedef enum {
	ISKU_RESET_FUNCTION_UNKNOWN1 = 0x1,
	ISKU_RESET_FUNCTION_UNKNOWN2 = 0x2,
	ISKU_RESET_FUNCTION_UNKNOWN3 = 0x3,
	ISKU_RESET_FUNCTION_RESET = 0x4,
} IskuResetFunction;

gboolean isku_reset(RoccatDevice *isku, guint function, GError **error);

struct _IskuTalk {
	guint8 report_id; /* ISKU_REPORT_ID_TALK */
	guint8 size; /* always 0x10 */
	guint8 easyshift;
	guint8 easyshift_lock;
	guint8 unused[12];
} __attribute__ ((packed));

/* also valid as IskuTalkEasyshiftLock */
typedef enum {
	ISKU_TALK_EASYSHIFT_OFF = 0,
	ISKU_TALK_EASYSHIFT_ON = 1,
	ISKU_TALK_EASYSHIFT_UNUSED = 0xff,
} IskuTalkEasyshift;

struct _IskuKeyMask {
	guint8 report_id; /* ISKU_REPORT_ID_KEY_MASK */
	guint8 size; /* always 6 */
	guint8 profile_number; /* 0-4 */
	guint8 mask;
	guint16 checksum;
} __attribute__ ((packed));

typedef enum {
	ISKU_KEY_MASK_BIT_TAB = 0,
	/* TODO 1 seems to do nothing, remaining capslock which is controled differently or should be right shift or something? */
	ISKU_KEY_MASK_BIT_LEFT_WIN = 2,
	ISKU_KEY_MASK_BIT_RIGHT_WIN = 3,
	ISKU_KEY_MASK_BIT_APP_KEY = 4,
	ISKU_KEY_MASK_BIT_LEFT_SHIFT = 5,
} IskuKeyMaskBit;

gboolean isku_key_mask_write(RoccatDevice *isku, guint profile_number, IskuKeyMask *key_mask, GError **error);
IskuKeyMask *isku_key_mask_read(RoccatDevice *isku, guint profile_number, GError **error);
void isku_rkp_update_with_key_mask(IskuRkp *rkp, IskuKeyMask const *key_mask);
gboolean isku_key_mask_equal(IskuKeyMask const *left, IskuKeyMask const *right);

struct _IskuKeysFunction {
	guint8 report_id; /* ISKU_REPORT_ID_KEYS_FUNCTION */
	guint8 size; /* always 0x29 */
	guint8 profile_number; /* 0-4 */
	guint8 types[ISKU_KEYS_FUNCTION_NUM];
	guint8 shortcut_modifier[ISKU_KEYS_FUNCTION_NUM];
	guint8 shortcut_keys[ISKU_KEYS_FUNCTION_NUM];
	guint16 checksum;
} __attribute__ ((packed));

gboolean isku_keys_function_write(RoccatDevice *isku, guint profile_number, IskuKeysFunction *keys_function, GError **error);
IskuKeysFunction *isku_keys_function_read(RoccatDevice *isku, guint profile_number, GError **error);
void isku_rkp_update_with_keys_function(IskuRkp *rkp, IskuKeysFunction const *keys);
gboolean isku_keys_function_equal(IskuKeysFunction const *left, IskuKeysFunction const *right);
gboolean isku_key_index_is_keys_function(guint key_index);

struct _IskuKeysEasyzone {
	guint8 report_id; /* ISKU_REPORT_ID_KEYS_EASYZONE */
	guint8 size; /* always 0x41 */
	guint8 profile_number; /* 0-4 */
	guint8 types[ISKU_KEYS_EASYZONE_NUM];
	guint8 shortcut_modifier[ISKU_KEYS_EASYZONE_NUM];
	guint8 shortcut_keys[ISKU_KEYS_EASYZONE_NUM];
	guint16 checksum;
} __attribute__ ((packed));

gboolean isku_keys_easyzone_write(RoccatDevice *isku, guint profile_number, IskuKeysEasyzone *keys_easyzone, GError **error);
IskuKeysEasyzone *isku_keys_easyzone_read(RoccatDevice *isku, guint profile_number, GError **error);
void isku_rkp_update_with_keys_easyzone(IskuRkp *rkp, IskuKeysEasyzone const *keys);
gboolean isku_keys_easyzone_equal(IskuKeysEasyzone const *left, IskuKeysEasyzone const *right);
gboolean isku_key_index_is_keys_easyzone(guint key_index);

struct _IskuKeysMedia {
	guint8 report_id; /* ISKU_REPORT_ID_KEYS_MEDIA */
	guint8 size; /* always 0x1d */
	guint8 profile_number; /* 0-4 */
	guint8 types[ISKU_KEYS_MEDIA_NUM]; /* No easyshift */
	guint8 shortcut_modifier[ISKU_KEYS_MEDIA_NUM];
	guint8 shortcut_keys[ISKU_KEYS_MEDIA_NUM];
	guint16 checksum;
} __attribute__ ((packed));

gboolean isku_keys_media_write(RoccatDevice *isku, guint profile_number, IskuKeysMedia *keys_media, GError **error);
IskuKeysMedia *isku_keys_media_read(RoccatDevice *isku, guint profile_number, GError **error);
void isku_rkp_update_with_keys_media(IskuRkp *rkp, IskuKeysMedia const *keys);
gboolean isku_keys_media_equal(IskuKeysMedia const *left, IskuKeysMedia const *right);
gboolean isku_key_index_is_keys_media(guint key_index);

struct _IskuKeysThumbster {
	guint8 report_id; /* ISKU_REPORT_ID_KEYS_THUMBSTER */
	guint8 size; /* always 0x17 */
	guint8 profile_number; /* 0-4 */
	guint8 types[ISKU_KEYS_THUMBSTER_NUM];
	guint8 shortcut_modifier[ISKU_KEYS_THUMBSTER_NUM];
	guint8 shortcut_keys[ISKU_KEYS_THUMBSTER_NUM];
	guint16 checksum;
} __attribute__ ((packed));

gboolean isku_keys_thumbster_write(RoccatDevice *isku, guint profile_number, IskuKeysThumbster *keys_thumbster, GError **error);
IskuKeysThumbster *isku_keys_thumbster_read(RoccatDevice *isku, guint profile_number, GError **error);
void isku_rkp_update_with_keys_thumbster(IskuRkp *rkp, IskuKeysThumbster const *keys);
gboolean isku_keys_thumbster_equal(IskuKeysThumbster const *left, IskuKeysThumbster const *right);
gboolean isku_key_index_is_keys_thumbster(guint key_index);

struct _IskuKeysMacro {
	guint8 report_id; /* ISKU_REPORT_ID_KEYS_MACRO */
	guint8 size; /* always 0x23 */
	guint8 profile_number; /* 0-4 */
	guint8 types[ISKU_KEYS_MACRO_NUM];
	guint8 shortcut_modifier[ISKU_KEYS_MACRO_NUM];
	guint8 shortcut_keys[ISKU_KEYS_MACRO_NUM];
	guint16 checksum;
} __attribute__ ((packed));

gboolean isku_keys_macro_write(RoccatDevice *isku, guint profile_number, IskuKeysMacro *keys_macro, GError **error);
IskuKeysMacro *isku_keys_macro_read(RoccatDevice *isku, guint profile_number, GError **error);
void isku_rkp_update_with_keys_macro(IskuRkp *rkp, IskuKeysMacro const *keys_macro);
gboolean isku_keys_macro_equal(IskuKeysMacro const *left, IskuKeysMacro const *right);
gboolean isku_key_index_is_keys_macro(guint key_index);

/* No shortcut possible */
struct _IskuKeysCapslock {
	guint8 report_id; /* ISKU_REPORT_ID_KEYS_CAPSLOCK */
	guint8 size; /* always 0x6 */
	guint8 profile_number; /* 0-4 */
	guint8 type;
	guint16 checksum;
} __attribute__ ((packed));

gboolean isku_keys_capslock_write(RoccatDevice *isku, guint profile_number, IskuKeysCapslock *keys_capslock, GError **error);
IskuKeysCapslock *isku_keys_capslock_read(RoccatDevice *isku, guint profile_number, GError **error);
void isku_rkp_update_with_keys_capslock(IskuRkp *rkp, IskuKeysCapslock const *capslock);
gboolean isku_keys_capslock_equal(IskuKeysCapslock const *left, IskuKeysCapslock const *right);
gboolean isku_key_index_is_keys_capslock(guint key_index);

typedef enum {
	ISKU_KEY_TYPE_DISABLED = 0x00, /* disabled by user */
	ISKU_KEY_TYPE_SYSTEM_SHUT_DOWN = 0x81,
	ISKU_KEY_TYPE_SYSTEM_SLEEP = 0x82,
	ISKU_KEY_TYPE_SYSTEM_WAKE = 0x83,
	ISKU_KEY_TYPE_TALK_BOTH_EASYSHIFT = 0x8c,
	ISKU_KEY_TYPE_STANDARD_MULTIMEDIA_MUTE = 0xa0,
	ISKU_KEY_TYPE_STANDARD_MULTIMEDIA_VOLUME_UP = 0xa1,
	ISKU_KEY_TYPE_STANDARD_MULTIMEDIA_VOLUME_DOWN = 0xa2,
	ISKU_KEY_TYPE_STANDARD_MULTIMEDIA_PLAY_PAUSE = 0xa4,
	ISKU_KEY_TYPE_STANDARD_MULTIMEDIA_PREV_TRACK = 0xa5,
	ISKU_KEY_TYPE_STANDARD_MULTIMEDIA_NEXT_TRACK = 0xa6,
	ISKU_KEY_TYPE_STANDARD_APP_IE_BROWSER = 0xa8,
	ISKU_KEY_TYPE_STANDARD_APP_MY_COMPUTER = 0xa9,
	ISKU_KEY_TYPE_M1 = 0xaa,
	ISKU_KEY_TYPE_M2 = 0xab,
	ISKU_KEY_TYPE_M3 = 0xac,
	ISKU_KEY_TYPE_M4 = 0xad,
	ISKU_KEY_TYPE_M5 = 0xae,
	ISKU_KEY_TYPE_PROFILE_UP = 0xb0,
	ISKU_KEY_TYPE_PROFILE_DOWN = 0xb1,
	ISKU_KEY_TYPE_PROFILE_CYCLE = 0xb2,
	ISKU_KEY_TYPE_PROFILE_1 = 0xb3,
	ISKU_KEY_TYPE_PROFILE_2 = 0xb4,
	ISKU_KEY_TYPE_PROFILE_3 = 0xb5,
	ISKU_KEY_TYPE_PROFILE_4 = 0xb6,
	ISKU_KEY_TYPE_PROFILE_5 = 0xb7,
	ISKU_KEY_TYPE_SHORTCUT = 0xb8,
	ISKU_KEY_TYPE_QUICKLAUNCH = 0xb9,
	ISKU_KEY_TYPE_MACRO_PLAY_PAUSE = 0xba,
	ISKU_KEY_TYPE_TIMER = 0xbc,
	ISKU_KEY_TYPE_TIMER_STOP = 0xbd,
	ISKU_KEY_TYPE_MACRO = 0xbe,
	ISKU_KEY_TYPE_APP_EMAIL = 0xc0,
	ISKU_KEY_TYPE_APP_CALCULATOR = 0xc1,
	ISKU_KEY_TYPE_APP_MY_COMPUTER = 0xc2,
	ISKU_KEY_TYPE_APP_IE_BROWSER = 0xc3,
	ISKU_KEY_TYPE_WWW_SEARCH = 0xc4,
	ISKU_KEY_TYPE_WWW_HOME = 0xc5,
	ISKU_KEY_TYPE_WWW_BACK = 0xc6,
	ISKU_KEY_TYPE_WWW_FORWARD = 0xc7,
	ISKU_KEY_TYPE_WWW_STOP = 0xc8,
	ISKU_KEY_TYPE_WWW_REFRESH = 0xc9,
	ISKU_KEY_TYPE_WWW_FAVORITES = 0xca,
	ISKU_KEY_TYPE_MULTIMEDIA_OPEN_PLAYER = 0xcb,
	ISKU_KEY_TYPE_MULTIMEDIA_PLAY_PAUSE = 0xcc,
	ISKU_KEY_TYPE_MULTIMEDIA_NEXT_TRACK = 0xcd,
	ISKU_KEY_TYPE_MULTIMEDIA_PREV_TRACK = 0xce,
	ISKU_KEY_TYPE_MULTIMEDIA_STOP = 0xcf,
	ISKU_KEY_TYPE_MULTIMEDIA_VOLUME_UP = 0xd0,
	ISKU_KEY_TYPE_MULTIMEDIA_VOLUME_DOWN = 0xd1,
	ISKU_KEY_TYPE_MULTIMEDIA_MUTE = 0xd2,
	ISKU_KEY_TYPE_STANDARD_SHIFT_1 = 0xd3,
	ISKU_KEY_TYPE_STANDARD_SHIFT_2 = 0xd4,
	ISKU_KEY_TYPE_STANDARD_SHIFT_3 = 0xd5,
	ISKU_KEY_TYPE_STANDARD_SHIFT_4 = 0xd6,
	ISKU_KEY_TYPE_STANDARD_SHIFT_5 = 0xd7,
	ISKU_KEY_TYPE_STANDARD_CTRL_1 = 0xd8,
	ISKU_KEY_TYPE_STANDARD_CTRL_2 = 0xd9,
	ISKU_KEY_TYPE_STANDARD_CTRL_3 = 0xda,
	ISKU_KEY_TYPE_STANDARD_CTRL_4 = 0xdb,
	ISKU_KEY_TYPE_STANDARD_CTRL_5 = 0xdc,
	ISKU_KEY_TYPE_OPEN_DRIVER = 0xdd,
	ISKU_KEY_TYPE_UNUSED = 0xdf, /* disabled by software */
	ISKU_KEY_TYPE_STANDARD = 0xef, /* placeholder used in default rkp */
	ISKU_KEY_TYPE_TALK_OTHER_EASYSHIFT = 0xf5,
	ISKU_KEY_TYPE_TALK_OTHER_EASYSHIFT_LOCK = 0xf6,
	ISKU_KEY_TYPE_TALK_OTHER_EASYAIM_1 = 0xf7,
	ISKU_KEY_TYPE_TALK_OTHER_EASYAIM_2 = 0xf8,
	ISKU_KEY_TYPE_TALK_OTHER_EASYAIM_3 = 0xf9,
	ISKU_KEY_TYPE_TALK_OTHER_EASYAIM_4 = 0xfa,
	ISKU_KEY_TYPE_TALK_OTHER_EASYAIM_5 = 0xfb,
	ISKU_KEY_TYPE_T1 = 0xfc,
	ISKU_KEY_TYPE_T2 = 0xfd,
	ISKU_KEY_TYPE_T3 = 0xfe,
	ISKU_KEY_TYPE_EASYSHIFT = 0xff,
} IskuKeyType;

struct _IskuMacro {
	guint8 report_id; /* ISKU_REPORT_ID_MACRO */
	guint16 size; /* always 0x823 */
	guint8 profile_number;
	guint8 index;
	guint8 type;
	guint8 loop;
	guint8 talk_unknown[24];
	guint8 macroset_name[ISKU_MACRO_MACROSET_NAME_LENGTH];
	guint8 macro_name[ISKU_MACRO_MACRO_NAME_LENGTH];
	guint16 count;
	RoccatKeystroke keystrokes[ISKU_MACRO_KEYSTROKES_NUM];
	guint16 checksum;
} __attribute__ ((packed));

static inline guint16 isku_macro_get_size(IskuMacro const *macro) {
	return GUINT16_FROM_LE(macro->size);
}

static inline void isku_macro_set_size(IskuMacro *macro, guint16 new_value) {
	macro->size = GUINT16_TO_LE(new_value);
}

static inline guint16 isku_macro_get_count(IskuMacro const *macro) {
	return GUINT16_FROM_LE(macro->count);
}

static inline void isku_macro_set_count(IskuMacro *macro, guint16 new_value) {
	macro->count = GUINT16_TO_LE(new_value);
}

IskuMacro *isku_rkp_macro_key_info_to_macro(IskuRkpMacroKeyInfo const *macro_key_info);
gboolean isku_macro_write(RoccatDevice *isku, guint profile_number, guint key_index, IskuMacro *macro, GError **error);
void isku_rkp_update_with_macro(IskuRkp *rkp, guint index, IskuMacro const *macro);
IskuMacro *isku_macro_read(RoccatDevice *isku, guint profile_number, guint key_index, GError **error);
IskuRkpMacroKeyInfo *isku_macro_to_rkp_macro_key_info(IskuMacro const *macro);

struct _IskuRkpMacroKeyInfo { /* size 2533 bytes */
	guint8 index;
	guint8 type;
	guint16 unused1;
	guint16 talk_device;
	guint16 unused2;
	guint8 talk_device_name[ISKU_RKP_MACRO_KEY_INFO_TALK_DEVICE_NAME_LENGTH];
	guint8 macroset_name[ISKU_RKP_MACRO_KEY_INFO_MACROSET_NAME_LENGTH];
	guint8 macro_name[ISKU_RKP_MACRO_KEY_INFO_MACRO_NAME_LENGTH];
	guint16 unused3;
	guint32 loop;
	guint16 count;
	RoccatKeystroke keystrokes[ISKU_RKP_MACRO_KEY_INFO_KEYSTROKES_NUM];
	guint16 unused4;
	guint32 timer_length; /* secs */
	guint8 timer_name[ISKU_RKP_MACRO_KEY_INFO_TIMER_NAME_LENGTH];
	guint8 filename[ISKU_RKP_MACRO_KEY_INFO_FILENAME_LENGTH];
	guint8 checksum;
} __attribute__ ((packed));

static inline guint16 isku_rkp_macro_key_info_get_talk_device(IskuRkpMacroKeyInfo const *info) {
	return GUINT16_FROM_LE(info->talk_device);
}

static inline void isku_rkp_macro_key_info_set_talk_device(IskuRkpMacroKeyInfo *info, guint16 new_value) {
	info->talk_device = GUINT16_TO_LE(new_value);
}

static inline guint32 isku_rkp_macro_key_info_get_loop(IskuRkpMacroKeyInfo const *info) {
	return GUINT32_FROM_LE(info->loop);
}

static inline void isku_rkp_macro_key_info_set_loop(IskuRkpMacroKeyInfo *info, guint32 new_value) {
	info->loop = GUINT32_TO_LE(new_value);
}

static inline guint16 isku_rkp_macro_key_info_get_count(IskuRkpMacroKeyInfo const *info) {
	return GUINT16_FROM_LE(info->count);
}

static inline void isku_rkp_macro_key_info_set_count(IskuRkpMacroKeyInfo *info, guint16 new_value) {
	info->count = GUINT16_TO_LE(new_value);
}

static inline guint32 isku_rkp_macro_key_info_get_timer_length(IskuRkpMacroKeyInfo const *info) {
	return GUINT32_FROM_LE(info->timer_length);
}

static inline void isku_rkp_macro_key_info_set_timer_length(IskuRkpMacroKeyInfo *info, guint32 new_value) {
	info->timer_length = GUINT32_TO_LE(new_value);
}

guint8 isku_rkp_macro_key_info_calc_checksum(IskuRkpMacroKeyInfo const *key_info);
void isku_rkp_macro_key_info_set_checksum(IskuRkpMacroKeyInfo *key_info);
IskuRkpMacroKeyInfo *isku_rkp_macro_key_info_new(void);
void isku_rkp_macro_key_info_free(IskuRkpMacroKeyInfo *key_info);
IskuRkpMacroKeyInfo *gaminggear_macro_to_isku_rkp_macro_key_info(GaminggearMacro const *gaminggear_macro, GError **error);
GaminggearMacro *isku_rkp_macro_key_info_to_gaminggear_macro(IskuRkpMacroKeyInfo const *key_info);
IskuRkpMacroKeyInfo *isku_rkp_macro_key_info_new_quicklaunch(gchar const *filename);
IskuRkpMacroKeyInfo *isku_rkp_macro_key_info_new_special(IskuKeyType type);
IskuRkpMacroKeyInfo *isku_rkp_macro_key_info_new_timer(gchar const *name, guint seconds);
IskuRkpMacroKeyInfo *isku_rkp_macro_key_info_new_talk(IskuKeyType type, guint talk_device);
IskuRkpMacroKeyInfo *isku_rkp_macro_key_info_new_shortcut(guint hid_usage_id, guint8 modifier);
gboolean isku_rkp_macro_key_info_equal(IskuRkpMacroKeyInfo const *left, IskuRkpMacroKeyInfo const *right);
IskuRkpMacroKeyInfo *isku_rkp_macro_key_info_dup(IskuRkpMacroKeyInfo const *source);

/* keys 0x00 - 0x23 can hold macros */
typedef enum {
	ISKU_KEY_INDEX_M1 = 0x00,
	ISKU_KEY_INDEX_M2 = 0x01,
	ISKU_KEY_INDEX_M3 = 0x02,
	ISKU_KEY_INDEX_M4 = 0x03,
	ISKU_KEY_INDEX_M5 = 0x04,
	ISKU_KEY_INDEX_SHIFT_M1 = 0x05,
	ISKU_KEY_INDEX_SHIFT_M2 = 0x06,
	ISKU_KEY_INDEX_SHIFT_M3 = 0x07,
	ISKU_KEY_INDEX_SHIFT_M4 = 0x08,
	ISKU_KEY_INDEX_SHIFT_M5 = 0x09,
	ISKU_KEY_INDEX_1 = 0x0a,
	/* Easyzone keys 12345qwertasdfgyxcvb */
	ISKU_KEY_INDEX_B = 0x1d,
	ISKU_KEY_INDEX_T1 = 0x1e,
	ISKU_KEY_INDEX_T2 = 0x1f,
	ISKU_KEY_INDEX_T3 = 0x20,
	ISKU_KEY_INDEX_SHIFT_T1 = 0x21,
	ISKU_KEY_INDEX_SHIFT_T2 = 0x22,
	ISKU_KEY_INDEX_SHIFT_T3 = 0x23,
	ISKU_KEY_INDEX_F1 = 0x24,
	/* F keys */
	ISKU_KEY_INDEX_F12 = 0x2f,
	ISKU_KEY_INDEX_MEDIA_MUTE = 0x30,
	ISKU_KEY_INDEX_MEDIA_DOWN = 0x31,
	ISKU_KEY_INDEX_MEDIA_UP = 0x32,
	ISKU_KEY_INDEX_MEDIA_PLAY = 0x33,
	ISKU_KEY_INDEX_MEDIA_PREV = 0x34,
	ISKU_KEY_INDEX_MEDIA_NEXT = 0x35,
	ISKU_KEY_INDEX_MEDIA_WWW = 0x36,
	ISKU_KEY_INDEX_MEDIA_COMPUTER = 0x37,
	ISKU_KEY_INDEX_CAPSLOCK = 0x38,
/*	ISKU_KEY_INDEX_CAPSLOCK_SHIFT = 0x39, FIXME not possible, but in rkp */
} IskuKeyIndex;

struct _IskuLight {
	guint8 report_id; /* ISKU_REPORT_ID_LIGHT */
	guint8 size; /* always 0xa */
	guint8 profile_number; /* 0-4 */
	guint8 unknown1; /* 3 maybe should be effect */
	guint8 unknown2; /* 0 */
	guint8 brightness; /* 0-5 */
	guint8 dimness;
	guint8 timeout; /* minutes */
	guint16 checksum;
} __attribute__ ((packed));

static inline guint16 isku_light_get_checksum(IskuLight const *light) {
	return GUINT16_FROM_LE(light->checksum);
}

static inline void isku_light_set_checksum(IskuLight *light, guint16 new_value) {
	light->checksum = GUINT16_TO_LE(new_value);
}

gboolean isku_light_write(RoccatDevice *isku, guint profile_number, IskuLight *light, GError **error);
IskuLight *isku_light_read(RoccatDevice *isku, guint profile_number, GError **error);
void isku_rkp_update_with_light(IskuRkp *rkp, IskuLight const *light);
guint16 isku_light_calc_checksum(IskuLight const *light);
gboolean isku_light_equal(IskuLight const *left, IskuLight const *right);

/* value = profile number 0-4 */
typedef enum {
	ISKU_CONTROL_REQUEST_KEY_MASK = 0x50,
	ISKU_CONTROL_REQUEST_KEYS_FUNCTION = 0x60,
	ISKU_CONTROL_REQUEST_LIGHT = 0x70,
	ISKU_CONTROL_REQUEST_KEYS_CAPSLOCK = 0x90,
	ISKU_CONTROL_REQUEST_KEYS_MACRO = 0xa0,
	ISKU_CONTROL_REQUEST_KEYS_EASYZONE = 0xb0,
	ISKU_CONTROL_REQUEST_KEYS_MEDIA = 0xc0,
	ISKU_CONTROL_REQUEST_KEYS_THUMBSTER = 0xd0,
} IskuControlRequest;

gboolean isku_select(RoccatDevice *isku, guint profile_index, guint request, GError **error);

struct _IskuInfo {
	guint8 report_id; /* ISKU_REPORT_ID_F */
	guint8 size; /* always 6 */
	guint8 firmware_version;
	guint8 dfu_version;
	guint8 unknown[2];
} __attribute__ ((packed));

IskuInfo *isku_info_read(RoccatDevice *isku, GError **error);
guint isku_firmware_version_read(RoccatDevice *isku, GError **error);

typedef enum {
	ISKU_INTERFACE_KEYBOARD = 0,
	ISKU_INTERFACE_MOUSE = 1,
} IskuInterface;

struct _IskuSpecial {
	guint8 report_id; /* ISKU_REPORT_ID_SPECIAL */
	guint8 rad;
	guint8 event;
	guint8 data1;
	guint8 data2;
};

/* also valid as IskuChrdevEventEvent */
typedef enum {
	/* data1 = new profile 1-5
	 * data2 = 00
	 */
	ISKU_SPECIAL_EVENT_PROFILE_START = 0x1,
	ISKU_SPECIAL_EVENT_PROFILE = 0x2,

	/* data1 = IskuKeyIndex + 1 */
	ISKU_SPECIAL_EVENT_MACRO = 0x3,

	/* data1 = IskuKeyIndex + 1
	 * data2 = IskuSpecialLiveRecordingData2
	 */
	ISKU_SPECIAL_EVENT_LIVE_RECORDING = 0x4,

	/* data1 = new brightness 1-6
	 * data2 = 00
	 */
	ISKU_SPECIAL_EVENT_LIGHT = 0xc,

	/* data1 = IskuKeyIndex + 1
	 * data2 = IskuSpecialEventAction
	 */
	ISKU_SPECIAL_EVENT_QUICKLAUNCH = 0x7,
	ISKU_SPECIAL_EVENT_TIMER_START = 0xd,
	ISKU_SPECIAL_EVENT_TIMER_STOP = 0xe,
	ISKU_SPECIAL_EVENT_MULTIMEDIA = 0xf,

	/* data1 = IskuSpecialOpenDriverAction
	 * data2 = 00
	 */
	ISKU_SPECIAL_EVENT_OPEN_DRIVER = 0x10,

	/* Isku issues this partially, only IskuFX makes full use */
	ISKU_SPECIAL_EVENT_RAD = 0xfa,

	ISKU_SPECIAL_EVENT_PROFILE_END = 0xfb,
	ISKU_SPECIAL_EVENT_EASYSHIFT_END = 0xfc,
	ISKU_SPECIAL_EVENT_MACRO_END = 0xfd,

	/* data1 = Some matrix code?
	 * data2 = IskuSpecialEventAction
	 */
	ISKU_SPECIAL_EVENT_TALK = 0xff,
} IskuSpecialEvent;

typedef enum  {
	ISKU_SPECIAL_EVENT_ACTION_PRESS = 0,
	ISKU_SPECIAL_EVENT_ACTION_RELEASE = 1,
} IskuSpecialEventAction;

typedef enum {
	ISKU_SPECIAL_OPEN_DRIVER_ACTION_PRESS = 1,
	ISKU_SPECIAL_OPEN_DRIVER_ACTION_RELEASE = 0,
} IskuSpecialOpenDriverAction;

typedef enum {
	ISKU_SPECIAL_LIVE_RECORDING_START = 1, /* data1 = 00 */
	ISKU_SPECIAL_LIVE_RECORDING_MACRO_KEY_SELECTED = 2, /* data1 = KeyIndex + 1 */
	ISKU_SPECIAL_LIVE_RECORDING_END_SUCCESS = 3, /* data1 = KeyIndex + 1 (driver gets macro then and stores updated keys_* in device) */
	ISKU_SPECIAL_LIVE_RECORDING_END_ABORT = 4, /* data1 = 00 */
	ISKU_SPECIAL_LIVE_RECORDING_INVALID_KEY = 5, /* data1 = 00 (event is sent on press and release) */
} IskuSpecialLiveRecordingData2;

typedef enum {
	ISKU_REPORT_SYSTEM_EVENT_WAKE = 0x4,
} IskuReportSystemEvent;

typedef enum {
	ISKU_REPORT_ID_MOUSE = 0x01,
	ISKU_REPORT_ID_2 = 0x02,
	ISKU_REPORT_ID_SPECIAL = 0x03,
	ISKU_REPORT_ID_CONTROL = 0x04, /* rw */
	ISKU_REPORT_ID_ACTUAL_PROFILE = 0x05, /* rw */
	ISKU_REPORT_ID_6 = 0x06, /* r 0x95 bytes */
	ISKU_REPORT_ID_KEY_MASK = 0x07, /* rw */
	ISKU_REPORT_ID_KEYS_FUNCTION = 0x08, /* rw */
	ISKU_REPORT_ID_KEYS_EASYZONE = 0x09, /* rw */
	ISKU_REPORT_ID_KEYS_MEDIA = 0x0a, /* rw */
	ISKU_REPORT_ID_KEYS_THUMBSTER = 0x0b, /* rw */
	ISKU_REPORT_ID_C, /* r preselection needed? */
	ISKU_REPORT_ID_KEYS_MACRO = 0x0d, /* rw */
	ISKU_REPORT_ID_MACRO = 0x0e, /* rw */
	ISKU_REPORT_ID_INFO = 0x0f, /* ro */
	ISKU_REPORT_ID_LIGHT = 0x10, /* rw */
	ISKU_REPORT_ID_RESET = 0x11, /* wo */
	ISKU_REPORT_ID_12 = 0x12, /* r 0x7 bytes */
	ISKU_REPORT_ID_KEYS_CAPSLOCK = 0x13, /* rw */
	ISKU_REPORT_ID_LAST_SET = 0x14, /* rw */
	ISKU_REPORT_ID_DEVICE_STATE = 0x15, /* wo */
	ISKU_REPORT_ID_TALK = 0x16, /* rw */
	ISKU_REPORT_ID_FIRMWARE_WRITE = 0x1b, /* wo */
	ISKU_REPORT_ID_FIRMWARE_WRITE_CONTROL = 0x1c, /* ro */
	ISKU_REPORT_ID_SYSTEM = 0x20,
} IskuReportId;

struct _IskuDeviceState {
	guint8 report_id; /* ISKU_REPORT_ID_DEVICE_STATE */
	guint8 size; /* always 3 */
	guint8 state;
} __attribute__ ((packed));

typedef enum {
	/* Device sends HID multimedia events */
	ISKU_DEVICE_STATE_STATE_OFF = 0x00,
	/* Driver handles multimedia events */
	ISKU_DEVICE_STATE_STATE_ON = 0x01,
} IskuDeviceStateState;

gboolean isku_device_state_write(RoccatDevice *device, guint state, GError **error);

gboolean isku_play_quicklaunch_forked(IskuRkpMacroKeyInfo const *macro_key_info);

RoccatKeyFile *isku_configuration_load(void);
void isku_configuration_free(RoccatKeyFile *config);
gboolean isku_configuration_save(RoccatKeyFile *config, GError **error);

gchar *isku_configuration_get_rkp_path(RoccatKeyFile *config);
void isku_configuration_set_rkp_path(RoccatKeyFile *config, gchar const *path);

gchar *isku_configuration_get_macro_path(RoccatKeyFile *config);
void isku_configuration_set_macro_path(RoccatKeyFile *config, gchar const *path);

gchar *isku_configuration_get_timer_path(RoccatKeyFile *config);
void isku_configuration_set_timer_path(RoccatKeyFile *config, gchar const *path);

gint isku_configuration_get_timer_notification_type(RoccatKeyFile *config);
void isku_configuration_set_timer_notification_type(RoccatKeyFile *config, gint new_value);

gint isku_configuration_get_profile_notification_type(RoccatKeyFile *config);
void isku_configuration_set_profile_notification_type(RoccatKeyFile *config, gint new_value);

gint isku_configuration_get_live_recording_notification_type(RoccatKeyFile *config);
void isku_configuration_set_live_recording_notification_type(RoccatKeyFile *config, gint new_value);

gdouble isku_configuration_get_notification_volume(RoccatKeyFile *config);
void isku_configuration_set_notification_volume(RoccatKeyFile *config, gdouble new_value);

gint isku_configuration_get_default_profile_number(RoccatKeyFile *config);
void isku_configuration_set_default_profile_number(RoccatKeyFile *config, gint new_value);

RoccatDevice *isku_device_first(void);
RoccatDeviceScanner *isku_device_scanner_new(void);
gchar *isku_device_read(RoccatDevice *roccat_device, guint report_id, gssize length, GError **error);
gboolean isku_device_write(RoccatDevice *roccat_device, gchar const *buffer, gssize length, GError **error);

IskuRkp *isku_rkp_load(RoccatDevice *isku, guint profile_index, GError **error);
gboolean isku_rkp_save(RoccatDevice *isku, IskuRkp *rkp, guint profile_index, GError **error);
IskuRkp *isku_rkp_load_save_after_reset(RoccatDevice *isku, guint profile_index, GError **error);

IskuRkp *isku_rkp_load_actual(guint profile_index);
gboolean isku_rkp_save_actual(IskuRkp *rkp, guint profile_index, GError **error);
void isku_rkp_update_from_device(RoccatDevice *isku, IskuRkp *rkp, guint profile_index);

IskuRkp *isku_rkp_read_with_path(gchar const *path, GKeyFile *default_key_file, GError **error);
gboolean isku_rkp_write_with_path(gchar const *path, IskuRkp *rkp, GError **error);
IskuRkp *isku_default_rkp(void);
GKeyFile *isku_rkp_defaults(void);
void isku_rkp_free(IskuRkp *rkp);
IskuRkp *isku_rkp_dup(IskuRkp const *src);
gboolean isku_rkp_get_modified(IskuRkp const *rkp);
void isku_rkp_set_modified(IskuRkp *rkp);
void isku_rkp_set_unmodified(IskuRkp *rkp);

void isku_rkp_set_profile_name(IskuRkp *rkp, gchar const *string);
gchar *isku_rkp_get_profile_name(IskuRkp *rkp);
void isku_rkp_set_light_brightness_without_modified(IskuRkp *rkp, guint value);
void isku_rkp_set_light_brightness(IskuRkp *rkp, guint value);
guint isku_rkp_get_light_brightness(IskuRkp *rkp);
void isku_rkp_set_light_dimness(IskuRkp *rkp, guint value);
guint isku_rkp_get_light_dimness(IskuRkp *rkp);
void isku_rkp_set_light_timeout(IskuRkp *rkp, guint value);
guint isku_rkp_get_light_timeout(IskuRkp *rkp);
void isku_rkp_set_macro_key_info_without_modified(IskuRkp *rkp, guint index, IskuRkpMacroKeyInfo *key_info);
void isku_rkp_set_macro_key_info(IskuRkp *rkp, guint index, IskuRkpMacroKeyInfo *key_info);
IskuRkpMacroKeyInfo *isku_rkp_get_macro_key_info(IskuRkp*rkp, guint index);
gboolean isku_rkp_macro_key_info_equal_type_actual(IskuRkp *rkp, guint key_index, guint type);
void isku_rkp_set_modify_key(IskuRkp *rkp, guint index, gboolean value);
gboolean isku_rkp_get_modify_key(IskuRkp *rkp, guint index);
void isku_rkp_set_key_mask(IskuRkp *rkp, guint8 mask);
guint8 isku_rkp_get_key_mask(IskuRkp *rkp);
void isku_rkp_set_game_file_name(IskuRkp *rkp, guint index, gchar const *string);
gchar *isku_rkp_get_game_file_name(IskuRkp *rkp, guint index);

IskuKeyMask *isku_rkp_to_key_mask(IskuRkp *rkp);
IskuKeysFunction *isku_rkp_to_keys_function(IskuRkp *rkp);
IskuKeysEasyzone *isku_rkp_to_keys_easyzone(IskuRkp *rkp);
IskuKeysMedia *isku_rkp_to_keys_media(IskuRkp *rkp);
IskuKeysThumbster *isku_rkp_to_keys_thumbster(IskuRkp *rkp);
IskuKeysMacro *isku_rkp_to_keys_macro(IskuRkp *rkp);
IskuKeysCapslock *isku_rkp_to_keys_capslock(IskuRkp *rkp);
IskuLight *isku_rkp_to_light(IskuRkp *rkp);
IskuMacro *isku_rkp_to_macro(IskuRkp *rkp, guint index);

gboolean isku_talk_write(RoccatDevice *isku, IskuTalk *talk, GError **error);
gboolean isku_talk_easyshift(RoccatDevice *isku, guint state, GError **error);
gboolean isku_talk_easyshift_lock(RoccatDevice *isku, guint state, GError **error);

#define ISKU_DBUS_SERVER_PATH "/org/roccat/Isku"
#define ISKU_DBUS_SERVER_INTERFACE "org.roccat.Isku"

DBusGProxy *isku_dbus_proxy_new(void);
gboolean isku_dbus_emit_profile_data_changed_outside(DBusGProxy *proxy, guint profile_number);
gboolean isku_dbus_emit_profile_data_changed_outside_instant(guint profile_number);
gboolean isku_dbus_emit_profile_changed_outside(DBusGProxy *proxy, guint profile_number);
gboolean isku_dbus_emit_profile_changed_outside_instant(guint profile_number);
gboolean isku_dbus_emit_configuration_changed_outside(DBusGProxy *proxy);

#define ISKU_DEVICE_NAME "Isku"

G_END_DECLS

#endif
