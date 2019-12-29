#ifndef __ROCCAT_RYOS_LIGHT_H__
#define __ROCCAT_RYOS_LIGHT_H__

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

#include "ryos.h"

G_BEGIN_DECLS

typedef struct _RyosLight RyosLight;

struct _RyosLight {
	guint8 report_id; /* RYOS_REPORT_ID_LIGHT */
	guint8 size; /* always 0x10 */
	guint8 profile_index;
	guint8 brightness; /* 0-5 */
	guint8 dimness;
	guint8 timeout; /* minutes */
	guint8 unknown1;
	guint8 unknown2;
	guint8 unknown3;
	guint8 unknown4;
	guint8 illumination_mode;
	guint8 macro_exec_feedback;
	guint8 dimness_type;
	guint8 unknown5;
	guint16 checksum;
} __attribute__ ((packed));

typedef enum {
	RYOS_LIGHT_MACRO_EXEC_FEEDBACK_ON = 1,
	RYOS_LIGHT_MACRO_EXEC_FEEDBACK_OFF = 0,
} RyosLightMacroExecFeedback;

typedef enum {
	RYOS_LIGHT_DIMNESS_TYPE_VALUE = 0,
	RYOS_LIGHT_DIMNESS_TYPE_STARLIT_SKY = 1,
	RYOS_LIGHT_DIMNESS_TYPE_MACRO = 2,
	RYOS_LIGHT_DIMNESS_TYPE_FALL_ASLEEP = 3,
} RyosLightDimnessType;

gboolean ryos_light_write(RoccatDevice *ryos, guint profile_index, RyosLight *light, GError **error);
RyosLight *ryos_light_read(RoccatDevice *ryos, guint profile_index, GError **error);
gboolean ryos_light_equal(RyosLight const *left, RyosLight const *right);
void ryos_light_copy(RyosLight *destination, RyosLight const *source);
RyosLight const *ryos_light_default(void);

G_END_DECLS

#endif
