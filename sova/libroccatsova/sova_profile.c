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

#include "sova_profile.h"
#include "sova.h"

SovaProfile *sova_profile_read(RoccatDevice *device, GError **error) {
	return (SovaProfile *)sova_device_read(device, SOVA_REPORT_ID_PROFILE, sizeof(SovaProfile), error);
}

static void sova_profile_finalize(SovaProfile *profile) {
	profile->report_id = SOVA_REPORT_ID_PROFILE;
	profile->size = sizeof(SovaProfile);
}

gboolean sova_profile_write(RoccatDevice *device, SovaProfile *profile, GError **error) {
	sova_profile_finalize(profile);
	return sova_device_write(device, (gchar const *)profile, sizeof(SovaProfile), error);
}

guint sova_profile_read_index(RoccatDevice *device, GError **error) {
	SovaProfile *profile;
	guint index;
	profile = sova_profile_read(device, error);
	if (profile == NULL)
		return 0;
	index = profile->index;
	g_free(profile);
	return index;
}

gboolean sova_profile_write_index(RoccatDevice *device, guint index, guint count, GError **error) {
	SovaProfile profile;
	profile.index = index;
	profile.count = count;
	return sova_profile_write(device, &profile, error);
}
