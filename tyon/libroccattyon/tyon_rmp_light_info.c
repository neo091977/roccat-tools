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

#include "tyon_rmp_light_info.h"
#include "roccat_helper.h"
#include <string.h>

static TyonRmpLightInfo const colors[TYON_RMP_LIGHT_INFO_COLORS_NUM] = {
	{0x00, TYON_RMP_LIGHT_INFO_STATE_ON, 0x05, 0x90, 0xfe, 0x00, 0x94},
	{0x01, TYON_RMP_LIGHT_INFO_STATE_ON, 0x00, 0x71, 0xff, 0x00, 0x72},
	{0x02, TYON_RMP_LIGHT_INFO_STATE_ON, 0x00, 0x00, 0xff, 0x00, 0x02},
	{0x03, TYON_RMP_LIGHT_INFO_STATE_ON, 0x5c, 0x18, 0xe6, 0x00, 0x5e},
	{0x04, TYON_RMP_LIGHT_INFO_STATE_ON, 0x81, 0x18, 0xe6, 0x00, 0x84},
	{0x05, TYON_RMP_LIGHT_INFO_STATE_ON, 0xc5, 0x18, 0xe6, 0x00, 0xc9},
	{0x06, TYON_RMP_LIGHT_INFO_STATE_ON, 0xf8, 0x04, 0x7c, 0x00, 0x7f},
	{0x07, TYON_RMP_LIGHT_INFO_STATE_ON, 0xff, 0x00, 0x00, 0x00, 0x07},
	{0x08, TYON_RMP_LIGHT_INFO_STATE_ON, 0xf7, 0x79, 0x00, 0x00, 0x79},
	{0x09, TYON_RMP_LIGHT_INFO_STATE_ON, 0xe7, 0xdc, 0x00, 0x00, 0xcd},
	{0x0a, TYON_RMP_LIGHT_INFO_STATE_ON, 0xc2, 0xf2, 0x08, 0x00, 0xc7},
	{0x0b, TYON_RMP_LIGHT_INFO_STATE_ON, 0x00, 0xff, 0x00, 0x00, 0x0b},
	{0x0c, TYON_RMP_LIGHT_INFO_STATE_ON, 0x18, 0xa6, 0x2a, 0x00, 0xf5},
	{0x0d, TYON_RMP_LIGHT_INFO_STATE_ON, 0x13, 0xec, 0x96, 0x00, 0xa3},
	{0x0e, TYON_RMP_LIGHT_INFO_STATE_ON, 0x0d, 0xe2, 0xd9, 0x00, 0xd7},
	{0x0f, TYON_RMP_LIGHT_INFO_STATE_ON, 0x00, 0xbe, 0xf4, 0x00, 0xc2},
};

TyonRmpLightInfo const *tyon_rmp_light_info_get_standard(guint index) {
	index = roccat_sanitize_index_min(TYON_RMP_LIGHT_INFO_COLORS_NUM, index);
	return &colors[index];
}

guint8 tyon_rmp_light_info_calc_checksum(TyonRmpLightInfo const *rmp_light_info) {
	return ROCCAT_BYTESUM_PARTIALLY(rmp_light_info, TyonRmpLightInfo, index, checksum);
}

void tyon_rmp_light_info_set_checksum(TyonRmpLightInfo *rmp_light_info) {
	rmp_light_info->checksum = tyon_rmp_light_info_calc_checksum(rmp_light_info);
}

void tyon_rmp_light_info_to_light(TyonRmpLightInfo const *from, TyonLight *to) {
	to->index = from->index;
	to->red = from->red;
	to->green = from->green;
	to->blue = from->blue;
}

void tyon_light_to_rmp_light_info(TyonLight const *from, TyonRmpLightInfo *to, guint state) {
	to->index = from->index;
	to->state = state;
	to->red = from->red;
	to->green = from->green;
	to->blue = from->blue;
	to->null = 0;
	tyon_rmp_light_info_set_checksum(to);
}

gboolean tyon_rmp_light_info_equal(TyonRmpLightInfo const *left, TyonRmpLightInfo const *right) {
	gboolean equal;
	equal = ROCCAT_MEMCMP_PARTIALLY(left, right, TyonRmpLightInfo, state, checksum);
	return equal ? FALSE : TRUE;
}
