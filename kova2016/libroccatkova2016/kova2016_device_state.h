#ifndef __ROCCAT_KOVA2016_DEVICE_STATE_H__
#define __ROCCAT_KOVA2016_DEVICE_STATE_H__

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

typedef enum {
	/* Device sends HID multimedia events */
	KOVA2016_DEVICE_STATE_STATE_OFF = 0x00,
	/* Driver handles multimedia events */
	KOVA2016_DEVICE_STATE_STATE_ON = 0x01,
	KOVA2016_DEVICE_STATE_DEACTIVE = 0x10,
} Kova2016DeviceStateState;

gboolean kova2016_device_state_write(RoccatDevice *kova2016, guint state, GError **error);

G_END_DECLS

#endif
