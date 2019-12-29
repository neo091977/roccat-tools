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

#include "kovaplus.h"
#include "roccat_helper.h"

static guint16 kovaplus_profile_settings_calc_checksum(KovaplusProfileSettings const *profile_settings) {
	return ROCCAT_BYTESUM_PARTIALLY(profile_settings, KovaplusProfileSettings, report_id, checksum);
}

void kovaplus_profile_settings_set_checksum(KovaplusProfileSettings *profile_settings) {
	profile_settings->checksum = GUINT16_TO_LE(kovaplus_profile_settings_calc_checksum(profile_settings));
}

void kovaplus_profile_settings_finalize(KovaplusProfileSettings *profile_settings, guint profile_index) {
	g_assert(profile_index < KOVAPLUS_PROFILE_NUM);
	profile_settings->report_id = KOVAPLUS_REPORT_ID_PROFILE_SETTINGS;
	profile_settings->size = sizeof(KovaplusProfileSettings);
	profile_settings->profile_index = profile_index;
	kovaplus_profile_settings_set_checksum(profile_settings);
}

gboolean kovaplus_profile_settings_write(RoccatDevice *kovaplus, guint profile_index, KovaplusProfileSettings *profile_settings, GError **error) {
	gboolean result;
	g_assert(profile_index < KOVAPLUS_PROFILE_NUM);
	kovaplus_profile_settings_finalize(profile_settings, profile_index);
	result = kovaplus_device_write(kovaplus, (gchar const *)profile_settings, sizeof(KovaplusProfileSettings), error);
	return result;
}

KovaplusProfileSettings *kovaplus_profile_settings_read(RoccatDevice *kovaplus, guint profile_number, GError **error) {
	KovaplusProfileSettings *settings;

	g_assert(profile_number < KOVAPLUS_PROFILE_NUM);

	gaminggear_device_lock(GAMINGGEAR_DEVICE(kovaplus));

	if (!kovaplus_select(kovaplus, profile_number, KOVAPLUS_CONTROL_REQUEST_PROFILE_SETTINGS, error)) {
		gaminggear_device_unlock(GAMINGGEAR_DEVICE(kovaplus));
		return NULL;
	}

	settings = (KovaplusProfileSettings *)kovaplus_device_read(kovaplus, KOVAPLUS_REPORT_ID_PROFILE_SETTINGS,
			sizeof(KovaplusProfileSettings), error);

	gaminggear_device_unlock(GAMINGGEAR_DEVICE(kovaplus));

	return settings;
}

guint kovaplus_cpi_level_to_value(guint level) {
	switch (level) {
	case 7:
		return 3200;
	case 4:
		return 1600;
	case 2:
		return 800;
	case 1:
		return 400;
	default:
		return 0;
	}
}
