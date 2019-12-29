/*
 * This file is part of roccat-tools.
 *
 * roccat-tools is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Macro Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * roccat-tools is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Macro Public License for more details.
 *
 * You should have received a copy of the GNU Macro Public License
 * along with roccat-tools. If not, see <http://www.gnu.org/licenses/>.
 */

#include "ryos_device_state.h"
#include "ryos_device.h"

typedef struct _RyosDeviceState RyosDeviceState;

struct _RyosDeviceState {
	guint8 report_id; /* RYOS_REPORT_ID_DEVICE_STATE */
	guint8 size; /* always 0x03 */
	guint8 state;
} __attribute__ ((packed));

gboolean ryos_device_state_write(RoccatDevice *ryos, guint state, GError **error) {
	RyosDeviceState device_state;

	device_state.report_id = RYOS_REPORT_ID_DEVICE_STATE;
	device_state.size = sizeof(RyosDeviceState);
	device_state.state = state;

	return ryos_device_write(ryos, (gchar const *)&device_state, sizeof(RyosDeviceState), error);
}
