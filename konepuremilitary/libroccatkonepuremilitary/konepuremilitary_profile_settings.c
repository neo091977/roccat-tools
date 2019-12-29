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

gboolean konepuremilitary_profile_settings_write(RoccatDevice *konepuremilitary, guint profile_index, KonepuremilitaryProfileSettings *profile_settings, GError **error) {
	g_assert(profile_index < KONEPLUS_PROFILE_NUM);
	profile_settings->profile_index = profile_index;
	return koneplus_device_write(konepuremilitary, (gchar const *)profile_settings, sizeof(KonepuremilitaryProfileSettings), error);
}

KonepuremilitaryProfileSettings *konepuremilitary_profile_settings_read(RoccatDevice *konepuremilitary, guint profile_index, GError **error) {
	KonepuremilitaryProfileSettings *settings;

	g_assert(profile_index < KONEPLUS_PROFILE_NUM);

	gaminggear_device_lock(GAMINGGEAR_DEVICE(konepuremilitary));

	if (!koneplus_select(konepuremilitary, profile_index, KONEPLUS_CONTROL_REQUEST_PROFILE_SETTINGS, error)) {
		gaminggear_device_unlock(GAMINGGEAR_DEVICE(konepuremilitary));
		return NULL;
	}

	settings = (KonepuremilitaryProfileSettings *)koneplus_device_read(konepuremilitary, KONEPLUS_REPORT_ID_PROFILE_SETTINGS, sizeof(KonepuremilitaryProfileSettings), error);

	gaminggear_device_unlock(GAMINGGEAR_DEVICE(konepuremilitary));

	return settings;
}

gboolean konepuremilitary_profile_settings_equal(KonepuremilitaryProfileSettings const *left, KonepuremilitaryProfileSettings const *right) {
	gboolean equal;
	equal = ROCCAT_MEMCMP_PARTIALLY_TO_END(left, right, KonepuremilitaryProfileSettings, xy_sync);
	return equal ? FALSE : TRUE;
}
