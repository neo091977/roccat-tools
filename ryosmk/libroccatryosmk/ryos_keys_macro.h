#ifndef __ROCCAT_RYOS_KEYS_MACRO_H__
#define __ROCCAT_RYOS_KEYS_MACRO_H__

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

typedef struct _RyosKeysMacro RyosKeysMacro;

struct _RyosKeysMacro {
	guint8 report_id; /* RYOS_REPORT_ID_KEYS_MACRO */
	guint8 size; /* always 0x23 */
	guint8 profile_index;
	RoccatButton keys[RYOS_KEYS_MACRO_NUM * 2];
	guint16 checksum;
} __attribute__ ((packed));

gboolean ryos_keys_macro_write(RoccatDevice *ryos, guint profile_index, RyosKeysMacro *keys_macro, GError **error);
RyosKeysMacro *ryos_keys_macro_read(RoccatDevice *ryos, guint profile_index, GError **error);
gboolean ryos_keys_macro_equal(RyosKeysMacro const *left, RyosKeysMacro const *right);
void ryos_keys_macro_copy(RyosKeysMacro *destination, RyosKeysMacro const *source);
guint ryos_keys_macro_index_to_macro_index(guint macro_index);
guint ryos_macro_index_to_keys_macro_index(guint macro_index);
gboolean ryos_macro_index_is_keys_macro(guint macro_index);
RyosKeysMacro const *ryos_keys_macro_default(void);

G_END_DECLS

#endif
