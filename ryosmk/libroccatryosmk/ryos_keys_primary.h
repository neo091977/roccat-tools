#ifndef __ROCCAT_RYOS_KEYS_PRIMARY_H__
#define __ROCCAT_RYOS_KEYS_PRIMARY_H__

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

#include "ryos.h"

G_BEGIN_DECLS

typedef struct _RyosKeysPrimary RyosKeysPrimary;

struct _RyosKeysPrimary {
	guint8 report_id; /* RYOS_REPORT_ID_KEYS_PRIMARY */
	guint8 size; /* always 0x7d */
	guint8 profile_index;
	guint8 keys[RYOS_KEYS_PRIMARY_NUM]; /* no thumbster keys */
	guint16 checksum;
} __attribute__ ((packed));

gboolean ryos_keys_primary_write(RoccatDevice *ryos, guint profile_index, RyosKeysPrimary *keys_primary, GError **error);
RyosKeysPrimary *ryos_keys_primary_read(RoccatDevice *ryos, guint profile_index, GError **error);
gboolean ryos_keys_primary_equal(RyosKeysPrimary const *left, RyosKeysPrimary const *right);
void ryos_keys_primary_copy(RyosKeysPrimary *destination, RyosKeysPrimary const *source);
guint8 ryos_keys_primary_correct_remapped(RyosKeysPrimary const *actual_keys, guint8 hid);
RyosKeysPrimary const *ryos_keys_primary_default(void);

G_END_DECLS

#endif
