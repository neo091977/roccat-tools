#ifndef __ROCCAT_FIRMWARE_H__
#define __ROCCAT_FIRMWARE_H__

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

#include "roccat_device.h"

G_BEGIN_DECLS

typedef struct _RoccatFirmwareState RoccatFirmwareState;

enum {
	ROCCAT_FIRMWARE_CHUNK_SIZE = 1024,
	ROCCAT_FIRMWARE_NUMBER_DEFAULT = 0,
	ROCCAT_FIRMWARE_SIZE_VARIABLE = 0,
	ROCCAT_FIRMWARE_UPDATE_WAIT_PREPARE_DEFAULT = 110,
	ROCCAT_FIRMWARE_UPDATE_WAIT_INIT_DATA_DEFAULT = 600,
	ROCCAT_FIRMWARE_UPDATE_WAIT_DATA_DEFAULT = 110,
	ROCCAT_FIRMWARE_UPDATE_FINAL_WRITE_CHECK_DEFAULT = TRUE,
};

gchar *roccat_firmware_version_to_string(guint firmware_version);

/*
 * Returns TRUE if firmware version is equal or greater than needed
 */
gboolean roccat_check_firmware_version(guint has, guint needs);

/*
 * Call this only after giving firmware size on call to roccat_firmware_state_new()
 * or after roccat_firmware_state_read_firmware()
 */
guint roccat_firmware_fractions(RoccatFirmwareState const *state);

void roccat_firmware_state_reset(RoccatFirmwareState *state);

/*
 * If firmware_size is 0 real file size is used, else file size is checked against firmware_size.
 * Upper 4 bits of number are used to identify firmware image.
 */
RoccatFirmwareState *roccat_firmware_state_new(RoccatDevice *device, guint endpoint, guint firmware_size, guint8 number);

gboolean roccat_firmware_state_read_firmware(RoccatFirmwareState *state, gchar const *path, GError **error);
gchar *roccat_firmware_state_get_md5(RoccatFirmwareState const *state);
gboolean roccat_firmware_state_tick(RoccatFirmwareState *state, GError **error);
void roccat_firmware_state_free(RoccatFirmwareState *state);
void roccat_firmware_state_set_wait_prepare(RoccatFirmwareState *state, guint wait);
void roccat_firmware_state_set_wait_init_data(RoccatFirmwareState *state, guint wait);
void roccat_firmware_state_set_wait_data(RoccatFirmwareState *state, guint wait);
void roccat_firmware_state_set_final_write_check(RoccatFirmwareState *state, gboolean final_write_check);

gboolean roccat_firmware_update_console(RoccatDevice *device, guint endpoint, gchar const *path, guint firmware_size, guint firmware_number,
		guint wait_prepare, guint wait_init_data, guint wait_data, gboolean final_write_check);

G_END_DECLS

#endif
