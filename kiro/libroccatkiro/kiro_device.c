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

#include "kiro_device.h"
#include "kiro.h"
#include "roccat.h"
#include "roccat_control.h"
#include "roccat_device_hidraw.h"

static guint const device_ids[2] = { USB_DEVICE_ID_ROCCAT_KIRO, 0 };

RoccatDevice *kiro_device_first(void) {
	return roccat_device_first(device_ids);
}

RoccatDeviceScanner *kiro_device_scanner_new(void) {
	return roccat_device_scanner_new(device_ids);
}

gchar *kiro_device_read(RoccatDevice *device, guint report_id, gssize length, GError **error) {
	return roccat_device_hidraw_read(device, KIRO_INTERFACE_MOUSE, report_id, length, error);
}

static gboolean kiro_check_write(RoccatDevice *device, GError **error) {
	return roccat_check_write(device, KIRO_INTERFACE_MOUSE, KIRO_REPORT_ID_CONTROL, 150, 200, error);
}

gboolean kiro_device_write(RoccatDevice *device, gchar const *buffer, gssize length, GError **error) {
	gboolean retval;

	retval = roccat_device_hidraw_write(device, KIRO_INTERFACE_MOUSE, buffer, length, error);
	if (!retval)
		return FALSE;

	return kiro_check_write(device, error);
}
