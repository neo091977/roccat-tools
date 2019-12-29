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

#include "skeltr_bluetooth.h"

static void skeltr_bluetooth_finalize(SkeltrBluetooth *bluetooth) {
	bluetooth->report_id = SKELTR_REPORT_ID_BLUETOOTH;
}

gboolean skeltr_bluetooth_write(RoccatDevice *device, SkeltrBluetooth *bluetooth, GError **error) {
	skeltr_bluetooth_finalize(bluetooth);
	return skeltr_device_write(device, (gchar const *)bluetooth, sizeof(SkeltrBluetooth), error);
}

SkeltrBluetooth *skeltr_bluetooth_read(RoccatDevice *device, GError **error) {
	SkeltrBluetooth *bluetooth;

	gaminggear_device_lock(GAMINGGEAR_DEVICE(device));

	// FIXME needed?
	if (!skeltr_select(device, 0, SKELTR_CONTROL_REQUEST_BLUETOOTH, error)) {
		gaminggear_device_unlock(GAMINGGEAR_DEVICE(device));
		return NULL;
	}

	bluetooth = (SkeltrBluetooth *)skeltr_device_read(device, SKELTR_REPORT_ID_BLUETOOTH, sizeof(SkeltrBluetooth), error);

	gaminggear_device_unlock(GAMINGGEAR_DEVICE(device));

	return bluetooth;
}
