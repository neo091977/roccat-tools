#ifndef __ROCCAT_SKELTR_KEYS_EXTRA_H__
#define __ROCCAT_SKELTR_KEYS_EXTRA_H__

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

typedef struct _SkeltrKeysExtra SkeltrKeysExtra;

struct _SkeltrKeysExtra { /* Secondary key functions */
	guint8 report_id; /* SKELTR_REPORT_ID_KEYS_EXTRA */
	guint8 size; /* always 0x08 */
	guint8 profile_index;
	guint8 capslock;
	guint8 fn;
	guint8 unused;
	guint16 checksum;
} __attribute__ ((packed));

gboolean skeltr_keys_extra_write(RoccatDevice *device, guint profile_index, SkeltrKeysExtra *keys_extra, GError **error);
SkeltrKeysExtra *skeltr_keys_extra_read(RoccatDevice *device, guint profile_index, GError **error);
gboolean skeltr_keys_extra_equal(SkeltrKeysExtra const *left, SkeltrKeysExtra const *right);
void skeltr_keys_extra_copy(SkeltrKeysExtra *destination, SkeltrKeysExtra const *source);
SkeltrKeysExtra const *skeltr_keys_extra_default(void);

G_END_DECLS

#endif
