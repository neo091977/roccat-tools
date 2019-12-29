#ifndef __ROCCAT_SKELTR_LIGHT_CUSTOM_H__
#define __ROCCAT_SKELTR_LIGHT_CUSTOM_H__

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

#include "skeltr_light.h"

G_BEGIN_DECLS

typedef struct _SkeltrLightCustom SkeltrLightCustom;
typedef struct _SkeltrLightCustomColor SkeltrLightCustomColor;

struct _SkeltrLightCustomColor {
	guint8 red, green, blue;
} __attribute__ ((packed));

struct _SkeltrLightCustom {
	guint8 report_id; /* SKELTR_REPORT_ID_LIGHT_CUSTOM */
	guint8 size; /* 0x11 */
	SkeltrLightCustomColor colors[SKELTR_LIGHTS_NUM]; /* SkeltrLightColorIndex */
} __attribute__ ((packed));

gboolean skeltr_light_custom_write(RoccatDevice *device, SkeltrLightCustom *custom, GError **error);

G_END_DECLS

#endif
