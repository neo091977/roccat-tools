#ifndef __ROCCAT_RYOS_KEYS_THUMBSTER_H__
#define __ROCCAT_RYOS_KEYS_THUMBSTER_H__

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

typedef struct _RyosKeysThumbster RyosKeysThumbster;

struct _RyosKeysThumbster {
	guint8 report_id; /* RYOS_REPORT_ID_KEYS_THUMBSTER */
	guint8 size; /* always 0x17 */
	guint8 profile_index;
	RoccatButton keys[RYOS_KEYS_THUMBSTER_NUM * 2];
	guint16 checksum;
} __attribute__ ((packed));

gboolean ryos_keys_thumbster_write(RoccatDevice *ryos, guint profile_index, RyosKeysThumbster *keys_thumbster, GError **error);
RyosKeysThumbster *ryos_keys_thumbster_read(RoccatDevice *ryos, guint profile_index, GError **error);
gboolean ryos_keys_thumbster_equal(RyosKeysThumbster const *left, RyosKeysThumbster const *right);
void ryos_keys_thumbster_copy(RyosKeysThumbster *destination, RyosKeysThumbster const *source);
guint ryos_keys_thumbster_index_to_macro_index(guint thumbster_index);
guint ryos_macro_index_to_keys_thumbster_index(guint macro_index);
gboolean ryos_macro_index_is_keys_thumbster(guint macro_index);
RyosKeysThumbster const *ryos_keys_thumbster_default(void);

G_END_DECLS

#endif
