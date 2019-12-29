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

#include "kova2016_info.h"
#include "kova2016_device.h"

Kova2016Info *kova2016_info_read(RoccatDevice *device, GError **error) {
	return (Kova2016Info *)kova2016_device_read(device, KOVA2016_REPORT_ID_INFO, sizeof(Kova2016Info), error);
}

static void kova2016_info_finalize(Kova2016Info *info) {
	info->report_id = KOVA2016_REPORT_ID_INFO;
	info->size = sizeof(Kova2016Info);
}

gboolean kova2016_info_write(RoccatDevice *device, Kova2016Info *info, GError **error) {
	kova2016_info_finalize(info);
	return kova2016_device_write(device, (gchar const *)info, sizeof(Kova2016Info), error);
}

guint kova2016_firmware_version_read(RoccatDevice *device, GError **error) {
	Kova2016Info *info;
	guint result;

	info = kova2016_info_read(device, error);
	if (!info)
		return 0;
	result = info->firmware_version;
	g_free(info);
	return result;
}

gboolean kova2016_reset(RoccatDevice *device, GError **error) {
	Kova2016Info info = { 0 };
	info.function = KOVA2016_INFO_FUNCTION_RESET;
	return kova2016_info_write(device, &info, error);
}
