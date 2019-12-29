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

#include "isku.h"

guint isku_actual_profile_read(RoccatDevice *device, GError **error) {
	guint result;
	IskuActualProfile *profile;

	profile = (IskuActualProfile *)isku_device_read(device, ISKU_REPORT_ID_ACTUAL_PROFILE, sizeof(IskuActualProfile), error);
	if (*error)
		return 0;

	result = profile->actual_profile;
	g_free(profile);
	return result;
}

gboolean isku_actual_profile_write(RoccatDevice *device, guint profile_index, GError **error) {
	IskuActualProfile profile;

	g_assert(profile_index < ISKU_PROFILE_NUM);

	profile.report_id = ISKU_REPORT_ID_ACTUAL_PROFILE;
	profile.size = sizeof(IskuActualProfile);
	profile.actual_profile = profile_index;

	return isku_device_write(device, (gchar const *)&profile, sizeof(IskuActualProfile), error);
}
