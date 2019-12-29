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

static guint16 konepureoptical_profile_settings_calc_checksum(KonepureopticalProfileSettings const *profile_settings) {
	return ROCCAT_BYTESUM_PARTIALLY(profile_settings, KonepureopticalProfileSettings, report_id, checksum);
}

static void konepureoptical_profile_settings_set_checksum(KonepureopticalProfileSettings *profile_settings) {
	guint16 checksum = konepureoptical_profile_settings_calc_checksum(profile_settings);
	profile_settings->checksum = GUINT16_TO_LE(checksum);
}

void konepureoptical_profile_settings_finalize(KonepureopticalProfileSettings *profile_settings, guint index) {
	g_assert(index < KONEPLUS_PROFILE_NUM);
	profile_settings->profile_number = index;
	konepureoptical_profile_settings_set_checksum(profile_settings);
}

gboolean konepureoptical_profile_settings_write(RoccatDevice *konepureoptical, guint profile_number, KonepureopticalProfileSettings *profile_settings, GError **error) {
	g_assert(profile_number < KONEPLUS_PROFILE_NUM);
	konepureoptical_profile_settings_finalize(profile_settings, profile_number);
	return koneplus_device_write(konepureoptical, (gchar const *)profile_settings, sizeof(KonepureopticalProfileSettings), error);
}

KonepureopticalProfileSettings *konepureoptical_profile_settings_read(RoccatDevice *konepureoptical, guint profile_number, GError **error) {
	KonepureopticalProfileSettings *settings;

	g_assert(profile_number < KONEPLUS_PROFILE_NUM);

	gaminggear_device_lock(GAMINGGEAR_DEVICE(konepureoptical));

	if (!koneplus_select(konepureoptical, profile_number, KONEPLUS_CONTROL_REQUEST_PROFILE_SETTINGS, error)) {
		gaminggear_device_unlock(GAMINGGEAR_DEVICE(konepureoptical));
		return NULL;
	}

	settings = (KonepureopticalProfileSettings *)koneplus_device_read(konepureoptical, KONEPLUS_REPORT_ID_PROFILE_SETTINGS, sizeof(KonepureopticalProfileSettings), error);

	gaminggear_device_unlock(GAMINGGEAR_DEVICE(konepureoptical));

	return settings;
}

gboolean konepureoptical_profile_settings_equal(KonepureopticalProfileSettings const *left, KonepureopticalProfileSettings const *right) {
	gboolean equal;
	equal = ROCCAT_MEMCMP_PARTIALLY(left, right, KonepureopticalProfileSettings, xy_sync, checksum);
	return equal ? FALSE : TRUE;
}
