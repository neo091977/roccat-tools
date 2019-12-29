#ifndef __ROCCAT_SOVA_KEYS_EASYZONE_H__
#define __ROCCAT_SOVA_KEYS_EASYZONE_H__

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

#include "sova.h"
#include "sova_device.h"
#include "roccat_button.h"

G_BEGIN_DECLS

typedef struct _SovaKeysEasyzone SovaKeysEasyzone;

struct _SovaKeysEasyzone { /* Secondary key functions */
	guint8 report_id; /* SOVA_REPORT_ID_KEYS_EASYZONE */
	guint16 size; /* always 0x0126 */
	guint8 profile_index;
	RoccatButton keys[SOVA_MACRO_NUM];
	guint8 unknown[168];
	guint16 checksum;
} __attribute__ ((packed));

gboolean sova_keys_easyzone_write(RoccatDevice *device, guint profile_index, SovaKeysEasyzone *keys_easyzone, GError **error);
SovaKeysEasyzone *sova_keys_easyzone_read(RoccatDevice *device, guint profile_index, GError **error);
gboolean sova_keys_easyzone_equal(SovaKeysEasyzone const *left, SovaKeysEasyzone const *right);
void sova_keys_easyzone_copy(SovaKeysEasyzone *destination, SovaKeysEasyzone const *source);
guint sova_keys_easyzone_index_to_macro_index(guint easyzone_index);
guint sova_macro_index_to_keys_easyzone_index(guint macro_index);
gboolean sova_macro_index_is_keys_easyzone(guint macro_index);
SovaKeysEasyzone const *sova_keys_easyzone_default(void);

G_END_DECLS

#endif
