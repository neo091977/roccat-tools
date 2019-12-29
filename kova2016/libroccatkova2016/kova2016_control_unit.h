#ifndef __ROCCAT_KOVA2016_CONTROL_UNIT_H__
#define __ROCCAT_KOVA2016_CONTROL_UNIT_H__

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

typedef struct _Kova2016ControlUnit Kova2016ControlUnit;

struct _Kova2016ControlUnit {
	guint8 report_id; /* KOVA2016_REPORT_ID_CONTROL_UNIT */
	guint8 size; /* always 6 */
	guint8 dcu;
	guint8 tcu;
	guint8 unused;
	guint8 action;
} __attribute__ ((packed));

typedef enum {
	KOVA2016_CONTROL_UNIT_ACTION_OFF = 0x00,
	KOVA2016_CONTROL_UNIT_ACTION_START = 0x01,
	KOVA2016_CONTROL_UNIT_ACTION_ACCEPT = 0x03,
	KOVA2016_CONTROL_UNIT_ACTION_CANCEL = 0x04,
} Kova2016ControlUnitAction;

typedef enum {
	KOVA2016_TRACKING_CONTROL_UNIT_OFF = 0,
	KOVA2016_TRACKING_CONTROL_UNIT_ON = 1,
} Kova2016ControlUnitTcu;

typedef enum {
	KOVA2016_DISTANCE_CONTROL_UNIT_OFF = 0,
	KOVA2016_DISTANCE_CONTROL_UNIT_EXTRA_LOW = 1,
	KOVA2016_DISTANCE_CONTROL_UNIT_LOW = 2,
	KOVA2016_DISTANCE_CONTROL_UNIT_NORMAL = 3,
} Kova2016ControlUnitDcu;

Kova2016ControlUnit *kova2016_control_unit_read(RoccatDevice *kova2016, GError **error);

gboolean kova2016_tracking_control_unit_start(RoccatDevice *kova2016, GError **error);
gboolean kova2016_tracking_control_unit_cancel(RoccatDevice *kova2016, GError **error);
gboolean kova2016_tracking_control_unit_accept(RoccatDevice *kova2016, GError **error);
gboolean kova2016_tracking_control_unit_off(RoccatDevice *kova2016, GError **error);

guint kova2016_distance_control_unit_get(RoccatDevice *kova2016, GError **error);
gboolean kova2016_distance_control_unit_set(RoccatDevice *kova2016, guint new_dcu, GError **error);

G_END_DECLS

#endif
