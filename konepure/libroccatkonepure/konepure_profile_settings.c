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

#include "konepure.h"
#include "roccat_helper.h"

static guint16 konepure_profile_settings_calc_checksum(KonepureProfileSettings const *profile_settings) {
	return ROCCAT_BYTESUM_PARTIALLY(profile_settings, KonepureProfileSettings, report_id, checksum);
}

static void konepure_profile_settings_set_checksum(KonepureProfileSettings *profile_settings) {
	guint16 checksum = konepure_profile_settings_calc_checksum(profile_settings);
	profile_settings->checksum = GUINT16_TO_LE(checksum);
}

void konepure_profile_settings_finalize(KonepureProfileSettings *profile_settings, guint index) {
	g_assert(index < KONEPLUS_PROFILE_NUM);
	profile_settings->profile_number = index;
	konepure_profile_settings_set_checksum(profile_settings);
}

gboolean konepure_profile_settings_write(RoccatDevice *konepure, guint profile_number, KonepureProfileSettings *profile_settings, GError **error) {
	g_assert(profile_number < KONEPLUS_PROFILE_NUM);
	konepure_profile_settings_finalize(profile_settings, profile_number);
	return koneplus_device_write(konepure, (gchar const *)profile_settings, sizeof(KonepureProfileSettings), error);
}

KonepureProfileSettings *konepure_profile_settings_read(RoccatDevice *konepure, guint profile_number, GError **error) {
	KonepureProfileSettings *settings;

	g_assert(profile_number < KONEPLUS_PROFILE_NUM);

	gaminggear_device_lock(GAMINGGEAR_DEVICE(konepure));

	if (!koneplus_select(konepure, profile_number, KONEPLUS_CONTROL_REQUEST_PROFILE_SETTINGS, error)) {
		gaminggear_device_unlock(GAMINGGEAR_DEVICE(konepure));
		return NULL;
	}

	settings = (KonepureProfileSettings *)koneplus_device_read(konepure, KONEPLUS_REPORT_ID_PROFILE_SETTINGS, sizeof(KonepureProfileSettings), error);

	gaminggear_device_unlock(GAMINGGEAR_DEVICE(konepure));

	return settings;
}

gboolean konepure_profile_settings_equal(KonepureProfileSettings const *left, KonepureProfileSettings const *right) {
	gboolean equal;
	equal = ROCCAT_MEMCMP_PARTIALLY(left, right, KonepureProfileSettings, xy_sync, checksum);
	return equal ? FALSE : TRUE;
}
