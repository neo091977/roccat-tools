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

#include "ryos_device.h"
#include "ryos.h"
#include "roccat.h"
#include "roccat_control.h"
#include "roccat_device_hidraw.h"

static guint const device_ids[4] = { USB_DEVICE_ID_ROCCAT_RYOS_MK,
	USB_DEVICE_ID_ROCCAT_RYOS_MK_GLOW,
	USB_DEVICE_ID_ROCCAT_RYOS_MK_PRO,
	0 };

RoccatDevice *ryos_device_first(void) {
	return roccat_device_first(device_ids);
}

RoccatDeviceScanner *ryos_device_scanner_new(void) {
	return roccat_device_scanner_new(device_ids);
}

gchar *ryos_device_read(RoccatDevice *roccat_device, guint report_id, gssize length, GError **error) {
	return roccat_device_hidraw_read(roccat_device, RYOS_INTERFACE_KEYBOARD, report_id, length, error);
}

static gboolean ryos_check_write(RoccatDevice *ryos, guint init_wait, GError **error) {
	return roccat_check_write(ryos, RYOS_INTERFACE_KEYBOARD, RYOS_REPORT_ID_CONTROL, init_wait, 10, error);
}

gboolean ryos_device_write_with_custom_wait(RoccatDevice *roccat_device, gchar const *buffer, gssize length, guint init_wait, GError **error) {
	gboolean retval;

	retval = roccat_device_hidraw_write(roccat_device, RYOS_INTERFACE_KEYBOARD, buffer, length, error);
	if (!retval)
		return FALSE;

	return ryos_check_write(roccat_device, init_wait, error);
}

gboolean ryos_device_write(RoccatDevice *roccat_device, gchar const *buffer, gssize length, GError **error) {
	return ryos_device_write_with_custom_wait(roccat_device, buffer, length, 150, error);
}

gboolean ryos_select(RoccatDevice *ryos, guint profile_index, guint request, GError **error) {
	gboolean retval;

	if (!roccat_select(ryos, RYOS_INTERFACE_KEYBOARD, RYOS_REPORT_ID_CONTROL, profile_index, request, error))
		return FALSE;

	retval = ryos_check_write(ryos, 150, error);

	/* FIXME Ignoring inapropriate INVALID or CRITICAL answers as long
	 * as firmware requires it
	 */
	if ((request == RYOS_CONTROL_REQUEST_KEYS_EXTRA || request == RYOS_CONTROL_REQUEST_LIGHT_MACRO) && retval == FALSE) {
		g_clear_error(error);
		return TRUE;
	}

	return retval;
}
