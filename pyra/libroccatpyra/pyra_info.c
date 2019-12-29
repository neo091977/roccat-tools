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

static void pyra_info_finalize(PyraInfo *info) {
	info->report_id = PYRA_REPORT_ID_INFO;
	info->size = sizeof(PyraInfo);
}

static gboolean pyra_info_write(RoccatDevice *pyra, PyraInfo *info, GError **error) {
	pyra_info_finalize(info);
	return pyra_device_write(pyra, (gchar const *)info, sizeof(PyraInfo), error);
}

gboolean pyra_reset(RoccatDevice *pyra, GError **error) {
	PyraInfo info = { 0 };
	info.firmware_version = PYRA_INFO_FUNCTION_RESET;
	return pyra_info_write(pyra, &info, error);
}

guint pyra_firmware_version_read(RoccatDevice *pyra, GError **error) {
	PyraInfo *info;
	guint result;

	info = pyra_info_read(pyra, error);
	if (info) {
		result = info->firmware_version;
		g_free(info);
		return result;
	}

	return 0;
}

PyraInfo *pyra_info_read(RoccatDevice *pyra, GError **error) {
	return (PyraInfo *)pyra_device_read(pyra, PYRA_REPORT_ID_INFO, sizeof(PyraInfo), error);
}
