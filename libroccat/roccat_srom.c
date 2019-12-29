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

#include "roccat_srom.h"
#include "roccat_helper.h"
#include "roccat_device_hidraw.h"
#include "roccat_control.h"
#include "roccat_strings.h"
#include "i18n-lib.h"
#include <string.h>

typedef struct _RoccatSROMChunk RoccatSROMChunk;

enum {
	ROCCAT_SROM_SIZE = 3070,
	ROCCAT_SROM_CHUNK_COUNT = 3,
	ROCCAT_SROM_CHUNK_SIZE = 1024,
	ROCCAT_REPORT_ID_SROM_WRITE = 0x0d,
	ROCCAT_REPORT_ID_CONTROL = 0x04, // FIXME put this in roccat_device and use instead of device specific ones?
};

struct _RoccatSROMChunk {
	guint8 report_id; /* ROCCAT_REPORT_ID_SROM_WRITE */
	guint8 number; /* 1-3 */
	guint8 data[ROCCAT_SROM_CHUNK_SIZE];
	guint16 checksum;
} __attribute__ ((packed));

static guint16 roccat_srom_chunk_calc_checksum(RoccatSROMChunk const *srom_chunk) {
	return ROCCAT_BYTESUM_PARTIALLY(srom_chunk, RoccatSROMChunk, report_id, checksum);
}

static void roccat_srom_chunk_set_checksum(RoccatSROMChunk *srom_chunk) {
	srom_chunk->checksum = GUINT16_TO_LE(roccat_srom_chunk_calc_checksum(srom_chunk));
}

static void roccat_srom_chunk_finalize(RoccatSROMChunk *srom_chunk, guint number) {
	srom_chunk->report_id = ROCCAT_REPORT_ID_SROM_WRITE;
	srom_chunk->number = number;
	roccat_srom_chunk_set_checksum(srom_chunk);
}

static gboolean roccat_srom_chunk_write(RoccatDevice *roccat_device, guint endpoint, RoccatSROMChunk const *buffer, GError **error) {
	if (!roccat_device_hidraw_write(roccat_device, endpoint, (gchar const *)buffer, sizeof(RoccatSROMChunk), error))
		return FALSE;

	return roccat_check_write(roccat_device, endpoint, ROCCAT_REPORT_ID_CONTROL, 1100, 110, error);
}

gboolean roccat_srom_write(RoccatDevice *roccat_device, guint endpoint, gchar const *data, GError **error) {
	RoccatSROMChunk chunk;
	guint i;

	for (i = 0; i < ROCCAT_SROM_CHUNK_COUNT; ++i) {
		memcpy(&chunk.data, data + i * ROCCAT_SROM_CHUNK_SIZE, ROCCAT_SROM_CHUNK_SIZE);
		roccat_srom_chunk_finalize(&chunk, i + 1);
		if (!roccat_srom_chunk_write(roccat_device, endpoint, &chunk, error))
			return FALSE;
	}

	return TRUE;
}

gchar *roccat_srom_read_from_file(gchar const *path, GError **error) {
	gchar *result;
	gsize bytes;
	guint i;

	if (!g_file_get_contents(path, &result, &bytes, error))
		return FALSE;

	if (bytes != ROCCAT_SROM_SIZE) {
		g_set_error(error, G_FILE_ERROR, G_FILE_ERROR_INVAL, _("SROM image has wrong size: %zu bytes instead of %zu"), bytes, (gsize)ROCCAT_SROM_SIZE);
		return FALSE;
	}

	result = g_realloc(result, ROCCAT_SROM_CHUNK_COUNT * ROCCAT_SROM_CHUNK_SIZE);

	for (i = ROCCAT_SROM_SIZE; i < ROCCAT_SROM_CHUNK_COUNT * ROCCAT_SROM_CHUNK_SIZE; ++i)
		result[i] = 0;

	return result;
}

gboolean roccat_srom_update_console(RoccatDevice *device, guint endpoint, gchar const *path) {
	gchar *srom;
	GError *local_error = NULL;

	g_print("%s", roccat_string_firmware_liability());
	if (!roccat_user_decision_console(_("Continue?"))) {
		g_message(_("SROM update aborted by user."));
		return TRUE;
	}

	srom = roccat_srom_read_from_file(path, &local_error);
	if (!srom) {
		g_critical(_("Could not update sensor rom: %s"), local_error->message);
		g_clear_error(&local_error);
		return FALSE;
	}

	if (!roccat_srom_write(device, endpoint, srom, &local_error)) {
		g_critical(_("Could not update sensor rom: %s"), local_error->message);
		g_clear_error(&local_error);
		g_free(srom);
		return FALSE;
	}

	g_message(_("Sensor rom updated successfully. Please reconnect device."));
	g_free(srom);
	return TRUE;
}
