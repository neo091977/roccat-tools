#ifndef __ROCCAT_H__
#define __ROCCAT_H__

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

#include <glib.h>

G_BEGIN_DECLS

#define USB_VENDOR_ID_ROCCAT 0x1e7d
#define USB_DEVICE_ID_ROCCAT_SOFTWARE 0xffff

#define ROCCAT_VENDOR_NAME "ROCCAT"

/* Partially defined in linux/hid.h
 * Defined here to reduce Linux specific code
 */
#define ROCCAT_USB_INTERFACE_PROTOCOL_NONE 0
#define ROCCAT_USB_INTERFACE_PROTOCOL_KEYBOARD 1
#define ROCCAT_USB_INTERFACE_PROTOCOL_MOUSE 2

typedef enum {
	ROCCAT_INPUT_EVENT_VALUE_PRESS = 1,
	ROCCAT_INPUT_EVENT_VALUE_RELEASE = 0,
} RoccatInputEventValue;

#define ROCCAT_DBUS_SERVER_NAME "org.roccat"
#define ROCCAT_DBUS_SERVER_PATH "/org/roccat"
#define ROCCAT_DBUS_SERVER_INTERFACE "org.roccat"

/* For raw error numbers */
#define ROCCAT_ERRNO_ERROR roccat_errno_error_quark()
GQuark roccat_errno_error_quark(void);

#define ROCCAT_ERROR roccat_error_quark()
GQuark roccat_error_quark(void);

typedef enum {
	ROCCAT_ERROR_MACRO_TOO_LONG,
	ROCCAT_ERROR_INVAL,
} RoccatError;

#define ROCCAT_MOUSE_PROFILE_EXTENSION "rmp"
#define ROCCAT_KEYBOARD_PROFILE_EXTENSION "rkp"

typedef enum {
	ROCCAT_POLLING_RATE_125 = 0,
	ROCCAT_POLLING_RATE_250 = 1,
	ROCCAT_POLLING_RATE_500 = 2,
	ROCCAT_POLLING_RATE_1000 = 3,
} RoccatPollingRate;

typedef enum {
	ROCCAT_SENSITIVITY_MIN = 0x01,
	ROCCAT_SENSITIVITY_CENTER = 0x06,
	ROCCAT_SENSITIVITY_MAX = 0x0b,
} RoccatSensitivity;

static inline guint roccat_sensitivity_from_value_to_bin(gint scale) {
	return (guint)(scale + ROCCAT_SENSITIVITY_CENTER);
}

static inline gint roccat_sensitivity_from_bin_to_value(guint value) {
	return (gint)value - ROCCAT_SENSITIVITY_CENTER;
}

typedef enum {
	ROCCAT_SENSITIVITY_ADVANCED_OFF = 0,
	ROCCAT_SENSITIVITY_ADVANCED_ON = 1,
} RoccatSensitivityAdvanced;

#define ROCCAT_BLUE "#419ed7"
#define ROCCAT_BLUE_RED 0x41
#define ROCCAT_BLUE_GREEN 0x9e
#define ROCCAT_BLUE_BLUE 0xd7

G_END_DECLS

#endif
