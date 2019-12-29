#ifndef __ROCCAT_SOVA_KEYS_FUNCTION_H__
#define __ROCCAT_SOVA_KEYS_FUNCTION_H__

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

typedef struct _SovaKeysFunction SovaKeysFunction;

struct _SovaKeysFunction {
	guint8 report_id; /* SOVA_REPORT_ID_KEYS_FUNCTION */
	guint8 size; /* always 0x5f */
	guint8 profile_index;
	RoccatButton keys_normal[SOVA_KEYS_FUNCTION_NUM];
	RoccatButton keys_fn[SOVA_KEYS_FUNCTION_NUM];
	guint16 checksum;
} __attribute__ ((packed));

gboolean sova_keys_function_write(RoccatDevice *device, guint profile_index, SovaKeysFunction *keys_function, GError **error);
SovaKeysFunction *sova_keys_function_read(RoccatDevice *device, guint profile_index, GError **error);
gboolean sova_keys_function_equal(SovaKeysFunction const *left, SovaKeysFunction const *right);
void sova_keys_function_copy(SovaKeysFunction *destination, SovaKeysFunction const *source);
SovaKeysFunction const *sova_keys_function_default(void);

G_END_DECLS

#endif
