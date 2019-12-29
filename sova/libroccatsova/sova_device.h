#ifndef __ROCCAT_SOVA_DEVICE_H__
#define __ROCCAT_SOVA_DEVICE_H__

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

enum {
	SOVA_KEYBOARD_EVENT_SIZE = 8,
};

/*
 * write selection: (before profile specific read)
 *   value = profile index 0-4
 *   request = SovaControlRequest or key index for macros
 */
typedef enum {
	SOVA_CONTROL_REQUEST_KEYS_PRIMARY = 0xa0,
	SOVA_CONTROL_REQUEST_KEYS_FUNCTION = 0xa1,
	SOVA_CONTROL_REQUEST_KEYS_EXTRA = 0xa4,
	SOVA_CONTROL_REQUEST_KEYS_EASYZONE = 0xa5,
	SOVA_CONTROL_REQUEST_KEY_MASK = 0xb0,
	SOVA_CONTROL_REQUEST_LIGHT = 0xb1,
} SovaControlRequest;

typedef enum {
	SOVA_INTERFACE_KEYBOARD = 0,
	SOVA_INTERFACE_MOUSE = 1,
	SOVA_INTERFACE_KEYPAD = 2,
} SovaInterface;

RoccatDevice *sova_device_first(void);
RoccatDeviceScanner *sova_device_scanner_new(void);

gchar *sova_device_read(RoccatDevice *roccat_device, guint report_id, gssize length, GError **error);
gboolean sova_device_write(RoccatDevice *roccat_device, gchar const *buffer, gssize length, GError **error);
gboolean sova_device_write_with_custom_wait(RoccatDevice *roccat_device, gchar const *buffer, gssize length, guint init_wait, GError **error);
gboolean sova_select(RoccatDevice *sova, guint profile_index, guint request, GError **error);

G_END_DECLS

#endif
