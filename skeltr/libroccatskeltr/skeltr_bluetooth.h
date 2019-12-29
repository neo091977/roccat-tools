#ifndef __ROCCAT_SKELTR_BLUETOOTH_H__
#define __ROCCAT_SKELTR_BLUETOOTH_H__

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

#include "skeltr_device.h"

G_BEGIN_DECLS

typedef struct _SkeltrBluetooth SkeltrBluetooth;

struct _SkeltrBluetooth {
	guint report_id; /* SKELTR_REPORT_ID_BLUETOOTH */
	guint unknown1; /* always 4? */
	guint unknown2;
	guint unknown3;
	guint unknown4;
} __attribute__ ((packed));

gboolean skeltr_bluetooth_write(RoccatDevice *device, SkeltrBluetooth *bluetooth, GError **error);
SkeltrBluetooth *skeltr_bluetooth_read(RoccatDevice *device, GError **error);

G_END_DECLS

#endif
