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

#include "sova_light_control.h"
#include <string.h>

gboolean sova_light_control_write(RoccatDevice *device, SovaLightControl *light_control, GError **error) {
	light_control->report_id = SOVA_REPORT_ID_LIGHT_CONTROL;
	light_control->size = sizeof(SovaLightControl);

	return sova_device_write(device, (gchar const *)light_control, sizeof(SovaLightControl), error);
}

SovaLightControl *sova_light_control_read(RoccatDevice *device, GError **error) {
	return (SovaLightControl *)sova_device_read(device, SOVA_REPORT_ID_LIGHT_CONTROL, sizeof(SovaLightControl), error);
}

gboolean sova_light_control_custom(RoccatDevice *device, gboolean on, GError **error) {
	SovaLightControl light_control;

	memset(&light_control, 0, sizeof(SovaLightControl));
	light_control.custom = on ? SOVA_LIGHT_CONTROL_CUSTOM : SOVA_LIGHT_CONTROL_STORED;
	return sova_light_control_write(device, &light_control, error);
}
