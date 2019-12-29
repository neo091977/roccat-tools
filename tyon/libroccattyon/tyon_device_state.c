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

#include "tyon_device_state.h"
#include "tyon_device.h"

typedef struct _TyonDeviceState TyonDeviceState;

struct _TyonDeviceState {
	guint8 report_id; /* TYON_REPORT_ID_DEVICE_STATE */
	guint8 size; /* always 0x03 */
	guint8 state;
} __attribute__ ((packed));

gboolean tyon_device_state_write(RoccatDevice *tyon, guint state, GError **error) {
	TyonDeviceState device_state;

	device_state.report_id = TYON_REPORT_ID_DEVICE_STATE;
	device_state.size = sizeof(TyonDeviceState);
	device_state.state = state;

	return tyon_device_write(tyon, (gchar const *)&device_state, sizeof(TyonDeviceState), error);
}
