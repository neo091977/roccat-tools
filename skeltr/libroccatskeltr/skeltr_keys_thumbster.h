#ifndef __ROCCAT_SKELTR_KEYS_THUMBSTER_H__
#define __ROCCAT_SKELTR_KEYS_THUMBSTER_H__

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
#include "roccat_button.h"

G_BEGIN_DECLS

typedef struct _SkeltrKeysThumbster SkeltrKeysThumbster;

enum {
	SKELTR_KEYS_THUMBSTER_NUM = 3,
};

struct _SkeltrKeysThumbster {
	guint8 report_id; /* SKELTR_REPORT_ID_KEYS_THUMBSTER */
	guint8 size; /* always 0x17 */
	guint8 profile_index;
	RoccatButton keys[SKELTR_KEYS_THUMBSTER_NUM * 2];
	guint16 checksum;
} __attribute__ ((packed));

gboolean skeltr_keys_thumbster_write(RoccatDevice *device, guint profile_index, SkeltrKeysThumbster *keys_thumbster, GError **error);
SkeltrKeysThumbster *skeltr_keys_thumbster_read(RoccatDevice *device, guint profile_index, GError **error);
gboolean skeltr_keys_thumbster_equal(SkeltrKeysThumbster const *left, SkeltrKeysThumbster const *right);
void skeltr_keys_thumbster_copy(SkeltrKeysThumbster *destination, SkeltrKeysThumbster const *source);
SkeltrKeysThumbster const *skeltr_keys_thumbster_default(void);

guint skeltr_keys_thumbster_index_to_macro_index(guint thumbster_index);
guint skeltr_macro_index_to_keys_thumbster_index(guint macro_index);
gboolean skeltr_macro_index_is_keys_thumbster(guint macro_index);

G_END_DECLS

#endif
