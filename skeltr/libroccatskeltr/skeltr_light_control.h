#ifndef __ROCCAT_SKELTR_LIGHT_CONTROL_H__
#define __ROCCAT_SKELTR_LIGHT_CONTROL_H__

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

#include "skeltr_device.h"

G_BEGIN_DECLS

typedef struct _SkeltrLightControl SkeltrLightControl;

struct _SkeltrLightControl {
	guint8 report_id; /* SKELTR_REPORT_ID_LIGHT_CONTROL */
	guint8 size; /* 0x08 */
	guint8 mode;
	guint8 unused[5];
} __attribute__ ((packed));

typedef enum {
	SKELTR_LIGHT_CONTROL_MODE_PRESET = 0,
	SKELTR_LIGHT_CONTROL_MODE_CUSTOM = 1,
} SkeltrLightControlMode;

gboolean skeltr_light_control_write(RoccatDevice *device, guint mode, GError **error);

G_END_DECLS

#endif
