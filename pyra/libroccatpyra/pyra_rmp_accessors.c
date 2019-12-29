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
#include "g_roccat_helper.h"
#include "i18n-lib.h"
#include <string.h>

static gchar const * const pyra_rmp_group_name = "Setting";

static gchar const * const pyra_rmp_profile_name_name = "ProFileName";
static gchar const * const pyra_rmp_orientation_name = "Orientation";
static gchar const * const pyra_rmp_xy_synchronous_name = "XYSynchronous";
static gchar const * const pyra_rmp_sensitivity_x_name = "Sensitivity_X";
static gchar const * const pyra_rmp_sensitivity_y_name = "Sensitivity_Y";
static gchar const * const pyra_rmp_cpi_x_name = "Cpi_X";
static gchar const * const pyra_rmp_cpi_y_name = "Cpi_Y";
static gchar const * const pyra_rmp_light_switch_name = "LightSwitch";
static gchar const * const pyra_rmp_light_type_name = "LightType";

static gchar *pyra_rmp_create_game_file_name(guint index) {
	return g_strdup_printf("GameFile%i", index);
}

static gchar *pyra_rmp_create_macro_key_info_name(guint index) {
	g_assert(index < PYRA_BUTTONS_NUM);
	return g_strdup_printf("MacroKeyInfo%i", index);
}

static void pyra_rmp_macro_key_info_finalize(PyraRMPMacroKeyInfo *rmp_macro_key_info, guint index) {
	rmp_macro_key_info->number = index;
	pyra_rmp_macro_key_info_set_checksum(rmp_macro_key_info);
}

void pyra_rmp_set_macro_key_info(PyraRMP *rmp, guint index, PyraRMPMacroKeyInfo *rmp_macro_key_info) {
	PyraRMPMacroKeyInfo *actual_macro_key_info;
	gchar *key;
	actual_macro_key_info = pyra_rmp_get_macro_key_info(rmp, index);
	if (!pyra_rmp_macro_key_info_equal(rmp_macro_key_info, actual_macro_key_info)) {
		key = pyra_rmp_create_macro_key_info_name(index);
		pyra_rmp_macro_key_info_finalize(rmp_macro_key_info, index);
		roccat_key_file_set_binary(rmp->key_file, pyra_rmp_group_name, key, (gconstpointer)rmp_macro_key_info, sizeof(PyraRMPMacroKeyInfo));
		g_free(key);
		rmp->modified_profile_buttons = TRUE;
	}
	g_free(actual_macro_key_info);
}

void pyra_rmp_set_profile_name(PyraRMP *rmp, gchar const *string) {
	gchar *actual_string;
	actual_string = pyra_rmp_get_profile_name(rmp);
	if (strcmp(string, actual_string)) {
		g_key_file_set_string(rmp->key_file, pyra_rmp_group_name, pyra_rmp_profile_name_name, string);
		rmp->modified_rmp = TRUE;
	}
	g_free(actual_string);
}

/*
 * May terminate application if error occurs
 */
static guint pyra_rmp_get_default_value(gchar const *key) {
	PyraRMP const *default_rmp = pyra_default_rmp();
	GError *error = NULL;
	guint result;
	result = g_key_file_get_integer(default_rmp->key_file, pyra_rmp_group_name, key, &error);
	if (error)
		g_error(_("Could not get default value for key '%s': %s"), key, error->message);
	return result;
}

static guint pyra_rmp_get_value(PyraRMP *rmp, gchar const *key) {
	GError *error = NULL;
	guint result = g_key_file_get_integer(rmp->key_file, pyra_rmp_group_name, key, &error);
	if (error) {
		g_clear_error(&error);
		result = pyra_rmp_get_default_value(key);
	}
	return result;
}

static void pyra_rmp_set_value(PyraRMP *rmp, gchar const *key, guint new_value) {
	if (pyra_rmp_get_value(rmp, key) != new_value) {
		g_key_file_set_integer(rmp->key_file, pyra_rmp_group_name, key, new_value);
		rmp->modified_profile_settings = TRUE;
	}
}

void pyra_rmp_set_orientation(PyraRMP *rmp, guint value) {
	g_assert(value == PYRA_ORIENTATION_RIGHT || value == PYRA_ORIENTATION_LEFT);
	pyra_rmp_set_value(rmp, pyra_rmp_orientation_name, value);
}

void pyra_rmp_set_xy_synchronous(PyraRMP *rmp, guint value) {
 	g_assert(value == PYRA_XY_SYNC_ON || value == PYRA_XY_SYNC_OFF);
	pyra_rmp_set_value(rmp, pyra_rmp_xy_synchronous_name, value);
}

void pyra_rmp_set_sensitivity_x(PyraRMP *rmp, guint value) {
	g_assert(value >= PYRA_SENSITIVITY_MIN && value <= PYRA_SENSITIVITY_MAX);
	pyra_rmp_set_value(rmp, pyra_rmp_sensitivity_x_name, value);
}

void pyra_rmp_set_sensitivity_y(PyraRMP *rmp, guint value) {
	g_assert(value >= PYRA_SENSITIVITY_MIN && value <= PYRA_SENSITIVITY_MAX);
	pyra_rmp_set_value(rmp, pyra_rmp_sensitivity_y_name, value);
}

void pyra_rmp_set_cpi(PyraRMP *rmp, guint value) {
	pyra_rmp_set_cpi_x(rmp, value);
	pyra_rmp_set_cpi_y(rmp, value);
}

void pyra_rmp_set_cpi_x(PyraRMP *rmp, guint value) {
	g_assert(value == PYRA_CPI_400 || value == PYRA_CPI_800 || value == PYRA_CPI_1600);
	pyra_rmp_set_value(rmp, pyra_rmp_cpi_x_name, value);
}

void pyra_rmp_set_cpi_y(PyraRMP *rmp, guint value) {
	g_assert(value == PYRA_CPI_400 || value == PYRA_CPI_800 || value == PYRA_CPI_1600);
	pyra_rmp_set_value(rmp, pyra_rmp_cpi_y_name, value);
}

void pyra_rmp_set_light_switch(PyraRMP *rmp, guint value) {
	g_assert(value == PYRA_LIGHTSWITCH_OFF || value == PYRA_LIGHTSWITCH_ON);
	pyra_rmp_set_value(rmp, pyra_rmp_light_switch_name, value);
}

void pyra_rmp_set_light_type(PyraRMP *rmp, guint value) {
	g_assert(value == PYRA_LIGHT_TYPE_FULL_LIGHTENED || value == PYRA_LIGHT_TYPE_PULSATING);
	pyra_rmp_set_value(rmp, pyra_rmp_light_type_name, value);
}

PyraRMPMacroKeyInfo *pyra_rmp_get_macro_key_info(PyraRMP *rmp, guint index) {
	PyraRMP const *default_rmp;
	GError *error = NULL;
	gchar *key;
	PyraRMPMacroKeyInfo *rmp_macro_key_info;

	key = pyra_rmp_create_macro_key_info_name(index);
	rmp_macro_key_info = roccat_key_file_get_binary(rmp->key_file, pyra_rmp_group_name, key, sizeof(PyraRMPMacroKeyInfo), &error);
	if (error) {
		g_clear_error(&error);
		default_rmp = pyra_default_rmp();
		rmp_macro_key_info = roccat_key_file_get_binary(default_rmp->key_file, pyra_rmp_group_name, key, sizeof(PyraRMPMacroKeyInfo), &error);
		if (error)
			g_error(_("Could not get default value for key '%s': %s"), key, error->message);
	}

	g_free(key);
	return rmp_macro_key_info;
}

gchar *pyra_rmp_get_profile_name(PyraRMP *rmp) {
	PyraRMP const *default_rmp;
	GError *error = NULL;
	gchar *result;

	result = g_key_file_get_string(rmp->key_file, pyra_rmp_group_name, pyra_rmp_profile_name_name, &error);
	if (error) {
		g_clear_error(&error);
		default_rmp = pyra_default_rmp();
		result = g_key_file_get_string(default_rmp->key_file, pyra_rmp_group_name, pyra_rmp_profile_name_name, &error);
		if (error)
			g_error(_("Could not get default value for key '%s': %s"), pyra_rmp_profile_name_name, error->message);
	}
	return result;
}

guint pyra_rmp_get_orientation(PyraRMP *rmp) {
	return pyra_rmp_get_value(rmp, pyra_rmp_orientation_name);
}

guint pyra_rmp_get_xy_synchronous(PyraRMP *rmp) {
	return pyra_rmp_get_value(rmp, pyra_rmp_xy_synchronous_name);
}

guint pyra_rmp_get_sensitivity_x(PyraRMP *rmp) {
	return pyra_rmp_get_value(rmp, pyra_rmp_sensitivity_x_name);
}

guint pyra_rmp_get_sensitivity_y(PyraRMP *rmp) {
	return pyra_rmp_get_value(rmp, pyra_rmp_sensitivity_y_name);
}

guint pyra_rmp_get_cpi(PyraRMP *rmp) {
	return pyra_rmp_get_cpi_y(rmp);
}

guint pyra_rmp_get_cpi_x(PyraRMP *rmp) {
	return pyra_rmp_get_value(rmp, pyra_rmp_cpi_x_name);
}

guint pyra_rmp_get_cpi_y(PyraRMP *rmp) {
	return pyra_rmp_get_value(rmp, pyra_rmp_cpi_y_name);
}

guint pyra_rmp_get_light_switch(PyraRMP *rmp) {
	return pyra_rmp_get_value(rmp, pyra_rmp_light_switch_name);
}

guint pyra_rmp_get_light_type(PyraRMP *rmp) {
	return pyra_rmp_get_value(rmp, pyra_rmp_light_type_name);
}

void pyra_rmp_set_game_file_name(PyraRMP *rmp, guint index, gchar const *string) {
	gchar *key;
	gchar *actual_string;

	key = pyra_rmp_create_game_file_name(index);
	actual_string = pyra_rmp_get_game_file_name(rmp, index);
	if (strcmp(string, actual_string)) {
		g_key_file_set_string(rmp->key_file, pyra_rmp_group_name, key, string);
		rmp->modified_rmp = TRUE;
	}
	g_free(actual_string);
	g_free(key);
}

gchar *pyra_rmp_get_game_file_name(PyraRMP *rmp, guint index) {
	PyraRMP const *default_rmp;
	GError *error = NULL;
	gchar *result;
	gchar *key;

	key = pyra_rmp_create_game_file_name(index);
	result = g_key_file_get_string(rmp->key_file, pyra_rmp_group_name, key, &error);
	if (error) {
		g_clear_error(&error);
		default_rmp = pyra_default_rmp();
		result = g_key_file_get_string(default_rmp->key_file, pyra_rmp_group_name, key, &error);
		if (error)
			g_error(_("Could not get default value for key '%s': %s"), key, error->message);
	}
	g_free(key);
	return result;
}
