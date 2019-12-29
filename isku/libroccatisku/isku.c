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

#include "isku.h"
#include "roccat_device_hidraw.h"
#include "roccat.h"
#include "roccat_control.h"

static guint const device_ids[2] = { USB_DEVICE_ID_ROCCAT_ISKU, 0 };

RoccatDevice *isku_device_first(void) {
	return roccat_device_first(device_ids);
}

RoccatDeviceScanner *isku_device_scanner_new(void) {
	return roccat_device_scanner_new(device_ids);
}

gchar *isku_device_read(RoccatDevice *roccat_device, guint report_id, gssize length, GError **error) {
	return roccat_device_hidraw_read(roccat_device, ISKU_INTERFACE_KEYBOARD, report_id, length, error);
}

static gboolean isku_check_write(RoccatDevice *device, GError **error) {
	return roccat_check_write(device, ISKU_INTERFACE_KEYBOARD, ISKU_REPORT_ID_CONTROL, 110, 110, error);
}

gboolean isku_device_write(RoccatDevice *roccat_device, gchar const *buffer, gssize length, GError **error) {
	gboolean retval;

	retval = roccat_device_hidraw_write(roccat_device, ISKU_INTERFACE_KEYBOARD, buffer, length, error);
	if (!retval)
		return FALSE;

	return isku_check_write(roccat_device, error);
}

gboolean isku_select(RoccatDevice *device, guint profile_index, guint request, GError **error) {
	g_assert(profile_index < ISKU_PROFILE_NUM);
	if (!roccat_select(device, ISKU_INTERFACE_KEYBOARD, ISKU_REPORT_ID_CONTROL, profile_index, request, error))
		return FALSE;
	return isku_check_write(device, error);
}

gboolean isku_device_state_write(RoccatDevice *device, guint state, GError **error) {
	IskuDeviceState device_state;

	device_state.report_id = ISKU_REPORT_ID_DEVICE_STATE;
	device_state.size = sizeof(IskuDeviceState);
	device_state.state = state;

	return isku_device_write(device, (gchar const *)&device_state, sizeof(IskuDeviceState), error);
}
