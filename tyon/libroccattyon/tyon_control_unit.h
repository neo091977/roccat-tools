#ifndef __ROCCAT_TYON_CONTROL_UNIT_H__
#define __ROCCAT_TYON_CONTROL_UNIT_H__

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

#include "roccat_device.h"

G_BEGIN_DECLS

typedef struct _TyonControlUnit TyonControlUnit;

struct _TyonControlUnit {
	guint8 report_id; /* TYON_REPORT_ID_CONTROL_UNIT */
	guint8 size; /* always 6 */
	guint8 dcu;
	guint8 tcu;
	guint8 median;
	guint8 action;
} __attribute__ ((packed));

typedef enum {
	TYON_CONTROL_UNIT_ACTION_CANCEL = 0x00,
	TYON_CONTROL_UNIT_ACTION_ACCEPT = 0x01,
	TYON_CONTROL_UNIT_ACTION_OFF = 0x02,
	TYON_CONTROL_UNIT_ACTION_UNDEFINED = 0xff,
} TyonControlUnitAction;

typedef enum {
	TYON_TRACKING_CONTROL_UNIT_OFF = 0,
	TYON_TRACKING_CONTROL_UNIT_ON = 1,
} TyonControlUnitTcu;

typedef enum {
	TYON_DISTANCE_CONTROL_UNIT_OFF = 0,
	TYON_DISTANCE_CONTROL_UNIT_EXTRA_LOW = 1,
	TYON_DISTANCE_CONTROL_UNIT_LOW = 2,
	TYON_DISTANCE_CONTROL_UNIT_NORMAL = 3,
} TyonControlUnitDcu;

TyonControlUnit *tyon_control_unit_read(RoccatDevice *tyon, GError **error);

gboolean tyon_tracking_control_unit_test(RoccatDevice *tyon, guint dcu, guint median, GError **error);
gboolean tyon_tracking_control_unit_cancel(RoccatDevice *tyon, guint dcu, GError **error);
gboolean tyon_tracking_control_unit_accept(RoccatDevice *tyon, guint dcu, guint median, GError **error);
gboolean tyon_tracking_control_unit_off(RoccatDevice *tyon, guint dcu, GError **error);

guint tyon_distance_control_unit_get(RoccatDevice *tyon, GError **error);
gboolean tyon_distance_control_unit_try(RoccatDevice *tyon, guint new_dcu, GError **error);
gboolean tyon_distance_control_unit_cancel(RoccatDevice *tyon, guint old_dcu, GError **error);
gboolean tyon_distance_control_unit_accept(RoccatDevice *tyon, guint new_dcu, GError **error);

G_END_DECLS

#endif
