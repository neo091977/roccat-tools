#ifndef __ROCCAT_RYOSTKL_DEVICE_H__
#define __ROCCAT_RYOSTKL_DEVICE_H__

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
#include "ryos_device.h"

G_BEGIN_DECLS

typedef enum {
	/*
	 * Have to be anded with RyosStoredLightsLayers
	 */
	RYOSTKL_CONTROL_REQUEST_STORED_LIGHTS_AUTOMATIC_BASE = 0xc0,
	RYOSTKL_CONTROL_REQUEST_STORED_LIGHTS_MANUAL_BASE = 0xd0,
} RyostklControlRequest;

typedef enum {
	RYOSTKL_INTERFACE_KEYBOARD = 0,
	RYOSTKL_INTERFACE_MOUSE = 1,
	RYOSTKL_INTERFACE_MISC = 2,
} RyostklInterface;

RoccatDevice *ryostkl_device_first(void);
RoccatDeviceScanner *ryostkl_device_scanner_new(void);

G_END_DECLS

#endif
