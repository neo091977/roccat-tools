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

#include "ryosmkfx_profile_data.h"

RyosmkfxProfileData *ryosmkfx_profile_data_new(void) {
	RyosmkfxProfileData *profile_data = (RyosmkfxProfileData *)g_malloc0(sizeof(RyosmkfxProfileData));
	ryosmkfx_profile_data_eventhandler_update_with_default(&profile_data->eventhandler);
	ryosmkfx_profile_data_hardware_update_with_default(&profile_data->hardware);
	return profile_data;
}

gboolean ryosmkfx_profile_data_update_filesystem(RyosmkfxProfileData *profile_data, guint profile_index, GError **error) {
	GError *local_error = NULL;
	if (!ryosmkfx_profile_data_eventhandler_update(&profile_data->eventhandler, profile_index, &local_error)) {
		if (g_error_matches(local_error, G_FILE_ERROR, G_FILE_ERROR_NOENT)) {
			ryosmkfx_profile_data_eventhandler_update_with_default(&profile_data->eventhandler);
			return TRUE;
		} else {
			g_propagate_error(error, local_error);
			return FALSE;
		}
	}
	return TRUE;
}

gboolean ryosmkfx_profile_data_update_hardware(RyosmkfxProfileData *profile_data, RoccatDevice *device, guint profile_index, GError **error) {
	return ryosmkfx_profile_data_hardware_update(&profile_data->hardware, device, profile_index, error);
}

gboolean ryosmkfx_profile_data_save(RoccatDevice *device, RyosmkfxProfileData *profile_data, guint profile_index, GError **error) {
	if (!ryosmkfx_profile_data_eventhandler_save(&profile_data->eventhandler, profile_index, error))
		return FALSE;
	return ryosmkfx_profile_data_hardware_save(device, &profile_data->hardware, profile_index, error);
}

void ryosmkfx_profile_data_set_modified(RyosmkfxProfileData *profile_data) {
	ryosmkfx_profile_data_eventhandler_set_modified(&profile_data->eventhandler);
	ryosmkfx_profile_data_hardware_set_modified(&profile_data->hardware);
}

gboolean ryosmkfx_profile_data_get_modified(RyosmkfxProfileData const *profile_data) {
	return ryosmkfx_profile_data_eventhandler_get_modified(&profile_data->eventhandler) ||
			ryosmkfx_profile_data_hardware_get_modified(&profile_data->hardware);
}

RyosmkfxProfileData *ryosmkfx_profile_data_dup(RyosmkfxProfileData const *other) {
	return (RyosmkfxProfileData *)g_memdup((gconstpointer)other, sizeof(RyosmkfxProfileData));
}
