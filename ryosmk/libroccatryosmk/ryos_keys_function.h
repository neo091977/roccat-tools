#ifndef __ROCCAT_RYOS_KEYS_FUNCTION_H__
#define __ROCCAT_RYOS_KEYS_FUNCTION_H__

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

typedef struct _RyosKeysFunction RyosKeysFunction;

struct _RyosKeysFunction {
	guint8 report_id; /* RYOS_REPORT_ID_KEYS_FUNCTION */
	guint8 size; /* always 0x5f */
	guint8 profile_index;
	RoccatButton keys[RYOS_KEYS_FUNCTION_NUM * 2];
	guint16 checksum;
} __attribute__ ((packed));

gboolean ryos_keys_function_write(RoccatDevice *ryos, guint profile_index, RyosKeysFunction *keys_function, GError **error);
RyosKeysFunction *ryos_keys_function_read(RoccatDevice *ryos, guint profile_index, GError **error);
gboolean ryos_keys_function_equal(RyosKeysFunction const *left, RyosKeysFunction const *right);
void ryos_keys_function_copy(RyosKeysFunction *destination, RyosKeysFunction const *source);
RyosKeysFunction const *ryos_keys_function_default(void);

G_END_DECLS

#endif
