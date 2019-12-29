#ifndef __ROCCAT_SOVA_EVENT_H__
#define __ROCCAT_SOVA_EVENT_H__

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

#include "sova.h"

G_BEGIN_DECLS

/* Content compatible with Sova */

typedef struct _SovaSpecial SovaSpecial;

struct _SovaSpecial {
	guint8 report_id; /* SOVA_REPORT_ID_SPECIAL */
	guint8 subtype;
	guint8 type;
	guint8 data;
	guint8 action;
} __attribute__ ((packed));

typedef enum {
	SOVA_SPECIAL_ACTION_PRESS = 0,
	SOVA_SPECIAL_ACTION_RELEASE = 1,
} SovaSpecialAction;

typedef enum {
	SOVA_SPECIAL_FUNCTION_ACTIVATE = 1,
	SOVA_SPECIAL_FUNCTION_DEACTIVATE = 0,
} SovaSpecialFunction;

typedef enum {
	SOVA_SPECIAL_LIVE_RECORDING_ACTION_START = 1, /* data = 0x00 */
	SOVA_SPECIAL_LIVE_RECORDING_ACTION_MACRO_KEY_SELECTED = 2, /* data = easyzone index */
	SOVA_SPECIAL_LIVE_RECORDING_ACTION_END_SUCCESS = 3, /* data = easyzone index */
	SOVA_SPECIAL_LIVE_RECORDING_ACTION_END_ABORT = 4, /* data = 0x00 */
	SOVA_SPECIAL_LIVE_RECORDING_ACTION_INVALID_KEY = 5, /* data = 0x00 */
} SovaSpecialLiveRecordingAction;

typedef enum {
	SOVA_SPECIAL_SUBTYPE_OPEN_FOLDER_DESKTOP = 0x02,
	SOVA_SPECIAL_SUBTYPE_OPEN_FOLDER_FAVORITES = 0x03,
	SOVA_SPECIAL_SUBTYPE_OPEN_FOLDER_FONTS = 0x04,
	SOVA_SPECIAL_SUBTYPE_OPEN_FOLDER_MY_DOCUMENTS = 0x05,
	SOVA_SPECIAL_SUBTYPE_OPEN_FOLDER_MY_DOWNLOADS = 0x06,
	SOVA_SPECIAL_SUBTYPE_OPEN_FOLDER_MY_MUSIC = 0x07,
	SOVA_SPECIAL_SUBTYPE_OPEN_FOLDER_MY_PICTURES = 0x08,
	SOVA_SPECIAL_SUBTYPE_OPEN_FOLDER_NETWORK = 0x09,
	SOVA_SPECIAL_SUBTYPE_OPEN_FOLDER_PRINTERS = 0x0a,
	SOVA_SPECIAL_SUBTYPE_OPEN_FOLDER_SYSTEM = 0x0b,

	SOVA_SPECIAL_SUBTYPE_OPEN_WINDOWS_CONTROL_PANEL = 0x20,
	SOVA_SPECIAL_SUBTYPE_OPEN_WINDOWS_SYSTEM_PANEL = 0x21,
	SOVA_SPECIAL_SUBTYPE_OPEN_WINDOWS_TASK_MANAGER = 0x22,
	SOVA_SPECIAL_SUBTYPE_OPEN_WINDOWS_SCREEN_SETTINGS = 0x23,
	SOVA_SPECIAL_SUBTYPE_OPEN_WINDOWS_SCREENSAVER = 0x24,
	SOVA_SPECIAL_SUBTYPE_OPEN_WINDOWS_THEMES = 0x25,
	SOVA_SPECIAL_SUBTYPE_OPEN_WINDOWS_DATE_TIME = 0x26,
	SOVA_SPECIAL_SUBTYPE_OPEN_WINDOWS_NETWORK_SETTINGS = 0x27,
	SOVA_SPECIAL_SUBTYPE_OPEN_WINDOWS_ADMIN_PANEL = 0x28,
	SOVA_SPECIAL_SUBTYPE_OPEN_WINDOWS_FIREWALL = 0x29,
	SOVA_SPECIAL_SUBTYPE_OPEN_WINDOWS_REGEDIT = 0x2a,
	SOVA_SPECIAL_SUBTYPE_OPEN_WINDOWS_EVENT_MONITOR = 0x2b,
	SOVA_SPECIAL_SUBTYPE_OPEN_WINDOWS_PERFORMANCE_MONITOR = 0x2c,
	SOVA_SPECIAL_SUBTYPE_OPEN_WINDOWS_SOUND_SETTINGS = 0x2d,
	SOVA_SPECIAL_SUBTYPE_OPEN_WINDOWS_INTERNET_SETTINGS = 0x2e,
	SOVA_SPECIAL_SUBTYPE_OPEN_WINDOWS_DIRECTX_DIAGNOSTICS = 0x2f,
	SOVA_SPECIAL_SUBTYPE_OPEN_COMMAND_LINE = 0x30,
	SOVA_SPECIAL_SUBTYPE_OPEN_DOCUMENT = 0x11,
	SOVA_SPECIAL_SUBTYPE_OPEN_FOLDER = 0x32,
	SOVA_SPECIAL_SUBTYPE_OPEN_WEBSITE = 0x33,
} SovaSpecialSubtypeOpen;

typedef enum {
	SOVA_SPECIAL_SUBTYPE_POWERSAVE_WAKEUP = 0x01,
	SOVA_SPECIAL_SUBTYPE_POWERSAVE_SLEEP = 0x02,
} SovaSpecialSubtypePowersave;

typedef enum {
	SOVA_SPECIAL_SUBTYPE_RAD_EASYSHIFT = 0x08,
	SOVA_SPECIAL_SUBTYPE_RAD_MULTIMEDIA = 0x09,
	SOVA_SPECIAL_SUBTYPE_RAD_MACRO_SHORTCUT = 0x0f,
	SOVA_SPECIAL_SUBTYPE_RAD_TALK = 0x10,
	SOVA_SPECIAL_SUBTYPE_RAD_LIVE_RECORDING = 0x11,
	SOVA_SPECIAL_SUBTYPE_RAD_TOTAL = 0x13,
} SovaSpecialSubtypeRad;

typedef enum {
	/* single event
	 * subtype = 0x00
	 * data = profile index
	 * action = 0x00
	 */
	SOVA_SPECIAL_TYPE_PROFILE_START = 0x01,
	SOVA_SPECIAL_TYPE_PROFILE = 0x02,
	
	/* single event
	 * subtype = 0x00
	 * data = easyzone index
	 * action = 0x00
	 */
	SOVA_SPECIAL_TYPE_MACRO = 0x03,

	/* single event
	 * subtype = 0x00
	 * data = see SovaSpecialLiveRecordingAction
	 * action = SovaSpecialLiveRecordingAction
	 */
	SOVA_SPECIAL_TYPE_LIVE_RECORDING = 0x04,

	/* subtype = 0x00
	 * data = easyzone index
	 * action = SovaSpecialAction
	 */
	SOVA_SPECIAL_TYPE_OPEN_APP = 0x07,

	/* subtype = 0x00
	 * data = 0xff
	 * action = SovaSpecialAction
	 */
	SOVA_SPECIAL_TYPE_EASYSHIFT = 0x0a,

	/* single event
	 * subtype = 0x00
	 * data = 0-5
	 * action = 0x00
	 */
	SOVA_SPECIAL_TYPE_BACKLIGHT = 0x0c,

	/* subtype = 0x00
	 * data = easyzone index
	 * action = SovaSpecialAction
	 */
	SOVA_SPECIAL_TYPE_TIMER_START = 0x0d,
	SOVA_SPECIAL_TYPE_TIMER_STOP = 0x0e,

	/* subtype = 0x00
	 * data = SovaSpecialFunction
	 */
	SOVA_SPECIAL_TYPE_OPEN_DRIVER = 0x10,
	
	/* subtype = SovaSpecialSubtypeOpen
	 * data = easyzone index
	 * action = SovaSpecialAction;
	 */
	SOVA_SPECIAL_TYPE_OPEN = 0x11,

	/* single event
	 * subtype = SovaSpecialSubtypePowersave
	 * data = 0x01
	 * action = 0x00
	 */
	SOVA_SPECIAL_TYPE_POWERSAVE = 0xef,
	
	/* single event
	 * subtype = SovaSpecialSubtypeRad
	 * data = 0x01
	 * action = 0x00
	 */
	SOVA_SPECIAL_TYPE_RAD = 0xfa,

	/* subtype = 0x00
	 * data = SovaSpecialFunction
	 * action = 0x00
	 */
	SOVA_SPECIAL_TYPE_EASYSHIFT_SELF = 0xfd,
	
	/* single event
	 * subtype = 0x00
	 * data = primary index
	 * action = 0x01
	 */
	SOVA_SPECIAL_TYPE_TALK = 0xff,
	
} SovaSpecialType;

G_END_DECLS

#endif
