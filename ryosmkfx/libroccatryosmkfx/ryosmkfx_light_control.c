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

#include "ryosmkfx_light_control.h"
#include "ryos_device.h"
#include "g_roccat_helper.h"
#include "i18n-lib.h"

gboolean ryosmkfx_light_control_check_write(RoccatDevice *device, GError **error) {
	RyosLightControl *light_control;
	guint result;

	g_usleep(RYOS_WRITE_CHECK_WAIT_LIGHT_CONTROL * G_ROCCAT_USEC_PER_MSEC);
	while (TRUE) {
		light_control = ryos_light_control_read(device, error);
		if (!light_control)
			return FALSE;
		result = light_control->write_check;
		g_free(light_control);

		switch (result) {
		case RYOSMKFX_LIGHT_CONTROL_WRITE_CHECK_OK:
			return TRUE;
			break;
		case RYOSMKFX_LIGHT_CONTROL_WRITE_CHECK_BUSY:
			break;
		default:
			g_set_error(error, G_FILE_ERROR, G_FILE_ERROR_FAILED, _("Write check returned %u"), result);
			return FALSE;
			break;
		}
	}
}
