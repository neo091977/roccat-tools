#ifndef __ROCCAT_RYOSMKFX_STORED_LIGHTS_H__
#define __ROCCAT_RYOSMKFX_STORED_LIGHTS_H__

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

typedef struct _RyosmkfxStoredLights RyosmkfxStoredLights;

struct _RyosmkfxStoredLights {
	guint8 report_id; /* RYOS_REPORT_ID_STORED_LIGHTS */
	guint8 size; /* 0xef */
	guint8 profile_index;
	RyosmkfxLightLayer light_layer;
	guint8 unknown; // gradient related?
	guint8 unused[5];
	guint16 checksum;
} __attribute__ ((packed));

gboolean ryosmkfx_stored_lights_write(RoccatDevice *device, guint profile_index, RyosmkfxStoredLights *stored_lights, GError **error);
RyosmkfxStoredLights *ryosmkfx_stored_lights_read(RoccatDevice *device, guint profile_index, GError **error);
gboolean ryosmkfx_stored_lights_equal(RyosmkfxStoredLights const *left, RyosmkfxStoredLights const *right);
void ryosmkfx_stored_lights_copy(RyosmkfxStoredLights *destination, RyosmkfxStoredLights const *source);
RyosmkfxStoredLights const *ryosmkfx_stored_lights_default(void);
RyosmkfxLightLayer *ryosmkfx_get_active_light_layer(RoccatDevice *device, GError **error);

G_END_DECLS

#endif
