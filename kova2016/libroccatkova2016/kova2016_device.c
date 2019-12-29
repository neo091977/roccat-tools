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

#include "kova2016_device.h"
#include "kova2016.h"
#include "roccat.h"
#include "roccat_control.h"
#include "roccat_device_hidraw.h"

static guint const device_ids[4] = {
		USB_DEVICE_ID_ROCCAT_KOVA2016_BLACK,
		USB_DEVICE_ID_ROCCAT_KOVA2016_GRAY,
		USB_DEVICE_ID_ROCCAT_KOVA2016_WHITE,
		0
};

RoccatDevice *kova2016_device_first(void) {
	return roccat_device_first(device_ids);
}

RoccatDeviceScanner *kova2016_device_scanner_new(void) {
	return roccat_device_scanner_new(device_ids);
}

gchar *kova2016_device_read(RoccatDevice *device, guint report_id, gssize length, GError **error) {
	return roccat_device_hidraw_read(device, KOVA2016_INTERFACE_MOUSE, report_id, length, error);
}

static gboolean kova2016_check_write(RoccatDevice *device, GError **error) {
	return roccat_check_write(device, KOVA2016_INTERFACE_MOUSE, KOVA2016_REPORT_ID_CONTROL, 150, 200, error);
}

gboolean kova2016_device_write(RoccatDevice *device, gchar const *buffer, gssize length, GError **error) {
	gboolean retval;

	retval = roccat_device_hidraw_write(device, KOVA2016_INTERFACE_MOUSE, buffer, length, error);
	if (!retval)
		return FALSE;

	return kova2016_check_write(device, error);
}

gboolean kova2016_select(RoccatDevice *device, guint profile_index, Kova2016ControlDataIndex data_index, guint request, GError **error) {
	if (!roccat_select(device, KOVA2016_INTERFACE_MOUSE, KOVA2016_REPORT_ID_CONTROL, data_index | profile_index, request, error))
		return FALSE;
	return kova2016_check_write(device, error);
}
