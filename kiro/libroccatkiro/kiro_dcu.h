#ifndef __ROCCAT_KIRO_DCU_H__
#define __ROCCAT_KIRO_DCU_H__

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

typedef struct _KiroDcu KiroDcu;

struct _KiroDcu {
	guint8 report_id; /* KIRO_REPORT_ID_DCU */
	guint8 size; /* always 3! */
	guint8 dcu; /* KiroDcuDcu */
	guint8 unused;
} __attribute__ ((packed));

typedef enum {
	KIRO_DCU_HIGH = 0,
	KIRO_DCU_LOW = 1,
	KIRO_DCU_NORMAL = 2,
} KiroDcuDcu;

KiroDcu *kiro_dcu_read(RoccatDevice *kiro, GError **error);

guint kiro_dcu_get(RoccatDevice *kiro, GError **error);
gboolean kiro_dcu_set(RoccatDevice *kiro, guint new_dcu, GError **error);

G_END_DECLS

#endif
