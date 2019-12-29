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

#include "iskufx.h"
#include "roccat_helper.h"
#include <string.h>

static IskufxRkpLightInfo const colors[ISKUFX_RKP_LIGHT_INFO_COLORS_NUM] = {
	{0x00, ISKUFX_RKP_LIGHT_INFO_STATE_ON, 0x05, 0x90, 0xfe, 0x94},
	{0x01, ISKUFX_RKP_LIGHT_INFO_STATE_ON, 0x00, 0x71, 0xff, 0x72},
	{0x02, ISKUFX_RKP_LIGHT_INFO_STATE_ON, 0x00, 0x00, 0xff, 0x02},
	{0x03, ISKUFX_RKP_LIGHT_INFO_STATE_ON, 0x5c, 0x18, 0xe6, 0x5e},
	{0x04, ISKUFX_RKP_LIGHT_INFO_STATE_ON, 0x81, 0x18, 0xe6, 0x84},
	{0x05, ISKUFX_RKP_LIGHT_INFO_STATE_ON, 0xc5, 0x18, 0xe6, 0xc9},
	{0x06, ISKUFX_RKP_LIGHT_INFO_STATE_ON, 0xf8, 0x04, 0x7c, 0x7f},
	{0x07, ISKUFX_RKP_LIGHT_INFO_STATE_ON, 0xff, 0x00, 0x00, 0x07},
	{0x08, ISKUFX_RKP_LIGHT_INFO_STATE_ON, 0xf7, 0x79, 0x00, 0x79},
	{0x09, ISKUFX_RKP_LIGHT_INFO_STATE_ON, 0xe7, 0xdc, 0x00, 0xcd},
	{0x0a, ISKUFX_RKP_LIGHT_INFO_STATE_ON, 0xc2, 0xf2, 0x08, 0xc7},
	{0x0b, ISKUFX_RKP_LIGHT_INFO_STATE_ON, 0x00, 0xff, 0x00, 0x0b},
	{0x0c, ISKUFX_RKP_LIGHT_INFO_STATE_ON, 0x18, 0xa6, 0x2a, 0xf5},
	{0x0d, ISKUFX_RKP_LIGHT_INFO_STATE_ON, 0x13, 0xec, 0x96, 0xa3},
	{0x0e, ISKUFX_RKP_LIGHT_INFO_STATE_ON, 0x0d, 0xe2, 0xd9, 0xd7},
	{0x0f, ISKUFX_RKP_LIGHT_INFO_STATE_ON, 0x00, 0xbe, 0xf4, 0xc2},
};

IskufxRkpLightInfo const *iskufx_rkp_light_info_get_standard(guint index) {
	index = roccat_sanitize_index_min(ISKUFX_RKP_LIGHT_INFO_COLORS_NUM, index);
	return &colors[index];
}

guint8 iskufx_rkp_light_info_calc_checksum(IskufxRkpLightInfo const *rkp_light_info) {
	return ROCCAT_BYTESUM_PARTIALLY(rkp_light_info, IskufxRkpLightInfo, index, checksum);
}

void iskufx_rkp_light_info_set_checksum(IskufxRkpLightInfo *rkp_light_info) {
	rkp_light_info->checksum = iskufx_rkp_light_info_calc_checksum(rkp_light_info);
}

void iskufx_rkp_light_info_to_roccat_light(IskufxRkpLightInfo const *from, RoccatLight *to) {
	to->index = from->index;
	to->red = from->red;
	to->green = from->green;
	to->blue = from->blue;
}

void roccat_light_to_rkp_light_info(RoccatLight const *light, IskufxRkpLightInfo *rkp_light_info) {
	rkp_light_info->index = light->index;
	rkp_light_info->state = ISKUFX_RKP_LIGHT_INFO_STATE_ON;
	rkp_light_info->red = light->red;
	rkp_light_info->green = light->green;
	rkp_light_info->blue = light->blue;
	iskufx_rkp_light_info_set_checksum(rkp_light_info);
}

gboolean iskufx_rkp_light_info_equal(IskufxRkpLightInfo const *left, IskufxRkpLightInfo const *right) {
	gboolean equal;
	equal = ROCCAT_MEMCMP_PARTIALLY(left, right, IskufxRkpLightInfo, state, checksum);
	return equal ? FALSE : TRUE;
}
