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

static guint16 pyra_profile_buttons_calc_checksum(PyraProfileButtons const *profile_buttons) {
	return ROCCAT_BYTESUM_PARTIALLY(profile_buttons, PyraProfileButtons, report_id, checksum);
}

static void pyra_profile_buttons_set_checksum(PyraProfileButtons *profile_buttons) {
	guint16 checksum = pyra_profile_buttons_calc_checksum(profile_buttons);
	profile_buttons->checksum = GUINT16_TO_LE(checksum);
}

void pyra_profile_buttons_finalize(PyraProfileButtons *profile_buttons, guint profile_number) {
	g_assert(profile_number < PYRA_PROFILE_NUM);
	profile_buttons->report_id = PYRA_REPORT_ID_PROFILE_BUTTONS;
	profile_buttons->number = profile_number;
	pyra_profile_buttons_set_checksum(profile_buttons);
}

gboolean pyra_profile_buttons_write(RoccatDevice *pyra, guint profile_number, PyraProfileButtons *profile_buttons, GError **error) {
	g_assert(profile_number < PYRA_PROFILE_NUM);
	pyra_profile_buttons_finalize(profile_buttons, profile_number);
	return pyra_device_write(pyra, (gchar const *)profile_buttons, sizeof(PyraProfileButtons), error);
}

PyraProfileButtons *pyra_profile_buttons_read(RoccatDevice *pyra, guint profile_number, GError **error) {
	PyraProfileButtons *buttons;

	g_assert(profile_number < PYRA_PROFILE_NUM);

	gaminggear_device_lock(GAMINGGEAR_DEVICE(pyra));

	if (!pyra_select(pyra, profile_number, PYRA_CONTROL_REQUEST_PROFILE_BUTTONS, error)) {
		gaminggear_device_unlock(GAMINGGEAR_DEVICE(pyra));
		return NULL;
	}

	buttons = (PyraProfileButtons *)pyra_device_read(pyra, PYRA_REPORT_ID_PROFILE_BUTTONS, sizeof(PyraProfileButtons), error);

	gaminggear_device_unlock(GAMINGGEAR_DEVICE(pyra));

	return buttons;
}
