#ifndef __ROCCAT_NYTH_PROFILE_BUTTON_H__
#define __ROCCAT_NYTH_PROFILE_BUTTON_H__

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

#include "roccat_button.h"

G_BEGIN_DECLS

typedef enum {
	NYTH_BUTTON_TYPE_DISABLED = 0x00,
	NYTH_BUTTON_TYPE_CLICK = 0x01,
	NYTH_BUTTON_TYPE_MENU = 0x02,
	NYTH_BUTTON_TYPE_UNIVERSAL_SCROLLING = 0x03,
	NYTH_BUTTON_TYPE_DOUBLE_CLICK = 0x04, /* missing in Windows */
	NYTH_BUTTON_TYPE_SHORTCUT = 0x05,

	NYTH_BUTTON_TYPE_BROWSER_FORWARD = 0x07,
	NYTH_BUTTON_TYPE_BROWSER_BACKWARD = 0x08,
	NYTH_BUTTON_TYPE_TILT_LEFT = 0x09,
	NYTH_BUTTON_TYPE_TILT_RIGHT = 0x0a,

	NYTH_BUTTON_TYPE_SCROLL_UP = 0x0d,
	NYTH_BUTTON_TYPE_SCROLL_DOWN = 0x0e,
	NYTH_BUTTON_TYPE_OPEN_APPLICATION = 0x0f,
	NYTH_BUTTON_TYPE_PROFILE_CYCLE = 0x10,
	NYTH_BUTTON_TYPE_PROFILE_UP = 0x11,
	NYTH_BUTTON_TYPE_PROFILE_DOWN = 0x12,

	NYTH_BUTTON_TYPE_CPI_CYCLE = 0x14,
	NYTH_BUTTON_TYPE_CPI_UP = 0x15,
	NYTH_BUTTON_TYPE_CPI_DOWN = 0x16,
	NYTH_BUTTON_TYPE_SENSITIVITY_CYCLE = 0x17,
	NYTH_BUTTON_TYPE_SENSITIVITY_UP = 0x18,
	NYTH_BUTTON_TYPE_SENSITIVITY_DOWN = 0x19,

	NYTH_BUTTON_TYPE_OPEN_DRIVER = 0x1b,

	NYTH_BUTTON_TYPE_PREV_TRACK = 0x21,
	NYTH_BUTTON_TYPE_NEXT_TRACK = 0x22,
	NYTH_BUTTON_TYPE_PLAY_PAUSE = 0x23,
	NYTH_BUTTON_TYPE_STOP = 0x24,
	NYTH_BUTTON_TYPE_MUTE_MASTER = 0x25,
	NYTH_BUTTON_TYPE_VOLUME_DOWN = 0x26,
	NYTH_BUTTON_TYPE_VOLUME_UP = 0x27,

	NYTH_BUTTON_TYPE_MACRO = 0x30,
	NYTH_BUTTON_TYPE_TIMER = 0x31,
	NYTH_BUTTON_TYPE_TIMER_STOP = 0x32,
	NYTH_BUTTON_TYPE_EASYAIM_1 = 0x33,
	NYTH_BUTTON_TYPE_EASYAIM_2 = 0x34,
	NYTH_BUTTON_TYPE_EASYAIM_3 = 0x35,
	NYTH_BUTTON_TYPE_EASYAIM_4 = 0x36,
	NYTH_BUTTON_TYPE_EASYAIM_5 = 0x37,

	NYTH_BUTTON_TYPE_EASYSHIFT_SELF = 0x41,
	NYTH_BUTTON_TYPE_EASYWHEEL_SENSITIVITY = 0x42,
	NYTH_BUTTON_TYPE_EASYWHEEL_PROFILE = 0x43,
	NYTH_BUTTON_TYPE_EASYWHEEL_CPI = 0x44,
	NYTH_BUTTON_TYPE_EASYWHEEL_VOLUME = 0x45,
	NYTH_BUTTON_TYPE_EASYWHEEL_ALT_TAB = 0x46,
	NYTH_BUTTON_TYPE_EASYWHEEL_AERO_FLIP_3D = 0x47,

	NYTH_BUTTON_TYPE_EASYSHIFT_OTHER = 0x51,
	NYTH_BUTTON_TYPE_EASYSHIFT_LOCK_OTHER = 0x52, /* missing in Windows */
	NYTH_BUTTON_TYPE_EASYSHIFT_ALL = 0x53,

	NYTH_BUTTON_TYPE_HOME = 0x62,
	NYTH_BUTTON_TYPE_END = 0x63,

	NYTH_BUTTON_TYPE_SENSITIVITY__5 = 0x71,
	NYTH_BUTTON_TYPE_SENSITIVITY__4 = 0x72,
	NYTH_BUTTON_TYPE_SENSITIVITY__3 = 0x73,
	NYTH_BUTTON_TYPE_SENSITIVITY__2 = 0x74,
	NYTH_BUTTON_TYPE_SENSITIVITY__1 = 0x75,
	NYTH_BUTTON_TYPE_SENSITIVITY_0 = 0x76,
	NYTH_BUTTON_TYPE_SENSITIVITY_1 = 0x77,
	NYTH_BUTTON_TYPE_SENSITIVITY_2 = 0x78,
	NYTH_BUTTON_TYPE_SENSITIVITY_3 = 0x79,
	NYTH_BUTTON_TYPE_SENSITIVITY_4 = 0x7a,
	NYTH_BUTTON_TYPE_SENSITIVITY_5 = 0x7b,

	NYTH_BUTTON_TYPE_BROWSER = 0x7e,
	NYTH_BUTTON_TYPE_BROWSER_FAVORITES = 0x7f,
	NYTH_BUTTON_TYPE_BROWSER_SEARCH = 0x80,
	NYTH_BUTTON_TYPE_BROWSER_HOME = 0x81,
	NYTH_BUTTON_TYPE_BROWSER_STOP = 0x82,
	NYTH_BUTTON_TYPE_BROWSER_REFRESH = 0x83,
	NYTH_BUTTON_TYPE_BROWSER_NEW_TAB = 0x84,
	NYTH_BUTTON_TYPE_BROWSER_NEW_WINDOW = 0x85,
	NYTH_BUTTON_TYPE_OPEN_FOLDER_COMPUTER = 0x86,
	NYTH_BUTTON_TYPE_OPEN_APPLICATION_CALCULATOR = 0x87,
	NYTH_BUTTON_TYPE_OPEN_APPLICATION_EMAIL = 0x88,
	NYTH_BUTTON_TYPE_OPEN_DOCUMENT = 0x89,
	NYTH_BUTTON_TYPE_OPEN_FOLDER = 0x8a,
	NYTH_BUTTON_TYPE_OPEN_WEBSITE = 0x8b,
	NYTH_BUTTON_TYPE_MUTE_MICROPHONE = 0x8c,
	NYTH_BUTTON_TYPE_OPEN_FOLDER_DESKTOP = 0x8d,
	NYTH_BUTTON_TYPE_OPEN_FOLDER_FAVORITES = 0x8e,
	NYTH_BUTTON_TYPE_OPEN_FOLDER_FONTS = 0x8f,
	NYTH_BUTTON_TYPE_OPEN_FOLDER_MY_DOCUMENTS = 0x90,
	NYTH_BUTTON_TYPE_OPEN_FOLDER_MY_DOWNLOADS = 0x91,
	NYTH_BUTTON_TYPE_OPEN_FOLDER_MY_MUSIC = 0x92,
	NYTH_BUTTON_TYPE_OPEN_FOLDER_MY_PICTURES = 0x93,
	NYTH_BUTTON_TYPE_OPEN_FOLDER_NETWORK = 0x94,
	NYTH_BUTTON_TYPE_OPEN_FOLDER_PRINTERS = 0x95,
	NYTH_BUTTON_TYPE_OPEN_FOLDER_SYSTEM = 0x96,

	NYTH_BUTTON_TYPE_SYSTEM_HIBERNATE = 0xa7,
	NYTH_BUTTON_TYPE_SYSTEM_REBOOT = 0xa8,
	NYTH_BUTTON_TYPE_SYSTEM_LOCK = 0xa9,
	NYTH_BUTTON_TYPE_SYSTEM_LOGOFF = 0xaa,
	NYTH_BUTTON_TYPE_WINDOWS_FUNCTION_CONTROL_PANEL = 0xab,
	NYTH_BUTTON_TYPE_WINDOWS_FUNCTION_SYSTEM_PANEL = 0xac,
	NYTH_BUTTON_TYPE_WINDOWS_FUNCTION_TASK_MANAGER = 0xad,
	NYTH_BUTTON_TYPE_WINDOWS_FUNCTION_SCREEN_SETTINGS = 0xae,
	NYTH_BUTTON_TYPE_WINDOWS_FUNCTION_SCREENSAVER = 0xaf,
	NYTH_BUTTON_TYPE_WINDOWS_FUNCTION_THEMES = 0xb0,
	NYTH_BUTTON_TYPE_WINDOWS_FUNCTION_DATE_TIME = 0xb1,
	NYTH_BUTTON_TYPE_WINDOWS_FUNCTION_NETWORK_SETTINGS = 0xb2,
	NYTH_BUTTON_TYPE_WINDOWS_FUNCTION_ADMIN_PANEL = 0xb3,
	NYTH_BUTTON_TYPE_WINDOWS_FUNCTION_FIREWALL = 0xb4,
	NYTH_BUTTON_TYPE_WINDOWS_FUNCTION_REGEDIT = 0xb5,
	NYTH_BUTTON_TYPE_WINDOWS_FUNCTION_EVENT_MONITOR = 0xb6,
	NYTH_BUTTON_TYPE_WINDOWS_FUNCTION_PERFORMANCE_MONITOR = 0xb7,
	NYTH_BUTTON_TYPE_WINDOWS_FUNCTION_SOUND_SETTINGS = 0xb8,
	NYTH_BUTTON_TYPE_WINDOWS_FUNCTION_INTERNET_SETTINGS = 0xb9,
	NYTH_BUTTON_TYPE_WINDOWS_FUNCTION_DIREKTX_DIAG= 0xba,
	NYTH_BUTTON_TYPE_WINDOWS_FUNCTION_COMMAND_LINE = 0xbb,
	NYTH_BUTTON_TYPE_SYSTEM_SHUTDOWN = 0xbc,
	NYTH_BUTTON_TYPE_SYSTEM_SLEEP = 0xbd,
	NYTH_BUTTON_TYPE_SYSTEM_WAKE = 0xbe,
	NYTH_BUTTON_TYPE_PROFILE_1 = 0xbf,
	NYTH_BUTTON_TYPE_PROFILE_2 = 0xc0,
	NYTH_BUTTON_TYPE_PROFILE_3 = 0xc1,
	NYTH_BUTTON_TYPE_PROFILE_4 = 0xc2,
	NYTH_BUTTON_TYPE_PROFILE_5 = 0xc3,
} NythButtonType;

G_END_DECLS

#endif
