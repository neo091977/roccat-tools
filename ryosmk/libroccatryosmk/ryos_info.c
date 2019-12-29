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

#include "ryos_info.h"
#include "ryos_device.h"

RyosInfo *ryos_info_read(RoccatDevice *ryos, GError **error) {
	return (RyosInfo *)ryos_device_read(ryos, RYOS_REPORT_ID_INFO, sizeof(RyosInfo), error);
}

guint ryos_firmware_version_read(RoccatDevice *ryos, GError **error) {
	RyosInfo *info;
	guint result;

	info = ryos_info_read(ryos, error);
	if (!info)
		return 0;
	result = info->firmware_version;
	g_free(info);
	return result;
}
