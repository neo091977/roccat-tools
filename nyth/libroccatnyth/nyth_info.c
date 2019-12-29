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

#include "nyth_info.h"
#include "nyth_device.h"

NythInfo *nyth_info_read(RoccatDevice *device, GError **error) {
	return (NythInfo *)nyth_device_read(device, NYTH_REPORT_ID_INFO, sizeof(NythInfo), error);
}

static void nyth_info_finalize(NythInfo *info) {
	info->report_id = NYTH_REPORT_ID_INFO;
	info->size = sizeof(NythInfo);
}

gboolean nyth_info_write(RoccatDevice *device, NythInfo *info, GError **error) {
	nyth_info_finalize(info);
	return nyth_device_write(device, (gchar const *)info, sizeof(NythInfo), error);
}

guint nyth_firmware_version_read(RoccatDevice *device, GError **error) {
	NythInfo *info;
	guint result;

	info = nyth_info_read(device, error);
	if (!info)
		return 0;
	result = info->firmware_version;
	g_free(info);
	return result;
}

gboolean nyth_reset(RoccatDevice *device, GError **error) {
	NythInfo info = { 0 };
	info.function = NYTH_INFO_FUNCTION_RESET;
	return nyth_info_write(device, &info, error);
}
