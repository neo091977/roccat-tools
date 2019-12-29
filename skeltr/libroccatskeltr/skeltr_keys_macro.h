#ifndef __ROCCAT_SKELTR_KEYS_MACRO_H__
#define __ROCCAT_SKELTR_KEYS_MACRO_H__

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

typedef struct _SkeltrKeysMacro SkeltrKeysMacro;

enum {
	SKELTR_KEYS_MACRO_NUM = 5,
};

struct _SkeltrKeysMacro {
	guint8 report_id; /* SKELTR_REPORT_ID_KEYS_MACRO */
	guint8 size; /* always 0x23 */
	guint8 profile_index;
	RoccatButton keys[SKELTR_KEYS_MACRO_NUM * 2];
	guint16 checksum;
} __attribute__ ((packed));

gboolean skeltr_keys_macro_write(RoccatDevice *device, guint profile_index, SkeltrKeysMacro *keys_macro, GError **error);
SkeltrKeysMacro *skeltr_keys_macro_read(RoccatDevice *device, guint profile_index, GError **error);
gboolean skeltr_keys_macro_equal(SkeltrKeysMacro const *left, SkeltrKeysMacro const *right);
void skeltr_keys_macro_copy(SkeltrKeysMacro *destination, SkeltrKeysMacro const *source);
SkeltrKeysMacro const *skeltr_keys_macro_default(void);

guint skeltr_keys_macro_index_to_macro_index(guint macro_index);
guint skeltr_macro_index_to_keys_macro_index(guint macro_index);
gboolean skeltr_macro_index_is_keys_macro(guint macro_index);

G_END_DECLS

#endif
