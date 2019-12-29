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

#include "roccat_device_sysfs.h"
#include "roccat_helper.h"
#include "i18n-lib.h"
#include <fcntl.h> /* open */
#include <unistd.h> /* write, close */
#include <errno.h>

static gchar *roccat_build_sysfs_path(RoccatDevice const *roccat_dev, gchar const *filename) {
	return g_build_filename(roccat_device_get_syspath(roccat_dev), filename, NULL);
}

static void roccat_device_sysfs_debug(RoccatDevice const *roccat_device, gboolean out, gchar const *attribute, guchar const *data, gsize length) {
#ifndef NDEBUG
	gchar *string;
	gchar *temp_string;

	temp_string = roccat_data8_to_string(data, length);
	string = g_strdup_printf("%s %04x/%s  %s",
			out ? "OUT" : "IN ",
			gaminggear_device_get_product_id(GAMINGGEAR_DEVICE(roccat_device)),
			attribute,
			temp_string);

	g_debug("%s", string);

	g_free(temp_string);
	g_free(string);
#endif
}

gchar *roccat_device_sysfs_read(RoccatDevice *roccat_device, gchar const *attribute, gsize length, GError **error) {
	gchar *path;
	gchar *data;

	path = roccat_build_sysfs_path(roccat_device, attribute);

	gaminggear_device_lock(GAMINGGEAR_DEVICE(roccat_device));

	data = roccat_read_from_file(path, length, error);
	g_free(path);
	if (data)
		roccat_device_sysfs_debug(roccat_device, FALSE, attribute, (guchar const *)data, length);

	gaminggear_device_unlock(GAMINGGEAR_DEVICE(roccat_device));

	return data;
}

gboolean roccat_device_sysfs_write(RoccatDevice *roccat_device, gchar const *attribute, gchar const *data, gsize length, GError **error) {
	gint file_descriptor;
	ssize_t bytes_written;
	gchar *path;
	gboolean retval = FALSE;

	if (length == 0)
		return TRUE;

	path = roccat_build_sysfs_path(roccat_device, attribute);

	gaminggear_device_lock(GAMINGGEAR_DEVICE(roccat_device));

	file_descriptor = open(path, O_WRONLY);

	if (file_descriptor == -1) {
		g_set_error(error, G_FILE_ERROR, g_file_error_from_errno(errno), _("Could not open file %s: %s"), path, g_strerror(errno));
		goto error;
	}

	bytes_written = write(file_descriptor, data, length);
	if (bytes_written != (ssize_t)length) {
		if (bytes_written == -1)
			g_set_error(error, G_FILE_ERROR, g_file_error_from_errno(errno), _("Could not write file %s: %s"), path, g_strerror(errno));
		else
			g_set_error(error, G_FILE_ERROR, G_FILE_ERROR_FAILED, _("Could not write file %s: partial write (%zi instead of %zu)"), path, bytes_written, (gsize)length);
		close(file_descriptor);
		goto error;
	}

	if (close(file_descriptor) == -1) {
		g_set_error(error, G_FILE_ERROR, g_file_error_from_errno(errno), _("Could not close file %s: %s"), path, g_strerror(errno));
		goto error;
	}

	roccat_device_sysfs_debug(roccat_device, TRUE, attribute, (guchar const *)data, length);

	retval = TRUE;
error:
	gaminggear_device_unlock(GAMINGGEAR_DEVICE(roccat_device));
	g_free(path);
	return retval;
}
