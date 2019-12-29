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

#include "konepureoptical.h"
#include "roccat_helper.h"
#include "roccat.h"

static guint cpi_bin_values[KONEPUREOPTICAL_CPI_VALUES_NUM] = { 0x10, 0x00, 0x11, 0x12, 0x13, 0x02, 0x03 };
static guint const default_rmp_value = 4;

static guint konepureoptical_rmp_cpi_to_bin(guint rmp_value) {
	if (rmp_value < 1 || rmp_value > KONEPUREOPTICAL_CPI_VALUES_NUM)
		rmp_value = default_rmp_value;

	return cpi_bin_values[rmp_value - 1];
}

static guint konepureoptical_bin_to_rmp_cpi(guint bin_value) {
	guint i;

	for (i = 0; i < KONEPUREOPTICAL_CPI_VALUES_NUM; ++i)
		if (bin_value == cpi_bin_values[i])
			return i + 1;

	return default_rmp_value;
}

KonepureopticalProfileSettings *konepureoptical_rmp_to_profile_settings(KoneplusRmp *rmp) {
	KonepureopticalProfileSettings *settings;
	guint i;
	KoneplusRmpLightInfo *rmp_light_info;

	settings = g_malloc0(sizeof(KonepureopticalProfileSettings));
	settings->report_id = KONEPLUS_REPORT_ID_PROFILE_SETTINGS;
	settings->size = sizeof(KonepureopticalProfileSettings);

	settings->xy_sync = koneplus_rmp_get_xy_synchronous(rmp);
	if (settings->xy_sync == ROCCAT_SENSITIVITY_ADVANCED_ON) {
		settings->sensitivity_x = koneplus_rmp_get_sensitivity_x(rmp);
		settings->sensitivity_y = koneplus_rmp_get_sensitivity_y(rmp);
	} else {
		settings->sensitivity_x = koneplus_rmp_get_sensitivity(rmp);
		settings->sensitivity_y = settings->sensitivity_x;
	}

	for (i = 0; i < KONEPLUS_PROFILE_SETTING_CPI_LEVELS_NUM; ++i)
		settings->cpi_levels[i] = konepureoptical_rmp_cpi_to_bin(koneplus_rmp_get_cpi_level_x(rmp, i));

	settings->cpi_levels_enabled = koneplus_rmp_get_cpi_all(rmp);
	settings->cpi_startup_level = koneplus_rmp_get_cpi_x(rmp);

	settings->polling_rate = koneplus_rmp_get_polling_rate(rmp);

	settings->color_flow_effect = koneplus_rmp_get_light_color_flow(rmp);
	settings->light_effect_type = koneplus_rmp_get_light_effect_type(rmp);
	settings->light_effect_speed = koneplus_rmp_get_light_effect_speed(rmp);
	settings->light_chose_type = konextd_rmp_get_light_chose_type(rmp);

	rmp_light_info = koneplus_rmp_get_rmp_light_info(rmp, 0);
	roccat_set_bit8(&settings->light_enabled, 0, rmp_light_info->state == KONEPLUS_RMP_LIGHT_INFO_STATE_ON);
	if (settings->light_chose_type == KONEXTD_RMP_LIGHT_CHOSE_TYPE_TABLE) {
		koneplus_rmp_light_info_to_light_info(rmp_light_info, &settings->light, FALSE);
	} else {
		g_free(rmp_light_info);
		rmp_light_info = konextd_rmp_get_custom_light_info(rmp, 0);
		koneplus_rmp_light_info_to_light_info(rmp_light_info, &settings->light, TRUE);
	}
	g_free(rmp_light_info);

	return settings;
}

void konepureoptical_rmp_update_with_profile_settings(KoneplusRmp *rmp, KonepureopticalProfileSettings const *profile_settings) {
	KoneplusRmpLightInfo rmp_light_info;
	KoneplusRmpLightInfo* rmp_light_info_pointer;
	guint i;

	koneplus_rmp_set_xy_synchronous(rmp, profile_settings->xy_sync);
	if (profile_settings->xy_sync == ROCCAT_SENSITIVITY_ADVANCED_ON) {
		koneplus_rmp_set_sensitivity_x(rmp, profile_settings->sensitivity_x);
		koneplus_rmp_set_sensitivity_y(rmp, profile_settings->sensitivity_y);
	} else {
		koneplus_rmp_set_sensitivity(rmp, profile_settings->sensitivity_x);
	}

	for (i = 0; i < KONEPLUS_PROFILE_SETTING_CPI_LEVELS_NUM; ++i)
		koneplus_rmp_set_cpi_level_x(rmp, i, konepureoptical_bin_to_rmp_cpi(profile_settings->cpi_levels[i]));

	koneplus_rmp_set_cpi_all(rmp, profile_settings->cpi_levels_enabled);
	koneplus_rmp_set_cpi_x(rmp, profile_settings->cpi_startup_level);
	koneplus_rmp_set_cpi_y(rmp, profile_settings->cpi_startup_level);

	if (profile_settings->light_chose_type == KONEXTD_RMP_LIGHT_CHOSE_TYPE_CUSTOM) {
		koneplus_light_info_to_custom_rmp_light_info(&profile_settings->light, &rmp_light_info);
		konextd_rmp_set_custom_light_info(rmp, 0, &rmp_light_info);

		rmp_light_info_pointer = koneplus_rmp_get_rmp_light_info(rmp, 0);
		rmp_light_info_pointer->state = roccat_get_bit8(profile_settings->light_enabled, 0) ? KONEPLUS_RMP_LIGHT_INFO_STATE_ON : KONEPLUS_RMP_LIGHT_INFO_STATE_OFF;
		koneplus_rmp_set_rmp_light_info(rmp, 0, rmp_light_info_pointer);
		g_free(rmp_light_info_pointer);
	} else {
		koneplus_light_info_to_rmp_light_info(&profile_settings->light, &rmp_light_info, roccat_get_bit8(profile_settings->light_enabled, 0));
		koneplus_rmp_set_rmp_light_info(rmp, 0, &rmp_light_info);
	}

	koneplus_rmp_set_light_effect_type(rmp, profile_settings->light_effect_type);
	koneplus_rmp_set_light_effect_speed(rmp, profile_settings->light_effect_speed);
	koneplus_rmp_set_light_color_flow(rmp, profile_settings->color_flow_effect);
	konextd_rmp_set_light_chose_type(rmp, profile_settings->light_chose_type);

	koneplus_rmp_set_polling_rate(rmp, profile_settings->polling_rate);
}
