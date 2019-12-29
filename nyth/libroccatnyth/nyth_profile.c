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

#include "nyth_profile.h"
#include "nyth_device.h"
#include "nyth.h"

NythProfile *nyth_profile_read(RoccatDevice *device, GError **error) {
	return (NythProfile *)nyth_device_read(device, NYTH_REPORT_ID_PROFILE, sizeof(NythProfile), error);
}

static void nyth_profile_finalize(NythProfile *profile) {
	profile->report_id = NYTH_REPORT_ID_PROFILE;
	profile->size = sizeof(NythProfile);
}

gboolean nyth_profile_write(RoccatDevice *device, NythProfile *profile, GError **error) {
	nyth_profile_finalize(profile);
	return nyth_device_write(device, (gchar const *)profile, sizeof(NythProfile), error);
}

guint nyth_profile_read_index(RoccatDevice *device, GError **error) {
	NythProfile *profile;
	guint index;
	profile = nyth_profile_read(device, error);
	if (profile == NULL)
		return 0;
	index = profile->index;
	g_free(profile);
	return index;
}

gboolean nyth_profile_write_index(RoccatDevice *device, guint index, guint count, GError **error) {
	NythProfile profile;
	profile.index = index;
	profile.count = count;
	return nyth_profile_write(device, &profile, error);
}
