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

#include "isku.h"
#include "roccat_helper.h"
#include <string.h>

IskuLight *isku_rkp_to_light(IskuRkp *rkp) {
	IskuLight *light;

	light = g_malloc0(sizeof(IskuLight));
	light->brightness = isku_rkp_get_light_brightness(rkp);
	light->dimness = isku_rkp_get_light_dimness(rkp);
	light->timeout = isku_rkp_get_light_timeout(rkp);

	return light;
}

void isku_rkp_update_with_light(IskuRkp *rkp, IskuLight const *light) {
	if (!light)
		return;
	isku_rkp_set_light_brightness(rkp, light->brightness);
	isku_rkp_set_light_dimness(rkp, light->dimness);
	isku_rkp_set_light_timeout(rkp, light->timeout);
}

guint16 isku_light_calc_checksum(IskuLight const *light) {
	return ROCCAT_BYTESUM_PARTIALLY(light, IskuLight, report_id, checksum);
}

static void isku_light_finalize(IskuLight *light, guint profile_number) {
	light->report_id = ISKU_REPORT_ID_LIGHT;
	light->size = sizeof(IskuLight);
	light->unknown1 = 3;
	light->unknown2 = 0;
	light->profile_number = profile_number;
	isku_light_set_checksum(light, isku_light_calc_checksum(light));
}

gboolean isku_light_write(RoccatDevice *isku, guint profile_number, IskuLight *light, GError **error) {
	g_assert(profile_number < ISKU_PROFILE_NUM);
	isku_light_finalize(light, profile_number);
	return isku_device_write(isku, (gchar const *)light, sizeof(IskuLight), error);
}

IskuLight *isku_light_read(RoccatDevice *isku, guint profile_number, GError **error) {
	IskuLight *light;

	g_assert(profile_number < ISKU_PROFILE_NUM);

	gaminggear_device_lock(GAMINGGEAR_DEVICE(isku));

	if (!isku_select(isku, profile_number, ISKU_CONTROL_REQUEST_LIGHT, error)) {
		gaminggear_device_unlock(GAMINGGEAR_DEVICE(isku));
		return NULL;
	}

	light = (IskuLight *)isku_device_read(isku, ISKU_REPORT_ID_LIGHT, sizeof(IskuLight), error);

	gaminggear_device_unlock(GAMINGGEAR_DEVICE(isku));

	return light;
}

/* TODO maybe test from unknown1 */
gboolean isku_light_equal(IskuLight const *left, IskuLight const *right) {
	gboolean equal;
	equal = ROCCAT_MEMCMP_PARTIALLY(left, right, IskuLight, brightness, checksum);
	return equal ? FALSE : TRUE;
}
