#ifndef __ROCCAT_RYOSTKL_H__
#define __ROCCAT_RYOSTKL_H__

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

#include "ryos.h"

G_BEGIN_DECLS

#define USB_DEVICE_ID_ROCCAT_RYOS_TKL 0x3106
#define USB_DEVICE_ID_ROCCAT_RYOS_TKL_PRO 0x316a

#define RYOSTKL_DEVICE_NAME "Ryos TKL"
#define RYOSTKL_PRO_DEVICE_NAME "Ryos TKL Pro"
#define RYOSTKL_DEVICE_NAME_COMBINED "Ryos TKL/Pro"

enum {
	RYOSTKL_PRO_FIRMWARE_SIZE = 52768,
	RYOSTKL_PRO_LIGHT_FIRMWARE_SIZE = 12288,
	RYOSTKL_PRO_LIGHT_FIRMWARE_NUMBER = 0x80,
};

enum {
	// RYOSTKL_FIRMWARE_UPDATE_WAIT_INIT = 180,
	RYOSTKL_FIRMWARE_UPDATE_WAIT_PREPARE = 180,
	RYOSTKL_FIRMWARE_UPDATE_WAIT_INIT_DATA = 2130,
	RYOSTKL_FIRMWARE_UPDATE_WAIT_DATA = 170,
	RYOSTKL_FIRMWARE_UPDATE_FINAL_WRITE_CHECK = TRUE,
};

G_END_DECLS

#endif
