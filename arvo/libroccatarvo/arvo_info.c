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

#include "arvo.h"

ArvoInfo *arvo_info_read(RoccatDevice *arvo, GError **error) {
	return (ArvoInfo *)arvo_device_read(arvo, ARVO_REPORT_ID_INFO, sizeof(ArvoInfo), error);
}

guint arvo_firmware_version_read(RoccatDevice *arvo, GError **error) {
	ArvoInfo *info;
	guint result;

	info = arvo_info_read(arvo, error);
	if (!info)
		return 0;

	result = info->firmware_version;

	g_free(info);
	return result;

}
