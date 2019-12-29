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
#include "roccat_firmware.h"
#include "roccat_helper.h"
#include "roccat_device_hidraw.h"
#include "roccat_device_scanner.h"
#include "roccat_control.h"
#include "roccat_strings.h"
#include "g_roccat_helper.h"
#include "i18n-lib.h"
#include <string.h>
#include <errno.h>

typedef struct _RoccatFirmwareWrite RoccatFirmwareWrite;

struct _RoccatFirmwareWrite {
	guint8 report_id;
	guint8 type;
	guint8 data[ROCCAT_FIRMWARE_CHUNK_SIZE];
} __attribute__ ((packed));

enum {
	ROCCAT_REPORT_ID_FIRMWARE_WRITE = 0x1b,
	ROCCAT_REPORT_ID_FIRMWARE_WRITE_CONTROL = 0x1c,
};

typedef enum {
	ROCCAT_FIRMWARE_WRITE_TYPE_INIT = 0x00,
	ROCCAT_FIRMWARE_WRITE_TYPE_PREPARE = 0x01,
	ROCCAT_FIRMWARE_WRITE_TYPE_INIT_DATA = 0x02,
	ROCCAT_FIRMWARE_WRITE_TYPE_PREPARE_DATA = 0x03,
	ROCCAT_FIRMWARE_WRITE_TYPE_DATA = 0x04,
	ROCCAT_FIRMWARE_WRITE_TYPE_FINISH = 0x0f,
} RoccatFirmwareWriteType;

enum {
	ROCCAT_FIRMWARE_WAIT_AFTER_RESET = 3000,
};

struct _RoccatFirmwareState {
	RoccatDevice *device;
	guint endpoint;
	guint update_state;
	guint chunk_number;
	gboolean chunk_init;
	guint firmware_size;
	guint firmware_chunks;
	guint8 *firmware;
	guint wait_after_prepare;
	guint wait_after_init_data;
	guint wait_after_data;
	gboolean final_write_check;
	guint8 number;
};

static guint16 calc_chunk_bytesum(guint8 const *data) {
	guint i;
	guint16 result = 0;

	for (i = 0; i < ROCCAT_FIRMWARE_CHUNK_SIZE; ++i)
		result += data[i];

	return result;
}

static gboolean roccat_firmware_write_step(RoccatDevice *roccat_device, guint endpoint, RoccatFirmwareWrite const *buffer, GError **error) {
	return roccat_device_hidraw_write(roccat_device, endpoint, (gchar const *)buffer, sizeof(RoccatFirmwareWrite), error);
}

static gboolean roccat_firmware_write_check_immediate(RoccatDevice *roccat_device, guint endpoint, GError **error) {
	return roccat_check_write_immediate(roccat_device, endpoint, ROCCAT_REPORT_ID_FIRMWARE_WRITE_CONTROL, 110, error);
}

static gboolean roccat_firmware_write_check(RoccatDevice *roccat_device, guint endpoint, guint check_wait, GError **error) {
	g_usleep(check_wait * G_ROCCAT_USEC_PER_MSEC);
	return roccat_firmware_write_check_immediate(roccat_device, endpoint, error);
}

static void report_init(RoccatFirmwareWrite *report, guint8 type) {
	report->report_id = ROCCAT_REPORT_ID_FIRMWARE_WRITE;
	report->type = type;
	memset(&report->data, 0, ROCCAT_FIRMWARE_CHUNK_SIZE);
}

static void report_store16(RoccatFirmwareWrite *report, guint index, guint16 value) {
	value = GUINT16_TO_LE(value);
	report->data[index] = (guint8)value;
	report->data[index + 1] = (guint8)(value >> 8);
}

static void report_store32(RoccatFirmwareWrite *report, guint index, guint32 value) {
	value = GUINT32_TO_LE(value);
	report->data[index] = (guint8)value;
	report->data[index + 1] = (guint8)(value >> 8);
	report->data[index + 2] = (guint8)(value >> 16);
	report->data[index + 3] = (guint8)(value >> 24);
}

static gboolean roccat_firmware_init(RoccatFirmwareState const *state, GError **error) {
	RoccatFirmwareWrite report;

	report_init(&report, state->number | ROCCAT_FIRMWARE_WRITE_TYPE_INIT);
	report_store32(&report, 2, 0x1032AC08);
	report_store32(&report, 6, 0x0100CB03);

	if (!roccat_firmware_write_step(state->device, state->endpoint, &report, error))
		return FALSE;

	g_usleep(ROCCAT_FIRMWARE_WAIT_AFTER_RESET * G_ROCCAT_USEC_PER_MSEC);
	roccat_device_reenumerate(state->device);

	return roccat_firmware_write_check_immediate(state->device, state->endpoint, error);
}

static gboolean roccat_firmware_prepare(RoccatFirmwareState const *state, GError **error) {
	RoccatFirmwareWrite report;

	report_init(&report, state->number | ROCCAT_FIRMWARE_WRITE_TYPE_PREPARE);
	report_store32(&report, 2, state->firmware_size);
	report_store32(&report, 6, 0x8822AC08);

	if (!roccat_firmware_write_step(state->device, state->endpoint, &report, error))
		return FALSE;
	return roccat_firmware_write_check(state->device, state->endpoint, state->wait_after_prepare, error);
}

static gboolean roccat_firmware_init_data(RoccatFirmwareState const *state, GError **error) {
	RoccatFirmwareWrite report;

	report_init(&report, state->number | ROCCAT_FIRMWARE_WRITE_TYPE_INIT_DATA);
	report_store32(&report, 2, 0xEC);

	if (!roccat_firmware_write_step(state->device, state->endpoint, &report, error))
		return FALSE;
	
	/* Play it save with increased waiting time
	 * At least Savu might bail out with 110
	 */
	return roccat_firmware_write_check(state->device, state->endpoint, state->wait_after_init_data, error);
}

static gboolean roccat_firmware_prepare_data(RoccatFirmwareState const *state, GError **error) {
	RoccatFirmwareWrite report;
	guint16 bytesum;
	guint8 const *chunk;

	chunk = state->firmware + state->chunk_number * ROCCAT_FIRMWARE_CHUNK_SIZE;
	bytesum = calc_chunk_bytesum(chunk);

	report_init(&report, state->number | ROCCAT_FIRMWARE_WRITE_TYPE_PREPARE_DATA);
	report_store16(&report, 2, state->chunk_number);
	report_store16(&report, 4, bytesum);
	report_store32(&report, 6, 0x8822AC08);

	if (!roccat_firmware_write_step(state->device, state->endpoint, &report, error))
		return FALSE;
	return roccat_firmware_write_check(state->device, state->endpoint, state->wait_after_data, error);
}

static gboolean roccat_firmware_write_data(RoccatFirmwareState const *state, GError **error) {
	RoccatFirmwareWrite report;
	guint8 const *chunk;

	chunk = state->firmware + state->chunk_number * ROCCAT_FIRMWARE_CHUNK_SIZE;
	
	report_init(&report, state->number | ROCCAT_FIRMWARE_WRITE_TYPE_DATA);
	memcpy(&report.data, chunk, ROCCAT_FIRMWARE_CHUNK_SIZE);

	if (!roccat_firmware_write_step(state->device, state->endpoint, &report, error))
		return FALSE;
	return roccat_firmware_write_check(state->device, state->endpoint, state->wait_after_data, error);
}

static gboolean roccat_firmware_finish(RoccatFirmwareState const *state, GError **error) {
	RoccatFirmwareWrite report;
	GError *local_error = NULL;

	report_init(&report, state->number | ROCCAT_FIRMWARE_WRITE_TYPE_FINISH);

	if (!roccat_firmware_write_step(state->device, state->endpoint, &report, &local_error)) {
		if (!state->final_write_check && g_error_matches(local_error, ROCCAT_ERRNO_ERROR, EPIPE)) {
			/* It is normal for e.g. Tyon to sometimes reset without even finishing f write */
			g_debug(_("Firmware update finish step: Catching errno %i: %s"), local_error->code, local_error->message);
			g_clear_error(&local_error);
		} else {
			g_propagate_error(error, local_error);
			return FALSE;
		}
	}

	/* Update the device structure, so software can continue to work with it */
	g_usleep(ROCCAT_FIRMWARE_WAIT_AFTER_RESET * G_ROCCAT_USEC_PER_MSEC);
	roccat_device_reenumerate(state->device);

	if (state->final_write_check)
		return roccat_firmware_write_check_immediate(state->device, state->endpoint, error);
	else
		return TRUE;
}

guint roccat_firmware_fractions(RoccatFirmwareState const *state) {
	return state->firmware_chunks * 2 + 4;
}

void roccat_firmware_state_reset(RoccatFirmwareState *state) {
	state->update_state = 0;
}

void roccat_firmware_state_set_wait_prepare(RoccatFirmwareState *state, guint wait) {
	state->wait_after_prepare = wait;
}

void roccat_firmware_state_set_wait_init_data(RoccatFirmwareState *state, guint wait) {
	state->wait_after_init_data = wait;
}

void roccat_firmware_state_set_wait_data(RoccatFirmwareState *state, guint wait) {
	state->wait_after_data = wait;
}

void roccat_firmware_state_set_final_write_check(RoccatFirmwareState *state, gboolean final_write_check) {
	state->final_write_check = final_write_check;
}

static void set_firmware_size(RoccatFirmwareState *state, guint firmware_size) {
	state->firmware_size = firmware_size;
	state->firmware_chunks = (firmware_size + ROCCAT_FIRMWARE_CHUNK_SIZE - 1) / ROCCAT_FIRMWARE_CHUNK_SIZE;
}

RoccatFirmwareState *roccat_firmware_state_new(RoccatDevice *device, guint endpoint, guint firmware_size, guint8 number) {
	RoccatFirmwareState *state;
	state = (RoccatFirmwareState *)g_malloc(sizeof(RoccatFirmwareState));
	state->device = device;
	state->endpoint = endpoint;
	set_firmware_size(state, firmware_size);
	state->firmware = NULL;
	state->number = number & 0xf0;
	roccat_firmware_state_set_wait_prepare(state, ROCCAT_FIRMWARE_UPDATE_WAIT_PREPARE_DEFAULT);
	roccat_firmware_state_set_wait_init_data(state, ROCCAT_FIRMWARE_UPDATE_WAIT_INIT_DATA_DEFAULT);
	roccat_firmware_state_set_wait_data(state, ROCCAT_FIRMWARE_UPDATE_WAIT_DATA_DEFAULT);
	roccat_firmware_state_set_final_write_check(state, ROCCAT_FIRMWARE_UPDATE_FINAL_WRITE_CHECK_DEFAULT);
	roccat_firmware_state_reset(state);
	return state;
}

/* loads firmware and reallocs it into @chunks * @chunk_size memory */
gboolean roccat_firmware_state_read_firmware(RoccatFirmwareState *state, gchar const *path, GError **error) {
	gchar *result;
	gsize bytes;
	guint i;

	if (!g_file_get_contents(path, &result, &bytes, error))
		return FALSE;

	if (state->firmware_size == 0) {
			set_firmware_size(state, bytes);
	} else {
		if (bytes != state->firmware_size) {
			g_set_error(error, G_FILE_ERROR, G_FILE_ERROR_INVAL, _("Firmware image has wrong size: %zu bytes instead of %zu"), bytes, (gsize)state->firmware_size);
			return FALSE;
		}
	}

	result = g_realloc(result, ROCCAT_FIRMWARE_CHUNK_SIZE * state->firmware_chunks);

	for (i = state->firmware_size; i < ROCCAT_FIRMWARE_CHUNK_SIZE * state->firmware_chunks; ++i)
		result[i] = 0;

	g_free(state->firmware);
	state->firmware = (guint8 *)result;
	return TRUE;
}

gchar *roccat_firmware_state_get_md5(RoccatFirmwareState const *state) {
	return g_compute_checksum_for_data(G_CHECKSUM_MD5, state->firmware, state->firmware_size);
}

gboolean roccat_firmware_state_tick(RoccatFirmwareState *state, GError **error) {
	switch (state->update_state) {
	case 0:
		if (!roccat_firmware_init(state, error))
			return FALSE;
		++state->update_state;
		break;
	case 1:
		if (!roccat_firmware_prepare(state, error))
			return FALSE;
		++state->update_state;
		break;
	case 2:
		if (!roccat_firmware_init_data(state, error))
			return FALSE;
		++state->update_state;
		state->chunk_number = 0;
		state->chunk_init = 1;
		break;
	case 3:
		/* This implementation needs one additional call to get to next state */
		if (state->chunk_number < state->firmware_chunks) {
			if (state->chunk_init) {
				if (!roccat_firmware_prepare_data(state, error))
					return FALSE;
				state->chunk_init = FALSE;
			} else {
				if (!roccat_firmware_write_data(state, error))
					return FALSE;
				++state->chunk_number;
				state->chunk_init = TRUE;
			}
		} else
			++state->update_state;
		break;
	case 4:
		if (!roccat_firmware_finish(state, error))
			return FALSE;
		++state->update_state;
		return FALSE;
		break;
	default:
		return FALSE;
		break;
	}

	return TRUE;
}

void roccat_firmware_state_free(RoccatFirmwareState *state) {
	g_free(state->firmware);
	g_free(state);
}

gboolean roccat_check_firmware_version(guint has, guint needs) {
	return has >= needs;
}

gchar *roccat_firmware_version_to_string(guint firmware_version) {
	return g_strdup_printf("%i.%02i", firmware_version / 100, firmware_version % 100);
}

gboolean roccat_firmware_update_console(RoccatDevice *device, guint endpoint, gchar const *path, guint firmware_size, guint firmware_number,
		guint wait_prepare, guint wait_init_data, guint wait_data, gboolean final_write_check) {
	RoccatFirmwareState *state;
	gboolean retval = FALSE;
	GError *local_error = NULL;

	g_print("%s", roccat_string_firmware_liability());
	if (!roccat_user_decision_console(_("Continue?"))) {
		g_message(_("Firmware update aborted by user."));
		return TRUE;
	}

	state = roccat_firmware_state_new(device, endpoint, firmware_size, firmware_number);
	roccat_firmware_state_set_wait_prepare(state, wait_prepare);
	roccat_firmware_state_set_wait_init_data(state, wait_init_data);
	roccat_firmware_state_set_wait_data(state, wait_data);
	roccat_firmware_state_set_final_write_check(state, final_write_check);

	if (!roccat_firmware_state_read_firmware(state, path, &local_error))
		goto error;

	while (roccat_firmware_state_tick(state, &local_error))
		g_print(".");
	g_print("\n");

	if (local_error)
		goto error;

	retval = TRUE;
	g_message(_("Firmware updated successfully. Please reconnect device."));
	goto out;

error:
	g_critical(_("Could not update firmware: %s"), local_error->message);
	g_clear_error(&local_error);
out:
	roccat_firmware_state_free(state);
	return retval;
}
