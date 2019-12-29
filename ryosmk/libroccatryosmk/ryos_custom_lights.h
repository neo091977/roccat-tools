#ifndef __ROCCAT_RYOS_CUSTOM_LIGHTS_H__
#define __ROCCAT_RYOS_CUSTOM_LIGHTS_H__

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

#include "ryos_stored_lights.h"

G_BEGIN_DECLS

typedef struct _RyosCustomLights RyosCustomLights;

enum {
	RYOS_CUSTOM_LIGHTS_DATA_LENGTH = 15,
};

struct _RyosCustomLights {
	guint8 report_id; /* RYOS_REPORT_ID_CUSTOM_LIGHTS */
	guint8 size; /* 20 */
	guint8 profile_index; /* FIXME not used in firmware? */
	guint8 data[RYOS_CUSTOM_LIGHTS_DATA_LENGTH]; /* index as in keys_primary */
	guint16 checksum;
} __attribute__ ((packed));

gboolean ryos_custom_lights_write(RoccatDevice *ryos, RyosCustomLights *custom_lights, GError **error);
gboolean ryos_custom_lights_empty(RyosCustomLights const *custom_lights);
void ryos_light_layer_to_custom_lights(RyosLightLayer const *light_layer, RyosCustomLights *custom_lights, gboolean blink_state);

G_END_DECLS

#endif
