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

#include "konepure.h"
#include "roccat_helper.h"
#include <string.h>

static KoneplusRmpLightInfo const colors[KONEPURE_RMP_LIGHT_INFO_COLORS_NUM] = {
	{0x00, KONEPLUS_RMP_LIGHT_INFO_STATE_ON, 0x05, 0x90, 0xfe, 0x94},
	{0x01, KONEPLUS_RMP_LIGHT_INFO_STATE_ON, 0x00, 0x71, 0xff, 0x72},
	{0x02, KONEPLUS_RMP_LIGHT_INFO_STATE_ON, 0x00, 0x00, 0xff, 0x02},
	{0x03, KONEPLUS_RMP_LIGHT_INFO_STATE_ON, 0x5c, 0x18, 0xe6, 0x5e},
	{0x04, KONEPLUS_RMP_LIGHT_INFO_STATE_ON, 0x81, 0x18, 0xe6, 0x84},
	{0x05, KONEPLUS_RMP_LIGHT_INFO_STATE_ON, 0xc5, 0x18, 0xe6, 0xc9},
	{0x06, KONEPLUS_RMP_LIGHT_INFO_STATE_ON, 0xf8, 0x04, 0x7c, 0x7f},
	{0x07, KONEPLUS_RMP_LIGHT_INFO_STATE_ON, 0xff, 0x00, 0x00, 0x07},
	{0x08, KONEPLUS_RMP_LIGHT_INFO_STATE_ON, 0xf7, 0x79, 0x00, 0x79},
	{0x09, KONEPLUS_RMP_LIGHT_INFO_STATE_ON, 0xe7, 0xdc, 0x00, 0xcd},
	{0x0a, KONEPLUS_RMP_LIGHT_INFO_STATE_ON, 0xc2, 0xf2, 0x08, 0xc7},
	{0x0b, KONEPLUS_RMP_LIGHT_INFO_STATE_ON, 0x00, 0xff, 0x00, 0x0b},
	{0x0c, KONEPLUS_RMP_LIGHT_INFO_STATE_ON, 0x18, 0xa6, 0x2a, 0xf5},
	{0x0d, KONEPLUS_RMP_LIGHT_INFO_STATE_ON, 0x13, 0xec, 0x96, 0xa3},
	{0x0e, KONEPLUS_RMP_LIGHT_INFO_STATE_ON, 0x0d, 0xe2, 0xd9, 0xd7},
	{0x0f, KONEPLUS_RMP_LIGHT_INFO_STATE_ON, 0x00, 0xbe, 0xf4, 0xc2},
};

KoneplusRmpLightInfo const *konepure_rmp_light_info_get_standard(guint index) {
	index = roccat_sanitize_index_min(KONEPURE_RMP_LIGHT_INFO_COLORS_NUM, index);
	return &colors[index];
}
