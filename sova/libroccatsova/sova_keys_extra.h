#ifndef __ROCCAT_SOVA_KEYS_EXTRA_H__
#define __ROCCAT_SOVA_KEYS_EXTRA_H__

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

typedef struct _SovaKeysExtra SovaKeysExtra;

struct _SovaKeysExtra { /* Secondary key functions */
	guint8 report_id; /* SOVA_REPORT_ID_KEYS_EXTRA */
	guint8 size; /* always 0x0d */
	guint8 profile_index;
	guint8 capslock;
	guint8 unknown; // TODO FN?
	guint8 unused[6];
	guint16 checksum;
} __attribute__ ((packed));

gboolean sova_keys_extra_write(RoccatDevice *device, guint profile_index, SovaKeysExtra *keys_extra, GError **error);
SovaKeysExtra *sova_keys_extra_read(RoccatDevice *device, guint profile_index, GError **error);
gboolean sova_keys_extra_equal(SovaKeysExtra const *left, SovaKeysExtra const *right);
void sova_keys_extra_copy(SovaKeysExtra *destination, SovaKeysExtra const *source);
SovaKeysExtra const *sova_keys_extra_default(void);

G_END_DECLS

#endif
