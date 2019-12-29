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

#include "kova2016_profile_data_hardware.h"
#include <string.h>

gboolean kova2016_profile_data_hardware_get_modified(Kova2016ProfileDataHardware const *profile_data) {
	guint i;

	if (profile_data->modified_profile_buttons ||
			profile_data->modified_profile_settings)
		return TRUE;

	for (i = 0; i < KOVA2016_PROFILE_BUTTON_NUM; ++i)
		if (profile_data->modified_macro[i])
			return TRUE;

	return FALSE;
}

static void kova2016_profile_data_hardware_set_modified_state(Kova2016ProfileDataHardware *profile_data, gboolean state) {
	guint i;

	profile_data->modified_profile_buttons = state;
	profile_data->modified_profile_settings = state;

	for (i = 0; i < KOVA2016_PROFILE_BUTTON_NUM; ++i)
		profile_data->modified_macro[i] = state;
}

void kova2016_profile_data_hardware_set_modified(Kova2016ProfileDataHardware *profile_data) {
	kova2016_profile_data_hardware_set_modified_state(profile_data, TRUE);
}

void kova2016_profile_data_hardware_set_unmodified(Kova2016ProfileDataHardware *profile_data) {
	kova2016_profile_data_hardware_set_modified_state(profile_data, FALSE);
}

void kova2016_profile_data_hardware_set_profile_settings(Kova2016ProfileDataHardware *profile_data, Kova2016ProfileSettings const *profile_settings) {
	if (!kova2016_profile_settings_equal(&profile_data->profile_settings, profile_settings)) {
		kova2016_profile_settings_copy(&profile_data->profile_settings, profile_settings);
		profile_data->modified_profile_settings = TRUE;
	}
}

void kova2016_profile_data_hardware_set_profile_buttons(Kova2016ProfileDataHardware *profile_data, Kova2016ProfileButtons const *profile_buttons) {
	if (!kova2016_profile_buttons_equal(&profile_data->profile_buttons, profile_buttons)) {
		kova2016_profile_buttons_copy(&profile_data->profile_buttons, profile_buttons);
		profile_data->modified_profile_buttons = TRUE;
	}
}

void kova2016_profile_data_hardware_set_macro(Kova2016ProfileDataHardware *profile_data, guint index, Kova2016Macro const *macro) {
	if (!kova2016_macro_equal(&profile_data->macros[index], macro)) {
		kova2016_macro_copy(&profile_data->macros[index], macro);
		profile_data->modified_macro[index] = TRUE;
	}
}

void kova2016_profile_data_hardware_update_with_default(Kova2016ProfileDataHardware *profile_data) {
	memset(profile_data, 0, sizeof(Kova2016ProfileDataHardware));
	kova2016_profile_data_hardware_set_profile_settings(profile_data, kova2016_profile_settings_default());
	kova2016_profile_data_hardware_set_profile_buttons(profile_data, kova2016_profile_buttons_default());

	kova2016_profile_data_hardware_set_modified(profile_data);
}

gboolean kova2016_profile_data_hardware_update(Kova2016ProfileDataHardware *profile_data, RoccatDevice *device, guint profile_index, GError **error) {
	guint i;
	void *pointer;

	pointer = kova2016_profile_settings_read(device, profile_index, error);
	if (!pointer)
		return FALSE;
	kova2016_profile_data_hardware_set_profile_settings(profile_data, (Kova2016ProfileSettings const *)pointer);
	g_free(pointer);

	pointer = kova2016_profile_buttons_read(device, profile_index, error);
	if (!pointer)
		return FALSE;
	kova2016_profile_data_hardware_set_profile_buttons(profile_data, (Kova2016ProfileButtons const *)pointer);
	g_free(pointer);

	for (i = 0; i < KOVA2016_PROFILE_BUTTON_NUM; ++i) {
		if (profile_data->profile_buttons.buttons[i].type == KOVA2016_BUTTON_TYPE_MACRO) {
			pointer = kova2016_macro_read(device, profile_index, i, error);
			if (!pointer)
				return FALSE;
			kova2016_profile_data_hardware_set_macro(profile_data, i, (Kova2016Macro const *)pointer);
			g_free(pointer);
		}
	}

	kova2016_profile_data_hardware_set_unmodified(profile_data);

	return TRUE;
}

gboolean kova2016_profile_data_hardware_save(RoccatDevice *device, Kova2016ProfileDataHardware *profile_data, guint profile_index, GError **error) {
	guint i;

	if (profile_data->modified_profile_settings)
		if (!kova2016_profile_settings_write(device, profile_index, &profile_data->profile_settings, error))
			return FALSE;

	if (profile_data->modified_profile_buttons)
		if (!kova2016_profile_buttons_write(device, profile_index, &profile_data->profile_buttons, error))
			return FALSE;

	for (i = 0; i < KOVA2016_PROFILE_BUTTON_NUM; ++i) {
		if (profile_data->profile_buttons.buttons[i].type == KOVA2016_BUTTON_TYPE_MACRO && profile_data->modified_macro[i])
			if (!kova2016_macro_write(device, profile_index, i, &profile_data->macros[i], error))
				return FALSE;
	}

	kova2016_profile_data_hardware_set_unmodified(profile_data);

	return TRUE;
}
