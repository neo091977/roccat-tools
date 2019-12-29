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

#include "koneplus.h"

guint koneplus_actual_profile_read(RoccatDevice *device, GError **error) {
	guint result;
	KoneplusActualProfile *profile;

	profile = (KoneplusActualProfile *)koneplus_device_read(device, KONEPLUS_REPORT_ID_ACTUAL_PROFILE, sizeof(KoneplusActualProfile), error);
	if (*error)
		return 0;

	result = profile->profile_index;
	g_free(profile);
	return result;
}

gboolean koneplus_actual_profile_write(RoccatDevice *device, guint profile_index, GError **error) {
	KoneplusActualProfile profile;

	g_assert(profile_index < KONEPLUS_PROFILE_NUM);

	profile.report_id = KONEPLUS_REPORT_ID_ACTUAL_PROFILE;
	profile.size = sizeof(KoneplusActualProfile);
	profile.profile_index = profile_index;

	return koneplus_device_write(device, (gchar const *)&profile, sizeof(KoneplusActualProfile), error);
}
