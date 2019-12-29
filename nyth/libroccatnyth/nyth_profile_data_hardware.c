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

#include "nyth_profile_data_hardware.h"
#include <string.h>

gboolean nyth_profile_data_hardware_get_modified(NythProfileDataHardware const *profile_data) {
	guint i;

	if (profile_data->modified_profile_buttons ||
			profile_data->modified_profile_settings ||
			profile_data->modified_button_set)
		return TRUE;

	for (i = 0; i < NYTH_PROFILE_BUTTON_NUM; ++i)
		if (profile_data->modified_macro[i])
			return TRUE;

	return FALSE;
}

static void nyth_profile_data_hardware_set_modified_state(NythProfileDataHardware *profile_data, gboolean state) {
	guint i;

	profile_data->modified_profile_buttons = state;
	profile_data->modified_profile_settings = state;
	profile_data->modified_button_set = state;

	for (i = 0; i < NYTH_PROFILE_BUTTON_NUM; ++i)
		profile_data->modified_macro[i] = state;
}

void nyth_profile_data_hardware_set_modified(NythProfileDataHardware *profile_data) {
	nyth_profile_data_hardware_set_modified_state(profile_data, TRUE);
}

void nyth_profile_data_hardware_set_unmodified(NythProfileDataHardware *profile_data) {
	nyth_profile_data_hardware_set_modified_state(profile_data, FALSE);
}

void nyth_profile_data_hardware_set_profile_settings(NythProfileDataHardware *profile_data, NythProfileSettings const *profile_settings) {
	if (!nyth_profile_settings_equal(&profile_data->profile_settings, profile_settings)) {
		nyth_profile_settings_copy(&profile_data->profile_settings, profile_settings);
		profile_data->modified_profile_settings = TRUE;
	}
}

void nyth_profile_data_hardware_set_profile_buttons(NythProfileDataHardware *profile_data, NythProfileButtons const *profile_buttons) {
	if (!nyth_profile_buttons_equal(&profile_data->profile_buttons, profile_buttons)) {
		nyth_profile_buttons_copy(&profile_data->profile_buttons, profile_buttons);
		profile_data->modified_profile_buttons = TRUE;
	}
}

void nyth_profile_data_hardware_set_macro(NythProfileDataHardware *profile_data, guint index, NythMacro const *macro) {
	if (!nyth_macro_equal(&profile_data->macros[index], macro)) {
		nyth_macro_copy(&profile_data->macros[index], macro);
		profile_data->modified_macro[index] = TRUE;
	}
}

void nyth_profile_data_hardware_set_button_set(NythProfileDataHardware *profile_data, NythButtonSet const *button_set) {
	if (!nyth_button_set_equal(&profile_data->button_set, button_set)) {
		nyth_button_set_copy(&profile_data->button_set, button_set);
		profile_data->modified_button_set = TRUE;
	}
}

void nyth_profile_data_hardware_update_with_default(NythProfileDataHardware *profile_data) {
	memset(profile_data, 0, sizeof(NythProfileDataHardware));
	nyth_profile_data_hardware_set_profile_settings(profile_data, nyth_profile_settings_default());
	nyth_profile_data_hardware_set_profile_buttons(profile_data, nyth_profile_buttons_default());
	nyth_profile_data_hardware_set_button_set(profile_data, nyth_button_set_default());

	nyth_profile_data_hardware_set_modified(profile_data);
}

gboolean nyth_profile_data_hardware_update(NythProfileDataHardware *profile_data, RoccatDevice *device, guint profile_index, GError **error) {
	guint i;
	void *pointer;

	pointer = nyth_profile_settings_read(device, profile_index, error);
	if (!pointer)
		return FALSE;
	nyth_profile_data_hardware_set_profile_settings(profile_data, (NythProfileSettings const *)pointer);
	g_free(pointer);

	pointer = nyth_profile_buttons_read(device, profile_index, error);
	if (!pointer)
		return FALSE;
	nyth_profile_data_hardware_set_profile_buttons(profile_data, (NythProfileButtons const *)pointer);
	g_free(pointer);

	pointer = nyth_button_set_read(device, profile_index, error);
	if (!pointer)
		return FALSE;
	nyth_profile_data_hardware_set_button_set(profile_data, (NythButtonSet const *)pointer);
	g_free(pointer);

	for (i = 0; i < NYTH_PROFILE_BUTTON_NUM; ++i) {
		if (profile_data->profile_buttons.buttons[i].type == NYTH_BUTTON_TYPE_MACRO) {
			pointer = nyth_macro_read(device, profile_index, i, error);
			if (!pointer)
				return FALSE;
			nyth_profile_data_hardware_set_macro(profile_data, i, (NythMacro const *)pointer);
			g_free(pointer);
		}
	}

	nyth_profile_data_hardware_set_unmodified(profile_data);

	return TRUE;
}

gboolean nyth_profile_data_hardware_save(RoccatDevice *device, NythProfileDataHardware *profile_data, guint profile_index, GError **error) {
	guint i;

	if (profile_data->modified_profile_settings)
		if (!nyth_profile_settings_write(device, profile_index, &profile_data->profile_settings, error))
			return FALSE;

	if (profile_data->modified_profile_buttons)
		if (!nyth_profile_buttons_write(device, profile_index, &profile_data->profile_buttons, error))
			return FALSE;

	if (profile_data->modified_button_set)
		if (!nyth_button_set_write(device, profile_index, &profile_data->button_set, error))
			return FALSE;

	for (i = 0; i < NYTH_PROFILE_BUTTON_NUM; ++i) {
		if (profile_data->profile_buttons.buttons[i].type == NYTH_BUTTON_TYPE_MACRO && profile_data->modified_macro[i])
			if (!nyth_macro_write(device, profile_index, i, &profile_data->macros[i], error))
				return FALSE;
	}

	nyth_profile_data_hardware_set_unmodified(profile_data);

	return TRUE;
}
