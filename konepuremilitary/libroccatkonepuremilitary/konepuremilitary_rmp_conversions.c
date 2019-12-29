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

#include "konepuremilitary.h"
#include "roccat_helper.h"
#include "roccat.h"

static void koneplus_rmp_light_info_to_konepuremilitary_light_info(KoneplusRmpLightInfo const *from, KonepuremilitaryLight *to, gboolean custom) {
	if (custom)
		to->index = KONEPLUS_LIGHT_INFO_INDEX_CUSTOM;
	else
		to->index = from->index;
	to->red = from->red;
	to->green = from->green;
	to->blue = from->blue;
}

static void konepuremilitary_light_info_to_custom_koneplus_rmp_light_info(KonepuremilitaryLight const *light_info, KoneplusRmpLightInfo *rmp_light_info) {
	rmp_light_info->index = 0;
	rmp_light_info->state = 0;
	rmp_light_info->red = light_info->red;
	rmp_light_info->green = light_info->green;
	rmp_light_info->blue = light_info->blue;
	koneplus_rmp_light_info_set_checksum(rmp_light_info);
}

static void konepuremilitary_light_info_to_koneplus_rmp_light_info(KonepuremilitaryLight const *light_info, KoneplusRmpLightInfo *rmp_light_info, gboolean state) {
	rmp_light_info->index = light_info->index;
	rmp_light_info->state = state ? KONEPLUS_RMP_LIGHT_INFO_STATE_ON : KONEPLUS_RMP_LIGHT_INFO_STATE_OFF;
	rmp_light_info->red = light_info->red;
	rmp_light_info->green = light_info->green;
	rmp_light_info->blue = light_info->blue;
	koneplus_rmp_light_info_set_checksum(rmp_light_info);
}

KonepuremilitaryProfileSettings *konepuremilitary_rmp_to_profile_settings(KoneplusRmp *rmp) {
	KonepuremilitaryProfileSettings *settings;
	guint i;
	KoneplusRmpLightInfo *rmp_light_info;

	settings = g_malloc0(sizeof(KonepuremilitaryProfileSettings));
	settings->report_id = KONEPLUS_REPORT_ID_PROFILE_SETTINGS;
	settings->size = sizeof(KonepuremilitaryProfileSettings);

	settings->xy_sync = koneplus_rmp_get_xy_synchronous(rmp);
	if (settings->xy_sync == ROCCAT_SENSITIVITY_ADVANCED_ON) {
		settings->sensitivity_x = koneplus_rmp_get_sensitivity_x(rmp);
		settings->sensitivity_y = koneplus_rmp_get_sensitivity_y(rmp);
	} else {
		settings->sensitivity_x = koneplus_rmp_get_sensitivity(rmp);
		settings->sensitivity_y = settings->sensitivity_x;
	}

	for (i = 0; i < KONEPLUS_PROFILE_SETTING_CPI_LEVELS_NUM; ++i) {
		settings->cpi_levels_x[i] = konepuremilitary_cpi_rpm_to_hardware(koneplus_rmp_get_cpi_level_x(rmp, i));
		settings->cpi_levels_y[i] = konepuremilitary_cpi_rpm_to_hardware(koneplus_rmp_get_cpi_level_y(rmp, i));
	}
	settings->cpi_levels_enabled = koneplus_rmp_get_cpi_all(rmp);
	settings->cpi_startup_level = koneplus_rmp_get_cpi_x(rmp);

	settings->polling_rate = koneplus_rmp_get_polling_rate(rmp);

	settings->color_flow_effect = koneplus_rmp_get_light_color_flow(rmp);
	settings->light_effect_type = koneplus_rmp_get_light_effect_type(rmp);
	settings->light_effect_speed = koneplus_rmp_get_light_effect_speed(rmp);
	settings->light_chose_type = konextd_rmp_get_light_chose_type(rmp);

	roccat_set_bit8(&settings->advanced, KONEPUREMILITARY_PROFILE_SETTINGS_ADVANCED_ANGLE_SNAP_BIT,
			konepuremilitary_rmp_get_anglesnap_flag(rmp) == KONEPUREMILITARY_RMP_ANGLESNAP_FLAG_ON);
	roccat_set_bit8(&settings->advanced, KONEPUREMILITARY_PROFILE_SETTINGS_ADVANCED_SENSOR_ALIGNMENT_BIT,
			konepuremilitary_rmp_get_sensoralign_flag(rmp) == KONEPUREMILITARY_RMP_SENSORALIGN_FLAG_ON);
	settings->angle_snapping_value = konepuremilitary_rmp_get_anglesnap_value(rmp);
	settings->sensor_alignment_value = konepuremilitary_rmp_get_sensoralign_value(rmp);

	rmp_light_info = koneplus_rmp_get_rmp_light_info(rmp, 0);
	roccat_set_bit8(&settings->light_enabled, 0, rmp_light_info->state == KONEPLUS_RMP_LIGHT_INFO_STATE_ON);
	if (settings->light_chose_type == KONEXTD_RMP_LIGHT_CHOSE_TYPE_TABLE) {
		koneplus_rmp_light_info_to_konepuremilitary_light_info(rmp_light_info, &settings->light, FALSE);
	} else {
		g_free(rmp_light_info);
		rmp_light_info = konextd_rmp_get_custom_light_info(rmp, 0);
		koneplus_rmp_light_info_to_konepuremilitary_light_info(rmp_light_info, &settings->light, TRUE);
	}
	g_free(rmp_light_info);

	return settings;
}

void konepuremilitary_rmp_update_with_profile_settings(KoneplusRmp *rmp, KonepuremilitaryProfileSettings const *profile_settings) {
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

	for (i = 0; i < KONEPLUS_PROFILE_SETTING_CPI_LEVELS_NUM; ++i) {
		koneplus_rmp_set_cpi_level_x(rmp, i, konepuremilitary_cpi_hardware_to_rpm(profile_settings->cpi_levels_x[i]));
		koneplus_rmp_set_cpi_level_y(rmp, i, konepuremilitary_cpi_hardware_to_rpm(profile_settings->cpi_levels_y[i]));
	}
	koneplus_rmp_set_cpi_all(rmp, profile_settings->cpi_levels_enabled);
	koneplus_rmp_set_cpi_x(rmp, profile_settings->cpi_startup_level);
	koneplus_rmp_set_cpi_y(rmp, profile_settings->cpi_startup_level);

	if (profile_settings->light_chose_type == KONEXTD_RMP_LIGHT_CHOSE_TYPE_CUSTOM) {
		konepuremilitary_light_info_to_custom_koneplus_rmp_light_info(&profile_settings->light, &rmp_light_info);
		konextd_rmp_set_custom_light_info(rmp, 0, &rmp_light_info);

		rmp_light_info_pointer = koneplus_rmp_get_rmp_light_info(rmp, 0);
		rmp_light_info_pointer->state = roccat_get_bit8(profile_settings->light_enabled, 0) ? KONEPLUS_RMP_LIGHT_INFO_STATE_ON : KONEPLUS_RMP_LIGHT_INFO_STATE_OFF;
		koneplus_rmp_set_rmp_light_info(rmp, 0, rmp_light_info_pointer);
		g_free(rmp_light_info_pointer);
	} else {
		konepuremilitary_light_info_to_koneplus_rmp_light_info(&profile_settings->light, &rmp_light_info, roccat_get_bit8(profile_settings->light_enabled, 0));
		koneplus_rmp_set_rmp_light_info(rmp, 0, &rmp_light_info);
	}

	koneplus_rmp_set_light_effect_type(rmp, profile_settings->light_effect_type);
	koneplus_rmp_set_light_effect_speed(rmp, profile_settings->light_effect_speed);
	koneplus_rmp_set_light_color_flow(rmp, profile_settings->color_flow_effect);
	konextd_rmp_set_light_chose_type(rmp, profile_settings->light_chose_type);

	koneplus_rmp_set_polling_rate(rmp, profile_settings->polling_rate);

	konepuremilitary_rmp_set_anglesnap_flag(rmp,
			roccat_get_bit8(profile_settings->advanced, KONEPUREMILITARY_PROFILE_SETTINGS_ADVANCED_ANGLE_SNAP_BIT) ?
					KONEPUREMILITARY_RMP_ANGLESNAP_FLAG_ON : KONEPUREMILITARY_RMP_ANGLESNAP_FLAG_OFF);
	konepuremilitary_rmp_set_sensoralign_flag(rmp,
			roccat_get_bit8(profile_settings->advanced, KONEPUREMILITARY_PROFILE_SETTINGS_ADVANCED_SENSOR_ALIGNMENT_BIT) ?
					KONEPUREMILITARY_RMP_SENSORALIGN_FLAG_ON : KONEPUREMILITARY_RMP_SENSORALIGN_FLAG_OFF);
	konepuremilitary_rmp_set_anglesnap_value(rmp, profile_settings->angle_snapping_value);
	konepuremilitary_rmp_set_sensoralign_value(rmp, profile_settings->sensor_alignment_value);
}
