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

#include "tyon_device.h"
#include "tyon.h"
#include "roccat.h"
#include "roccat_control.h"
#include "roccat_device_hidraw.h"

static guint const device_ids[3] = { USB_DEVICE_ID_ROCCAT_TYON_BLACK, USB_DEVICE_ID_ROCCAT_TYON_WHITE, 0 };

RoccatDevice *tyon_device_first(void) {
	return roccat_device_first(device_ids);
}

RoccatDeviceScanner *tyon_device_scanner_new(void) {
	return roccat_device_scanner_new(device_ids);
}

gchar *tyon_device_read(RoccatDevice *device, guint report_id, gssize length, GError **error) {
	return roccat_device_hidraw_read(device, TYON_INTERFACE_MOUSE, report_id, length, error);
}

static gboolean tyon_check_write(RoccatDevice *device, GError **error) {
	return roccat_check_write(device, TYON_INTERFACE_MOUSE, TYON_REPORT_ID_CONTROL, 200, 500, error);
}

gboolean tyon_device_write(RoccatDevice *device, gchar const *buffer, gssize length, GError **error) {
	gboolean retval;

	retval = roccat_device_hidraw_write(device, TYON_INTERFACE_MOUSE, buffer, length, error);
	if (!retval)
		return FALSE;

	return tyon_check_write(device, error);
}

gboolean tyon_select(RoccatDevice *device, guint profile_index, TyonControlDataIndex data_index, guint request, GError **error) {
	if (!roccat_select(device, TYON_INTERFACE_MOUSE, TYON_REPORT_ID_CONTROL, data_index | profile_index, request, error))
		return FALSE;
	return tyon_check_write(device, error);
}
