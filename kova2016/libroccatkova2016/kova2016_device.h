#ifndef __ROCCAT_KOVA2016_DEVICE_H__
#define __ROCCAT_KOVA2016_DEVICE_H__

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
	KOVA2016_CONTROL_REQUEST_CHECK = 0x00,
	/* button indexes for requesting macros instead of 0 */
	KOVA2016_CONTROL_REQUEST_PROFILE_SETTINGS = 0x80,
	KOVA2016_CONTROL_REQUEST_PROFILE_BUTTONS = 0x90,
} Kova2016ControlRequest;

typedef enum {
	KOVA2016_CONTROL_DATA_INDEX_NONE = 0x00,
	KOVA2016_CONTROL_DATA_INDEX_MACRO_1 = 0x10,
	KOVA2016_CONTROL_DATA_INDEX_MACRO_2 = 0x20,
} Kova2016ControlDataIndex;

typedef enum {
	KOVA2016_REPORT_ID_MOUSE = 0x01, /* ? bytes */
	KOVA2016_REPORT_ID_2 = 0x02, /* ? bytes */
	KOVA2016_REPORT_ID_SPECIAL = 0x03, /* ? bytes */
	KOVA2016_REPORT_ID_CONTROL = 0x04, /* 3 bytes */
	KOVA2016_REPORT_ID_PROFILE = 0x05, /* 4 bytes */
	KOVA2016_REPORT_ID_PROFILE_SETTINGS = 0x06, /* 28 bytes */
	KOVA2016_REPORT_ID_PROFILE_BUTTONS = 0x07, /* 75 bytes */
	KOVA2016_REPORT_ID_MACRO = 0x08, /* 1026 bytes */
	KOVA2016_REPORT_ID_INFO = 0x09, /* 8 bytes */
	KOVA2016_REPORT_ID_A = 0x0a, /* 8 bytes */
	KOVA2016_REPORT_ID_SENSOR = 0x0c, /* 4 bytes */
	KOVA2016_REPORT_ID_SROM_WRITE = 0x0d, /* 1028 bytes */
	KOVA2016_REPORT_ID_DEVICE_STATE = 0x0e, /* 3 bytes */
	KOVA2016_REPORT_ID_CONTROL_UNIT = 0x0f, /* 6 bytes */
	KOVA2016_REPORT_ID_TALK = 0x10, /* 16 bytes */
	KOVA2016_REPORT_ID_26 = 0x1a, /* 1029 bytes */
	KOVA2016_REPORT_ID_FIRMWARE_WRITE = 0x1b, /* 1026 bytes */
	KOVA2016_REPORT_ID_FIRMWARE_WRITE_CONTROL = 0x1c, /* 3 bytes */
	KOVA2016_REPORT_ID_SYSTEM = 0x20, /* ? bytes */
} Kova2016ReportIds;

typedef enum {
	KOVA2016_INTERFACE_MOUSE = 0,
	KOVA2016_INTERFACE_KEYBOARD = 1,
} Kova2016Interface;

RoccatDevice *kova2016_device_first(void);
RoccatDeviceScanner *kova2016_device_scanner_new(void);

gchar *kova2016_device_read(RoccatDevice *device, guint report_id, gssize length, GError **error);
gboolean kova2016_device_write(RoccatDevice *device, gchar const *buffer, gssize length, GError **error);
gboolean kova2016_select(RoccatDevice *device, guint profile_index, Kova2016ControlDataIndex data_index, guint request, GError **error);

G_END_DECLS

#endif
