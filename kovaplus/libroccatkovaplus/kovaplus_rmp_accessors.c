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

#include "kovaplus_rmp.h"
#include "roccat_helper.h"
#include "g_roccat_helper.h"
#include "i18n-lib.h"
#include <string.h>

static gchar const * const rmp_group_name = "Setting";

static gchar const * const rmp_orientation_name = "Orientation";
static gchar const * const rmp_light_switch_name = "LightSwitch";
static gchar const * const rmp_light_type_name = "LightType";
static gchar const * const rmp_color_flag_name = "ColorFlag";
static gchar const * const rmp_color_options_name = "ColorOptions";
static gchar const * const rmp_color_change_mode_name = "ColorChangeMode";
static gchar const * const rmp_cpi_x_name = "Cpi_X";
static gchar const * const rmp_cpi_y_name = "Cpi_Y";
static gchar const * const rmp_sensitivity_x_name = "Sensitivity_X";
static gchar const * const rmp_sensitivity_y_name = "Sensitivity_Y";
static gchar const * const rmp_profile_name_name = "ProFileName";
static gchar const * const rmp_polling_rate_name = "PollingRate";

static gchar *rmp_create_game_file_name(guint index) {
	return g_strdup_printf("GameFile%i", index);
}

static gchar *rmp_create_macro_key_info_name(guint index) {
	g_assert(index < KOVAPLUS_BUTTON_NUM);
	return g_strdup_printf("MacroKeyInfo%i", index);
}

static void rmp_macro_key_info_finalize(KovaplusRmpMacroKeyInfo *rmp_macro_key_info, guint index) {
	rmp_macro_key_info->button_index = index;
	kovaplus_rmp_macro_key_info_set_checksum(rmp_macro_key_info);
}

static gchar *rmp_create_cpi_name(guint bit) {
	g_assert(bit < KOVAPLUS_CPI_LEVELS_NUM);
	return g_strdup_printf("dpi%i", bit);
}

static guint rmp_get_default_value(gchar const *key) {
	KovaplusRmp const *default_rmp = kovaplus_default_rmp();
	GError *error = NULL;
	guint result;
	result = g_key_file_get_integer(default_rmp->key_file, rmp_group_name, key, &error);
	if (error)
		g_error(_("Could not get default value for key '%s': %s"), key, error->message);
	return result;
}

static guint rmp_get_value(KovaplusRmp *rmp, gchar const *key) {
	GError *error = NULL;
	guint result = g_key_file_get_integer(rmp->key_file, rmp_group_name, key, &error);
	if (error) {
		g_clear_error(&error);
		result = rmp_get_default_value(key);
	}
	return result;
}

static void rmp_set_value(KovaplusRmp *rmp, gchar const *key, guint new_value) {
	if (rmp_get_value(rmp, key) != new_value) {
		g_key_file_set_integer(rmp->key_file, rmp_group_name, key, new_value);
 		rmp->modified_profile_settings = TRUE;
	}
}

void kovaplus_rmp_set_macro_key_info(KovaplusRmp *rmp, guint index, KovaplusRmpMacroKeyInfo *rmp_macro_key_info) {
	KovaplusRmpMacroKeyInfo *actual_macro_key_info;
	gchar *key;
	actual_macro_key_info = kovaplus_rmp_get_macro_key_info(rmp, index);
	if (!kovaplus_rmp_macro_key_info_equal(rmp_macro_key_info, actual_macro_key_info)) {
		key = rmp_create_macro_key_info_name(index);
		rmp_macro_key_info_finalize(rmp_macro_key_info, index);
		roccat_key_file_set_binary(rmp->key_file, rmp_group_name, key, (gconstpointer)rmp_macro_key_info, sizeof(KovaplusRmpMacroKeyInfo));
		g_free(key);
		rmp->modified_profile_buttons = TRUE;
	}
	g_free(actual_macro_key_info);
}

KovaplusRmpMacroKeyInfo *kovaplus_rmp_get_macro_key_info(KovaplusRmp *rmp, guint index) {
	KovaplusRmp const *default_rmp;
	GError *error = NULL;
	gchar *key;
	KovaplusRmpMacroKeyInfo *rmp_macro_key_info;

	key = rmp_create_macro_key_info_name(index);
	rmp_macro_key_info = roccat_key_file_get_binary(rmp->key_file, rmp_group_name, key, sizeof(KovaplusRmpMacroKeyInfo), &error);
	if (error) {
		g_clear_error(&error);
		default_rmp = kovaplus_default_rmp();
		rmp_macro_key_info = roccat_key_file_get_binary(default_rmp->key_file, rmp_group_name, key, sizeof(KovaplusRmpMacroKeyInfo), &error);
		if (error)
			g_error(_("Could not get default value for key '%s': %s"), key, error->message);
	}

	g_free(key);
	return rmp_macro_key_info;
}

void kovaplus_rmp_set_profile_name(KovaplusRmp *rmp, gchar const *string) {
	gchar *actual_string;
	actual_string = kovaplus_rmp_get_profile_name(rmp);
	if (strcmp(string, actual_string)) {
		g_key_file_set_string(rmp->key_file, rmp_group_name, rmp_profile_name_name, string);
		rmp->modified_rmp = TRUE;
	}
	g_free(actual_string);
}

gchar *kovaplus_rmp_get_profile_name(KovaplusRmp *rmp) {
	KovaplusRmp const *default_rmp;
	GError *error = NULL;
	gchar *result;

	result = g_key_file_get_string(rmp->key_file, rmp_group_name, rmp_profile_name_name, &error);
	if (error) {
		g_clear_error(&error);
		default_rmp = kovaplus_default_rmp();
		result = g_key_file_get_string(default_rmp->key_file, rmp_group_name, rmp_profile_name_name, &error);
		if (error)
			g_error(_("Could not get default value for key '%s': %s"), rmp_profile_name_name, error->message);
	}
	return result;
}

void kovaplus_rmp_set_sensitivity_x(KovaplusRmp *rmp, guint value) {
	g_assert(value >= KOVAPLUS_SENSITIVITY_MIN && value <= KOVAPLUS_SENSITIVITY_MAX);
 	rmp_set_value(rmp, rmp_sensitivity_x_name, value);
}

guint kovaplus_rmp_get_sensitivity_x(KovaplusRmp *rmp) {
	return rmp_get_value(rmp, rmp_sensitivity_x_name);
}

void kovaplus_rmp_set_sensitivity_y(KovaplusRmp *rmp, guint value) {
	g_assert(value >= KOVAPLUS_SENSITIVITY_MIN && value <= KOVAPLUS_SENSITIVITY_MAX);
 	rmp_set_value(rmp, rmp_sensitivity_y_name, value);
}

guint kovaplus_rmp_get_sensitivity_y(KovaplusRmp *rmp) {
	return rmp_get_value(rmp, rmp_sensitivity_y_name);
}

void kovaplus_rmp_set_cpi(KovaplusRmp *rmp, guint bit, guint value) {
	gchar *key = NULL;
	g_assert(bit < KOVAPLUS_CPI_LEVELS_NUM);
	g_assert(value == KOVAPLUS_PROFILE_SETTINGS_CPI_LEVEL_BIT_STATE_ACTIVE || value == KOVAPLUS_PROFILE_SETTINGS_CPI_LEVEL_BIT_STATE_INACTIVE);
	key = rmp_create_cpi_name(bit);
 	rmp_set_value(rmp, key, value);
	g_free(key);
}

void kovaplus_rmp_set_cpi_all(KovaplusRmp *rmp, guint value) {
	guint i;
	g_assert(!(value & 0xc0));
	for (i = 0; i < KOVAPLUS_CPI_LEVELS_NUM; ++i)
		kovaplus_rmp_set_cpi(rmp, i, roccat_get_bit8(value, i));
}

guint kovaplus_rmp_get_cpi(KovaplusRmp *rmp, guint bit) {
	gchar *key = NULL;
	guint result;
	g_assert(bit < KOVAPLUS_CPI_LEVELS_NUM);
	key = rmp_create_cpi_name(bit);
	result = rmp_get_value(rmp, key);
	g_free(key);
	return result;
}

guint8 kovaplus_rmp_get_cpi_all(KovaplusRmp *rmp) {
	guint i;
	guint8 result = 0;
	for (i = 0; i < KOVAPLUS_CPI_LEVELS_NUM; ++i)
		roccat_set_bit8(&result, i, kovaplus_rmp_get_cpi(rmp, i));
	return result;
}

void kovaplus_rmp_set_orientation(KovaplusRmp *rmp, guint value) {
	g_assert(value == KOVAPLUS_PROFILE_SETTINGS_ORIENTATION_RIGHT || value == KOVAPLUS_PROFILE_SETTINGS_ORIENTATION_LEFT);
 	rmp_set_value(rmp, rmp_orientation_name, value);
}

guint kovaplus_rmp_get_orientation(KovaplusRmp *rmp) {
	return rmp_get_value(rmp, rmp_orientation_name);
}

void kovaplus_rmp_set_light_switch(KovaplusRmp *rmp, guint value) {
 	rmp_set_value(rmp, rmp_light_switch_name, value);
}

guint kovaplus_rmp_get_light_switch(KovaplusRmp *rmp) {
	return rmp_get_value(rmp, rmp_light_switch_name);
}

void kovaplus_rmp_set_light_type(KovaplusRmp *rmp, guint value) {
	rmp_set_value(rmp, rmp_light_type_name, value);
}

guint kovaplus_rmp_get_light_type(KovaplusRmp *rmp) {
	return rmp_get_value(rmp, rmp_light_type_name);
}

void kovaplus_rmp_set_color_flag(KovaplusRmp *rmp, guint value) {
 	rmp_set_value(rmp, rmp_color_flag_name, value);
}

guint kovaplus_rmp_get_color_flag(KovaplusRmp *rmp) {
	return rmp_get_value(rmp, rmp_color_flag_name);
}

void kovaplus_rmp_set_color_options(KovaplusRmp *rmp, guint value) {
 	rmp_set_value(rmp, rmp_color_options_name, value);
}

guint kovaplus_rmp_get_color_options(KovaplusRmp *rmp) {
	return rmp_get_value(rmp, rmp_color_options_name);
}

void kovaplus_rmp_set_color_change_mode(KovaplusRmp *rmp, guint value) {
 	rmp_set_value(rmp, rmp_color_change_mode_name, value);
}

guint kovaplus_rmp_get_color_change_mode(KovaplusRmp *rmp) {
	return rmp_get_value(rmp, rmp_color_change_mode_name);
}

void kovaplus_rmp_set_cpi_x(KovaplusRmp *rmp, guint value) {
 	g_assert(value >= KOVAPLUS_PROFILE_SETTINGS_CPI_LEVEL_400 && value <= KOVAPLUS_PROFILE_SETTINGS_CPI_LEVEL_3200);
 	rmp_set_value(rmp, rmp_cpi_x_name, value);
}

guint kovaplus_rmp_get_cpi_x(KovaplusRmp *rmp) {
	return rmp_get_value(rmp, rmp_cpi_x_name);
}

void kovaplus_rmp_set_cpi_y(KovaplusRmp *rmp, guint value) {
 	g_assert(value >= KOVAPLUS_PROFILE_SETTINGS_CPI_LEVEL_400 && value <= KOVAPLUS_PROFILE_SETTINGS_CPI_LEVEL_3200);
 	rmp_set_value(rmp, rmp_cpi_y_name, value);
}

guint kovaplus_rmp_get_cpi_y(KovaplusRmp *rmp) {
	return rmp_get_value(rmp, rmp_cpi_y_name);
}

void kovaplus_rmp_set_polling_rate(KovaplusRmp *rmp, guint value) {
 	rmp_set_value(rmp, rmp_polling_rate_name, value);
}

guint kovaplus_rmp_get_polling_rate(KovaplusRmp *rmp) {
	return rmp_get_value(rmp, rmp_polling_rate_name);
}

void kovaplus_rmp_set_game_file_name(KovaplusRmp *rmp, guint index, gchar const *string) {
	gchar *key;
	gchar *actual_string;

	key = rmp_create_game_file_name(index);
	actual_string = kovaplus_rmp_get_game_file_name(rmp, index);
	if (strcmp(string, actual_string)) {
		g_key_file_set_string(rmp->key_file, rmp_group_name, key, string);
		rmp->modified_rmp = TRUE;
	}
	g_free(actual_string);
	g_free(key);
}

gchar *kovaplus_rmp_get_game_file_name(KovaplusRmp *rmp, guint index) {
	KovaplusRmp const *default_rmp;
	GError *error = NULL;
	gchar *result;
	gchar *key;

	key = rmp_create_game_file_name(index);
	result = g_key_file_get_string(rmp->key_file, rmp_group_name, key, &error);
	if (error) {
		g_clear_error(&error);
		default_rmp = kovaplus_default_rmp();
		result = g_key_file_get_string(default_rmp->key_file, rmp_group_name, key, &error);
		if (error)
			g_error(_("Could not get default value for key '%s': %s"), key, error->message);
	}
	g_free(key);
	return result;
}
