#ifndef __ROCCAT_SUORA_H__
#define __ROCCAT_SUORA_H__

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

#define USB_DEVICE_ID_ROCCAT_SUORA 0x2fa8

#define SUORA_DEVICE_NAME "Suora"
#define SUORA_DEVICE_NAME_COMBINED "Suora"

enum {
	SUORA_KEYS_NUM = 160,
	SUORA_KEYBOARD_EVENT_SIZE = 32,
};

G_END_DECLS

#endif
