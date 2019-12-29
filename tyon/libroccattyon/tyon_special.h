#ifndef __ROCCAT_TYON_SPECIAL_H__
#define __ROCCAT_TYON_SPECIAL_H__

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

#include <glib.h>

G_BEGIN_DECLS

typedef struct _TyonSpecial TyonSpecial;

struct _TyonSpecial {
	guint8 report_id; /* TYON_REPORT_ID_SPECIAL */
	guint8 analogue;
	guint8 type;
	guint8 data;
	guint8 action;
} __attribute__ ((packed));

typedef enum {
	TYON_SPECIAL_TYPE_TILT = 0x10, /* data: 0xff = left, 0x01 = right */
	TYON_SPECIAL_TYPE_PROFILE = 0x20, /* data: profile_nr */
	/* gets sent on xcelerator down if set to shortcut */
	TYON_SPECIAL_TYPE_XCELERATOR = 0x25, /* data: value, action = 0xa9 */
	TYON_SPECIAL_TYPE_QUICKLAUNCH = 0x60, /* data: button_nr, action: press/release */
	TYON_SPECIAL_TYPE_TIMER_START = 0x80, /* data: button_nr, action: press/release */
	TYON_SPECIAL_TYPE_TIMER_STOP = 0x90, /* action: press/release */
	TYON_SPECIAL_TYPE_OPEN_DRIVER = 0xa0, /* data: press/release ! */
	TYON_SPECIAL_TYPE_CPI = 0xb0, /* data: 1-5 */
	TYON_SPECIAL_TYPE_SENSITIVITY = 0xc0, /* data: action: 1-b */
	TYON_SPECIAL_TYPE_ANALOGUE = 0xd1, /* analogue: value, data: 0x00, action: 0x10 */
	TYON_SPECIAL_TYPE_XCELERATOR_CALIBRATION = 0xe0, /* data: 0x06, action: value */
	TYON_SPECIAL_TYPE_RAD_LEFT = 0xe1, /* all rads: data: count */
	TYON_SPECIAL_TYPE_RAD_RIGHT = 0xe2,
	TYON_SPECIAL_TYPE_RAD_MIDDLE = 0xe3,
	TYON_SPECIAL_TYPE_RAD_THUMB_BACKWARD = 0xe4,
	TYON_SPECIAL_TYPE_RAD_THUMB_FORWARD = 0xe5,
	TYON_SPECIAL_TYPE_RAD_SCROLL_UP = 0xe6,
	TYON_SPECIAL_TYPE_RAD_SCROLL_DOWN = 0xe7,
	TYON_SPECIAL_TYPE_RAD_EASYSHIFT = 0xe8,
	TYON_SPECIAL_TYPE_RAD_EASYAIM = 0xe9,
	TYON_SPECIAL_TYPE_RAD_DISTANCE = 0xea,
	TYON_SPECIAL_TYPE_MULTIMEDIA = 0xf0, /* data: ?, action: press/release */
	TYON_SPECIAL_TYPE_TALK = 0xff, /* data: button_nr, action: press/release */
} TyonSpecialType;

typedef enum {
	TYON_SPECIAL_ACTION_PRESS = 0x00,
	TYON_SPECIAL_ACTION_RELEASE = 0x01,
} TyonSpecialAction;

G_END_DECLS

#endif
