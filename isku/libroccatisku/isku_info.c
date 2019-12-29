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

#include "isku.h"

IskuInfo *isku_info_read(RoccatDevice *isku, GError **error) {
	return (IskuInfo *)isku_device_read(isku, ISKU_REPORT_ID_INFO, sizeof(IskuInfo), error);
}

guint isku_firmware_version_read(RoccatDevice *isku, GError **error) {
	IskuInfo *info;
	guint result;

	info = isku_info_read(isku, error);
	if (!info)
		return 0;
	result = info->firmware_version;
	g_free(info);
	return result;
}
