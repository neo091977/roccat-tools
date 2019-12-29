#ifndef __ROCCAT_NYTH_H__
#define __ROCCAT_NYTH_H__

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

#define USB_DEVICE_ID_ROCCAT_NYTH_BLACK 0x2e7c
#define USB_DEVICE_ID_ROCCAT_NYTH_WHITE 0x2e7d

#define NYTH_DEVICE_NAME "Nyth"
#define NYTH_DEVICE_NAME_BLACK "Nyth Black"
#define NYTH_DEVICE_NAME_WHITE "Nyth White"
#define NYTH_DEVICE_NAME_COMBINED "Nyth Black/White"

#define NYTH_DBUS_SERVER_PATH "/org/roccat/Nyth"
#define NYTH_DBUS_SERVER_INTERFACE "org.roccat.Nyth"

enum {
	NYTH_PROFILE_NUM = 5,
	NYTH_LIGHTS_NUM = 2,
	NYTH_CPI_MIN = 200,
	NYTH_CPI_MAX = 12000,
	NYTH_CPI_STEP = 10,
	NYTH_PHYSICAL_BUTTON_NUM = 20,
	NYTH_PROFILE_BUTTON_NUM = NYTH_PHYSICAL_BUTTON_NUM * 2,
};

G_END_DECLS

#endif
