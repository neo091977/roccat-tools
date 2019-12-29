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

#include "konextdoptical.h"
#include "roccat_helper.h"
#include "roccat.h"

guint konextdoptical_rmp_cpi_to_bin(guint rmp_value) {
	return rmp_value << 1;
}

guint konextdoptical_bin_cpi_to_rmp(guint bin_value) {
	return bin_value >> 1;
}

KonextdopticalProfileSettings *konextdoptical_rmp_to_profile_settings(KoneplusRmp *rmp) {
	KonextdopticalProfileSettings *settings;
	guint i;
	KoneplusRmpLightInfo *rmp_light_info;

	settings = g_malloc0(sizeof(KonextdopticalProfileSettings));
	settings->report_id = KONEPLUS_REPORT_ID_PROFILE_SETTINGS;
	settings->size = sizeof(KonextdopticalProfileSettings);

	settings->xy_sync = koneplus_rmp_get_xy_synchronous(rmp);
	if (settings->xy_sync == ROCCAT_SENSITIVITY_ADVANCED_ON) {
		settings->sensitivity_x = koneplus_rmp_get_sensitivity_x(rmp);
		settings->sensitivity_y = koneplus_rmp_get_sensitivity_y(rmp);
	} else {
		settings->sensitivity_x = koneplus_rmp_get_sensitivity(rmp);
		settings->sensitivity_y = settings->sensitivity_x;
	}

	for (i = 0; i < KONEPLUS_PROFILE_SETTING_CPI_LEVELS_NUM; ++i) {
		settings->cpi_levels_x[i] = konextdoptical_rmp_cpi_to_bin(koneplus_rmp_get_cpi_level_x(rmp, i));
		settings->cpi_levels_y[i] = konextdoptical_rmp_cpi_to_bin(koneplus_rmp_get_cpi_level_y(rmp, i));
	}
	settings->cpi_levels_enabled = koneplus_rmp_get_cpi_all(rmp);
	settings->cpi_startup_level = koneplus_rmp_get_cpi_x(rmp);

	settings->polling_rate = koneplus_rmp_get_polling_rate(rmp);

	settings->color_flow = koneplus_rmp_get_light_color_flow(rmp);
	settings->light_effect = koneplus_rmp_get_light_effect_type(rmp);
	settings->light_effect_mode = koneplus_rmp_get_light_effect_mode(rmp);
	settings->effect_speed = koneplus_rmp_get_light_effect_speed(rmp);
	settings->angle_snapping = konepuremilitary_rmp_get_anglesnap_value(rmp);
	settings->sensor_alignment = konepuremilitary_rmp_get_sensoralign_value(rmp);
	roccat_set_bit8(&settings->advanced1, KONEPUREMILITARY_PROFILE_SETTINGS_ADVANCED_ANGLE_SNAP_BIT,
			konepuremilitary_rmp_get_anglesnap_flag(rmp) == KONEPUREMILITARY_RMP_ANGLESNAP_FLAG_ON);
	roccat_set_bit8(&settings->advanced1, KONEPUREMILITARY_PROFILE_SETTINGS_ADVANCED_SENSOR_ALIGNMENT_BIT,
			konepuremilitary_rmp_get_sensoralign_flag(rmp) == KONEPUREMILITARY_RMP_SENSORALIGN_FLAG_ON);
	settings->advanced2 = settings->advanced1;

	for (i = 0; i < KONEPLUS_PROFILE_SETTING_LIGHTS_NUM; ++i) {
		rmp_light_info = koneplus_rmp_get_rmp_light_info(rmp, i);
		roccat_set_bit8(&settings->lights_enabled, i, rmp_light_info->state == KONEPLUS_RMP_LIGHT_INFO_STATE_ON);
		if (konextd_rmp_get_light_chose_type(rmp) == KONEXTD_RMP_LIGHT_CHOSE_TYPE_TABLE) {
			koneplus_rmp_light_info_to_light_info(rmp_light_info, &settings->lights[i], FALSE);
		} else {
			g_free(rmp_light_info);
			rmp_light_info = konextd_rmp_get_custom_light_info(rmp, i);
			koneplus_rmp_light_info_to_light_info(rmp_light_info, &settings->lights[i], TRUE);
		}
		g_free(rmp_light_info);
	}

	return settings;
}

void konextdoptical_rmp_update_with_profile_settings(KoneplusRmp *rmp, KonextdopticalProfileSettings const *profile_settings) {
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
		koneplus_rmp_set_cpi_level_x(rmp, i, konextdoptical_bin_cpi_to_rmp(profile_settings->cpi_levels_x[i]));
		koneplus_rmp_set_cpi_level_y(rmp, i, konextdoptical_bin_cpi_to_rmp(profile_settings->cpi_levels_y[i]));
	}
	koneplus_rmp_set_cpi_all(rmp, profile_settings->cpi_levels_enabled);
	koneplus_rmp_set_cpi_x(rmp, profile_settings->cpi_startup_level);
	koneplus_rmp_set_cpi_y(rmp, profile_settings->cpi_startup_level);

	/* Using first light to determine if custom or table colors should be used */
	if (profile_settings->lights[0].index == KONEPLUS_LIGHT_INFO_INDEX_CUSTOM)
		konextd_rmp_set_light_chose_type(rmp, KONEXTD_RMP_LIGHT_CHOSE_TYPE_CUSTOM);
	else
		konextd_rmp_set_light_chose_type(rmp, KONEXTD_RMP_LIGHT_CHOSE_TYPE_TABLE);

	for (i = 0; i < KONEPLUS_PROFILE_SETTING_LIGHTS_NUM; ++i) {
		if (profile_settings->lights[0].index == KONEPLUS_LIGHT_INFO_INDEX_CUSTOM) {
			koneplus_light_info_to_custom_rmp_light_info(&profile_settings->lights[i], &rmp_light_info);
			konextd_rmp_set_custom_light_info(rmp, i, &rmp_light_info);

			rmp_light_info_pointer = koneplus_rmp_get_rmp_light_info(rmp, i);
			rmp_light_info_pointer->state = roccat_get_bit8(profile_settings->lights_enabled, i) ? KONEPLUS_RMP_LIGHT_INFO_STATE_ON : KONEPLUS_RMP_LIGHT_INFO_STATE_OFF;
			koneplus_rmp_set_rmp_light_info(rmp, i, rmp_light_info_pointer);
			g_free(rmp_light_info_pointer);
		} else {
			koneplus_light_info_to_rmp_light_info(&profile_settings->lights[i], &rmp_light_info, roccat_get_bit8(profile_settings->lights_enabled, i));
			koneplus_rmp_set_rmp_light_info(rmp, i, &rmp_light_info);
		}
	}
	koneplus_rmp_set_light_effect_type(rmp, profile_settings->light_effect);
	koneplus_rmp_set_light_effect_mode(rmp, profile_settings->light_effect_mode);
	koneplus_rmp_set_light_effect_speed(rmp, profile_settings->effect_speed);
	koneplus_rmp_set_light_color_flow(rmp, profile_settings->color_flow);
	konepuremilitary_rmp_set_anglesnap_value(rmp, profile_settings->angle_snapping);
	konepuremilitary_rmp_set_sensoralign_value(rmp, profile_settings->sensor_alignment);
	konepuremilitary_rmp_set_anglesnap_flag(rmp,
			roccat_get_bit8(profile_settings->advanced2, KONEPUREMILITARY_PROFILE_SETTINGS_ADVANCED_ANGLE_SNAP_BIT) ?
					KONEPUREMILITARY_RMP_ANGLESNAP_FLAG_ON : KONEPUREMILITARY_RMP_ANGLESNAP_FLAG_OFF);
	konepuremilitary_rmp_set_sensoralign_flag(rmp,
			roccat_get_bit8(profile_settings->advanced2, KONEPUREMILITARY_PROFILE_SETTINGS_ADVANCED_SENSOR_ALIGNMENT_BIT) ?
					KONEPUREMILITARY_RMP_SENSORALIGN_FLAG_ON : KONEPUREMILITARY_RMP_SENSORALIGN_FLAG_OFF);

	koneplus_rmp_set_polling_rate(rmp, profile_settings->polling_rate);
}
