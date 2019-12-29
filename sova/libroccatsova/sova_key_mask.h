#ifndef __ROCCAT_SOVA_KEY_MASK_H__
#define __ROCCAT_SOVA_KEY_MASK_H__

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

#include "sova_device.h"

G_BEGIN_DECLS

typedef struct _SovaKeyMask SovaKeyMask;

struct _SovaKeyMask {
	guint8 report_id; /* SOVA_REPORT_ID_KEY_MASK */
	guint8 size; /* always 0x06 */
	guint8 profile_index;
	guint8 mask;
	guint16 checksum;
} __attribute__ ((packed));

typedef enum {
	SOVA_KEY_MASK_BIT_TAB = 0,
	SOVA_KEY_MASK_BIT_CAPS_LOCK = 1,
	SOVA_KEY_MASK_BIT_LEFT_WIN = 2,
	SOVA_KEY_MASK_BIT_RIGHT_WIN = 3,
	SOVA_KEY_MASK_BIT_APP = 4,
	SOVA_KEY_MASK_BIT_LEFT_SHIFT = 5,
} SovaKeyMaskBit;

gboolean sova_key_mask_write(RoccatDevice *device, guint profile_index, SovaKeyMask *key_mask, GError **error);
SovaKeyMask *sova_key_mask_read(RoccatDevice *device, guint profile_index, GError **error);
gboolean sova_key_mask_equal(SovaKeyMask const *left, SovaKeyMask const *right);
void sova_key_mask_copy(SovaKeyMask *destination, SovaKeyMask const *source);
SovaKeyMask const *sova_key_mask_default(void);

G_END_DECLS

#endif
