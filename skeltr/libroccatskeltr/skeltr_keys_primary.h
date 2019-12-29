#ifndef __ROCCAT_SKELTR_KEYS_PRIMARY_H__
#define __ROCCAT_SKELTR_KEYS_PRIMARY_H__

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

typedef struct _SkeltrKeysPrimary SkeltrKeysPrimary;

enum {
	SKELTR_KEYS_PRIMARY_NUM = 144,
};

struct _SkeltrKeysPrimary {
	guint8 report_id; /* SKELTR_REPORT_ID_KEYS_PRIMARY */
	guint8 size; /* always 0x95 */
	guint8 profile_index;
	guint8 keys[SKELTR_KEYS_PRIMARY_NUM];
	guint16 checksum;
} __attribute__ ((packed));

gboolean skeltr_keys_primary_write(RoccatDevice *device, guint profile_index, SkeltrKeysPrimary *keys_primary, GError **error);
SkeltrKeysPrimary *skeltr_keys_primary_read(RoccatDevice *device, guint profile_index, GError **error);
gboolean skeltr_keys_primary_equal(SkeltrKeysPrimary const *left, SkeltrKeysPrimary const *right);
void skeltr_keys_primary_copy(SkeltrKeysPrimary *destination, SkeltrKeysPrimary const *source);
guint8 skeltr_keys_primary_correct_remapped(SkeltrKeysPrimary const *actual_keys, guint8 hid);
SkeltrKeysPrimary const *skeltr_keys_primary_default(void);

G_END_DECLS

#endif
