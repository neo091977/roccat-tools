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

#include "kova2016.h"
#include "kova2016_profile_settings.h"
#include "kova2016_device.h"
#include "roccat_helper.h"

static guint8 const default_profile_settings[sizeof(Kova2016ProfileSettings)] = {
	0x06, 0x1C, 0x00, 0x00, 0x06, 0xFF, 0x1F, 0x08, 0x10, 0x20, 0x46, 0x8C, 0x01, 0x03, 0x03, 0x01,
	0x03, 0x02, 0x00, 0x05, 0x90, 0xFE, 0x00, 0x05, 0x90, 0xFE, 0x83, 0x05
};

Kova2016ProfileSettings const *kova2016_profile_settings_default(void) {
	return (Kova2016ProfileSettings const *)default_profile_settings;
}

static guint16 kova2016_profile_settings_calc_checksum(Kova2016ProfileSettings const *profile_settings) {
	return ROCCAT_BYTESUM_PARTIALLY(profile_settings, Kova2016ProfileSettings, report_id, checksum);
}

static void kova2016_profile_settings_set_checksum(Kova2016ProfileSettings *profile_settings) {
	guint16 checksum = kova2016_profile_settings_calc_checksum(profile_settings);
	profile_settings->checksum = GUINT16_TO_LE(checksum);
}

void kova2016_profile_settings_finalize(Kova2016ProfileSettings *profile_settings, guint profile_index) {
	g_assert(profile_index < KOVA2016_PROFILE_NUM);
	profile_settings->report_id = KOVA2016_REPORT_ID_PROFILE_SETTINGS;
	profile_settings->size = sizeof(Kova2016ProfileSettings);
	profile_settings->profile_index = profile_index;
	kova2016_profile_settings_set_checksum(profile_settings);
}

gboolean kova2016_profile_settings_write(RoccatDevice *device, guint profile_index, Kova2016ProfileSettings *profile_settings, GError **error) {
	g_assert(profile_index < KOVA2016_PROFILE_NUM);
	kova2016_profile_settings_finalize(profile_settings, profile_index);
	return kova2016_device_write(device, (gchar const *)profile_settings, sizeof(Kova2016ProfileSettings), error);
}

Kova2016ProfileSettings *kova2016_profile_settings_read(RoccatDevice *device, guint profile_index, GError **error) {
	Kova2016ProfileSettings *profile_settings;

	g_assert(profile_index < KOVA2016_PROFILE_NUM);

	gaminggear_device_lock(GAMINGGEAR_DEVICE(device));

	if (!kova2016_select(device, profile_index, KOVA2016_CONTROL_DATA_INDEX_NONE, KOVA2016_CONTROL_REQUEST_PROFILE_SETTINGS, error)) {
		gaminggear_device_unlock(GAMINGGEAR_DEVICE(device));
		return NULL;
	}

	profile_settings = (Kova2016ProfileSettings *)kova2016_device_read(device, KOVA2016_REPORT_ID_PROFILE_SETTINGS, sizeof(Kova2016ProfileSettings), error);

	gaminggear_device_unlock(GAMINGGEAR_DEVICE(device));
	return profile_settings;
}

gboolean kova2016_profile_settings_equal(Kova2016ProfileSettings const *left, Kova2016ProfileSettings const *right) {
	gboolean equal;
	equal = ROCCAT_MEMCMP_PARTIALLY(left, right, Kova2016ProfileSettings, unknown1, checksum);
	return equal ? FALSE : TRUE;
}

void kova2016_profile_settings_copy(Kova2016ProfileSettings *destination, Kova2016ProfileSettings const *source) {
	memcpy(destination, source, sizeof(Kova2016ProfileSettings));
}
