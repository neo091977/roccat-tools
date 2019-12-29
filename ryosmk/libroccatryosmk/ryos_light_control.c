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

#include "ryos_light_control.h"
#include "ryos_device.h"
#include <string.h>

gboolean ryos_light_control_write(RoccatDevice *ryos, RyosLightControl *light_control, GError **error) {
	light_control->report_id = RYOS_REPORT_ID_LIGHT_CONTROL;
	light_control->size = sizeof(RyosLightControl);

	return ryos_device_write(ryos, (gchar const *)light_control, sizeof(RyosLightControl), error);
}

RyosLightControl *ryos_light_control_read(RoccatDevice *ryos, GError **error) {
	return (RyosLightControl *)ryos_device_read(ryos, RYOS_REPORT_ID_LIGHT_CONTROL, sizeof(RyosLightControl), error);
}

gboolean ryos_light_control_custom(RoccatDevice *ryos, gboolean on, GError **error) {
	RyosLightControl light_control;

	memset(&light_control, 0, sizeof(RyosLightControl));
	light_control.custom = on ? RYOS_LIGHT_CONTROL_CUSTOM : RYOS_LIGHT_CONTROL_STORED;
	return ryos_light_control_write(ryos, &light_control, error);
}
