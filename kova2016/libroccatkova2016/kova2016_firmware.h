#ifndef __ROCCAT_KOVA2016_FIRMWARE_H__
#define __ROCCAT_KOVA2016_FIRMWARE_H__

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

#include "roccat_firmware.h"

G_BEGIN_DECLS

enum {
	// KOVA2016_FIRMWARE_UPDATE_WAIT_INIT = 6800,
	KOVA2016_FIRMWARE_UPDATE_WAIT_PREPARE = 170,
	KOVA2016_FIRMWARE_UPDATE_WAIT_INIT_DATA = 1120,
	KOVA2016_FIRMWARE_UPDATE_WAIT_DATA = 160,
	KOVA2016_FIRMWARE_UPDATE_FINAL_WRITE_CHECK = FALSE,
	KOVA2016_FIRMWARE_SIZE = 24576,
};

G_END_DECLS

#endif
