#ifndef __ROCCAT_TYON_FIRMWARE_H__
#define __ROCCAT_TYON_FIRMWARE_H__

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
	// TYON_FIRMWARE_UPDATE_WAIT_INIT = 5100,
	TYON_FIRMWARE_UPDATE_WAIT_PREPARE = 3600,
	TYON_FIRMWARE_UPDATE_WAIT_INIT_DATA = 6100,
	TYON_FIRMWARE_UPDATE_WAIT_DATA = 400,
	TYON_FIRMWARE_UPDATE_FINAL_WRITE_CHECK = FALSE,
	TYON_FIRMWARE_SIZE = ROCCAT_FIRMWARE_SIZE_VARIABLE,
};

G_END_DECLS

#endif
