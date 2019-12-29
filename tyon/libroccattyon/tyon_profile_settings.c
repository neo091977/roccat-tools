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

#include "tyon.h"
#include "tyon_profile_settings.h"
#include "tyon_device.h"
#include "roccat_helper.h"

static guint16 tyon_profile_settings_calc_checksum(TyonProfileSettings const *profile_settings) {
	return ROCCAT_BYTESUM_PARTIALLY(profile_settings, TyonProfileSettings, report_id, checksum);
}

static void tyon_profile_settings_set_checksum(TyonProfileSettings *profile_settings) {
	guint16 checksum = tyon_profile_settings_calc_checksum(profile_settings);
	profile_settings->checksum = GUINT16_TO_LE(checksum);
}

void tyon_profile_settings_finalize(TyonProfileSettings *profile_settings, guint profile_index) {
	g_assert(profile_index < TYON_PROFILE_NUM);
	profile_settings->profile_index = profile_index;
	tyon_profile_settings_set_checksum(profile_settings);
}

gboolean tyon_profile_settings_write(RoccatDevice *device, guint profile_index, TyonProfileSettings *profile_settings, GError **error) {
	g_assert(profile_index < TYON_PROFILE_NUM);
	tyon_profile_settings_finalize(profile_settings, profile_index);
	return tyon_device_write(device, (gchar const *)profile_settings, sizeof(TyonProfileSettings), error);
}

TyonProfileSettings *tyon_profile_settings_read(RoccatDevice *device, guint profile_index, GError **error) {
	TyonProfileSettings *profile_settings;

	g_assert(profile_index < TYON_PROFILE_NUM);

	gaminggear_device_lock(GAMINGGEAR_DEVICE(device));

	if (!tyon_select(device, profile_index, TYON_CONTROL_DATA_INDEX_NONE, TYON_CONTROL_REQUEST_PROFILE_SETTINGS, error)) {
		gaminggear_device_unlock(GAMINGGEAR_DEVICE(device));
		return NULL;
	}

	profile_settings = (TyonProfileSettings *)tyon_device_read(device, TYON_REPORT_ID_PROFILE_SETTINGS, sizeof(TyonProfileSettings), error);

	gaminggear_device_unlock(GAMINGGEAR_DEVICE(device));
	return profile_settings;
}

gboolean tyon_profile_settings_equal(TyonProfileSettings const *left, TyonProfileSettings const *right) {
	gboolean equal;
	equal = ROCCAT_MEMCMP_PARTIALLY(left, right, TyonProfileSettings, advanced_sensitivity, checksum);
	return equal ? FALSE : TRUE;
}
