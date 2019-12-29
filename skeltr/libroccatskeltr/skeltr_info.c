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

#include "skeltr_info.h"
#include "skeltr_device.h"

SkeltrInfo *skeltr_info_read(RoccatDevice *device, GError **error) {
	return (SkeltrInfo *)skeltr_device_read(device, SKELTR_REPORT_ID_INFO, sizeof(SkeltrInfo), error);
}

guint skeltr_firmware_version_read(RoccatDevice *device, GError **error) {
	SkeltrInfo *info;
	guint result;

	info = skeltr_info_read(device, error);
	if (!info)
		return 0;
	result = info->firmware_version;
	g_free(info);
	return result;
}
