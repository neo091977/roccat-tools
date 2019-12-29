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

#include "tyon_profile.h"
#include "tyon_device.h"
#include "tyon.h"

typedef struct _TyonProfile TyonProfile;

struct _TyonProfile {
	guint8 report_id; /* TYON_REPORT_ID_PROFILE */
	guint8 size; /* always 0x03 */
	guint8 profile_index;
} __attribute__ ((packed));

guint tyon_profile_read(RoccatDevice *tyon, GError **error) {
	TyonProfile *profile;
	guint result;

	profile = (TyonProfile *)tyon_device_read(tyon, TYON_REPORT_ID_PROFILE, sizeof(TyonProfile), error);
	if (*error)
		return 0;

	result = profile->profile_index;
	g_free(profile);
	return result;
}

gboolean tyon_profile_write(RoccatDevice *tyon, guint profile_index, GError **error) {
	TyonProfile profile;

	g_assert(profile_index < TYON_PROFILE_NUM);

	profile.report_id = TYON_REPORT_ID_PROFILE;
	profile.size = sizeof(TyonProfile);
	profile.profile_index = profile_index;

	return tyon_device_write(tyon, (gchar const *)&profile, sizeof(TyonProfile), error);
}
