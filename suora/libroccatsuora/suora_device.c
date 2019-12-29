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

#include "suora_device.h"
#include "suora.h"
#include "roccat_device_hidraw.h"
#include "roccat_helper.h"
#include "g_roccat_helper.h"
#include "i18n-lib.h"
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>

enum {
    SUORA_INTERRUPT_PACKET_SIZE = 64,
    SUORA_CHECKSUM_MAGIC_NUMBER = 0x8b,
    SUORA_FEATURE_TYPE_MASK = 0x80,
    SUORA_FEATURE_TYPE_GET = 0x80,
    SUORA_FEATURE_TYPE_SET = 0x00,
    SUORA_FEATURE_REQUEST_MASK = 0x7f,
};

static guint const device_ids[2] = { USB_DEVICE_ID_ROCCAT_SUORA, 0 };

RoccatDevice *suora_device_first(void) {
	return roccat_device_first(device_ids);
}

RoccatDeviceScanner *suora_device_scanner_new(void) {
	return roccat_device_scanner_new(device_ids);
}

// FIXME use roccat_device_hidraw_debug()
static void G_GNUC_UNUSED suora_debug_interrupt(RoccatDevice const *roccat_device, gboolean out, guchar const *data, gssize length) {
#ifndef NDEBUG
	gchar *temp_string;
	struct timeval time;

	gettimeofday(&time, NULL);

	temp_string = roccat_data8_to_string(data, length);

	g_debug("%llu.%06llu %s %04x:%i/%02x  %s",
			(unsigned long long)time.tv_sec, (unsigned long long)time.tv_usec,
			out ? "OUT" : "IN ",
			gaminggear_device_get_product_id(GAMINGGEAR_DEVICE(roccat_device)),
			SUORA_INTERFACE_VENDOR, 0,
			temp_string);

	g_free(temp_string);
#endif
}

static guint8 suora_feature_buffer_calc_checksum(SuoraFeatureBuffer const *buffer) {
	guint8 sum = roccat_calc_bytesum(buffer, sizeof(SuoraFeatureBuffer) - 1);
	return sum ^ SUORA_CHECKSUM_MAGIC_NUMBER;
}

void suora_feature_buffer_set_checksum(SuoraFeatureBuffer *buffer) {
	buffer->checksum = suora_feature_buffer_calc_checksum(buffer);
}

static gboolean suora_write_buffer(RoccatDevice *device, SuoraFeatureBuffer *out, GError **error) {
	suora_feature_buffer_set_checksum(out);
	return roccat_device_hidraw_write_0(device, SUORA_INTERFACE_VENDOR, (gchar const *)out, sizeof(SuoraFeatureBuffer), error);
}

gboolean suora_write(RoccatDevice *device, guint request, SuoraFeaturePayload const *payload, GError **error) {
	SuoraFeatureBuffer buffer;
	buffer.action = SUORA_FEATURE_TYPE_SET | request;
	buffer.payload = *payload;
	return suora_write_buffer(device, &buffer, error);
}

gboolean suora_write_interrupt(RoccatDevice *device, guint request, SuoraDataDeclaration const *declaration, gchar const *data, guint length, GError **error) {
	SuoraDataDeclaration internal_declaration;
	gboolean result = FALSE;
	ssize_t written_bytes;
	gint fd;
	guint i;
	guint packet_count;
	gchar *extended_data;
	
	packet_count = length / SUORA_INTERRUPT_PACKET_SIZE;
    if (length % SUORA_INTERRUPT_PACKET_SIZE != 0)
        ++packet_count;

    internal_declaration = *declaration;
    internal_declaration.packet_count = packet_count;

    gaminggear_device_lock(GAMINGGEAR_DEVICE(device));

	fd = roccat_device_get_hidraw_endpoint(device, SUORA_INTERFACE_VENDOR, error);
	if (fd == -1)
		goto out;

	if (!suora_write(device, request, (SuoraFeaturePayload const *)&internal_declaration, error))
		goto out;

	extended_data = g_malloc(SUORA_INTERRUPT_PACKET_SIZE + 1); // add report_id

	for (i = 0; i < packet_count; ++i) {
		memset(extended_data, 0, SUORA_INTERRUPT_PACKET_SIZE + 1);
		memcpy(extended_data + 1, data + SUORA_INTERRUPT_PACKET_SIZE * i, MIN(SUORA_INTERRUPT_PACKET_SIZE, length - SUORA_INTERRUPT_PACKET_SIZE * i));
		
		written_bytes = write(fd, extended_data, SUORA_INTERRUPT_PACKET_SIZE + 1);
		if (written_bytes != SUORA_INTERRUPT_PACKET_SIZE + 1) {
			if (written_bytes == -1)
				g_set_error(error, G_FILE_ERROR, g_file_error_from_errno(errno), _("Could not write interrupt data: %s"), g_strerror(errno));
			else
				g_set_error(error, G_FILE_ERROR, G_FILE_ERROR_INVAL, _("Wrote too small data package: %li instead of %i"), written_bytes - 1, SUORA_INTERRUPT_PACKET_SIZE);
			goto free_data;
		}

		suora_debug_interrupt(device, TRUE, (guchar const *)extended_data + 1, SUORA_INTERRUPT_PACKET_SIZE);
	}

	result = TRUE;
free_data:
	g_free(extended_data);
out:
	gaminggear_device_unlock(GAMINGGEAR_DEVICE(device));
	return result;
}

SuoraFeaturePayload *suora_read(RoccatDevice *device, guint request, SuoraFeaturePayload const *payload, GError **error) {
	SuoraFeatureBuffer out;
	SuoraFeatureBuffer *in;
	SuoraFeaturePayload *result;

	out.action = SUORA_FEATURE_TYPE_GET | request;
	out.payload = *payload;
	
	if (!suora_write_buffer(device, &out, error))
		return NULL;

	g_usleep(100 * G_ROCCAT_USEC_PER_MSEC);

	in = (SuoraFeatureBuffer *)roccat_device_hidraw_read_0(device, SUORA_INTERFACE_VENDOR, sizeof(SuoraFeatureBuffer), error);
	if (!in)
		return NULL;

	if (in->action == 0x00) {
		g_set_error(error, G_FILE_ERROR, G_FILE_ERROR_INVAL, _("Wrong return value"));
		g_free(in);
		return NULL;
	}

	result = (SuoraFeaturePayload *)g_malloc(sizeof(SuoraFeaturePayload));
	*result = in->payload;

	g_free(in);

	return result;
}

gchar *suora_read_interrupt(RoccatDevice *device, guint request, SuoraDataDeclaration const *declaration, guint length, GError **error) {
	SuoraDataDeclaration *answer;
	guint total_bytes;
	gint fd;
	ssize_t read_bytes;
	gchar *buffer = NULL;
	guint i;

	gaminggear_device_lock(GAMINGGEAR_DEVICE(device));

	fd = roccat_device_get_hidraw_endpoint(device, SUORA_INTERFACE_VENDOR, error);
	if (fd == -1)
		goto out;

	if (lseek(fd, 0, SEEK_END) == -1) {
		g_set_error(error, G_FILE_ERROR, g_file_error_from_errno(errno), _("Could not seek to file end: %s"), g_strerror(errno));
		goto out;
	}

	answer = (SuoraDataDeclaration *)suora_read(device, request, (SuoraFeaturePayload const *)declaration, error);
	if (!answer)
		goto out;

	total_bytes = SUORA_INTERRUPT_PACKET_SIZE * answer->packet_count;

	if (length != total_bytes) {
		g_set_error(error, G_FILE_ERROR, G_FILE_ERROR_INVAL, _("Not enough data: %i instead of %i"), total_bytes, length);
		goto free_answer;
	}

	buffer = (gchar *)g_malloc(total_bytes);
	for (i = 0; i < answer->packet_count; ++i) {
		read_bytes = read(fd, buffer + SUORA_INTERRUPT_PACKET_SIZE * i, SUORA_INTERRUPT_PACKET_SIZE);
		if (read_bytes != SUORA_INTERRUPT_PACKET_SIZE) {
			if (read_bytes == -1)
				g_set_error(error, G_FILE_ERROR, g_file_error_from_errno(errno), _("Could not read interrupt data: %s"), g_strerror(errno));
			else
				g_set_error(error, G_FILE_ERROR, G_FILE_ERROR_INVAL, _("Read too small data package: %li instead of %i"), read_bytes, SUORA_INTERRUPT_PACKET_SIZE);

			g_free(buffer);
			buffer = NULL;
			goto free_answer;
		}

		suora_debug_interrupt(device, FALSE, (guchar const *)buffer + SUORA_INTERRUPT_PACKET_SIZE * i, SUORA_INTERRUPT_PACKET_SIZE);
	}

free_answer:
    g_free(answer);
out:
	gaminggear_device_unlock(GAMINGGEAR_DEVICE(device));
	return buffer;
}
