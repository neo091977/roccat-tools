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

#include "skeltr_rkp_keys_easyzone.h"
#include <string.h>

static void skeltr_rkp_keys_easyzone_finalize(SkeltrRkpKeysEasyzone *rkp_keys) {
	rkp_keys->report_id = 0x0b;
	skeltr_rkp_keys_easyzone_set_size(rkp_keys, sizeof(SkeltrRkpKeysEasyzone));
	rkp_keys->profile_index = 0;
	memset(&rkp_keys->unused, 0, SKELTR_RKP_KEYS_EASYZONE_UNUSED_SIZE);
}

void skeltr_keys_easyzone_to_rkp_keys_easyzone(SkeltrRkpKeysEasyzone *rkp_keys, SkeltrKeysEasyzone const *keys) {
	skeltr_rkp_keys_easyzone_finalize(rkp_keys);
	memcpy(rkp_keys->keys, keys->keys, sizeof(rkp_keys->keys));
}

void skeltr_rkp_keys_easyzone_to_keys_easyzone(SkeltrKeysEasyzone *keys, SkeltrRkpKeysEasyzone const *rkp_keys) {
	memcpy(keys->keys, rkp_keys->keys, sizeof(keys->keys));
}
