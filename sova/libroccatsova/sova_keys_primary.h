#ifndef __ROCCAT_SOVA_KEYS_PRIMARY_H__
#define __ROCCAT_SOVA_KEYS_PRIMARY_H__

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

typedef struct _SovaKeysPrimary SovaKeysPrimary;

struct _SovaKeysPrimary {
	guint8 report_id; /* SOVA_REPORT_ID_KEYS_PRIMARY */
	guint8 size; /* always 0x95 */
	guint8 profile_index;
	guint8 keys[SOVA_KEYS_PRIMARY_NUM];
	guint16 checksum;
} __attribute__ ((packed));

gboolean sova_keys_primary_write(RoccatDevice *device, guint profile_index, SovaKeysPrimary *keys_primary, GError **error);
SovaKeysPrimary *sova_keys_primary_read(RoccatDevice *device, guint profile_index, GError **error);
gboolean sova_keys_primary_equal(SovaKeysPrimary const *left, SovaKeysPrimary const *right);
void sova_keys_primary_copy(SovaKeysPrimary *destination, SovaKeysPrimary const *source);
guint8 sova_keys_primary_correct_remapped(SovaKeysPrimary const *actual_keys, guint8 hid);
SovaKeysPrimary const *sova_keys_primary_default(void);

G_END_DECLS

#endif
