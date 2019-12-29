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

#include "nyth.h"
#include "nyth_profile_settings.h"
#include "nyth_device.h"
#include "roccat_helper.h"

static guint8 const default_profile_settings[sizeof(NythProfileSettings)] = {
	0x06, 0x21, 0x00, 0x00, 0x06, 0x06, 0x1F, 0x20, 0x03, 0x40, 0x06, 0x80, 0x0C, 0x00, 0x19, 0xE0,
	0x2E, 0x00, 0x03, 0x03, 0x01, 0x03, 0x02, 0x01, 0x00, 0x64, 0xFA, 0x01, 0x00, 0x64, 0xFA, 0x64,
	0x05
};

NythProfileSettings const *nyth_profile_settings_default(void) {
	return (NythProfileSettings const *)default_profile_settings;
}

static guint16 nyth_profile_settings_calc_checksum(NythProfileSettings const *profile_settings) {
	return ROCCAT_BYTESUM_PARTIALLY(profile_settings, NythProfileSettings, report_id, checksum);
}

static void nyth_profile_settings_set_checksum(NythProfileSettings *profile_settings) {
	guint16 checksum = nyth_profile_settings_calc_checksum(profile_settings);
	profile_settings->checksum = GUINT16_TO_LE(checksum);
}

void nyth_profile_settings_finalize(NythProfileSettings *profile_settings, guint profile_index) {
	g_assert(profile_index < NYTH_PROFILE_NUM);
	profile_settings->report_id = NYTH_REPORT_ID_PROFILE_SETTINGS;
	profile_settings->size = sizeof(NythProfileSettings);
	profile_settings->profile_index = profile_index;
	nyth_profile_settings_set_checksum(profile_settings);
}

gboolean nyth_profile_settings_write(RoccatDevice *device, guint profile_index, NythProfileSettings *profile_settings, GError **error) {
	g_assert(profile_index < NYTH_PROFILE_NUM);
	nyth_profile_settings_finalize(profile_settings, profile_index);
	return nyth_device_write(device, (gchar const *)profile_settings, sizeof(NythProfileSettings), error);
}

NythProfileSettings *nyth_profile_settings_read(RoccatDevice *device, guint profile_index, GError **error) {
	NythProfileSettings *profile_settings;

	g_assert(profile_index < NYTH_PROFILE_NUM);

	gaminggear_device_lock(GAMINGGEAR_DEVICE(device));

	if (!nyth_select(device, profile_index, NYTH_CONTROL_DATA_INDEX_NONE, NYTH_CONTROL_REQUEST_PROFILE_SETTINGS, error)) {
		gaminggear_device_unlock(GAMINGGEAR_DEVICE(device));
		return NULL;
	}

	profile_settings = (NythProfileSettings *)nyth_device_read(device, NYTH_REPORT_ID_PROFILE_SETTINGS, sizeof(NythProfileSettings), error);

	gaminggear_device_unlock(GAMINGGEAR_DEVICE(device));
	return profile_settings;
}

gboolean nyth_profile_settings_equal(NythProfileSettings const *left, NythProfileSettings const *right) {
	gboolean equal;
	equal = ROCCAT_MEMCMP_PARTIALLY(left, right, NythProfileSettings, advanced_sensitivity, checksum);
	return equal ? FALSE : TRUE;
}

void nyth_profile_settings_copy(NythProfileSettings *destination, NythProfileSettings const *source) {
	memcpy(destination, source, sizeof(NythProfileSettings));
}
