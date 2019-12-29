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

#include "kiro_info.h"
#include "kiro_device.h"

KiroInfo *kiro_info_read(RoccatDevice *device, GError **error) {
	return (KiroInfo *)kiro_device_read(device, KIRO_REPORT_ID_INFO, sizeof(KiroInfo), error);
}

static void kiro_info_finalize(KiroInfo *info) {
	info->report_id = KIRO_REPORT_ID_INFO;
	info->size = sizeof(KiroInfo);
}

gboolean kiro_info_write(RoccatDevice *device, KiroInfo *info, GError **error) {
	kiro_info_finalize(info);
	return kiro_device_write(device, (gchar const *)info, sizeof(KiroInfo), error);
}

guint kiro_firmware_version_read(RoccatDevice *device, GError **error) {
	KiroInfo *info;
	guint result;

	info = kiro_info_read(device, error);
	if (!info)
		return 0;
	result = info->firmware_version;
	g_free(info);
	return result;
}

gboolean kiro_reset(RoccatDevice *device, GError **error) {
	KiroInfo info = { 0 };
	info.function = KIRO_INFO_FUNCTION_RESET;
	return kiro_info_write(device, &info, error);
}
