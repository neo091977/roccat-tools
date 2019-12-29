#ifndef __ROCCAT_KOVA2016_SPECIAL_H__
#define __ROCCAT_KOVA2016_SPECIAL_H__

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

typedef struct _Kova2016Special Kova2016Special;

struct _Kova2016Special {
	guint8 report_id; /* KOVA2016_REPORT_ID_SPECIAL */
	guint8 unused;
	guint8 type;
	guint8 data1;
	guint8 data2;
} __attribute__ ((packed));

typedef enum {
	/* profile_number, 0 */
	KOVA2016_SPECIAL_TYPE_PROFILE = 0x20,
	/* button_number, action */
	KOVA2016_SPECIAL_TYPE_OPEN_APPLICATION = 0x60,
	/* button_number, action */
	KOVA2016_SPECIAL_TYPE_TIMER_START = 0x80,
	/* 0, action */
	KOVA2016_SPECIAL_TYPE_TIMER_STOP = 0x90,
	/* action, 0 */
	KOVA2016_SPECIAL_TYPE_OPEN_DRIVER = 0xa0,
	/* cpi_number, 0 */
	KOVA2016_SPECIAL_TYPE_CPI = 0xb0,
	/* count */
	KOVA2016_SPECIAL_TYPE_RAD_LEFT = 0xe1,
	KOVA2016_SPECIAL_TYPE_RAD_RIGHT = 0xe2,
	KOVA2016_SPECIAL_TYPE_RAD_MIDDLE = 0xe3,
	KOVA2016_SPECIAL_TYPE_RAD_FORWARD = 0xe4, /* FIXME handedness? */
	KOVA2016_SPECIAL_TYPE_RAD_BACKWARD = 0xe5, /* FIXME handedness? */
	KOVA2016_SPECIAL_TYPE_RAD_SCROLL_UP = 0xe6,
	KOVA2016_SPECIAL_TYPE_RAD_SCROLL_DOWN = 0xe7,
	KOVA2016_SPECIAL_TYPE_RAD_EASYAIM = 0xe9,
	/* subtype, action */
	KOVA2016_SPECIAL_TYPE_MULTIMEDIA = 0xf0,
	/* subtype, button_number */
	KOVA2016_SPECIAL_TYPE_OPEN = 0xf3,
	/* button_number, action */
	KOVA2016_SPECIAL_TYPE_EASYSHIFT = 0xff,

} Kova2016SpecialType;

typedef enum {
	KOVA2016_SPECIAL_SUBTYPE_MULTIMEDIA_PREV_TRACK = 0x21,
	KOVA2016_SPECIAL_SUBTYPE_MULTIMEDIA_NEXT_TRACK = 0x22,
	KOVA2016_SPECIAL_SUBTYPE_MULTIMEDIA_PLAY_PAUSE = 0x23,
	KOVA2016_SPECIAL_SUBTYPE_MULTIMEDIA_STOP = 0x24,
	KOVA2016_SPECIAL_SUBTYPE_MULTIMEDIA_MUTE_MASTER = 0x25,
	KOVA2016_SPECIAL_SUBTYPE_MULTIMEDIA_VOLUME_UP = 0x26,
	KOVA2016_SPECIAL_SUBTYPE_MULTIMEDIA_VOLUME_DOWN = 0x27,
} Kova2016SpecialSubtypeMultimedia;

typedef enum {
	KOVA2016_SPECIAL_SUBTYPE_OPEN_DOCUMENT = 0x01,
	KOVA2016_SPECIAL_SUBTYPE_OPEN_FOLDER = 0x02,
	KOVA2016_SPECIAL_SUBTYPE_OPEN_WEBSITE = 0x03,
} Kova2016SpecialSubtypeOpen;

typedef enum {
	KOVA2016_SPECIAL_ACTION_PRESS = 0x00,
	KOVA2016_SPECIAL_ACTION_RELEASE = 0x01,
} Kova2016SpecialAction;

#endif
