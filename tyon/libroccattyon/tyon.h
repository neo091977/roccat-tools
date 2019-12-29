#ifndef __ROCCAT_TYON_H__
#define __ROCCAT_TYON_H__

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

#define USB_DEVICE_ID_ROCCAT_TYON_BLACK 0x2e4a
#define USB_DEVICE_ID_ROCCAT_TYON_WHITE 0x2e4b

#define TYON_DEVICE_NAME "Tyon"
#define TYON_DEVICE_NAME_BLACK "Tyon Black"
#define TYON_DEVICE_NAME_WHITE "Tyon White"
#define TYON_DEVICE_NAME_COMBINED "Tyon Black/White"

enum {
	TYON_PROFILE_NUM = 5,
	TYON_LIGHTS_NUM = 2,
	TYON_CPI_MIN = 200,
	TYON_CPI_MAX = 8200,
	TYON_CPI_STEP = 200,
};

G_END_DECLS

#endif
