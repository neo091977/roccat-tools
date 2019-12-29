#ifndef __ROCCAT_KIRO_INFO_H__
#define __ROCCAT_KIRO_INFO_H__

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

typedef struct _KiroInfo KiroInfo;

struct _KiroInfo {
	guint8 report_id; /* KIRO_REPORT_ID_INFO */
	guint8 size; /* read 0x00, write 0x06 */
	union {
		guint8 firmware_version; /* r */
		guint8 function; /* w */
	};
	guint8 unknown;
	guint8 unused[3];
} __attribute__ ((packed));

typedef enum {
	KIRO_INFO_FUNCTION_RESET = 0x1,
} KiroInfoFunction;

KiroInfo *kiro_info_read(RoccatDevice *device, GError **error);
gboolean kiro_info_write(RoccatDevice *device, KiroInfo *info, GError **error);

guint kiro_firmware_version_read(RoccatDevice *device, GError **error);

gboolean kiro_reset(RoccatDevice *device, GError **error);

G_END_DECLS

#endif
