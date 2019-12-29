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

#include "skeltr_light_custom.h"

static void skeltr_light_custom_finalize(SkeltrLightCustom *custom) {
	custom->report_id = SKELTR_REPORT_ID_LIGHT_CUSTOM;
	custom->size = sizeof(SkeltrLightCustom);
}

gboolean skeltr_light_custom_write(RoccatDevice *device, SkeltrLightCustom *custom, GError **error) {
	skeltr_light_custom_finalize(custom);
	return skeltr_device_write(device, (gchar const *)custom, sizeof(SkeltrLightCustom), error);
}
