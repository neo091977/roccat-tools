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

#include "savu.h"
#include <string.h>
#include <stdlib.h>

guint savu_profile_read(RoccatDevice *savu, GError **error) {
	guint result;
	SavuProfile *profile;

	profile = (SavuProfile *)savu_device_read(savu, SAVU_REPORT_ID_PROFILE, sizeof(SavuProfile), error);
	if (*error)
		return 0;

	result = profile->profile_index;
	g_free(profile);
	return result;
}

gboolean savu_profile_write(RoccatDevice *savu, guint profile_index, GError **error) {
	SavuProfile profile;

	g_assert(profile_index < SAVU_PROFILE_NUM);

	profile.report_id = SAVU_REPORT_ID_PROFILE;
	profile.size = sizeof(SavuProfile);
	profile.profile_index = profile_index;

	return savu_device_write(savu, (gchar const *)&profile, sizeof(SavuProfile), error);
}
