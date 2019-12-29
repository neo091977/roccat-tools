#ifndef __ROCCAT_KOVA2016_H__
#define __ROCCAT_KOVA2016_H__

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

#define USB_DEVICE_ID_ROCCAT_KOVA2016_GRAY 0x2cee
#define USB_DEVICE_ID_ROCCAT_KOVA2016_WHITE 0x2cef
#define USB_DEVICE_ID_ROCCAT_KOVA2016_BLACK 0x2cf0

#define KOVA2016_DEVICE_NAME "Kova2016"
#define KOVA2016_DEVICE_NAME_BLACK "Kova 2016 Black"
#define KOVA2016_DEVICE_NAME_GRAY "Kova 2016 Gray"
#define KOVA2016_DEVICE_NAME_WHITE "Kova 2016 White"
#define KOVA2016_DEVICE_NAME_COMBINED "Kova 2016 Black/Gray/White"

#define KOVA2016_DBUS_SERVER_PATH "/org/roccat/Kova2016"
#define KOVA2016_DBUS_SERVER_INTERFACE "org.roccat.Kova2016"

enum {
	KOVA2016_PROFILE_NUM = 5,
	KOVA2016_LIGHTS_NUM = 2,
	KOVA2016_CPI_MIN = 250,
	KOVA2016_CPI_MAX = 7000,
	KOVA2016_CPI_STEP = 50,
	KOVA2016_PHYSICAL_BUTTON_NUM = 12,
	KOVA2016_PROFILE_BUTTON_NUM = KOVA2016_PHYSICAL_BUTTON_NUM * 2,
};

G_END_DECLS

#endif
