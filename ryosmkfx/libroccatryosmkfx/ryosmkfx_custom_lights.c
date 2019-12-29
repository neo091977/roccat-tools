/*
 * This file is part of roccat-tools.
 *
 * roccat-tools is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Macro Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * roccat-tools is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Macro Public License for more details.
 *
 * You should have received a copy of the GNU Macro Public License
 * along with roccat-tools. If not, see <http://www.gnu.org/licenses/>.
 */

#include "ryosmkfx_custom_lights.h"
#include "ryosmkfx_device.h"
#include "ryosmkfx_light_control.h"
#include "ryosmkfx.h"
#include "roccat_device_hidraw.h"
#include "roccat_helper.h"

static guint16 ryosmkfx_custom_lights_calc_checksum(RyosmkfxCustomLights const *custom_lights) {
	return ROCCAT_BYTESUM_PARTIALLY(custom_lights, RyosmkfxCustomLights, report_id, checksum);
}

static void ryosmkfx_custom_lights_set_checksum(RyosmkfxCustomLights *custom_lights, guint16 new_value) {
	custom_lights->checksum = GUINT16_TO_LE(new_value);
}

static void ryosmkfx_custom_lights_finalize(RyosmkfxCustomLights *custom_lights) {
	custom_lights->report_id = RYOS_REPORT_ID_CUSTOM_LIGHTS;
	custom_lights->size = sizeof(RyosmkfxCustomLights);
	ryosmkfx_custom_lights_set_checksum(custom_lights, ryosmkfx_custom_lights_calc_checksum(custom_lights));
}

gboolean ryosmkfx_custom_lights_write(RoccatDevice *device, RyosmkfxCustomLights *custom_lights, GError **error) {
	gboolean retval;

	ryosmkfx_custom_lights_finalize(custom_lights);

	retval = roccat_device_hidraw_write(device, RYOS_INTERFACE_KEYBOARD, (gchar const *)custom_lights, sizeof(RyosmkfxCustomLights), error);
	if (!retval)
		return FALSE;

	return ryosmkfx_light_control_check_write(device, error);
}
