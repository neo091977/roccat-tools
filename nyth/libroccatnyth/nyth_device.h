#ifndef __ROCCAT_NYTH_DEVICE_H__
#define __ROCCAT_NYTH_DEVICE_H__

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
	NYTH_CONTROL_REQUEST_CHECK = 0x00,
	/* button indexes for requesting macros instead of 0 */
	NYTH_CONTROL_REQUEST_PROFILE_SETTINGS = 0x80,
	NYTH_CONTROL_REQUEST_PROFILE_BUTTONS = 0x90,
	NYTH_CONTROL_REQUEST_BUTTON_SET_PRESET = 0xa0,
} NythControlRequest;

typedef enum {
	NYTH_CONTROL_DATA_INDEX_NONE = 0x00,
	NYTH_CONTROL_DATA_INDEX_MACRO_1 = 0x10,
	NYTH_CONTROL_DATA_INDEX_MACRO_2 = 0x20,
} NythControlDataIndex;

typedef enum {
	NYTH_REPORT_ID_MOUSE = 0x01, /* 6 bytes */
	NYTH_REPORT_ID_2 = 0x02, /* 3 bytes */
	NYTH_REPORT_ID_SPECIAL = 0x03, /* 5 bytes */
	NYTH_REPORT_ID_CONTROL = 0x04, /* 3 bytes */
	NYTH_REPORT_ID_PROFILE = 0x05, /* 4 bytes */
	NYTH_REPORT_ID_PROFILE_SETTINGS = 0x06, /* 33 bytes */
	NYTH_REPORT_ID_PROFILE_BUTTONS = 0x07, /* 123 bytes */
	NYTH_REPORT_ID_MACRO = 0x08, /* 1026 bytes */
	NYTH_REPORT_ID_INFO = 0x09, /* 6 bytes */
	NYTH_REPORT_ID_BUTTON_SET = 0x0a, /* 30 bytes */
	NYTH_REPORT_ID_12 = 0x0c, /* 6 bytes */
	NYTH_REPORT_ID_SROM_WRITE = 0x0d, /* 1028 bytes */
	NYTH_REPORT_ID_DEVICE_STATE = 0x0e, /* 3 bytes */
	NYTH_REPORT_ID_CONTROL_UNIT = 0x0f, /* 6 bytes */
	NYTH_REPORT_ID_TALK = 0x10, /* 16 bytes */
	NYTH_REPORT_ID_26 = 0x1a, /* 1029 bytes */
	NYTH_REPORT_ID_FIRMWARE_WRITE = 0x1b, /* 1026 bytes */
	NYTH_REPORT_ID_FIRMWARE_WRITE_CONTROL = 0x1c, /* 3 bytes */
	NYTH_REPORT_ID_SYSTEM = 0x20, /* 2 bytes */
} NythReportIds;

typedef enum {
	NYTH_INTERFACE_MOUSE = 0,
	NYTH_INTERFACE_KEYBOARD = 1,
} NythInterface;

RoccatDevice *nyth_device_first(void);
RoccatDeviceScanner *nyth_device_scanner_new(void);

gchar *nyth_device_read(RoccatDevice *device, guint report_id, gssize length, GError **error);
gboolean nyth_device_write(RoccatDevice *device, gchar const *buffer, gssize length, GError **error);
gboolean nyth_select(RoccatDevice *device, guint profile_index, NythControlDataIndex data_index, guint request, GError **error);

G_END_DECLS

#endif
