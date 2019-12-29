#ifndef __ROCCAT_RYOSMKFX_LIGHT_CONTROL_H__
#define __ROCCAT_RYOSMKFX_LIGHT_CONTROL_H__

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

#include "ryos_light_control.h"

G_BEGIN_DECLS

typedef enum {
	RYOSMKFX_LIGHT_CONTROL_WRITE_CHECK_OK = 1,
	RYOSMKFX_LIGHT_CONTROL_WRITE_CHECK_INVALID = 2,
	RYOSMKFX_LIGHT_CONTROL_WRITE_CHECK_BUSY = 3,
} RyosmkfxLightControlWriteCheck;

gboolean ryosmkfx_light_control_check_write(RoccatDevice *device, GError **error);

G_END_DECLS

#endif
