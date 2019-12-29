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

static void kone_profile_set_button_info_from_rmp(KoneProfile *profile, KoneRMP *rmp, guint i) {
	KoneRMPButtonInfo *rmp_button_info;
	rmp_button_info = kone_rmp_get_rmp_button_info(rmp, i);
	kone_rmp_button_info_to_button_info(rmp_button_info, &profile->button_info[i]);
	g_free(rmp_button_info);
}

static void kone_profile_set_light_info_from_rmp(KoneProfile *profile, KoneRMP *rmp, guint i) {
	KoneRMPLightInfo *rmp_light_info;
	rmp_light_info = kone_rmp_get_rmp_light_info(rmp, i);
	kone_rmp_light_info_to_light_info(rmp_light_info, &profile->light_info[i]);
	g_free(rmp_light_info);
}

static void kone_profile_set_dpi_all_from_rmp(KoneProfile *profile, KoneRMP *rmp) {
	guint i;
	for (i = 0; i < KONE_DPI_NUM; ++i)
		roccat_set_bit8(&profile->dpi_enabled, i, kone_rmp_get_dpi(rmp, i));
}

KoneProfile *kone_rmp_to_profile(KoneRMP *rmp) {
	guint i;
	KoneProfile *profile = g_malloc0(sizeof(KoneProfile));

	kone_profile_set_main_sensitivity(profile, kone_rmp_get_main_sensitivity(rmp));
	profile->advanced_sensitivity = kone_rmp_get_advanced_sensitivity(rmp);
	kone_profile_set_x_sensitivity(profile, kone_rmp_get_x_sensitivity(rmp));
	kone_profile_set_y_sensitivity(profile, kone_rmp_get_y_sensitivity(rmp));
	profile->startup_dpi = kone_rmp_get_startup_dpi(rmp);
	profile->polling_rate = kone_rmp_get_polling_rate(rmp);
	profile->dcu_flag = kone_rmp_get_dcu_flag(rmp);
	profile->light_effect_1 = kone_rmp_get_light_effect_1(rmp);
	profile->light_effect_2 = kone_rmp_get_light_effect_2(rmp);
	profile->light_effect_3 = kone_rmp_get_light_effect_3(rmp);
	profile->light_effect_speed = kone_rmp_get_light_effect_speed(rmp);

	kone_profile_set_dpi_all_from_rmp(profile, rmp);

	for (i = 0; i < KONE_LIGHT_INFO_NUM; ++i)
		kone_profile_set_light_info_from_rmp(profile, rmp, i);

	for (i = 0; i < KONE_BUTTON_INFO_NUM; ++i)
		kone_profile_set_button_info_from_rmp(profile, rmp, i);

	return profile;
}

/*
 * \brief converts KoneButtonInfo to KoneRMPButtonInfo and stores it in rmp file
 */
static void kone_rmp_set_button_info(KoneRMP *rmp, guint index, KoneButtonInfo const *button_info) {
	KoneRMPButtonInfo rmp_button_info;

	kone_button_info_to_rmp_button_info(button_info, &rmp_button_info);
	kone_rmp_set_rmp_button_info(rmp, index, &rmp_button_info);
}

static void kone_rmp_set_light_info(KoneRMP *rmp, guint index, KoneLightInfo const *light_info) {
	KoneRMPLightInfo rmp_light_info;

	kone_light_info_to_rmp_light_info(light_info, &rmp_light_info);
	kone_rmp_set_rmp_light_info(rmp, index, &rmp_light_info);
}

void kone_rmp_update_with_profile(KoneRMP *rmp, KoneProfile const *profile) {
	guint i;

	kone_rmp_set_main_sensitivity(rmp, kone_profile_get_main_sensitivity(profile));
	kone_rmp_set_advanced_sensitivity(rmp, profile->advanced_sensitivity);
	kone_rmp_set_x_sensitivity(rmp, kone_profile_get_x_sensitivity(profile));
	kone_rmp_set_y_sensitivity(rmp, kone_profile_get_y_sensitivity(profile));
	kone_rmp_set_dpi_all(rmp, profile->dpi_enabled);
	kone_rmp_set_startup_dpi(rmp, profile->startup_dpi);
	kone_rmp_set_polling_rate(rmp, profile->polling_rate);
	kone_rmp_set_dcu_flag(rmp, profile->dcu_flag);
	kone_rmp_set_light_effect_1(rmp, profile->light_effect_1);
	kone_rmp_set_light_effect_2(rmp, profile->light_effect_2);
	kone_rmp_set_light_effect_3(rmp, profile->light_effect_3);
	kone_rmp_set_light_effect_speed(rmp, profile->light_effect_speed);

	for (i = 0; i < KONE_LIGHT_INFO_NUM; ++i)
		kone_rmp_set_light_info(rmp, i, &profile->light_info[i]);

	for (i = 0; i < KONE_BUTTON_INFO_NUM; ++i)
		kone_rmp_set_button_info(rmp, i, &profile->button_info[i]);
}
