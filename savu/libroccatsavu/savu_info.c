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

#include "savu.h"

SavuInfo *savu_info_read(RoccatDevice *savu, GError **error) {
	return (SavuInfo *)savu_device_read(savu, SAVU_REPORT_ID_INFO, sizeof(SavuInfo), error);
}

static void savu_info_finalize(SavuInfo *info) {
	info->report_id = SAVU_REPORT_ID_INFO;
	info->size = sizeof(SavuInfo);
}

static gboolean savu_info_write(RoccatDevice *savu, SavuInfo *info, GError **error) {
	savu_info_finalize(info);
	return savu_device_write(savu, (gchar const *)info, sizeof(SavuInfo), error);
}

gboolean savu_reset(RoccatDevice *savu, GError **error) {
	SavuInfo info = { 0 };
	info.firmware_version = SAVU_INFO_FUNCTION_RESET;
	return savu_info_write(savu, &info, error);
}

guint savu_firmware_version_read(RoccatDevice *savu, GError **error) {
	SavuInfo *info;
	guint result;

	info = savu_info_read(savu, error);
	if (!info)
		return 0;
	result = info->firmware_version;
	g_free(info);
	return result;
}
