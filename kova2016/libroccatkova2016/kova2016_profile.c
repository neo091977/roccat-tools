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

#include "kova2016_profile.h"
#include "kova2016_device.h"
#include "kova2016.h"

Kova2016Profile *kova2016_profile_read(RoccatDevice *device, GError **error) {
	return (Kova2016Profile *)kova2016_device_read(device, KOVA2016_REPORT_ID_PROFILE, sizeof(Kova2016Profile), error);
}

static void kova2016_profile_finalize(Kova2016Profile *profile) {
	profile->report_id = KOVA2016_REPORT_ID_PROFILE;
	profile->size = sizeof(Kova2016Profile);
}

gboolean kova2016_profile_write(RoccatDevice *device, Kova2016Profile *profile, GError **error) {
	kova2016_profile_finalize(profile);
	return kova2016_device_write(device, (gchar const *)profile, sizeof(Kova2016Profile), error);
}

guint kova2016_profile_read_index(RoccatDevice *device, GError **error) {
	Kova2016Profile *profile;
	guint index;
	profile = kova2016_profile_read(device, error);
	if (profile == NULL)
		return 0;
	index = profile->index;
	g_free(profile);
	return index;
}

gboolean kova2016_profile_write_index(RoccatDevice *device, guint index, guint count, GError **error) {
	Kova2016Profile profile;
	profile.index = index;
	profile.count = count;
	return kova2016_profile_write(device, &profile, error);
}
