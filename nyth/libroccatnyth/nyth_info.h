#ifndef __ROCCAT_NYTH_INFO_H__
#define __ROCCAT_NYTH_INFO_H__

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

typedef struct _NythInfo NythInfo;

struct _NythInfo {
	guint8 report_id; /* NYTH_REPORT_ID_INFO */
	guint8 size; /* always 0x06 */
	union {
		guint8 firmware_version; /* r */
		guint8 function; /* w */
	};
	guint8 unknown1;
	guint8 unknown2;
	guint8 unknown3;
} __attribute__ ((packed));

typedef enum {
	NYTH_INFO_FUNCTION_RESET = 0x1,
} NythInfoFunction;

NythInfo *nyth_info_read(RoccatDevice *device, GError **error);
gboolean nyth_info_write(RoccatDevice *device, NythInfo *info, GError **error);

guint nyth_firmware_version_read(RoccatDevice *device, GError **error);

gboolean nyth_reset(RoccatDevice *device, GError **error);

G_END_DECLS

#endif
