#ifndef __ROCCAT_KIRO_SPECIAL_H__
#define __ROCCAT_KIRO_SPECIAL_H__

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

typedef struct _KiroSpecial KiroSpecial;

struct _KiroSpecial {
	guint8 report_id; /* KIRO_REPORT_ID_SPECIAL */
	guint8 unused;
	guint8 type;
	guint8 data1;
	guint8 data2;
} __attribute__ ((packed));

typedef enum {
	/* direction, 0 */
	KIRO_SPECIAL_TYPE_TILT = 0x10,
	/* button_number, action */
	KIRO_SPECIAL_TYPE_OPEN_APPLICATION = 0x60,
	/* action, 0 */
	KIRO_SPECIAL_TYPE_OPEN_DRIVER = 0xa0,
	/* cpi_number, 0 */
	KIRO_SPECIAL_TYPE_CPI = 0xb0,
	/* sensitivity_number, 0 */
	KIRO_SPECIAL_TYPE_SENSITIVITY = 0xc0,
	/* subtype, action */
	KIRO_SPECIAL_TYPE_MULTIMEDIA = 0xf0,
} KiroSpecialType;

typedef enum {
	KIRO_SPECIAL_SUBTYPE_MULTIMEDIA_PREV_TRACK = 0x21,
	KIRO_SPECIAL_SUBTYPE_MULTIMEDIA_NEXT_TRACK = 0x22,
	KIRO_SPECIAL_SUBTYPE_MULTIMEDIA_PLAY_PAUSE = 0x23,
	KIRO_SPECIAL_SUBTYPE_MULTIMEDIA_STOP = 0x24,
	KIRO_SPECIAL_SUBTYPE_MULTIMEDIA_MUTE_MASTER = 0x25,
	KIRO_SPECIAL_SUBTYPE_MULTIMEDIA_VOLUME_UP = 0x26,
	KIRO_SPECIAL_SUBTYPE_MULTIMEDIA_VOLUME_DOWN = 0x27,
} KiroSpecialSubtypeMultimedia;

typedef enum {
	KIRO_SPECIAL_SUBTYPE_OPEN_APPLICATION = 0x03,
} KiroSpecialSubtypeOpen;

typedef enum {
	KIRO_SPECIAL_ACTION_PRESS = 0x00,
	KIRO_SPECIAL_ACTION_RELEASE = 0x01,
} KiroSpecialAction;

#endif
