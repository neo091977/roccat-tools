#ifndef __ROCCAT_KIRO_FIRMWARE_H__
#define __ROCCAT_KIRO_FIRMWARE_H__

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
	// KIRO_FIRMWARE_UPDATE_WAIT_INIT = 120,
	KIRO_FIRMWARE_UPDATE_WAIT_PREPARE = 180,
	KIRO_FIRMWARE_UPDATE_WAIT_INIT_DATA = 2100,
	KIRO_FIRMWARE_UPDATE_WAIT_DATA = 170,
	KIRO_FIRMWARE_UPDATE_FINAL_WRITE_CHECK = TRUE,
	KIRO_FIRMWARE_SIZE = 40944,
};

G_END_DECLS

#endif
