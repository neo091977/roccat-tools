#ifndef __ROCCAT_NYTH_PROFILE_H__
#define __ROCCAT_NYTH_PROFILE_H__

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

#include "nyth_device.h"

G_BEGIN_DECLS

typedef struct _NythProfile NythProfile;

struct _NythProfile {
	guint8 report_id; /* NYTH_REPORT_ID_PROFILE */
	guint8 size; /* always 0x04 */
	guint8 index;
	guint8 count;
} __attribute__ ((packed));

NythProfile *nyth_profile_read(RoccatDevice *device, GError **error);
gboolean nyth_profile_write(RoccatDevice *device, NythProfile *profile, GError **error);

guint nyth_profile_read_index(RoccatDevice *device, GError **error);
gboolean nyth_profile_write_index(RoccatDevice *device, guint index, guint count, GError **error);

G_END_DECLS

#endif
