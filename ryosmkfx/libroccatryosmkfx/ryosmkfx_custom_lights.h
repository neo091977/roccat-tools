#ifndef __ROCCAT_RYOSMKFX_CUSTOM_LIGHTS_H__
#define __ROCCAT_RYOSMKFX_CUSTOM_LIGHTS_H__

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

#include "ryosmkfx_light_layer.h"
#include "roccat_device.h"

G_BEGIN_DECLS

typedef struct _RyosmkfxCustomLights RyosmkfxCustomLights;

struct _RyosmkfxCustomLights {
	guint8 report_id; /* RYOS_REPORT_ID_CUSTOM_LIGHTS */
	guint8 size; /* 0xe8 */
	RyosmkfxLightLayer light_layer;
	guint16 checksum;
} __attribute__ ((packed));

gboolean ryosmkfx_custom_lights_write(RoccatDevice *device, RyosmkfxCustomLights *custom_lights, GError **error);

G_END_DECLS

#endif
