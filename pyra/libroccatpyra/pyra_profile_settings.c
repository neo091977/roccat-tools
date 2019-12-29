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

#include "pyra.h"
#include "roccat_helper.h"

static guint16 pyra_profile_settings_calc_checksum(PyraProfileSettings const *profile_settings) {
	return ROCCAT_BYTESUM_PARTIALLY(profile_settings, PyraProfileSettings, report_id, checksum);
}

static void pyra_profile_settings_set_checksum(PyraProfileSettings *profile_settings) {
	guint16 checksum = pyra_profile_settings_calc_checksum(profile_settings);
	profile_settings->checksum = GUINT16_TO_LE(checksum);
}

void pyra_profile_settings_finalize(PyraProfileSettings *profile_settings, guint index) {
	g_assert(index < PYRA_PROFILE_NUM);
	profile_settings->number = index;
	profile_settings->report_id = PYRA_REPORT_ID_PROFILE_SETTINGS;
	pyra_profile_settings_set_checksum(profile_settings);
}

gboolean pyra_profile_settings_write(RoccatDevice *pyra, guint profile_number, PyraProfileSettings *profile_settings, GError **error) {
	g_assert(profile_number < PYRA_PROFILE_NUM);
	pyra_profile_settings_finalize(profile_settings, profile_number);
	return pyra_device_write(pyra, (gchar const *)profile_settings, sizeof(PyraProfileSettings), error);
}

PyraProfileSettings *pyra_profile_settings_read(RoccatDevice *pyra, guint profile_number, GError **error) {
	PyraProfileSettings *settings;

	g_assert(profile_number < PYRA_PROFILE_NUM);

	gaminggear_device_lock(GAMINGGEAR_DEVICE(pyra));

	if (!pyra_select(pyra, profile_number, PYRA_CONTROL_REQUEST_PROFILE_SETTINGS, error)) {
		gaminggear_device_unlock(GAMINGGEAR_DEVICE(pyra));
		return NULL;
	}

	settings = (PyraProfileSettings *)pyra_device_read(pyra, PYRA_REPORT_ID_PROFILE_SETTINGS, sizeof(PyraProfileSettings), error);

	gaminggear_device_unlock(GAMINGGEAR_DEVICE(pyra));

	return settings;
}
