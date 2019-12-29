#ifndef __ROCCAT_SKELTR_DEVICE_H__
#define __ROCCAT_SKELTR_DEVICE_H__

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
	SKELTR_CONTROL_REQUEST_KEYS_PRIMARY = 0x40,
	SKELTR_CONTROL_REQUEST_KEY_MASK = 0x50,
	SKELTR_CONTROL_REQUEST_KEYS_FUNCTION = 0x60,
	SKELTR_CONTROL_REQUEST_LIGHT = 0x70,
	SKELTR_CONTROL_REQUEST_KEYS_EXTRA = 0x90,
	SKELTR_CONTROL_REQUEST_KEYS_MACRO = 0xa0,
	SKELTR_CONTROL_REQUEST_KEYS_EASYZONE = 0xb0,
	SKELTR_CONTROL_REQUEST_BLUETOOTH = 0xc0, /* FIXME only profile index 0? */
	SKELTR_CONTROL_REQUEST_KEYS_THUMBSTER = 0xd0,
} SkeltrControlRequest;

typedef enum {
	SKELTR_INTERFACE_KEYBOARD = 0,
	SKELTR_INTERFACE_MOUSE = 1,
	SKELTR_INTERFACE_KEYPAD = 2,
} SkeltrInterface;

enum {
	SKELTR_KEYBOARD_REPORT_SIZE = 8,
};

typedef enum {
	SKELTR_REPORT_ID_SPECIAL = 0x03,
	SKELTR_REPORT_ID_CONTROL = 0x04,
	SKELTR_REPORT_ID_PROFILE = 0x05,
	SKELTR_REPORT_ID_KEYS_PRIMARY = 0x06,
	SKELTR_REPORT_ID_KEY_MASK = 0x07,
	SKELTR_REPORT_ID_KEYS_FUNCTION = 0x08,
	SKELTR_REPORT_ID_KEYS_EASYZONE = 0x09,
	SKELTR_REPORT_ID_KEYS_EXTRA = 0x0a,
	SKELTR_REPORT_ID_KEYS_THUMBSTER = 0x0b,
	SKELTR_REPORT_ID_KEYS_MACRO = 0x0d,
	SKELTR_REPORT_ID_MACRO = 0x0e,
	SKELTR_REPORT_ID_INFO = 0x0f,
	SKELTR_REPORT_ID_LIGHT = 0x10,
	SKELTR_REPORT_ID_RESET = 0x11,
	SKELTR_REPORT_ID_LIGHT_CONTROL = 0x13,
	SKELTR_REPORT_ID_TALK = 0x16, // FIXME confirm
	SKELTR_REPORT_ID_BLUETOOTH = 0x17,
	SKELTR_REPORT_ID_LIGHT_CUSTOM = 0x18,
} SkeltrReportId;

RoccatDevice *skeltr_device_first(void);
RoccatDeviceScanner *skeltr_device_scanner_new(void);

gchar *skeltr_device_read(RoccatDevice *roccat_device, guint report_id, gssize length, GError **error);
gboolean skeltr_device_write(RoccatDevice *roccat_device, gchar const *buffer, gssize length, GError **error);
gboolean skeltr_device_write_with_custom_wait(RoccatDevice *roccat_device, gchar const *buffer, gssize length, guint init_wait, GError **error);
gboolean skeltr_select(RoccatDevice *skeltr, guint profile_index, guint request, GError **error);

G_END_DECLS

#endif
