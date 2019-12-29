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

#include "koneplus.h"
#include "roccat_helper.h"

static guint16 koneplus_profile_settings_calc_checksum(KoneplusProfileSettings const *profile_settings) {
	return ROCCAT_BYTESUM_PARTIALLY(profile_settings, KoneplusProfileSettings, report_id, checksum);
}

static void koneplus_profile_settings_set_checksum(KoneplusProfileSettings *profile_settings) {
	guint16 checksum = koneplus_profile_settings_calc_checksum(profile_settings);
	profile_settings->checksum = GUINT16_TO_LE(checksum);
}

void koneplus_profile_settings_finalize(KoneplusProfileSettings *profile_settings, guint profile_index) {
	g_assert(profile_index < KONEPLUS_PROFILE_NUM);
	profile_settings->profile_index = profile_index;
	koneplus_profile_settings_set_checksum(profile_settings);
}

gboolean koneplus_profile_settings_write(RoccatDevice *koneplus, guint profile_index, KoneplusProfileSettings *profile_settings, GError **error) {
	g_assert(profile_index < KONEPLUS_PROFILE_NUM);
	koneplus_profile_settings_finalize(profile_settings, profile_index);
	return koneplus_device_write(koneplus, (gchar const *)profile_settings, sizeof(KoneplusProfileSettings), error);
}

KoneplusProfileSettings *koneplus_profile_settings_read(RoccatDevice *koneplus, guint profile_index, GError **error) {
	KoneplusProfileSettings *settings;

	g_assert(profile_index < KONEPLUS_PROFILE_NUM);

	gaminggear_device_lock(GAMINGGEAR_DEVICE(koneplus));

	if (!koneplus_select(koneplus, profile_index, KONEPLUS_CONTROL_REQUEST_PROFILE_SETTINGS, error)) {
		gaminggear_device_unlock(GAMINGGEAR_DEVICE(koneplus));
		return NULL;
	}

	settings = (KoneplusProfileSettings *)koneplus_device_read(koneplus, KONEPLUS_REPORT_ID_PROFILE_SETTINGS, sizeof(KoneplusProfileSettings), error);

	gaminggear_device_unlock(GAMINGGEAR_DEVICE(koneplus));

	return settings;
}
