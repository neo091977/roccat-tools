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

#include "roccat.h"
#include "roccat_device_hidraw.h"
#include "roccat_helper.h"
#include "i18n-lib.h"
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/hidraw.h>
#include <sys/time.h>

static gchar *add_report_id(gchar const *buffer, gssize length, guint report_id) {
	gchar *extended = (gchar *)g_malloc(length + 1);
	extended[0] = report_id;
	memcpy(extended + 1, buffer, length);
	return extended;
}

static gchar *remove_report_id(gchar const *buffer, gssize length) {
	gchar *shortened = (gchar *)g_malloc(length);
	memcpy(shortened, buffer + 1, length);
	return shortened;
}

static void G_GNUC_UNUSED roccat_device_hidraw_debug(RoccatDevice const *roccat_device, guint endpoint, gboolean out, guint report_id, guchar const *data, gssize length) {
#ifndef NDEBUG
	gchar *string;
	gchar *temp_string;
	struct timeval time;

	gettimeofday(&time, NULL);

	if (report_id == 0)
		temp_string = roccat_data8_to_string(data + 1, length - 1);
	else
		temp_string = roccat_data8_to_string(data, length);

	string = g_strdup_printf("%llu.%06llu %s %04x:%i/%02x  %s",
			(unsigned long long)time.tv_sec, (unsigned long long)time.tv_usec,
			out ? "OUT" : "IN ",
			gaminggear_device_get_product_id(GAMINGGEAR_DEVICE(roccat_device)),
			endpoint,
			report_id,
			temp_string);

	g_debug("%s", string);

	g_free(temp_string);
	g_free(string);
#endif
}

gchar *roccat_device_hidraw_read(RoccatDevice *roccat_device, guint endpoint, guint report_id, gssize length, GError **error) {
#ifdef HIDIOCGFEATURE
	gchar *buffer;
	gint retval;
	gint fd;

	fd = roccat_device_get_hidraw_endpoint(roccat_device, endpoint, error);
	if (fd == -1)
		return NULL;

	buffer = g_malloc(length);
	buffer[0] = report_id;
	
	gaminggear_device_lock(GAMINGGEAR_DEVICE(roccat_device));

	retval = ioctl(fd, HIDIOCGFEATURE(length), buffer);

	gaminggear_device_unlock(GAMINGGEAR_DEVICE(roccat_device));

	if (retval == -1) {
		g_set_error(error, ROCCAT_ERRNO_ERROR, errno, _("Could not issue ioctl HIDIOCGFEATURE: %s"), g_strerror(errno));
		g_free(buffer);
		return NULL;
	}

	roccat_device_hidraw_debug(roccat_device, endpoint, FALSE, report_id, (guchar const *)buffer, length);

	return buffer;
#else
	g_set_error(error, G_FILE_ERROR, G_FILE_ERROR_NOSYS, _("Ioctl HIDIOCGFEATURE is not implemented"));
	return NULL;
#endif
}

gchar *roccat_device_hidraw_read_0(RoccatDevice *roccat_device, guint endpoint, gssize length, GError **error) {
	gchar *extended_in;
	gchar *in;

	extended_in = roccat_device_hidraw_read(roccat_device, endpoint, 0, length + 1, error);
	if (!extended_in)
		return FALSE;
	in = remove_report_id(extended_in, length);
	g_free(extended_in);
	
	return in;
}

gboolean roccat_device_hidraw_write(RoccatDevice *roccat_device, guint endpoint, gchar const *buffer, gssize length, GError **error) {
#ifdef HIDIOCSFEATURE
	int retval;
	int fd;

	fd = roccat_device_get_hidraw_endpoint(roccat_device, endpoint, error);
	if (fd == -1)
		return FALSE;

	roccat_device_hidraw_debug(roccat_device, endpoint, TRUE, buffer[0], (guchar const *)buffer, length);

	gaminggear_device_lock(GAMINGGEAR_DEVICE(roccat_device));

	retval = ioctl(fd, HIDIOCSFEATURE(length), buffer);

	gaminggear_device_unlock(GAMINGGEAR_DEVICE(roccat_device));

	if (retval == -1) {
		g_set_error(error, ROCCAT_ERRNO_ERROR, errno, _("Could not issue ioctl HIDIOCSFEATURE: %s"), g_strerror(errno));
		return FALSE;
	}

	return TRUE;
#else
	g_set_error(error, G_FILE_ERROR, G_FILE_ERROR_NOSYS, _("Ioctl HIDIOCSFEATURE is not implemented"));
	return FALSE;
#endif
}

gboolean roccat_device_hidraw_write_0(RoccatDevice *roccat_device, guint endpoint, gchar const *buffer, gssize length, GError **error) {
	gchar *extended_buffer;
	gboolean retval = TRUE;

	extended_buffer = add_report_id(buffer, length, 0);
	if (!roccat_device_hidraw_write(roccat_device, endpoint, extended_buffer, length + 1, error))
		retval = FALSE;
	g_free(extended_buffer);

	return retval;
}

guint roccat_device_hidraw_add_event_watch(RoccatDevice *roccat_device, guint endpoint, GIOFunc callback, gpointer user_data, GError **error) {
	guint event_source_id;
	GIOChannel *channel;
	GIOStatus status;

	channel = g_io_channel_new_file(roccat_device_get_hidraw_endpoint_dev(roccat_device, endpoint), "r", error);
	if (!channel)
		return 0;

	status = g_io_channel_set_encoding(channel, NULL, error);
	if (status != G_IO_STATUS_NORMAL) {
		g_io_channel_unref(channel);
		return 0;
	}

	g_io_channel_set_buffered(channel, FALSE);

	event_source_id = g_io_add_watch(channel, G_IO_IN | G_IO_HUP | G_IO_PRI, callback, user_data);
	/* g_io_add_watch increases reference count
	 * this passes ownership of the channel to the watch */
	g_io_channel_unref(channel);

	return event_source_id;
}
