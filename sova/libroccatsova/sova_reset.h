#ifndef __ROCCAT_SOVA_RESET_H__
#define __ROCCAT_SOVA_RESET_H__

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

#include "sova_device.h"

G_BEGIN_DECLS

typedef struct _SovaReset SovaReset;

struct _SovaReset {
	guint8 report_id; /* SOVA_REPORT_ID_RESET */
	guint8 size; /* always 0x03 */
	guint8 function;
} __attribute__ ((packed));

typedef enum {
	SOVA_RESET_FUNCTION_RESET = 0x4,
} SovaResetFunction;

gboolean sova_reset(RoccatDevice *sova, guint function, GError **error);

G_END_DECLS

#endif
