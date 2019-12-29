#ifndef __ROCCAT_RYOS_KEYS_EASYZONE_H__
#define __ROCCAT_RYOS_KEYS_EASYZONE_H__

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

typedef struct _RyosKeysEasyzone RyosKeysEasyzone;

struct _RyosKeysEasyzone { /* Secondary key functions */
	guint8 report_id; /* RYOS_REPORT_ID_KEYS_EASYZONE */
	guint16 size; /* always 0x0126 */
	guint8 profile_index;
	RoccatButton keys[RYOS_KEYS_EASYZONE_NUM];
	guint16 checksum;
} __attribute__ ((packed));

gboolean ryos_keys_easyzone_write(RoccatDevice *ryos, guint profile_index, RyosKeysEasyzone *keys_easyzone, GError **error);
RyosKeysEasyzone *ryos_keys_easyzone_read(RoccatDevice *ryos, guint profile_index, GError **error);
gboolean ryos_keys_easyzone_equal(RyosKeysEasyzone const *left, RyosKeysEasyzone const *right);
void ryos_keys_easyzone_copy(RyosKeysEasyzone *destination, RyosKeysEasyzone const *source);
guint ryos_keys_easyzone_index_to_macro_index(guint easyzone_index);
guint ryos_macro_index_to_keys_easyzone_index(guint macro_index);
gboolean ryos_macro_index_is_keys_easyzone(guint macro_index);
RyosKeysEasyzone const *ryos_keys_easyzone_default(void);

G_END_DECLS

#endif
