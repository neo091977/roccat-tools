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

#include "ryos_profile.h"
#include "ryos_device.h"

guint ryos_profile_read(RoccatDevice *ryos, GError **error) {
	RyosProfile *profile;
	guint result;

	profile = (RyosProfile *)ryos_device_read(ryos, RYOS_REPORT_ID_PROFILE, sizeof(RyosProfile), error);
	if (*error)
		return 0;

	result = profile->profile_index;
	g_free(profile);
	return result;
}

gboolean ryos_profile_write(RoccatDevice *ryos, guint profile_index, GError **error) {
	RyosProfile profile;

	profile.report_id = RYOS_REPORT_ID_PROFILE;
	profile.size = sizeof(RyosProfile);
	profile.profile_index = profile_index;

	return ryos_device_write(ryos, (gchar const *)&profile, sizeof(RyosProfile), error);
}
