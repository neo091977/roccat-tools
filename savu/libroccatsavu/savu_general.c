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

#include "savu.h"
#include "roccat_helper.h"
#include <string.h>
#include <stdlib.h>

guint16 savu_general_calc_checksum(SavuGeneral const *general) {
	return ROCCAT_BYTESUM_PARTIALLY(general, SavuGeneral, report_id, checksum);
}

static void savu_general_finalize(SavuGeneral *general, guint profile_index) {
	general->report_id = SAVU_REPORT_ID_GENERAL;
	general->size = sizeof(SavuGeneral);
	general->profile_index = profile_index;
	savu_general_set_checksum(general, savu_general_calc_checksum(general));
}

SavuGeneral *savu_general_read(RoccatDevice *savu, guint profile_index, GError **error) {
	SavuGeneral *general;

	g_assert(profile_index < SAVU_PROFILE_NUM);

	gaminggear_device_lock(GAMINGGEAR_DEVICE(savu));

	if (!savu_select(savu, profile_index, SAVU_CONTROL_REQUEST_GENERAL, error)) {
		gaminggear_device_unlock(GAMINGGEAR_DEVICE(savu));
		return NULL;
	}

	general = (SavuGeneral *)savu_device_read(savu, SAVU_REPORT_ID_GENERAL, sizeof(SavuGeneral), error);

	gaminggear_device_unlock(GAMINGGEAR_DEVICE(savu));

	return general;
}

gboolean savu_general_write(RoccatDevice *savu, guint profile_index, SavuGeneral *general, GError **error) {
	g_assert(profile_index < SAVU_PROFILE_NUM);
	savu_general_finalize(general, profile_index);
	return savu_device_write(savu, (gchar const *)general, sizeof(SavuGeneral), error);
}

static guint8 savu_rmp_get_cpi_mask(SavuRmp *rmp) {
	guint8 result = 0;
	guint i;

	for (i = 0; i < SAVU_GENERAL_CPI_LEVELS_NUM; ++i)
		roccat_set_bit8(&result, i, savu_rmp_get_cpi(rmp, i));

	return result;
}

static void savu_rmp_update_with_cpi_mask(SavuRmp *rmp, guint8 cpi_mask) {
	guint i;

	for (i = 0; i < SAVU_GENERAL_CPI_LEVELS_NUM; ++i)
		savu_rmp_set_cpi(rmp, i, roccat_get_bit8(cpi_mask, i));
}

static guint8 savu_rmp_get_lighting_mask(SavuRmp *rmp) {
	guint8 result = 0;

	roccat_set_bit8(&result, SAVU_GENERAL_LIGHTING_MASK_BIT_LIGHT, savu_rmp_get_light_switch(rmp));
	roccat_set_bit8(&result, SAVU_GENERAL_LIGHTING_MASK_BIT_BREATHING, savu_rmp_get_breath_on(rmp));
	roccat_set_bit8(&result, SAVU_GENERAL_LIGHTING_MASK_BIT_COLOR_FLOW, savu_rmp_get_color_flow(rmp));

	return result;
}

static void savu_rmp_update_with_lighting_mask(SavuRmp *rmp, guint8 lighting_mask) {
	savu_rmp_set_light_switch(rmp, roccat_get_bit8(lighting_mask, SAVU_GENERAL_LIGHTING_MASK_BIT_LIGHT));
	savu_rmp_set_breath_on(rmp, roccat_get_bit8(lighting_mask, SAVU_GENERAL_LIGHTING_MASK_BIT_BREATHING));
	savu_rmp_set_color_flow(rmp, roccat_get_bit8(lighting_mask, SAVU_GENERAL_LIGHTING_MASK_BIT_COLOR_FLOW));
}

SavuGeneral *savu_rmp_to_general(SavuRmp *rmp) {
	SavuGeneral *general;

	general = (SavuGeneral *)g_malloc0(sizeof(SavuGeneral));

	general->sensitivity_sync = savu_rmp_get_xy_synchronous(rmp);
	general->sensitivity_x = savu_rmp_get_sensitivity_x(rmp);
	general->sensitivity_y = savu_rmp_get_sensitivity_y(rmp);
	general->polling_rate = savu_rmp_get_polling_rate(rmp);
	general->light.index = savu_rmp_get_color_database(rmp);
	general->light.red = savu_rmp_get_color_red(rmp);
	general->light.green = savu_rmp_get_color_green(rmp);
	general->light.blue = savu_rmp_get_color_blue(rmp);
	general->cpi = savu_rmp_get_cpi_step(rmp);
	general->cpi_mask = savu_rmp_get_cpi_mask(rmp);
	general->lighting_mask = savu_rmp_get_lighting_mask(rmp);

	return general;
}

void savu_rmp_update_with_general(SavuRmp *rmp, SavuGeneral *general) {
	savu_rmp_set_xy_synchronous(rmp, general->sensitivity_sync);
	savu_rmp_set_sensitivity_x(rmp, general->sensitivity_x);
	savu_rmp_set_sensitivity_y(rmp, general->sensitivity_y);
	savu_rmp_set_polling_rate(rmp, general->polling_rate);
	savu_rmp_set_color_database(rmp, general->light.index);
	savu_rmp_set_color_red(rmp, general->light.red);
	savu_rmp_set_color_green(rmp, general->light.green);
	savu_rmp_set_color_blue(rmp, general->light.blue);
	savu_rmp_set_cpi_step(rmp, general->cpi);
	savu_rmp_update_with_cpi_mask(rmp, general->cpi_mask);
	savu_rmp_update_with_lighting_mask(rmp, general->lighting_mask);
}
