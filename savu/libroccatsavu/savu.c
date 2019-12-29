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

#include "savu.h"
#include "roccat_device_hidraw.h"
#include "roccat_control.h"
#include "roccat.h"

static guint const device_ids[2] = { USB_DEVICE_ID_ROCCAT_SAVU, 0 };

RoccatDevice *savu_device_first(void) {
	return roccat_device_first(device_ids);
}

RoccatDeviceScanner *savu_device_scanner_new(void) {
	return roccat_device_scanner_new(device_ids);
}

gchar *savu_device_read(RoccatDevice *roccat_device, guint report_id, gssize length, GError **error) {
	return roccat_device_hidraw_read(roccat_device, SAVU_INTERFACE_MOUSE, report_id, length, error);
}

static gboolean savu_check_write(RoccatDevice *device, GError **error) {
	return roccat_check_write(device, SAVU_INTERFACE_MOUSE, SAVU_REPORT_ID_CONTROL, 300, 300, error);
}

gboolean savu_device_write(RoccatDevice *roccat_device, gchar const *buffer, gssize length, GError **error) {
	gboolean retval;

	retval = roccat_device_hidraw_write(roccat_device, SAVU_INTERFACE_MOUSE, buffer, length, error);
	if (!retval)
		return FALSE;

	return savu_check_write(roccat_device, error);
}

gboolean savu_select(RoccatDevice *device, guint profile_index, guint request, GError **error) {
	g_assert(profile_index < SAVU_PROFILE_NUM);
	if (!roccat_select(device, SAVU_INTERFACE_MOUSE, SAVU_REPORT_ID_CONTROL, profile_index, request, error))
		return FALSE;
	return savu_check_write(device, error);
}

gboolean savu_device_state_write(RoccatDevice *savu, guint state, GError **error) {
	SavuDeviceState device_state;

	device_state.report_id = SAVU_REPORT_ID_DEVICE_STATE;
	device_state.size = sizeof(SavuDeviceState);
	device_state.state = state;

	return savu_device_write(savu, (gchar const *)&device_state, sizeof(SavuDeviceState), error);
}
