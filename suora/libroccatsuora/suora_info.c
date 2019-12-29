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

#include "suora_info.h"
#include "suora_device.h"

SuoraInfo *suora_info_read(RoccatDevice *device, GError **error) {
	SuoraFeaturePayload const out = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	return (SuoraInfo *)suora_read(device, SUORA_FEATURE_REQUEST_INFO, &out, error);
}

gchar *suora_firmware_version_to_string(SuoraInfo const *info) {
	return g_strdup_printf("%i.%02i", info->version_major, info->version_minor);
}
