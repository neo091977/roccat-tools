#ifndef __ROCCAT_NYTH_CONTROL_UNIT_H__
#define __ROCCAT_NYTH_CONTROL_UNIT_H__

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

typedef struct _NythControlUnit NythControlUnit;

struct _NythControlUnit {
	guint8 report_id; /* NYTH_REPORT_ID_CONTROL_UNIT */
	guint8 size; /* always 6 */
	guint8 dcu;
	guint8 tcu;
	guint8 unused;
	guint8 action;
} __attribute__ ((packed));

typedef enum {
	NYTH_CONTROL_UNIT_ACTION_OFF = 0x00,
	NYTH_CONTROL_UNIT_ACTION_START = 0x01,
	NYTH_CONTROL_UNIT_ACTION_ACCEPT = 0x03,
	NYTH_CONTROL_UNIT_ACTION_CANCEL = 0x04,
} NythControlUnitAction;

typedef enum {
	NYTH_TRACKING_CONTROL_UNIT_OFF = 0,
	NYTH_TRACKING_CONTROL_UNIT_ON = 1,
} NythControlUnitTcu;

typedef enum {
	NYTH_DISTANCE_CONTROL_UNIT_OFF = 0,
	NYTH_DISTANCE_CONTROL_UNIT_EXTRA_LOW = 1,
	NYTH_DISTANCE_CONTROL_UNIT_LOW = 2,
	NYTH_DISTANCE_CONTROL_UNIT_NORMAL = 3,
} NythControlUnitDcu;

NythControlUnit *nyth_control_unit_read(RoccatDevice *nyth, GError **error);

gboolean nyth_tracking_control_unit_start(RoccatDevice *nyth, GError **error);
gboolean nyth_tracking_control_unit_cancel(RoccatDevice *nyth, GError **error);
gboolean nyth_tracking_control_unit_accept(RoccatDevice *nyth, GError **error);
gboolean nyth_tracking_control_unit_off(RoccatDevice *nyth, GError **error);

guint nyth_distance_control_unit_get(RoccatDevice *nyth, GError **error);
gboolean nyth_distance_control_unit_set(RoccatDevice *nyth, guint new_dcu, GError **error);

G_END_DECLS

#endif
