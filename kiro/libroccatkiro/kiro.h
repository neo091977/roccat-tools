#ifndef __ROCCAT_KIRO_H__
#define __ROCCAT_KIRO_H__

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

#define USB_DEVICE_ID_ROCCAT_KIRO 0x2c38

#define KIRO_DEVICE_NAME "Kiro"

#define KIRO_DBUS_SERVER_PATH "/org/roccat/Kiro"
#define KIRO_DBUS_SERVER_INTERFACE "org.roccat.Kiro"

enum {
	KIRO_PROFILE_NUM = 1,
	KIRO_LIGHTS_NUM = 1,
	KIRO_CPI_MIN = 250,
	KIRO_CPI_MAX = 4000,
	KIRO_CPI_STEP = 50,
	KIRO_BUTTON_NUM = 10,
};

G_END_DECLS

#endif
