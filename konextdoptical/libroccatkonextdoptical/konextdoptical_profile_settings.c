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

static guint16 konextdoptical_profile_settings_calc_checksum(KonextdopticalProfileSettings const *profile_settings) {
	return ROCCAT_BYTESUM_PARTIALLY(profile_settings, KonextdopticalProfileSettings, report_id, checksum);
}

static void konextdoptical_profile_settings_set_checksum(KonextdopticalProfileSettings *profile_settings) {
	guint16 checksum = konextdoptical_profile_settings_calc_checksum(profile_settings);
	profile_settings->checksum = GUINT16_TO_LE(checksum);
}

void konextdoptical_profile_settings_finalize(KonextdopticalProfileSettings *profile_settings, guint profile_index) {
	g_assert(profile_index < KONEPLUS_PROFILE_NUM);
	profile_settings->profile_index = profile_index;
	konextdoptical_profile_settings_set_checksum(profile_settings);
}

gboolean konextdoptical_profile_settings_write(RoccatDevice *konextdoptical, guint profile_index, KonextdopticalProfileSettings *profile_settings, GError **error) {
	g_assert(profile_index < KONEPLUS_PROFILE_NUM);
	konextdoptical_profile_settings_finalize(profile_settings, profile_index);
	return koneplus_device_write(konextdoptical, (gchar const *)profile_settings, sizeof(KonextdopticalProfileSettings), error);
}

KonextdopticalProfileSettings *konextdoptical_profile_settings_read(RoccatDevice *konextdoptical, guint profile_index, GError **error) {
	KonextdopticalProfileSettings *settings;

	g_assert(profile_index < KONEPLUS_PROFILE_NUM);

	gaminggear_device_lock(GAMINGGEAR_DEVICE(konextdoptical));

	if (!koneplus_select(konextdoptical, profile_index, KONEPLUS_CONTROL_REQUEST_PROFILE_SETTINGS, error)) {
		gaminggear_device_unlock(GAMINGGEAR_DEVICE(konextdoptical));
		return NULL;
	}

	settings = (KonextdopticalProfileSettings *)koneplus_device_read(konextdoptical, KONEPLUS_REPORT_ID_PROFILE_SETTINGS, sizeof(KonextdopticalProfileSettings), error);

	gaminggear_device_unlock(GAMINGGEAR_DEVICE(konextdoptical));

	return settings;
}
