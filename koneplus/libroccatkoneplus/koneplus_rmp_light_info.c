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

#include "koneplus.h"
#include "roccat_helper.h"
#include <string.h>

static KoneplusRmpLightInfo const colors[KONEPLUS_RMP_LIGHT_INFO_COLORS_NUM] = {
	{0x00, KONEPLUS_RMP_LIGHT_INFO_STATE_ON, 0x64, 0x00, 0x00, 0x65},
	{0x01, KONEPLUS_RMP_LIGHT_INFO_STATE_ON, 0xfa, 0x00, 0x00, 0xfc},
	{0x02, KONEPLUS_RMP_LIGHT_INFO_STATE_ON, 0xfa, 0x0f, 0x00, 0x0c},
	{0x03, KONEPLUS_RMP_LIGHT_INFO_STATE_ON, 0xfa, 0x19, 0x00, 0x17},
	{0x04, KONEPLUS_RMP_LIGHT_INFO_STATE_ON, 0xfa, 0x28, 0x00, 0x27},
	{0x05, KONEPLUS_RMP_LIGHT_INFO_STATE_ON, 0xfa, 0x3c, 0x00, 0x3c},
	{0x06, KONEPLUS_RMP_LIGHT_INFO_STATE_ON, 0xfa, 0x50, 0x00, 0x51},
	{0x07, KONEPLUS_RMP_LIGHT_INFO_STATE_ON, 0xfa, 0x64, 0x00, 0x66},
	{0x08, KONEPLUS_RMP_LIGHT_INFO_STATE_ON, 0xfa, 0x7d, 0x00, 0x80},
	{0x09, KONEPLUS_RMP_LIGHT_INFO_STATE_ON, 0xfa, 0xaf, 0x00, 0xb3},
	{0x0a, KONEPLUS_RMP_LIGHT_INFO_STATE_ON, 0xfa, 0xfa, 0x00, 0xff},
	{0x0b, KONEPLUS_RMP_LIGHT_INFO_STATE_ON, 0x7d, 0xfa, 0x00, 0x83},
	{0x0c, KONEPLUS_RMP_LIGHT_INFO_STATE_ON, 0x00, 0xfa, 0x00, 0x07},
	{0x0d, KONEPLUS_RMP_LIGHT_INFO_STATE_ON, 0x00, 0x32, 0x00, 0x40},
	{0x0e, KONEPLUS_RMP_LIGHT_INFO_STATE_ON, 0x00, 0xfa, 0x32, 0x3b},
	{0x0f, KONEPLUS_RMP_LIGHT_INFO_STATE_ON, 0x00, 0xfa, 0x96, 0xa0},
	{0x10, KONEPLUS_RMP_LIGHT_INFO_STATE_ON, 0x00, 0xfa, 0xfa, 0x05},
	{0x11, KONEPLUS_RMP_LIGHT_INFO_STATE_ON, 0xc8, 0xfa, 0xfa, 0xce},
	{0x12, KONEPLUS_RMP_LIGHT_INFO_STATE_ON, 0x00, 0x96, 0xfa, 0xa3},
	{0x13, KONEPLUS_RMP_LIGHT_INFO_STATE_ON, 0x00, 0x64, 0xfa, 0x72},
	{0x14, KONEPLUS_RMP_LIGHT_INFO_STATE_ON, 0x64, 0x64, 0xfa, 0xd7},
	{0x15, KONEPLUS_RMP_LIGHT_INFO_STATE_ON, 0x00, 0x32, 0xfa, 0x42},
	{0x16, KONEPLUS_RMP_LIGHT_INFO_STATE_ON, 0x00, 0x19, 0xfa, 0x2a},
	{0x17, KONEPLUS_RMP_LIGHT_INFO_STATE_ON, 0x00, 0x00, 0xfa, 0x12},
	{0x18, KONEPLUS_RMP_LIGHT_INFO_STATE_ON, 0x00, 0x00, 0x7d, 0x96},
	{0x19, KONEPLUS_RMP_LIGHT_INFO_STATE_ON, 0x23, 0x00, 0xfa, 0x37},
	{0x1a, KONEPLUS_RMP_LIGHT_INFO_STATE_ON, 0x64, 0x00, 0xfa, 0x79},
	{0x1b, KONEPLUS_RMP_LIGHT_INFO_STATE_ON, 0xfa, 0x00, 0xfa, 0x10},
	{0x1c, KONEPLUS_RMP_LIGHT_INFO_STATE_ON, 0x9b, 0x32, 0xfa, 0xe4},
	{0x1d, KONEPLUS_RMP_LIGHT_INFO_STATE_ON, 0x9b, 0x00, 0x64, 0x1d},
	{0x1e, KONEPLUS_RMP_LIGHT_INFO_STATE_ON, 0xfa, 0x00, 0x64, 0x7d},
	{0x1f, KONEPLUS_RMP_LIGHT_INFO_STATE_ON, 0xfa, 0x00, 0x32, 0x4c},
	{0x20, KONEPLUS_RMP_LIGHT_INFO_STATE_ON, 0xfa, 0x00, 0x19, 0x34},
};

KoneplusRmpLightInfo const *koneplus_rmp_light_info_get_standard(guint index) {
	index = roccat_sanitize_index_min(KONEPLUS_RMP_LIGHT_INFO_COLORS_NUM, index);
	return &colors[index];
}

guint8 koneplus_rmp_light_info_calc_checksum(KoneplusRmpLightInfo const *rmp_light_info) {
	return ROCCAT_BYTESUM_PARTIALLY(rmp_light_info, KoneplusRmpLightInfo, index, checksum);
}

void koneplus_rmp_light_info_set_checksum(KoneplusRmpLightInfo *rmp_light_info) {
	rmp_light_info->checksum = koneplus_rmp_light_info_calc_checksum(rmp_light_info);
}

void koneplus_rmp_light_info_to_light_info(KoneplusRmpLightInfo const *from, RoccatLight *to, gboolean custom) {
	if (custom)
		to->index = KONEPLUS_LIGHT_INFO_INDEX_CUSTOM;
	else
		to->index = from->index;
	to->red = from->red;
	to->green = from->green;
	to->blue = from->blue;
}

void koneplus_light_info_to_rmp_light_info(RoccatLight const *light_info, KoneplusRmpLightInfo *rmp_light_info, gboolean state) {
	rmp_light_info->index = light_info->index;
	rmp_light_info->state = state ? KONEPLUS_RMP_LIGHT_INFO_STATE_ON : KONEPLUS_RMP_LIGHT_INFO_STATE_OFF;
	rmp_light_info->red = light_info->red;
	rmp_light_info->green = light_info->green;
	rmp_light_info->blue = light_info->blue;
	koneplus_rmp_light_info_set_checksum(rmp_light_info);
}

void koneplus_light_info_to_custom_rmp_light_info(RoccatLight const *light_info, KoneplusRmpLightInfo *rmp_light_info) {
	rmp_light_info->index = 0;
	rmp_light_info->state = 0;
	rmp_light_info->red = light_info->red;
	rmp_light_info->green = light_info->green;
	rmp_light_info->blue = light_info->blue;
	koneplus_rmp_light_info_set_checksum(rmp_light_info);
}

gboolean koneplus_rmp_light_info_equal(KoneplusRmpLightInfo const *left, KoneplusRmpLightInfo const *right) {
	gboolean equal;
	equal = ROCCAT_MEMCMP_PARTIALLY(left, right, KoneplusRmpLightInfo, state, checksum);
	return equal ? FALSE : TRUE;
}
