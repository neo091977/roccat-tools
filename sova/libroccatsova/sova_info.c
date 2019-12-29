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

#include "sova_info.h"
#include "sova_device.h"

SovaInfo *sova_info_read(RoccatDevice *device, GError **error) {
	return (SovaInfo *)sova_device_read(device, SOVA_REPORT_ID_INFO, sizeof(SovaInfo), error);
}

guint sova_firmware_version_read(RoccatDevice *device, GError **error) {
	SovaInfo *info;
	guint result;

	info = sova_info_read(device, error);
	if (!info)
		return 0;
	result = info->firmware_version;
	g_free(info);
	return result;
}
