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

#include "koneplus_rmp.h"

static gchar const * const konextd_rmp_light_chose_type_name = "LtChoseType";

static gchar *konextd_rmp_create_custom_light_info_name(guint index) {
	g_assert(index < KONEPLUS_PROFILE_SETTING_LIGHTS_NUM);
	return g_strdup_printf("CustomLightInfo%i", index);
}

void konextd_rmp_set_light_chose_type(KoneplusRmp *rmp, guint value) {
 	koneplus_rmp_set_value(rmp, konextd_rmp_light_chose_type_name, value);
}

guint konextd_rmp_get_light_chose_type(KoneplusRmp *rmp) {
	return koneplus_rmp_get_value(rmp, konextd_rmp_light_chose_type_name);
}

void konextd_rmp_set_custom_light_info(KoneplusRmp *rmp, guint index, KoneplusRmpLightInfo *rmp_light_info) {
	gchar *key = NULL;

	g_assert(index < KONEPLUS_PROFILE_SETTING_LIGHTS_NUM);
	key = konextd_rmp_create_custom_light_info_name(index);
	koneplus_rmp_set_light_info_value(rmp, key, rmp_light_info);
	g_free(key);
}

KoneplusRmpLightInfo *konextd_rmp_get_custom_light_info(KoneplusRmp *rmp, guint index) {
	KoneplusRmpLightInfo *rmp_light_info;
	gchar *key = NULL;

	g_assert(index < KONEPLUS_PROFILE_SETTING_LIGHTS_NUM);
	key = konextd_rmp_create_custom_light_info_name(index);
	rmp_light_info = koneplus_rmp_get_light_info_value(rmp, key);
	g_free(key);
	return rmp_light_info;
}

