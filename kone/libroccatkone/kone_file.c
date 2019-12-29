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

/*!
 * \file kone_file.c
 */

#include "kone_internal.h"
#include "roccat_device_sysfs.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>

static gchar const * const actual_dpi_name = "actual_dpi";
static gchar const * const actual_profile_name = "actual_profile";
static gchar const * const firmware_version_name = "firmware_version";
static gchar const * const weight_name = "weight";
static gchar const * const tcu_name = "tcu";
static gchar const * const startup_profile_name = "startup_profile";

gboolean kone_calibrate(RoccatDevice *kone, GError **error) {
	return roccat_device_sysfs_write(kone, tcu_name, "1", 1, error);
}

gboolean kone_profile_activate(RoccatDevice *kone, guint profile_number, GError **error) {
	gboolean result;
	gchar *string;

	g_assert(profile_number >= KONE_PROFILE_MIN && profile_number <= KONE_PROFILE_MAX);
	string = g_strdup_printf("%i", profile_number);
	result = roccat_device_sysfs_write(kone, startup_profile_name, string, strlen(string), error);
	g_free(string);
	return result;
}

guint kone_weight_read(RoccatDevice *kone, GError **error) {
	guint result;
	gchar *string = roccat_device_sysfs_read(kone, weight_name, 0, error);
	if (*error) return 0;
	result = strtoul(string, NULL, 10) * 5;
	g_free(string);
	return result;
}

guint kone_firmware_version_read(RoccatDevice *kone, GError **error) {
	guint result;
	gchar *string = roccat_device_sysfs_read(kone, firmware_version_name, 0, error);
	if (*error) return 0;
	result = strtoul(string, NULL, 10);
	g_free(string);
	return result;
}

gchar *kone_firmware_version_to_string(guint firmware_version) {
	return g_strdup_printf("%i.%i", firmware_version / 100, firmware_version % 100);
}

guint kone_actual_dpi_read(RoccatDevice *kone, GError **error) {
	guint dpi_raw;
	gchar *string = roccat_device_sysfs_read(kone, actual_dpi_name, 0, error);
	if (*error) return 0;
	dpi_raw = strtoul(string, NULL, 10);
	g_free(string);
	return kone_dpi_raw_to_dpi(dpi_raw);
}

guint kone_dpi_raw_to_dpi(guint dpi_raw) {
	g_assert(dpi_raw >= KONE_DPI_800 && dpi_raw <= KONE_DPI_3200);
	if (dpi_raw == KONE_DPI_3200)
		return 3200;
	else
		return (dpi_raw + 1) * 400;
}

guint kone_actual_profile_read(RoccatDevice *kone, GError **error) {
	guint result;
	gchar *string = roccat_device_sysfs_read(kone, actual_profile_name, 0, error);
	if (*error) return 0;
	result = strtoul(string, NULL, 10);
	g_free(string);
	return result;
}
