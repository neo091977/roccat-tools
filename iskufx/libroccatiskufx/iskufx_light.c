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

IskufxLight *iskufx_rkp_to_light(IskuRkp *rkp) {
	IskufxLight *light;
	IskufxRkpLightInfo *rkp_light_info;

	light = g_malloc0(sizeof(IskufxLight));
	light->state = iskufx_rkp_get_light_switch(rkp);
	light->breathing = iskufx_rkp_get_light_breath_switch(rkp);
	light->color_type = iskufx_rkp_get_light_color_type(rkp);
	light->brightness = isku_rkp_get_light_brightness(rkp);
	light->dimness = isku_rkp_get_light_dimness(rkp);
	light->timeout = isku_rkp_get_light_timeout(rkp);
	light->talkfx = iskufx_rkp_get_talkfx(rkp);

	rkp_light_info = iskufx_rkp_get_rkp_light_info(rkp);
	iskufx_rkp_light_info_to_roccat_light(rkp_light_info, &light->light);
	g_free(rkp_light_info);

	return light;
}

void iskufx_rkp_update_with_light(IskuRkp *rkp, IskufxLight const *light) {
	IskufxRkpLightInfo rkp_light_info;

	if (!light)
		return;
	iskufx_rkp_set_light_switch(rkp, light->state);
	iskufx_rkp_set_light_breath_switch(rkp, light->breathing);
	iskufx_rkp_set_light_color_type(rkp, light->color_type);
	isku_rkp_set_light_brightness(rkp, light->brightness);
	isku_rkp_set_light_dimness(rkp, light->dimness);
	isku_rkp_set_light_timeout(rkp, light->timeout);
	iskufx_rkp_set_talkfx(rkp, light->talkfx);

	roccat_light_to_rkp_light_info(&light->light, &rkp_light_info);
	iskufx_rkp_set_rkp_light_info(rkp, &rkp_light_info);
}

guint16 iskufx_light_calc_checksum(IskufxLight const *light) {
	return ROCCAT_BYTESUM_PARTIALLY(light, IskufxLight, report_id, checksum);
}

static void iskufx_light_finalize(IskufxLight *light, guint profile_index) {
	light->report_id = ISKU_REPORT_ID_LIGHT;
	light->size = sizeof(IskufxLight);
	light->profile_index = profile_index;
	iskufx_light_set_checksum(light, iskufx_light_calc_checksum(light));
}

gboolean iskufx_light_write(RoccatDevice *isku, guint profile_index, IskufxLight *light, GError **error) {
	g_assert(profile_index < ISKU_PROFILE_NUM);
	iskufx_light_finalize(light, profile_index);
	return isku_device_write(isku, (gchar const *)light, sizeof(IskufxLight), error);
}

IskufxLight *iskufx_light_read(RoccatDevice *isku, guint profile_index, GError **error) {
	IskufxLight *light;

	g_assert(profile_index < ISKU_PROFILE_NUM);

	gaminggear_device_lock(GAMINGGEAR_DEVICE(isku));

	if (!isku_select(isku, profile_index, ISKU_CONTROL_REQUEST_LIGHT, error)) {
		gaminggear_device_unlock(GAMINGGEAR_DEVICE(isku));
		return NULL;
	}

	light = (IskufxLight *)isku_device_read(isku, ISKU_REPORT_ID_LIGHT, sizeof(IskufxLight), error);

	gaminggear_device_unlock(GAMINGGEAR_DEVICE(isku));

	return light;
}

gboolean iskufx_light_equal(IskufxLight const *left, IskufxLight const *right) {
	gboolean equal;
	equal = ROCCAT_MEMCMP_PARTIALLY(left, right, IskufxLight, state, checksum);
	return equal ? FALSE : TRUE;
}
