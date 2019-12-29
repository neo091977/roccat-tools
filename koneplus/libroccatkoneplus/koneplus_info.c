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

#include "koneplus.h"
#include <stdlib.h>

static void koneplus_info_finalize(KoneplusInfo *info) {
	info->report_id = KONEPLUS_REPORT_ID_INFO;
	info->size = sizeof(KoneplusInfo);
}

static gboolean koneplus_info_write(RoccatDevice *koneplus, KoneplusInfo *info, GError **error) {
	koneplus_info_finalize(info);
	return koneplus_device_write(koneplus, (gchar const *)info, sizeof(KoneplusInfo), error);
}

gboolean koneplus_reset(RoccatDevice *koneplus, GError **error) {
	KoneplusInfo info = { 0 };
	info.firmware_version = KONEPLUS_INFO_FUNCTION_RESET;
	return koneplus_info_write(koneplus, &info, error);
}

guint koneplus_firmware_version_read(RoccatDevice *koneplus, GError **error) {
	KoneplusInfo *info;
	guint result;

	info = koneplus_info_read(koneplus, error);
	if (info) {
		result = info->firmware_version;
		g_free(info);
		return result;
	}

	return 0;
}

KoneplusInfo *koneplus_info_read(RoccatDevice *koneplus, GError **error) {
	return (KoneplusInfo *)koneplus_device_read(koneplus, KONEPLUS_REPORT_ID_INFO, sizeof(KoneplusInfo), error);
}
