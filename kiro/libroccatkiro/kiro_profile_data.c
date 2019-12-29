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

#include "kiro_profile_data.h"

KiroProfileData *kiro_profile_data_new(void) {
	KiroProfileData *profile_data = (KiroProfileData *)g_malloc0(sizeof(KiroProfileData));
	kiro_profile_data_eventhandler_update_with_default(&profile_data->eventhandler);
	kiro_profile_data_hardware_update_with_default(&profile_data->hardware);
	return profile_data;
}

gboolean kiro_profile_data_update_filesystem(KiroProfileData *profile_data, GError **error) {
	GError *local_error = NULL;
	if (!kiro_profile_data_eventhandler_update(&profile_data->eventhandler, &local_error)) {
		if (g_error_matches(local_error, G_FILE_ERROR, G_FILE_ERROR_NOENT)) {
			kiro_profile_data_eventhandler_update_with_default(&profile_data->eventhandler);
			return TRUE;
		} else {
			g_propagate_error(error, local_error);
			return FALSE;
		}
	}
	return TRUE;
}

gboolean kiro_profile_data_update_hardware(KiroProfileData *profile_data, RoccatDevice *device, GError **error) {
	return kiro_profile_data_hardware_update(&profile_data->hardware, device, error);
}

gboolean kiro_profile_data_save(RoccatDevice *device, KiroProfileData *profile_data, GError **error) {
	if (!kiro_profile_data_eventhandler_save(&profile_data->eventhandler, error))
		return FALSE;
	return kiro_profile_data_hardware_save(device, &profile_data->hardware, error);
}

void kiro_profile_data_set_modified(KiroProfileData *profile_data) {
	kiro_profile_data_eventhandler_set_modified(&profile_data->eventhandler);
	kiro_profile_data_hardware_set_modified(&profile_data->hardware);
}

gboolean kiro_profile_data_get_modified(KiroProfileData const *profile_data) {
	return kiro_profile_data_eventhandler_get_modified(&profile_data->eventhandler) ||
			kiro_profile_data_hardware_get_modified(&profile_data->hardware);
}

KiroProfileData *kiro_profile_data_dup(KiroProfileData const *other) {
	return (KiroProfileData *)g_memdup((gconstpointer)other, sizeof(KiroProfileData));
}
