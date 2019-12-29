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

#include "kovaplus.h"
#include <stdlib.h>

KovaplusInfo *kovaplus_info_read(RoccatDevice *kovaplus, GError **error) {
	return (KovaplusInfo *)kovaplus_device_read(kovaplus, KOVAPLUS_REPORT_ID_INFO, sizeof(KovaplusInfo), error);
}

static void kovaplus_info_finalize(KovaplusInfo *info) {
	info->report_id = KOVAPLUS_REPORT_ID_INFO;
	info->size = sizeof(KovaplusInfo);
}

static gboolean kovaplus_info_write(RoccatDevice *kovaplus, KovaplusInfo *info, GError **error) {
	kovaplus_info_finalize(info);
	return kovaplus_device_write(kovaplus, (gchar const *)info, sizeof(KovaplusInfo), error);
}

gboolean kovaplus_reset(RoccatDevice *kovaplus, GError **error) {
	KovaplusInfo info = { 0 };
	info.firmware_version = KOVAPLUS_INFO_FUNCTION_RESET;
	return kovaplus_info_write(kovaplus, &info, error);
}

guint kovaplus_firmware_version_read(RoccatDevice *kovaplus, GError **error) {
	KovaplusInfo *info;
	guint result;

	info = kovaplus_info_read(kovaplus, error);
	if (info) {
		result = info->firmware_version;
		g_free(info);
		return result;
	}
	return 0;
}
