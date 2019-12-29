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

#include "arvo.h"
#include "roccat_device_hidraw.h"
#include "roccat.h"
#include "roccat_control.h"

static guint const device_ids[2] = { USB_DEVICE_ID_ROCCAT_ARVO, 0 };

RoccatDevice *arvo_device_first(void) {
	return roccat_device_first(device_ids);
}

RoccatDeviceScanner *arvo_device_scanner_new(void) {
	return roccat_device_scanner_new(device_ids);
}

gchar *arvo_device_read(RoccatDevice *roccat_device, guint report_id, gssize length, GError **error) {
	return roccat_device_hidraw_read(roccat_device, ARVO_INTERFACE_KEYBOARD, report_id, length, error);
}

gboolean arvo_device_write(RoccatDevice *roccat_device, gchar const *buffer, gssize length, GError **error) {
	return roccat_device_hidraw_write(roccat_device, ARVO_INTERFACE_KEYBOARD, buffer, length, error);
}
