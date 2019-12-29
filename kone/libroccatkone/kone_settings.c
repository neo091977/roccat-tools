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

#include "kone_internal.h"
#include "roccat_helper.h"
#include "roccat_device_sysfs.h"

static gchar const * const settings_name = "settings";

guint16 kone_settings_calc_checksum(KoneSettings const *settings) {
	return ROCCAT_BYTESUM_PARTIALLY(settings, KoneSettings, size, checksum);
}

void kone_settings_set_checksum(KoneSettings *settings) {
	guint16 checksum = kone_settings_calc_checksum(settings);
	settings->checksum = GUINT16_TO_LE(checksum);
}

KoneSettings *kone_settings_read(RoccatDevice *kone, GError **error) {
	return (KoneSettings *)roccat_device_sysfs_read(kone, settings_name, sizeof(KoneSettings), error);
}


gboolean kone_settings_write(RoccatDevice *kone, KoneSettings const *settings, GError **error) {
	return roccat_device_sysfs_write(kone, settings_name, (gchar const *)settings, sizeof(KoneSettings), error);
}
