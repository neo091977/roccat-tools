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

#include "tyon_info.h"
#include "tyon_device.h"

TyonInfo *tyon_info_read(RoccatDevice *device, GError **error) {
	return (TyonInfo *)tyon_device_read(device, TYON_REPORT_ID_INFO, sizeof(TyonInfo), error);
}

static void tyon_info_finalize(TyonInfo *info) {
	info->report_id = TYON_REPORT_ID_INFO;
	info->size = sizeof(TyonInfo);
}

gboolean tyon_info_write(RoccatDevice *device, TyonInfo *info, GError **error) {
	tyon_info_finalize(info);
	return tyon_device_write(device, (gchar const *)info, sizeof(TyonInfo), error);
}

gboolean tyon_reset(RoccatDevice *device, GError **error) {
	TyonInfo info = { 0 };
	info.function = TYON_INFO_FUNCTION_RESET;
	return tyon_info_write(device, &info, error);
}

guint tyon_firmware_version_read(RoccatDevice *device, GError **error) {
	TyonInfo *info;
	guint result;

	info = tyon_info_read(device, error);
	if (!info)
		return 0;
	result = info->firmware_version;
	g_free(info);
	return result;
}
