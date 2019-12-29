#ifndef __ROCCAT_SOVA_INFO_H__
#define __ROCCAT_SOVA_INFO_H__

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

#include "sova.h"
#include "sova_device.h"

G_BEGIN_DECLS

typedef struct _SovaInfo SovaInfo;

struct _SovaInfo {
	guint8 report_id; /* SOVA_REPORT_ID_INFO */
	guint8 size; /* always 0x08 */
	guint8 firmware_version;
	guint8 dfu_version;
	guint8 unused1;

	/* FIXME maybe one of the following bytes could be keyboard layout
	 * 0 = us; 1 = de; 3 = fr; 5 = nr; 12 = jp; 13 = kr; 14 = cz
	 */
	guint16 unknown2; // FIXME could be checksum
	guint8 unused2;
} __attribute__ ((packed));

SovaInfo *sova_info_read(RoccatDevice *device, GError **error);
guint sova_firmware_version_read(RoccatDevice *device, GError **error);

G_END_DECLS

#endif
