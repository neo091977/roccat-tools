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

#include "kiro_profile_data_hardware.h"
#include "kiro_dcu.h"
#include <string.h>

gboolean kiro_profile_data_hardware_get_modified(KiroProfileDataHardware const *profile_data) {
	return profile_data->modified_profile || profile_data->modified_dcu;
}

static void kiro_profile_data_hardware_set_modified_state(KiroProfileDataHardware *profile_data, gboolean state) {
	profile_data->modified_profile = state;
	profile_data->modified_dcu = state;
}

void kiro_profile_data_hardware_set_modified(KiroProfileDataHardware *profile_data) {
	kiro_profile_data_hardware_set_modified_state(profile_data, TRUE);
}

void kiro_profile_data_hardware_set_unmodified(KiroProfileDataHardware *profile_data) {
	kiro_profile_data_hardware_set_modified_state(profile_data, FALSE);
}

void kiro_profile_data_hardware_set_profile(KiroProfileDataHardware *profile_data, KiroProfile const *profile) {
	if (!kiro_profile_equal(&profile_data->profile, profile)) {
		kiro_profile_copy(&profile_data->profile, profile);
		profile_data->modified_profile = TRUE;
	}
}

void kiro_profile_data_hardware_set_dcu(KiroProfileDataHardware *profile_data, guint8 new_dcu) {
	if (profile_data->dcu != new_dcu) {
		profile_data->dcu = new_dcu;
		profile_data->modified_dcu = TRUE;
	}
}

void kiro_profile_data_hardware_update_with_default(KiroProfileDataHardware *profile_data) {
	memset(profile_data, 0, sizeof(KiroProfileDataHardware));
	kiro_profile_data_hardware_set_profile(profile_data, kiro_profile_default());
	profile_data->dcu = KIRO_DCU_NORMAL;
	kiro_profile_data_hardware_set_modified(profile_data);
}

gboolean kiro_profile_data_hardware_update(KiroProfileDataHardware *profile_data, RoccatDevice *device, GError **error) {
	GError *local_error = NULL;
	void *pointer;
	guint dcu;
	
	pointer = kiro_profile_read(device, &local_error);
	if (!pointer) {
		g_propagate_error(error, local_error);
		return FALSE;
	}
	kiro_profile_data_hardware_set_profile(profile_data, (KiroProfile const *)pointer);
	g_free(pointer);

	dcu = kiro_dcu_get(device, &local_error);
	if (local_error) {
		g_propagate_error(error, local_error);
		return FALSE;
	}
	profile_data->dcu = dcu;
	
	kiro_profile_data_hardware_set_unmodified(profile_data);

	return TRUE;
}

gboolean kiro_profile_data_hardware_save(RoccatDevice *device, KiroProfileDataHardware *profile_data, GError **error) {
	if (profile_data->modified_profile)
		if (!kiro_profile_write(device, &profile_data->profile, error))
			return FALSE;

	if (profile_data->modified_dcu)
		if (!kiro_dcu_set(device, profile_data->dcu, error))
			return FALSE;

	kiro_profile_data_hardware_set_unmodified(profile_data);

	return TRUE;
}
