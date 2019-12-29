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

#include "tyon_rmp.h"
#include "tyon_profile_buttons.h"
#include "tyon_profile_settings.h"
#include "roccat_helper.h"
#include "g_roccat_helper.h"
#include "i18n-lib.h"
#include <string.h>

static gchar const * const tyon_rmp_group_name = "Setting";

static gchar const * const tyon_rmp_xy_sync_name = "XYSynchronous";
static gchar const * const tyon_rmp_sensitivity_x_name = "Sensitivity_X";
static gchar const * const tyon_rmp_sensitivity_y_name = "Sensitivity_Y";
static gchar const * const tyon_rmp_sensitivity_name = "Sensitivity";
static gchar const * const tyon_rmp_profile_name_name = "ProFileName";
static gchar const * const tyon_rmp_cpi_x_name = "Cpi_X";
static gchar const * const tyon_rmp_polling_rate_name = "PollingRate";
static gchar const * const tyon_rmp_light_color_flow_name = "LightColorFlow";
static gchar const * const tyon_rmp_light_effect_type_name = "LightEffectType";
static gchar const * const tyon_rmp_light_effect_speed_name = "LightEffectSpeed";
static gchar const * const tyon_rmp_light_chose_type_name = "LtChoseType";
static gchar const * const tyon_rmp_use_color_for_all_name = "UseColorForAll";
static gchar const * const tyon_rmp_talkfx_name = "HDDKeyboard";

static gchar *tyon_rmp_create_macro_key_info_name(guint index) {
	g_assert(index < TYON_PROFILE_BUTTON_NUM);
	return g_strdup_printf("MacroKeyInfo%u", index);
}

static gchar *tyon_rmp_create_light_info_name(guint index) {
	g_assert(index < TYON_LIGHTS_NUM);
	return g_strdup_printf("LightInfo%u", index);
}

static gchar *tyon_rmp_create_custom_light_info_name(guint index) {
	g_assert(index < TYON_LIGHTS_NUM);
	return g_strdup_printf("CustomLightInfo%u", index);
}

static gchar *tyon_rmp_create_cpi_name(guint bit) {
	g_assert(bit < TYON_PROFILE_SETTINGS_CPI_LEVELS_NUM);
	return g_strdup_printf("dpi%u", bit);
}

static gchar *tyon_rmp_create_cpi_level_x_name(guint bit) {
	g_assert(bit < TYON_PROFILE_SETTINGS_CPI_LEVELS_NUM);
	return g_strdup_printf("CpiLevel_X%u", bit);
}

static gchar *tyon_rmp_create_game_file_name(guint index) {
	return g_strdup_printf("GameFile%u", index);
}

static void tyon_rmp_macro_key_info_finalize(TyonRmpMacroKeyInfo *rmp_macro_key_info, guint index) {
	rmp_macro_key_info->button_index = index;
	tyon_rmp_macro_key_info_set_checksum(rmp_macro_key_info);
}

static void tyon_rmp_light_info_finalize(TyonRmpLightInfo *rmp_light_info) {
	tyon_rmp_light_info_set_checksum(rmp_light_info);
}

static guint tyon_rmp_get_value(TyonRmp *rmp, gchar const *key) {
	GError *error = NULL;
	guint result = g_key_file_get_integer(rmp->key_file, tyon_rmp_group_name, key, &error);
	if (error) {
		g_clear_error(&error);
		result = g_key_file_get_integer(rmp->default_key_file, tyon_rmp_group_name, key, &error);
	}
	return result;
}

static void tyon_rmp_set_value_settings(TyonRmp *rmp, gchar const *key, guint value) {
	if (tyon_rmp_get_value(rmp, key) != value) {
		g_key_file_set_integer(rmp->key_file, tyon_rmp_group_name, key, value);
		rmp->modified_settings = TRUE;
	}
}

static TyonRmpMacroKeyInfo *macro_key_info_without_default(GKeyFile *key_file, guint index) {
	gchar *key;
	GError *local_error = NULL;
	TyonRmpMacroKeyInfo *rmp_macro_key_info;

	key = tyon_rmp_create_macro_key_info_name(index);
	rmp_macro_key_info = roccat_key_file_get_binary(key_file, tyon_rmp_group_name, key, sizeof(TyonRmpMacroKeyInfo), &local_error);
	g_free(key);
	return rmp_macro_key_info;
}

TyonRmpMacroKeyInfo *tyon_rmp_get_macro_key_info(TyonRmp *rmp, guint index) {
	TyonRmpMacroKeyInfo *rmp_macro_key_info;

	rmp_macro_key_info = macro_key_info_without_default(rmp->key_file, index);
	if (rmp_macro_key_info)
		return rmp_macro_key_info;

	rmp_macro_key_info = macro_key_info_without_default(rmp->default_key_file, index);
	return rmp_macro_key_info;
}

void tyon_rmp_set_macro_key_info(TyonRmp *rmp, guint index, TyonRmpMacroKeyInfo *rmp_macro_key_info) {
	gchar *key;
	TyonRmpMacroKeyInfo *actual_key_info;

	actual_key_info = tyon_rmp_get_macro_key_info(rmp, index);
	if (!tyon_rmp_macro_key_info_equal(rmp_macro_key_info, actual_key_info)) {
		key = tyon_rmp_create_macro_key_info_name(index);
		tyon_rmp_macro_key_info_finalize(rmp_macro_key_info, index);
		roccat_key_file_set_binary(rmp->key_file, tyon_rmp_group_name, key, (gconstpointer)rmp_macro_key_info, sizeof(TyonRmpMacroKeyInfo));
		g_free(key);
		rmp->modified_macros[index] = TRUE;
	}
	tyon_rmp_macro_key_info_free(actual_key_info);
}

gchar *tyon_rmp_get_profile_name(TyonRmp *rmp) {
	GError *error = NULL;
	gchar *result;

	result = g_key_file_get_string(rmp->key_file, tyon_rmp_group_name, tyon_rmp_profile_name_name, &error);
	if (error) {
		g_clear_error(&error);
		result = g_key_file_get_string(rmp->default_key_file, tyon_rmp_group_name, tyon_rmp_profile_name_name, &error);
		if (error)
			g_error(_("Could not get default value for key '%s': %s"), tyon_rmp_profile_name_name, error->message);
	}
	return result;
}

void tyon_rmp_set_profile_name(TyonRmp *rmp, gchar const *string) {
	gchar *actual_string;
	actual_string = tyon_rmp_get_profile_name(rmp);
	if (strcmp(string, actual_string)) {
		g_key_file_set_string(rmp->key_file, tyon_rmp_group_name, tyon_rmp_profile_name_name, string);
		rmp->modified_rmp = TRUE;
	}
	g_free(actual_string);
}

void tyon_rmp_set_sensitivity_x(TyonRmp *rmp, guint value) {
 	tyon_rmp_set_value_settings(rmp, tyon_rmp_sensitivity_x_name, value);
}

guint tyon_rmp_get_sensitivity_x(TyonRmp *rmp) {
	return tyon_rmp_get_value(rmp, tyon_rmp_sensitivity_x_name);
}

void tyon_rmp_set_sensitivity_y(TyonRmp *rmp, guint value) {
 	tyon_rmp_set_value_settings(rmp, tyon_rmp_sensitivity_y_name, value);
}

guint tyon_rmp_get_sensitivity_y(TyonRmp *rmp) {
	return tyon_rmp_get_value(rmp, tyon_rmp_sensitivity_y_name);
}

void tyon_rmp_set_sensitivity(TyonRmp *rmp, guint value) {
 	tyon_rmp_set_value_settings(rmp, tyon_rmp_sensitivity_name, value);
}

guint tyon_rmp_get_sensitivity(TyonRmp *rmp) {
	return tyon_rmp_get_value(rmp, tyon_rmp_sensitivity_name);
}

void tyon_rmp_set_xy_synchronous(TyonRmp *rmp, guint value) {
	tyon_rmp_set_value_settings(rmp, tyon_rmp_xy_sync_name, value);
}

guint tyon_rmp_get_xy_synchronous(TyonRmp *rmp) {
	return tyon_rmp_get_value(rmp, tyon_rmp_xy_sync_name);
}

guint tyon_rmp_get_cpi(TyonRmp *rmp, guint bit) {
	gchar *key = NULL;
	guint result;
	key = tyon_rmp_create_cpi_name(bit);
	result = tyon_rmp_get_value(rmp, key);
	g_free(key);
	return result;
}

void tyon_rmp_set_cpi(TyonRmp *rmp, guint bit, guint value) {
	gchar *key = NULL;
	if (tyon_rmp_get_cpi(rmp, bit) != value) {
		key = tyon_rmp_create_cpi_name(bit);
		g_key_file_set_integer(rmp->key_file, tyon_rmp_group_name, key, value);
		g_free(key);
		rmp->modified_settings = TRUE;
	}
}

void tyon_rmp_set_cpi_all(TyonRmp *rmp, guint value) {
	guint i;
	for (i = 0; i < TYON_PROFILE_SETTINGS_CPI_LEVELS_NUM; ++i)
		tyon_rmp_set_cpi(rmp, i, roccat_get_bit8(value, i));
}

guint8 tyon_rmp_get_cpi_all(TyonRmp *rmp) {
	guint i;
	guint8 result = 0;
	for (i = 0; i < TYON_PROFILE_SETTINGS_CPI_LEVELS_NUM; ++i) {
		roccat_set_bit8(&result, i, tyon_rmp_get_cpi(rmp, i));
	}
	return result;
}

static TyonRmpLightInfo *tyon_rmp_get_light_info_value(TyonRmp *rmp, gchar const *key) {
	TyonRmpLightInfo *rmp_light_info;
	GError *error = NULL;

	rmp_light_info = roccat_key_file_get_binary(rmp->key_file, tyon_rmp_group_name, key, sizeof(TyonRmpLightInfo), &error);
	if (error) {
		g_clear_error(&error);
		rmp_light_info = roccat_key_file_get_binary(rmp->default_key_file, tyon_rmp_group_name, key, sizeof(TyonRmpLightInfo), &error);
		if (error)
			g_error(_("Could not get default value for key '%s': %s"), key, error->message);
	}

	return rmp_light_info;
}

TyonRmpLightInfo *tyon_rmp_get_custom_light_info(TyonRmp *rmp, guint index) {
	TyonRmpLightInfo *rmp_light_info;
	gchar *key = NULL;

	key = tyon_rmp_create_custom_light_info_name(index);
	rmp_light_info = tyon_rmp_get_light_info_value(rmp, key);
	g_free(key);
	return rmp_light_info;
}

TyonRmpLightInfo *tyon_rmp_get_rmp_light_info(TyonRmp *rmp, guint index) {
	TyonRmpLightInfo *rmp_light_info;
	gchar *key = NULL;

	key = tyon_rmp_create_light_info_name(index);
	rmp_light_info = tyon_rmp_get_light_info_value(rmp, key);
	g_free(key);
	return rmp_light_info;
}

static void tyon_rmp_set_light_info_value(TyonRmp *rmp, gchar const *key, TyonRmpLightInfo *rmp_light_info) {
	TyonRmpLightInfo *actual_light_info;

	actual_light_info = tyon_rmp_get_light_info_value(rmp, key);
	if (!tyon_rmp_light_info_equal(rmp_light_info, actual_light_info)) {
		tyon_rmp_light_info_finalize(rmp_light_info);
		roccat_key_file_set_binary(rmp->key_file, tyon_rmp_group_name, key, (gconstpointer)rmp_light_info, sizeof(TyonRmpLightInfo));
		rmp->modified_settings = TRUE;
	}

	g_free(actual_light_info);
}

void tyon_rmp_set_custom_light_info(TyonRmp *rmp, guint index, TyonRmpLightInfo *rmp_light_info) {
	gchar *key = NULL;

	key = tyon_rmp_create_custom_light_info_name(index);
	tyon_rmp_set_light_info_value(rmp, key, rmp_light_info);
	g_free(key);
}

void tyon_rmp_set_rmp_light_info(TyonRmp *rmp, guint index, TyonRmpLightInfo *rmp_light_info) {
	gchar *key = NULL;

	key = tyon_rmp_create_light_info_name(index);
	tyon_rmp_set_light_info_value(rmp, key, rmp_light_info);
	g_free(key);
}

void tyon_rmp_set_polling_rate(TyonRmp *rmp, guint value) {
 	tyon_rmp_set_value_settings(rmp, tyon_rmp_polling_rate_name, value);
}

guint tyon_rmp_get_polling_rate(TyonRmp *rmp) {
	return tyon_rmp_get_value(rmp, tyon_rmp_polling_rate_name);
}

void tyon_rmp_set_light_effect_type(TyonRmp *rmp, guint value) {
 	tyon_rmp_set_value_settings(rmp, tyon_rmp_light_effect_type_name, value);
}

guint tyon_rmp_get_light_effect_type(TyonRmp *rmp) {
	return tyon_rmp_get_value(rmp, tyon_rmp_light_effect_type_name);
}

void tyon_rmp_set_light_effect_speed(TyonRmp *rmp, guint value) {
 	tyon_rmp_set_value_settings(rmp, tyon_rmp_light_effect_speed_name, value);
}

guint tyon_rmp_get_light_effect_speed(TyonRmp *rmp) {
	return tyon_rmp_get_value(rmp, tyon_rmp_light_effect_speed_name);
}

void tyon_rmp_set_light_color_flow(TyonRmp *rmp, guint value) {
 	tyon_rmp_set_value_settings(rmp, tyon_rmp_light_color_flow_name, value);
}

guint tyon_rmp_get_light_color_flow(TyonRmp *rmp) {
	return tyon_rmp_get_value(rmp, tyon_rmp_light_color_flow_name);
}

void tyon_rmp_set_light_chose_type(TyonRmp *rmp, guint value) {
 	tyon_rmp_set_value_settings(rmp, tyon_rmp_light_chose_type_name, value);
}

guint tyon_rmp_get_light_chose_type(TyonRmp *rmp) {
	return tyon_rmp_get_value(rmp, tyon_rmp_light_chose_type_name);
}

guint tyon_rmp_get_cpi_level_x(TyonRmp *rmp, guint index) {
	gchar *key = NULL;
	guint result;

	key = tyon_rmp_create_cpi_level_x_name(index);
	result = tyon_rmp_get_value(rmp, key);
	g_free(key);
	return result;
}

void tyon_rmp_set_cpi_level_x(TyonRmp *rmp, guint index, guint value) {
	gchar *key = NULL;
	if (tyon_rmp_get_cpi_level_x(rmp, index) != value) {
		key = tyon_rmp_create_cpi_level_x_name(index);
		g_key_file_set_integer(rmp->key_file, tyon_rmp_group_name, key, value);
		g_free(key);
		rmp->modified_settings = TRUE;
	}
}

void tyon_rmp_set_cpi_x(TyonRmp *rmp, guint value) {
 	tyon_rmp_set_value_settings(rmp, tyon_rmp_cpi_x_name, value);
}

guint tyon_rmp_get_cpi_x(TyonRmp *rmp) {
	return tyon_rmp_get_value(rmp, tyon_rmp_cpi_x_name);
}

gchar *tyon_rmp_get_game_file_name(TyonRmp *rmp, guint index) {
	GError *error = NULL;
	gchar *result;
	gchar *key;

	key = tyon_rmp_create_game_file_name(index);
	result = g_key_file_get_string(rmp->key_file, tyon_rmp_group_name, key, &error);
	if (error) {
		g_clear_error(&error);
		result = g_key_file_get_string(rmp->default_key_file, tyon_rmp_group_name, key, &error);
		if (error)
			g_error(_("Could not get default value for key '%s': %s"), key, error->message);
	}
	g_free(key);
	return result;
}

void tyon_rmp_set_game_file_name(TyonRmp *rmp, guint index, gchar const *string) {
	gchar *key;
	gchar *actual_string;

	key = tyon_rmp_create_game_file_name(index);
	actual_string = tyon_rmp_get_game_file_name(rmp, index);
	if (strcmp(string, actual_string)) {
		g_key_file_set_string(rmp->key_file, tyon_rmp_group_name, key, string);
		rmp->modified_rmp = TRUE;
	}
	g_free(actual_string);
	g_free(key);
}

static void tyon_rmp_set_value_rmp(TyonRmp *rmp, gchar const *key, guint value) {
	if (tyon_rmp_get_value(rmp, key) != value) {
		g_key_file_set_integer(rmp->key_file, tyon_rmp_group_name, key, value);
		rmp->modified_rmp = TRUE;
	}
}

void tyon_rmp_set_use_color_for_all(TyonRmp *rmp, guint value) {
 	tyon_rmp_set_value_rmp(rmp, tyon_rmp_use_color_for_all_name, value);
}

guint tyon_rmp_get_use_color_for_all(TyonRmp *rmp) {
	return tyon_rmp_get_value(rmp, tyon_rmp_use_color_for_all_name);
}

void tyon_rmp_set_talkfx(TyonRmp *rmp, guint value) {
	tyon_rmp_set_value_settings(rmp, tyon_rmp_talkfx_name, value);
}

guint tyon_rmp_get_talkfx(TyonRmp *rmp) {
	return tyon_rmp_get_value(rmp, tyon_rmp_talkfx_name);
}
