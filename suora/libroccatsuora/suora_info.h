#ifndef __ROCCAT_SUORA_INFO_H__
#define __ROCCAT_SUORA_INFO_H__

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

typedef struct _SuoraInfo SuoraInfo;

struct _SuoraInfo {
	guint8 version_major;
	guint8 version_minor;
	guint8 unknown1;
	guint8 unknown2;
	guint8 unknown3;
	guint8 unknown4;
} __attribute__ ((packed));

SuoraInfo *suora_info_read(RoccatDevice *device, GError **error);
gchar *suora_firmware_version_to_string(SuoraInfo const *info);

G_END_DECLS

#endif
