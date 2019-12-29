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

#include "savu_rmp.h"
#include "g_roccat_helper.h"
#include "i18n-lib.h"
#include <string.h>

static gchar const * const savu_rmp_group_name = "Setting";

static gchar const * const savu_rmp_profile_name_name = "ProFileName";
static gchar const * const savu_rmp_xy_sync_name = "XYSynchronous";
static gchar const * const savu_rmp_sensitivity_x_name = "Sensitivity_X";
static gchar const * const savu_rmp_sensitivity_y_name = "Sensitivity_Y";
static gchar const * const savu_rmp_breath_on_name = "BreathOn";
static gchar const * const savu_rmp_color_flow_name = "*ColorFlow";
static gchar const * const savu_rmp_light_switch_name = "LightSwitch";
static gchar const * const savu_rmp_color_database_name = "ColorDatabase";
static gchar const * const savu_rmp_color_red_name = "ColorR";
static gchar const * const savu_rmp_color_green_name = "ColorG";
static gchar const * const savu_rmp_color_blue_name = "ColorB";
static gchar const * const savu_rmp_polling_rate_name = "PollingRate";
static gchar const * const savu_rmp_cpi_step_name = "Dpi Step";

static gchar *savu_rmp_create_game_file_name(guint index) {
	return g_strdup_printf("GameFile%i", index);
}

static gchar *savu_rmp_create_cpi_name(guint bit) {
	g_assert(bit < SAVU_GENERAL_CPI_LEVELS_NUM);
	return g_strdup_printf("dpi%i", bit);
}

static gchar *savu_rmp_create_button_name(guint index) {
	g_assert(index < SAVU_BUTTON_NUM);
	return g_strdup_printf("Button%i", index);
}

static gchar *savu_rmp_create_launchpath_name(guint index) {
	g_assert(index < SAVU_BUTTON_NUM);
	return g_strdup_printf("LaunchPath%i", index);
}

static gchar *savu_rmp_create_timer_name(guint index) {
	g_assert(index < SAVU_BUTTON_NUM);
	return g_strdup_printf("Timer%i", index);
}

static gchar *savu_rmp_create_macrokeyinfo_name(guint index) {
	g_assert(index < SAVU_BUTTON_NUM);
	return g_strdup_printf("MacroKeyInfo%i", index);
}

/*
 * May terminate application if error occurs
 */
static gint savu_rmp_get_default_integer(gchar const *key) {
	SavuRmp const *default_rmp;
	GError *error = NULL;
	gint result;

	default_rmp = savu_default_rmp();
	result = g_key_file_get_integer(default_rmp->key_file, savu_rmp_group_name, key, &error);
	if (error)
		g_error(_("Could not get default value for key '%s': %s"), key, error->message);
	return result;
}

static gint savu_rmp_get_integer(SavuRmp *rmp, gchar const *key) {
	GError *error = NULL;
	gint result = g_key_file_get_integer(rmp->key_file, savu_rmp_group_name, key, &error);
	if (error) {
		g_clear_error(&error);
		result = savu_rmp_get_default_integer(key);
	}
	return result;
}

static gboolean savu_rmp_set_integer(SavuRmp *rmp, gchar const *key, gint value) {
	if (savu_rmp_get_integer(rmp, key) != value) {
		g_key_file_set_integer(rmp->key_file, savu_rmp_group_name, key, value);
		return TRUE;
	}
	return FALSE;
}

static gconstpointer savu_rmp_get_default_binary(gchar const *key, size_t length) {
	SavuRmp const *default_rmp;
	GError *error = NULL;
	gpointer data;

	default_rmp = savu_default_rmp();
	data = roccat_key_file_get_binary(default_rmp->key_file, savu_rmp_group_name, key, length, &error);
	if (error)
		g_error(_("Could not get default value for key '%s': %s"), key, error->message);
	return data;
}

static gpointer savu_rmp_get_binary(SavuRmp *rmp, gchar const *key, size_t length) {
	GError *error = NULL;
	gpointer data;

	data = roccat_key_file_get_binary(rmp->key_file, savu_rmp_group_name, key, length, &error);
	if (error) {
		g_clear_error(&error);
		savu_rmp_get_default_binary(key, length);
	}
	return data;
}

static gboolean savu_rmp_set_binary(SavuRmp *rmp, gchar const *key, gconstpointer buffer, size_t length) {
	gpointer *actual_data;
	gboolean retval = FALSE;

	actual_data = savu_rmp_get_binary(rmp, key, length);
	// That is dumber than to test specificly, but easier
	// May result in unneeded writes if roccat still doesn't clean their memory
	if (memcmp(buffer, actual_data, length) != 0) {
		roccat_key_file_set_binary(rmp->key_file, savu_rmp_group_name, key, buffer, length);
		retval = TRUE;
	}
	g_free(actual_data);
	return retval;
}

static gchar *savu_rmp_get_default_string(gchar const *key) {
	SavuRmp const *default_rmp;
	GError *error = NULL;
	gchar *result;

	default_rmp = savu_default_rmp();
	result = g_key_file_get_string(default_rmp->key_file, savu_rmp_group_name, key, &error);
	if (error)
		g_error(_("Could not get default value for key '%s': %s"), key, error->message);
	return result;
}

static gchar *savu_rmp_get_string(SavuRmp *rmp, gchar const *key) {
	GError *error = NULL;
	gchar *result;

	result = g_key_file_get_string(rmp->key_file, savu_rmp_group_name, key, &error);
	if (error) {
		g_clear_error(&error);
		result = savu_rmp_get_default_string(key);
	}
	return result;
}

static gboolean savu_rmp_set_string(SavuRmp *rmp, gchar const *key, gchar const *string) {
	gchar *actual_string;
	gboolean retval = FALSE;

	actual_string = savu_rmp_get_string(rmp, key);
	if (strcmp(string, actual_string)) {
		g_key_file_set_string(rmp->key_file, savu_rmp_group_name, key, string);
		retval = TRUE;
	}
	g_free(actual_string);
	return retval;
}

gchar *savu_rmp_get_profile_name(SavuRmp *rmp) {
	return savu_rmp_get_string(rmp, savu_rmp_profile_name_name);
}

void savu_rmp_set_profile_name(SavuRmp *rmp, gchar const *string) {
	if (savu_rmp_set_string(rmp, savu_rmp_profile_name_name, string))
		rmp->modified_rmp = TRUE;
}

void savu_rmp_set_xy_synchronous(SavuRmp *rmp, guint value) {
 	if (savu_rmp_set_integer(rmp, savu_rmp_xy_sync_name, value))
 		rmp->modified_general = TRUE;
}

guint savu_rmp_get_xy_synchronous(SavuRmp *rmp) {
	return savu_rmp_get_integer(rmp, savu_rmp_xy_sync_name);
}

void savu_rmp_set_sensitivity_x(SavuRmp *rmp, guint value) {
 	if (savu_rmp_set_integer(rmp, savu_rmp_sensitivity_x_name, value))
 		rmp->modified_general = TRUE;
}

guint savu_rmp_get_sensitivity_x(SavuRmp *rmp) {
	return savu_rmp_get_integer(rmp, savu_rmp_sensitivity_x_name);
}

void savu_rmp_set_sensitivity_y(SavuRmp *rmp, guint value) {
 	if (savu_rmp_set_integer(rmp, savu_rmp_sensitivity_y_name, value))
 		rmp->modified_general = TRUE;
}

guint savu_rmp_get_sensitivity_y(SavuRmp *rmp) {
	return savu_rmp_get_integer(rmp, savu_rmp_sensitivity_y_name);
}

void savu_rmp_set_color_red(SavuRmp *rmp, guint value) {
	if (savu_rmp_set_integer(rmp, savu_rmp_color_red_name, value))
		rmp->modified_general = TRUE;
}

guint savu_rmp_get_color_red(SavuRmp *rmp) {
	return savu_rmp_get_integer(rmp, savu_rmp_color_red_name);
}

void savu_rmp_set_color_green(SavuRmp *rmp, guint value) {
	if (savu_rmp_set_integer(rmp, savu_rmp_color_green_name, value))
		rmp->modified_general = TRUE;
}

guint savu_rmp_get_color_green(SavuRmp *rmp) {
	return savu_rmp_get_integer(rmp, savu_rmp_color_green_name);
}

void savu_rmp_set_color_blue(SavuRmp *rmp, guint value) {
	if (savu_rmp_set_integer(rmp, savu_rmp_color_blue_name, value))
		rmp->modified_general = TRUE;
}

guint savu_rmp_get_color_blue(SavuRmp *rmp) {
	return savu_rmp_get_integer(rmp, savu_rmp_color_blue_name);
}

void savu_rmp_set_polling_rate(SavuRmp *rmp, guint value) {
	if (savu_rmp_set_integer(rmp, savu_rmp_polling_rate_name, value))
		rmp->modified_general = TRUE;
}

guint savu_rmp_get_polling_rate(SavuRmp *rmp) {
	return savu_rmp_get_integer(rmp, savu_rmp_polling_rate_name);
}

void savu_rmp_set_color_database(SavuRmp *rmp, guint value) {
	if (savu_rmp_set_integer(rmp, savu_rmp_color_database_name, value))
		rmp->modified_general = TRUE;
}

guint savu_rmp_get_color_database(SavuRmp *rmp) {
	return savu_rmp_get_integer(rmp, savu_rmp_color_database_name);
}

void savu_rmp_set_cpi_step(SavuRmp *rmp, guint value) {
	if (savu_rmp_set_integer(rmp, savu_rmp_cpi_step_name, value))
		rmp->modified_general = TRUE;
}

guint savu_rmp_get_cpi_step(SavuRmp *rmp) {
	return savu_rmp_get_integer(rmp, savu_rmp_cpi_step_name);
}

void savu_rmp_set_cpi(SavuRmp *rmp, guint index, guint value) {
	g_assert(index < SAVU_GENERAL_CPI_LEVELS_NUM);
	gchar *name = savu_rmp_create_cpi_name(index);
	if (savu_rmp_set_integer(rmp, name, value))
		rmp->modified_general = TRUE;
	g_free(name);
}

guint savu_rmp_get_cpi(SavuRmp *rmp, guint index) {
	g_assert(index < SAVU_GENERAL_CPI_LEVELS_NUM);
	gchar *name = savu_rmp_create_cpi_name(index);
	guint cpi = savu_rmp_get_integer(rmp, name);
	g_free(name);
	return cpi;
}

void savu_rmp_set_light_switch(SavuRmp *rmp, guint value) {
	if (savu_rmp_set_integer(rmp, savu_rmp_light_switch_name, value))
		rmp->modified_general = TRUE;
}

guint savu_rmp_get_light_switch(SavuRmp *rmp) {
	return savu_rmp_get_integer(rmp, savu_rmp_light_switch_name);
}

void savu_rmp_set_breath_on(SavuRmp *rmp, guint value) {
	if (savu_rmp_set_integer(rmp, savu_rmp_breath_on_name, value))
		rmp->modified_general = TRUE;
}

guint savu_rmp_get_color_flow(SavuRmp *rmp) {
	return savu_rmp_get_integer(rmp, savu_rmp_color_flow_name);
}

void savu_rmp_set_color_flow(SavuRmp *rmp, guint value) {
	if (savu_rmp_set_integer(rmp, savu_rmp_color_flow_name, value))
		rmp->modified_general = TRUE;
}

guint savu_rmp_get_breath_on(SavuRmp *rmp) {
	return savu_rmp_get_integer(rmp, savu_rmp_breath_on_name);
}

SavuRmpMacroKeyInfo *savu_rmp_get_macro_key_info(SavuRmp *rmp, guint index) {
	gchar *key;
	SavuRmpMacroKeyInfo *key_info;

	g_assert(index < SAVU_BUTTON_NUM);

	key = savu_rmp_create_macrokeyinfo_name(index);
	key_info = (SavuRmpMacroKeyInfo *)savu_rmp_get_binary(rmp, key, sizeof(SavuRmpMacroKeyInfo));
	g_free(key);
	return key_info;
}

void savu_rmp_set_macro_key_info(SavuRmp *rmp, guint index, SavuRmpMacroKeyInfo const *key_info) {
	gchar *key;

	g_assert(index < SAVU_BUTTON_NUM);

	key = savu_rmp_create_macrokeyinfo_name(index);
	if (savu_rmp_set_binary(rmp, key, (gconstpointer)key_info, sizeof(SavuRmpMacroKeyInfo)))
		rmp->modified_macros[index] = TRUE;
	g_free(key);
}

SavuRmpTimer *savu_rmp_get_timer(SavuRmp *rmp, guint index) {
	gchar *key;
	SavuRmpTimer *timer;

	g_assert(index < SAVU_BUTTON_NUM);

	key = savu_rmp_create_timer_name(index);
	timer = (SavuRmpTimer *)savu_rmp_get_binary(rmp, key, sizeof(SavuRmpTimer));
	g_free(key);
	return timer;
}

void savu_rmp_set_timer(SavuRmp *rmp, guint index, SavuRmpTimer const *timer) {
	gchar *key;

	g_assert(index < SAVU_BUTTON_NUM);

	key = savu_rmp_create_timer_name(index);
	if (savu_rmp_set_binary(rmp, key, (gconstpointer)timer, sizeof(SavuRmpTimer)))
		rmp->modified_rmp = TRUE;
	g_free(key);
}

RoccatButton *savu_rmp_get_button(SavuRmp *rmp, guint index) {
	gchar *key;
	RoccatButton *button;

	g_assert(index < SAVU_BUTTON_NUM);

	key = savu_rmp_create_button_name(index);
	button = (RoccatButton *)savu_rmp_get_binary(rmp, key, sizeof(RoccatButton));
	g_free(key);
	return button;
}

void savu_rmp_set_button(SavuRmp *rmp, guint index, RoccatButton const *button) {
	gchar *key;

	g_assert(index < SAVU_BUTTON_NUM);

	key = savu_rmp_create_button_name(index);
	if (savu_rmp_set_binary(rmp, key, (gconstpointer)button, sizeof(RoccatButton)))
		rmp->modified_buttons = TRUE;
	g_free(key);
}

gchar *savu_rmp_get_game_file(SavuRmp *rmp, guint index) {
	gchar *key;
	gchar *button;

	g_assert(index < SAVU_RMP_GAME_FILE_NUM);

	key = savu_rmp_create_game_file_name(index);
	button = (gchar *)savu_rmp_get_string(rmp, key);
	g_free(key);
	return button;
}

void savu_rmp_set_game_file(SavuRmp *rmp, guint index, gchar const *string) {
	gchar *key;

	g_assert(index < SAVU_RMP_GAME_FILE_NUM);

	key = savu_rmp_create_game_file_name(index);
	if (savu_rmp_set_string(rmp, key, (gconstpointer)string))
		rmp->modified_rmp = TRUE;
	g_free(key);
}

gchar *savu_rmp_get_launchpath(SavuRmp *rmp, guint index) {
	gchar *key;
	gchar *button;

	g_assert(index < SAVU_BUTTON_NUM);

	key = savu_rmp_create_launchpath_name(index);
	button = (gchar *)savu_rmp_get_string(rmp, key);
	g_free(key);
	return button;
}

void savu_rmp_set_launchpath(SavuRmp *rmp, guint index, gchar const *string) {
	gchar *key;

	g_assert(index < SAVU_BUTTON_NUM);

	key = savu_rmp_create_launchpath_name(index);
	if (savu_rmp_set_string(rmp, key, (gconstpointer)string))
		rmp->modified_rmp = TRUE;
	g_free(key);
}
