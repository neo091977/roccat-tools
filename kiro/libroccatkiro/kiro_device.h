#ifndef __ROCCAT_KIRO_DEVICE_H__
#define __ROCCAT_KIRO_DEVICE_H__

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

#include "roccat_device_scanner.h"

G_BEGIN_DECLS

typedef enum {
	KIRO_CONTROL_REQUEST_CHECK = 0x00,
} KiroControlRequest;

typedef enum {
	KIRO_REPORT_ID_MOUSE = 0x01, /* 8 bytes */
	KIRO_REPORT_ID_MULTIMEDIA = 0x02, /* 3 bytes */
	KIRO_REPORT_ID_SPECIAL = 0x03, /* 5 bytes */
	KIRO_REPORT_ID_CONTROL = 0x04, /* 3 bytes */
	KIRO_REPORT_ID_PROFILE = 0x06, /* 65 bytes */
	KIRO_REPORT_ID_INFO = 0x09, /* 7 bytes */
	KIRO_REPORT_ID_DEVICE_STATE = 0x0e, /* 3 bytes */
	KIRO_REPORT_ID_FX = 0x10, /* 13 bytes */
	KIRO_REPORT_ID_FIRMWARE_WRITE = 0x1b, /* 1027 bytes */
	KIRO_REPORT_ID_FIRMWARE_WRITE_CONTROL = 0x1c, /* 3 bytes */
	KIRO_REPORT_ID_DCU = 0x20, /* 4 bytes */
} KiroReportIds;

typedef enum {
	KIRO_INTERFACE_MOUSE = 0,
	KIRO_INTERFACE_KEYBOARD = 1,
} KiroInterface;

RoccatDevice *kiro_device_first(void);
RoccatDeviceScanner *kiro_device_scanner_new(void);

gchar *kiro_device_read(RoccatDevice *device, guint report_id, gssize length, GError **error);
gboolean kiro_device_write(RoccatDevice *device, gchar const *buffer, gssize length, GError **error);

G_END_DECLS

#endif
