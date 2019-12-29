#ifndef __ROCCAT_RYOS_KEY_MASK_H__
#define __ROCCAT_RYOS_KEY_MASK_H__

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

typedef struct _RyosKeyMask RyosKeyMask;

struct _RyosKeyMask {
	guint8 report_id; /* RYOS_REPORT_ID_KEY_MASK */
	guint8 size; /* always 0x06 */
	guint8 profile_index;
	guint8 mask;
	guint16 checksum;
} __attribute__ ((packed));

typedef enum {
	RYOS_KEY_MASK_BIT_TAB = 0,
	RYOS_KEY_MASK_BIT_CAPS_LOCK = 1,
	RYOS_KEY_MASK_BIT_LEFT_WIN = 2,
	RYOS_KEY_MASK_BIT_RIGHT_WIN = 3,
	RYOS_KEY_MASK_BIT_APP = 4,
	RYOS_KEY_MASK_BIT_LEFT_SHIFT = 5,
} RyosKeyMaskBit;

gboolean ryos_key_mask_write(RoccatDevice *ryos, guint profile_index, RyosKeyMask *key_mask, GError **error);
RyosKeyMask *ryos_key_mask_read(RoccatDevice *ryos, guint profile_index, GError **error);
gboolean ryos_key_mask_equal(RyosKeyMask const *left, RyosKeyMask const *right);
void ryos_key_mask_copy(RyosKeyMask *destination, RyosKeyMask const *source);
RyosKeyMask const *ryos_key_mask_default(void);

G_END_DECLS

#endif
