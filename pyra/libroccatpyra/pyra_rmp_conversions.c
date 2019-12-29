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

#include "pyra_rmp.h"

static void pyra_profile_buttons_set_button(PyraRMP *rmp, guint rmp_index,
		PyraProfileButtons *profile_buttons, guint button_index) {
	PyraRMPMacroKeyInfo *macro_key_info;
	macro_key_info = pyra_rmp_get_macro_key_info(rmp, rmp_index);
	profile_buttons->buttons[button_index] = macro_key_info->type;
	g_free(macro_key_info);
}

static void pyra_profile_button_to_rmp_macro_key_info(PyraProfileButtons const *profile_buttons, guint button_index,
		PyraRMP *rmp, guint rmp_index) {
	PyraRMPMacroKeyInfo macro_key_info;

	macro_key_info.type = profile_buttons->buttons[button_index];

	pyra_rmp_set_macro_key_info(rmp, rmp_index, &macro_key_info);
}

PyraProfileButtons *pyra_rmp_to_profile_buttons(PyraRMP *rmp) {
	PyraProfileButtons *result;
	result = g_malloc0(sizeof(PyraProfileButtons));

	result->report_id = PYRA_REPORT_ID_PROFILE_BUTTONS;
	result->size = sizeof(PyraProfileButtons);

	pyra_profile_buttons_set_button(rmp, PYRA_RMP_MACRO_KEY_INDEX_LEFT, result, PYRA_PROFILE_BUTTON_INDEX_LEFT);
	pyra_profile_buttons_set_button(rmp, PYRA_RMP_MACRO_KEY_INDEX_RIGHT, result, PYRA_PROFILE_BUTTON_INDEX_RIGHT);
	pyra_profile_buttons_set_button(rmp, PYRA_RMP_MACRO_KEY_INDEX_MIDDLE, result, PYRA_PROFILE_BUTTON_INDEX_MIDDLE);
	pyra_profile_buttons_set_button(rmp, PYRA_RMP_MACRO_KEY_INDEX_WHEEL_UP, result, PYRA_PROFILE_BUTTON_INDEX_WHEEL_UP);
	pyra_profile_buttons_set_button(rmp, PYRA_RMP_MACRO_KEY_INDEX_WHEEL_DOWN, result, PYRA_PROFILE_BUTTON_INDEX_WHEEL_DOWN);
	pyra_profile_buttons_set_button(rmp, PYRA_RMP_MACRO_KEY_INDEX_SHIFT_LEFT, result, PYRA_PROFILE_BUTTON_INDEX_SHIFT_LEFT);
	pyra_profile_buttons_set_button(rmp, PYRA_RMP_MACRO_KEY_INDEX_SHIFT_RIGHT, result, PYRA_PROFILE_BUTTON_INDEX_SHIFT_RIGHT);
	pyra_profile_buttons_set_button(rmp, PYRA_RMP_MACRO_KEY_INDEX_SHIFT_MIDDLE, result, PYRA_PROFILE_BUTTON_INDEX_SHIFT_MIDDLE);
	pyra_profile_buttons_set_button(rmp, PYRA_RMP_MACRO_KEY_INDEX_SHIFT_WHEEL_UP, result, PYRA_PROFILE_BUTTON_INDEX_SHIFT_WHEEL_UP);
	pyra_profile_buttons_set_button(rmp, PYRA_RMP_MACRO_KEY_INDEX_SHIFT_WHEEL_DOWN, result, PYRA_PROFILE_BUTTON_INDEX_SHIFT_WHEEL_DOWN);
	pyra_profile_buttons_set_button(rmp, PYRA_RMP_MACRO_KEY_INDEX_LEFT_SIDE, result, PYRA_PROFILE_BUTTON_INDEX_LEFT_SIDE);
	pyra_profile_buttons_set_button(rmp, PYRA_RMP_MACRO_KEY_INDEX_RIGHT_SIDE, result, PYRA_PROFILE_BUTTON_INDEX_RIGHT_SIDE);
	pyra_profile_buttons_set_button(rmp, PYRA_RMP_MACRO_KEY_INDEX_SHIFT_LEFT_SIDE, result, PYRA_PROFILE_BUTTON_INDEX_SHIFT_LEFT_SIDE);
	pyra_profile_buttons_set_button(rmp, PYRA_RMP_MACRO_KEY_INDEX_SHIFT_RIGHT_SIDE, result, PYRA_PROFILE_BUTTON_INDEX_SHIFT_RIGHT_SIDE);

	return result;
}

PyraProfileSettings *pyra_rmp_to_profile_settings(PyraRMP *rmp) {
	PyraProfileSettings *result;
	result = g_malloc0(sizeof(PyraProfileSettings));

	result->report_id = PYRA_REPORT_ID_PROFILE_SETTINGS;
	result->size = sizeof(PyraProfileSettings);
	result->xy_synchronous = pyra_rmp_get_xy_synchronous(rmp);
	result->x_sensitivity = pyra_rmp_get_sensitivity_x(rmp);
	result->y_sensitivity = pyra_rmp_get_sensitivity_y(rmp);
	result->x_cpi = pyra_rmp_get_cpi_x(rmp);
	result->y_cpi = pyra_rmp_get_cpi_y(rmp);
	result->lightswitch = pyra_rmp_get_light_switch(rmp);
	result->lighttype = pyra_rmp_get_light_type(rmp);
	result->orientation = pyra_rmp_get_orientation(rmp);

	return result;
}

void pyra_rmp_update_with_profile_buttons(PyraRMP *rmp, PyraProfileButtons const *profile_buttons) {
	pyra_profile_button_to_rmp_macro_key_info(profile_buttons, PYRA_PROFILE_BUTTON_INDEX_LEFT, rmp, PYRA_RMP_MACRO_KEY_INDEX_LEFT);
	pyra_profile_button_to_rmp_macro_key_info(profile_buttons, PYRA_PROFILE_BUTTON_INDEX_RIGHT, rmp, PYRA_RMP_MACRO_KEY_INDEX_RIGHT);
	pyra_profile_button_to_rmp_macro_key_info(profile_buttons, PYRA_PROFILE_BUTTON_INDEX_MIDDLE, rmp, PYRA_RMP_MACRO_KEY_INDEX_MIDDLE);
	pyra_profile_button_to_rmp_macro_key_info(profile_buttons, PYRA_PROFILE_BUTTON_INDEX_WHEEL_UP, rmp, PYRA_RMP_MACRO_KEY_INDEX_WHEEL_UP);
	pyra_profile_button_to_rmp_macro_key_info(profile_buttons, PYRA_PROFILE_BUTTON_INDEX_WHEEL_DOWN, rmp, PYRA_RMP_MACRO_KEY_INDEX_WHEEL_DOWN);
	pyra_profile_button_to_rmp_macro_key_info(profile_buttons, PYRA_PROFILE_BUTTON_INDEX_SHIFT_LEFT, rmp, PYRA_RMP_MACRO_KEY_INDEX_SHIFT_LEFT);
	pyra_profile_button_to_rmp_macro_key_info(profile_buttons, PYRA_PROFILE_BUTTON_INDEX_SHIFT_RIGHT, rmp, PYRA_RMP_MACRO_KEY_INDEX_SHIFT_RIGHT);
	pyra_profile_button_to_rmp_macro_key_info(profile_buttons, PYRA_PROFILE_BUTTON_INDEX_SHIFT_MIDDLE, rmp, PYRA_RMP_MACRO_KEY_INDEX_SHIFT_MIDDLE);
	pyra_profile_button_to_rmp_macro_key_info(profile_buttons, PYRA_PROFILE_BUTTON_INDEX_SHIFT_WHEEL_UP, rmp, PYRA_RMP_MACRO_KEY_INDEX_SHIFT_WHEEL_UP);
	pyra_profile_button_to_rmp_macro_key_info(profile_buttons, PYRA_PROFILE_BUTTON_INDEX_SHIFT_WHEEL_DOWN, rmp, PYRA_RMP_MACRO_KEY_INDEX_SHIFT_WHEEL_DOWN);
	pyra_profile_button_to_rmp_macro_key_info(profile_buttons, PYRA_PROFILE_BUTTON_INDEX_LEFT_SIDE, rmp, PYRA_RMP_MACRO_KEY_INDEX_LEFT_SIDE);
	pyra_profile_button_to_rmp_macro_key_info(profile_buttons, PYRA_PROFILE_BUTTON_INDEX_RIGHT_SIDE, rmp, PYRA_RMP_MACRO_KEY_INDEX_RIGHT_SIDE);
	pyra_profile_button_to_rmp_macro_key_info(profile_buttons, PYRA_PROFILE_BUTTON_INDEX_SHIFT_LEFT_SIDE, rmp, PYRA_RMP_MACRO_KEY_INDEX_SHIFT_LEFT_SIDE);
	pyra_profile_button_to_rmp_macro_key_info(profile_buttons, PYRA_PROFILE_BUTTON_INDEX_SHIFT_RIGHT_SIDE, rmp, PYRA_RMP_MACRO_KEY_INDEX_SHIFT_RIGHT_SIDE);
}

void pyra_rmp_update_with_profile_settings(PyraRMP *rmp, PyraProfileSettings const *profile_settings) {
	pyra_rmp_set_xy_synchronous(rmp, profile_settings->xy_synchronous);
	pyra_rmp_set_sensitivity_x(rmp, profile_settings->x_sensitivity);
	pyra_rmp_set_sensitivity_y(rmp, profile_settings->y_sensitivity);
	pyra_rmp_set_cpi_x(rmp, profile_settings->x_cpi);
	pyra_rmp_set_cpi_y(rmp, profile_settings->y_cpi);
	pyra_rmp_set_light_switch(rmp, profile_settings->lightswitch);
	pyra_rmp_set_light_type(rmp, profile_settings->lighttype);
	pyra_rmp_set_orientation(rmp, profile_settings->orientation);
}
