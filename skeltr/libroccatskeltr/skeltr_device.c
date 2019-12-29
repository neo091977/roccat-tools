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

#include "skeltr_device.h"
#include "skeltr.h"
#include "roccat.h"
#include "roccat_control.h"
#include "roccat_device_hidraw.h"

static guint const device_ids[2] = { USB_DEVICE_ID_ROCCAT_SKELTR, 0 };

RoccatDevice *skeltr_device_first(void) {
	return roccat_device_first(device_ids);
}

RoccatDeviceScanner *skeltr_device_scanner_new(void) {
	return roccat_device_scanner_new(device_ids);
}

gchar *skeltr_device_read(RoccatDevice *roccat_device, guint report_id, gssize length, GError **error) {
	return roccat_device_hidraw_read(roccat_device, SKELTR_INTERFACE_KEYBOARD, report_id, length, error);
}

static gboolean skeltr_check_write(RoccatDevice *skeltr, guint init_wait, GError **error) {
	return roccat_check_write(skeltr, SKELTR_INTERFACE_KEYBOARD, SKELTR_REPORT_ID_CONTROL, init_wait, 10, error);
}

gboolean skeltr_device_write_with_custom_wait(RoccatDevice *roccat_device, gchar const *buffer, gssize length, guint init_wait, GError **error) {
	gboolean retval;

	retval = roccat_device_hidraw_write(roccat_device, SKELTR_INTERFACE_KEYBOARD, buffer, length, error);
	if (!retval)
		return FALSE;

	return skeltr_check_write(roccat_device, init_wait, error);
}

gboolean skeltr_device_write(RoccatDevice *roccat_device, gchar const *buffer, gssize length, GError **error) {
	return skeltr_device_write_with_custom_wait(roccat_device, buffer, length, 160, error);
}

gboolean skeltr_select(RoccatDevice *skeltr, guint profile_index, guint request, GError **error) {
	if (!roccat_select(skeltr, SKELTR_INTERFACE_KEYBOARD, SKELTR_REPORT_ID_CONTROL, profile_index, request, error))
		return FALSE;

	return skeltr_check_write(skeltr, 160, error);
}
