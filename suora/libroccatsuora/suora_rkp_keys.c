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

#include "suora_rkp_keys.h"
#include "suora_key_relations.h"
#include <string.h>

static guint8 const default_keys[sizeof(SuoraRkpKeys)] = {
	0x00, 0x00, 0x00, 0x1E, 0x00, 0x1E, 0x1F, 0x00, 0x1F, 0x20, 0x00, 0x20, 0x21, 0x00, 0x21, 0x22,
	0x00, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC6, 0xCF, 0xDF, 0x5D, 0xF4, 0x7F, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0x10, 0x21, 0x5E, 0xF4, 0x7F, 0x00, 0x00,
	0x2B, 0x00, 0x2B, 0x1D, 0x00, 0x1D, 0x1B, 0x00, 0x1B, 0x06, 0x00, 0x06, 0x15, 0x00, 0x15, 0x17,
	0x00, 0x17, 0x0A, 0x00, 0x0A, 0x19, 0x00, 0x19, 0x14, 0x00, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4A, 0x00, 0x4A, 0x4C, 0x00, 0x4C, 0x0B, 0x00, 0x0B,
	0x1A, 0x00, 0x1A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x64, 0x00, 0x64, 0x00, 0x00, 0x00, 0x00,
	0xB0, 0x91, 0x31, 0x00, 0x31, 0x4B, 0x00, 0x4B, 0x08, 0x00, 0x08, 0x5D, 0xF4, 0x7F, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x8F, 0x27, 0x00, 0x27, 0x4E, 0x00, 0x4E,
	0x04, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x13, 0x00, 0x13, 0x53, 0x00, 0x53, 0x5F,
	0x00, 0x5F, 0x5C, 0x00, 0x5C, 0x59, 0x00, 0x59, 0x16, 0x00, 0x16, 0x28, 0x00, 0x28, 0x00, 0x00,
	0x00, 0x1C, 0x00, 0x1C, 0x4D, 0x00, 0x4D, 0x24, 0x00, 0x24, 0x40, 0x00, 0x00, 0x34, 0x00, 0x34,
	0x07, 0x00, 0x07, 0xA3, 0xFD, 0x7F, 0x00, 0x00, 0x20, 0x18, 0x00, 0x18, 0x2F, 0x00, 0x2F, 0x25,
	0x00, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0x00, 0x09, 0x35, 0x00, 0x35, 0x00, 0x00,
	0xB0, 0x0C, 0x00, 0x0C, 0x30, 0x00, 0x30, 0x26, 0x00, 0x26, 0x91, 0x01, 0x00, 0x00, 0x00, 0x00,
	0x2C, 0x00, 0x2C, 0x2D, 0x00, 0x2D, 0x00, 0x00, 0xC0, 0x12, 0x00, 0x12, 0x00, 0x00, 0x00, 0x00,
	0xB0, 0x91, 0x11, 0xA3, 0xFD, 0x11, 0x00, 0x11, 0x52, 0x00, 0x52, 0x2E, 0x00, 0x2E, 0x38, 0x00,
	0x38, 0x54, 0x00, 0x54, 0x60, 0x00, 0x60, 0x5D, 0x00, 0x5D, 0x5A, 0x00, 0x5A, 0x62, 0x00, 0x62,
	0x51, 0x00, 0x51, 0x2A, 0x00, 0x2A, 0x36, 0x00, 0x36, 0x55, 0x00, 0x55, 0x61, 0x00, 0x61, 0x5E,
	0x00, 0x5E, 0x5B, 0x00, 0x5B, 0x63, 0x00, 0x63, 0x50, 0x00, 0x50, 0xE6, 0x00, 0x17, 0x00, 0x00,
	0x80, 0xE5, 0x00, 0xE5, 0x10, 0x00, 0x10, 0x00, 0x80, 0x62, 0x33, 0x00, 0x33, 0x00, 0x00, 0x00,
	0x4F, 0x00, 0x4F, 0xE4, 0x00, 0x18, 0x00, 0x03, 0x08, 0x05, 0x00, 0x05, 0x0D, 0x00, 0x0D, 0x0E,
	0x00, 0x0E, 0x0F, 0x00, 0x0F, 0x23, 0x00, 0x23, 0xE1, 0x00, 0xE1, 0x65, 0x00, 0x65, 0x00, 0x00,
	0x50, 0xF1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x90, 0xD7, 0x90, 0x01, 0x00, 0x00, 0x00, 0x00,
	0xE2, 0x00, 0xE2, 0xA3, 0xFD, 0x7F, 0x00, 0x00, 0xC0, 0x12, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xB0, 0x91, 0x11, 0xA3, 0xFD, 0x7F, 0x00, 0x00, 0xE0, 0x00, 0xE0, 0x49, 0x00, 0x49, 0x37, 0x00,
	0x37, 0x56, 0x00, 0x56, 0x57, 0x00, 0x57, 0x39, 0x00, 0x39, 0x58, 0x00, 0x58, 0x7F, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xC0, 0x00, 0x5E, 0xF4, 0x7F, 0x00, 0x00,
	0xC0, 0x12, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA0, 0x90, 0x11, 0xA3, 0xFD, 0x7F, 0x00, 0x00,
	0x78, 0x1E, 0x91, 0x01, 0x00, 0x00, 0x00, 0x00, 0x80, 0x62, 0x91, 0x01, 0x00, 0x00, 0x00, 0x00
};

SuoraRkpKeys const *suora_rkp_keys_default(void) {
	return (SuoraRkpKeys const *)default_keys;
}

void suora_rkp_keys_copy(SuoraRkpKeys *destination, SuoraRkpKeys const *source) {
	memcpy(destination, source, sizeof(SuoraRkpKeys));
}

SuoraRkpKeys *suora_rkp_keys_dup(SuoraRkpKeys const *old) {
	return g_memdup(old, sizeof(SuoraRkpKeys));
}

gboolean suora_rkp_keys_equal(SuoraRkpKeys const *left, SuoraRkpKeys const *right) {
	return memcmp(left, right, sizeof(SuoraRkpKeys)) ? FALSE : TRUE;
}

void suora_keys_to_rkp_keys(SuoraRkpKeys *rkp_keys, SuoraKeys const *keys) {
	guint keys_index;

	for (keys_index = 0; keys_index < SUORA_KEYS_NUM; ++keys_index) {
		if (suora_key_relations[keys_index].type != SUORA_KEY_RELATION_TYPE_NONE)
			suora_key_to_roccat_button(&rkp_keys->keys[keys_index], &keys->keys[keys_index], keys_index);
	}
}

void suora_rkp_keys_to_keys(SuoraKeys *keys, SuoraRkpKeys const *rkp_keys) {
	guint keys_index;
	
	suora_keys_copy(keys, suora_keys_default());
	
	for (keys_index = 0; keys_index < SUORA_KEYS_NUM; ++keys_index) {
		if (suora_key_relations[keys_index].type != SUORA_KEY_RELATION_TYPE_NONE)
			roccat_button_to_suora_key(&keys->keys[keys_index], &rkp_keys->keys[keys_index], keys_index);
	}
}
