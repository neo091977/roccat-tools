#ifndef __ROCCAT_SKELTR_RKP_KEYS_EASYZONE_H__
#define __ROCCAT_SKELTR_RKP_KEYS_EASYZONE_H__

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

#include "skeltr_keys_easyzone.h"

typedef struct _SkeltrRkpKeysEasyzone SkeltrRkpKeysEasyzone;

enum {
	SKELTR_RKP_KEYS_EASYZONE_UNUSED_SIZE = 230,
};

struct _SkeltrRkpKeysEasyzone {
	guint8 report_id; /* 0x0b */
	guint16 size; /* 0x126 */
	guint8 profile_index; /* 0 */
	RoccatButton keys[SKELTR_KEYS_EASYZONE_NUM];
	guint8 unused[SKELTR_RKP_KEYS_EASYZONE_UNUSED_SIZE];
} __attribute__ ((packed));

static inline guint16 skeltr_rkp_keys_easyzone_get_size(SkeltrRkpKeysEasyzone const *keys) {
	return GUINT16_FROM_LE(keys->size);
}

static inline void skeltr_rkp_keys_easyzone_set_size(SkeltrRkpKeysEasyzone *keys, guint16 new_value) {
	keys->size = GUINT16_TO_LE(new_value);
}

void skeltr_keys_easyzone_to_rkp_keys_easyzone(SkeltrRkpKeysEasyzone *rkp_keys, SkeltrKeysEasyzone const *keys);
void skeltr_rkp_keys_easyzone_to_keys_easyzone(SkeltrKeysEasyzone *keys, SkeltrRkpKeysEasyzone const *rkp_keys);

#endif
