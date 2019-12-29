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

#include "kone_internal.h"
#include "roccat_helper.h"
#include <string.h>

guint8 kone_rmp_light_info_calc_checksum(KoneRMPLightInfo const *rmp_light_info) {
	return ROCCAT_BYTESUM_PARTIALLY(rmp_light_info, KoneRMPLightInfo, number, checksum);
}

void kone_rmp_light_info_set_checksum(KoneRMPLightInfo *rmp_light_info) {
	rmp_light_info->checksum = kone_rmp_light_info_calc_checksum(rmp_light_info);
}

void kone_rmp_light_info_to_light_info(KoneRMPLightInfo const *from, KoneLightInfo *to) {
	/* silently discarding checksum without checking */
	memcpy(to, from, sizeof(KoneLightInfo));
}

void kone_light_info_to_rmp_light_info(KoneLightInfo const *light_info, KoneRMPLightInfo *rmp_light_info) {
	rmp_light_info->number = light_info->number;
	rmp_light_info->mod = light_info->mod;
	rmp_light_info->red = light_info->red;
	rmp_light_info->green = light_info->green;
	rmp_light_info->blue = light_info->blue;
	kone_rmp_light_info_set_checksum(rmp_light_info);
}

gboolean kone_rmp_light_info_equal(KoneRMPLightInfo const *left, KoneRMPLightInfo const *right) {
	gboolean equal;
	equal = ROCCAT_MEMCMP_PARTIALLY(left, right, KoneRMPLightInfo, mod, checksum);
	return equal ? FALSE : TRUE;
}
