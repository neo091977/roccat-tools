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

#include "skeltr_profile.h"
#include "skeltr.h"

SkeltrProfile *skeltr_profile_read(RoccatDevice *device, GError **error) {
	return (SkeltrProfile *)skeltr_device_read(device, SKELTR_REPORT_ID_PROFILE, sizeof(SkeltrProfile), error);
}

static void skeltr_profile_finalize(SkeltrProfile *profile) {
	profile->report_id = SKELTR_REPORT_ID_PROFILE;
	profile->size = sizeof(SkeltrProfile);
}

gboolean skeltr_profile_write(RoccatDevice *device, SkeltrProfile *profile, GError **error) {
	skeltr_profile_finalize(profile);
	return skeltr_device_write(device, (gchar const *)profile, sizeof(SkeltrProfile), error);
}

guint skeltr_profile_read_index(RoccatDevice *device, GError **error) {
	SkeltrProfile *profile;
	guint index;
	profile = skeltr_profile_read(device, error);
	if (profile == NULL)
		return 0;
	index = profile->index;
	g_free(profile);
	return index;
}

gboolean skeltr_profile_write_index(RoccatDevice *device, guint index, guint count, GError **error) {
	SkeltrProfile profile;
	profile.index = index;
	profile.count = count;
	return skeltr_profile_write(device, &profile, error);
}
