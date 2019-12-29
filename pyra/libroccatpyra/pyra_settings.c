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

PyraSettings *pyra_settings_read(RoccatDevice *pyra, GError **error) {
	return (PyraSettings *)pyra_device_read(pyra, PYRA_REPORT_ID_SETTINGS, sizeof(PyraSettings), error);
}

static void pyra_settings_finalize(PyraSettings *settings) {
	settings->report_id = PYRA_REPORT_ID_SETTINGS;
	settings->size = sizeof(PyraSettings);
}

gboolean pyra_settings_write(RoccatDevice *pyra, PyraSettings *settings, GError **error) {
	pyra_settings_finalize(settings);
	return pyra_device_write(pyra, (gchar const *)settings, sizeof(PyraSettings), error);
}

guint pyra_actual_profile_read(RoccatDevice *pyra, GError **error) {
	guint result;
	PyraSettings *settings;

	settings = pyra_settings_read(pyra, error);
	if (*error)
		return 0;

	result = settings->startup_profile;
	g_free(settings);
	return result;
}

gboolean pyra_actual_profile_write(RoccatDevice *pyra, guint profile_index, GError **error) {
	PyraSettings settings;

	g_assert(profile_index < PYRA_PROFILE_NUM);

	settings.startup_profile = profile_index;

	return pyra_settings_write(pyra, &settings, error);
}
