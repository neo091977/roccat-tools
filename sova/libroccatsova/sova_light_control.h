#ifndef __ROCCAT_SOVA_LIGHT_CONTROL_H__
#define __ROCCAT_SOVA_LIGHT_CONTROL_H__

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

#include "sova.h"
#include "sova_device.h"

G_BEGIN_DECLS

typedef struct _SovaLightControl SovaLightControl;

struct _SovaLightControl {
	guint8 report_id; /* SOVA_REPORT_ID_LIGHT_CONTROL */
	guint8 size; /* 0x08 */
	guint8 custom;
	guint8 unknown1[5];
} __attribute__ ((packed));

typedef enum {
	SOVA_LIGHT_CONTROL_CUSTOM = 0x1,
	SOVA_LIGHT_CONTROL_STORED = 0x0,
} SovaLightControlCustom;

gboolean sova_light_control_write(RoccatDevice *device, SovaLightControl *light_control, GError **error);
SovaLightControl *sova_light_control_read(RoccatDevice *device, GError **error);

gboolean sova_light_control_custom(RoccatDevice *device, gboolean on, GError **error);

G_END_DECLS

#endif
