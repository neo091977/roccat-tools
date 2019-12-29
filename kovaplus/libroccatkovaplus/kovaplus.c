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

#include "kovaplus.h"
#include "roccat_device_hidraw.h"
#include "roccat_control.h"
#include "roccat.h"

static guint const device_ids[2] = { USB_DEVICE_ID_ROCCAT_KOVAPLUS, 0 };

RoccatDevice *kovaplus_device_first(void) {
	return roccat_device_first(device_ids);
}

RoccatDeviceScanner *kovaplus_device_scanner_new(void) {
	return roccat_device_scanner_new(device_ids);
}

gchar *kovaplus_device_read(RoccatDevice *roccat_device, guint report_id, gssize length, GError **error) {
	return roccat_device_hidraw_read(roccat_device, KOVAPLUS_INTERFACE_MOUSE, report_id, length, error);
}

static gboolean kovaplus_check_write(RoccatDevice *device, GError **error) {
	return roccat_check_write(device, KOVAPLUS_INTERFACE_MOUSE, KOVAPLUS_REPORT_ID_CONTROL, 110, 110, error);
}

gboolean kovaplus_device_write(RoccatDevice *roccat_device, gchar const *buffer, gssize length, GError **error) {
	gboolean retval;

	retval = roccat_device_hidraw_write(roccat_device, KOVAPLUS_INTERFACE_MOUSE, buffer, length, error);
	if (!retval)
		return FALSE;

	return kovaplus_check_write(roccat_device, error);
}

gboolean kovaplus_select(RoccatDevice *device, guint profile_index, guint request, GError **error) {
	g_assert(profile_index < KOVAPLUS_PROFILE_NUM);
	return roccat_select(device, KOVAPLUS_INTERFACE_MOUSE, KOVAPLUS_REPORT_ID_CONTROL, profile_index, request, error);
}

gboolean kovaplus_device_state_write(RoccatDevice *device, guint state, GError **error) {
	KovaplusDeviceState device_state;

	device_state.report_id = KOVAPLUS_REPORT_ID_DEVICE_STATE;
	device_state.size = sizeof(KovaplusDeviceState);
	device_state.state = state;

	return kovaplus_device_write(device, (gchar const *)&device_state, sizeof(KovaplusDeviceState), error);
}
