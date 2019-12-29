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

#include "koneplus_rmp.h"
#include "roccat_helper.h"
#include "g_roccat_helper.h"
#include "i18n-lib.h"
#include <string.h>

static gchar const * const koneplus_rmp_group_name = "Setting";

static gchar const * const koneplus_rmp_xy_sync_name = "XYSynchronous";
static gchar const * const koneplus_rmp_sensitivity_x_name = "Sensitivity_X";
static gchar const * const koneplus_rmp_sensitivity_y_name = "Sensitivity_Y";
static gchar const * const koneplus_rmp_sensitivity_name = "Sensitivity";
static gchar const * const koneplus_rmp_profile_name_name = "ProFileName";
static gchar const * const koneplus_rmp_cpi_x_name = "Cpi_X";
static gchar const * const koneplus_rmp_cpi_y_name = "Cpi_Y";
static gchar const * const koneplus_rmp_polling_rate_name = "PollingRate";
static gchar const * const koneplus_rmp_light_color_flow_name = "LightColorFlow";
static gchar const * const koneplus_rmp_light_effect_type_name = "LightEffectType";
static gchar const * const koneplus_rmp_light_effect_speed_name = "LightEffectSpeed";
static gchar const * const koneplus_rmp_light_effect_mode_name = "LightEffectMode";
static gchar const * const koneplus_rmp_talkfx_name = "HDDKeyboard";

static gchar *koneplus_rmp_create_macro_key_info_name(guint index) {
	g_assert(index < KONEPLUS_PROFILE_BUTTON_NUM);
	return g_strdup_printf("MacroKeyInfo%i", index);
}

static gchar *koneplus_rmp_create_light_info_name(guint index) {
	g_assert(index < KONEPLUS_PROFILE_SETTING_LIGHTS_NUM);
	return g_strdup_printf("LightInfo%i", index);
}

static gchar *koneplus_rmp_create_cpi_name(guint bit) {
	g_assert(bit < KONEPLUS_PROFILE_SETTING_CPI_LEVELS_NUM);
	return g_strdup_printf("dpi%i", bit);
}

static gchar *koneplus_rmp_create_cpi_level_x_name(guint bit) {
	g_assert(bit < KONEPLUS_PROFILE_SETTING_CPI_LEVELS_NUM);
	return g_strdup_printf("CpiLevel_X%i", bit);
}

static gchar *koneplus_rmp_create_cpi_level_y_name(guint bit) {
	g_assert(bit < KONEPLUS_PROFILE_SETTING_CPI_LEVELS_NUM);
	return g_strdup_printf("CpiLevel_Y%i", bit);
}

static gchar *koneplus_rmp_create_game_file_name(guint index) {
	return g_strdup_printf("GameFile%i", index);
}

static void koneplus_rmp_macro_key_info_finalize(KoneplusRmpMacroKeyInfo *rmp_macro_key_info, guint index) {
	rmp_macro_key_info->button_number = index;
	koneplus_rmp_macro_key_info_set_checksum(rmp_macro_key_info);
}

static void koneplus_rmp_light_info_finalize(KoneplusRmpLightInfo *rmp_light_info) {
	koneplus_rmp_light_info_set_checksum(rmp_light_info);
}

guint koneplus_rmp_get_value(KoneplusRmp *rmp, gchar const *key) {
	GError *error = NULL;
	guint result = g_key_file_get_integer(rmp->key_file, koneplus_rmp_group_name, key, &error);
	if (error) {
		g_clear_error(&error);
		result = g_key_file_get_integer(rmp->default_key_file, koneplus_rmp_group_name, key, &error);
	}
	return result;
}

void koneplus_rmp_set_value(KoneplusRmp *rmp, gchar const *key, guint value) {
	if (koneplus_rmp_get_value(rmp, key) != value) {
		g_key_file_set_integer(rmp->key_file, koneplus_rmp_group_name, key, value);
		rmp->modified_settings = TRUE;
	}
}

void koneplus_rmp_set_macro_key_info(KoneplusRmp *rmp, guint index, KoneplusRmpMacroKeyInfo *rmp_macro_key_info) {
	gchar *key;
	KoneplusRmpMacroKeyInfo *actual_key_info;

	actual_key_info = koneplus_rmp_get_macro_key_info(rmp, index);
	if (!koneplus_rmp_macro_key_info_equal(rmp_macro_key_info, actual_key_info)) {
		key = koneplus_rmp_create_macro_key_info_name(index);
		koneplus_rmp_macro_key_info_finalize(rmp_macro_key_info, index);
		roccat_key_file_set_binary(rmp->key_file, koneplus_rmp_group_name, key, (gconstpointer)rmp_macro_key_info, sizeof(KoneplusRmpMacroKeyInfo));
		g_free(key);
		rmp->modified_macros[index] = TRUE;
	}
	koneplus_rmp_macro_key_info_free(actual_key_info);
}

static KoneplusRmpMacroKeyInfo *macro_key_info_with_fallback_without_default(GKeyFile *key_file, guint index) {
	gchar *key;
	GError *local_error = NULL;
	KoneplusRmpMacroKeyInfo *rmp_macro_key_info;
	KoneplusRmpMacroKeyInfoV1 *rmp_macro_key_info_v1;

	key = koneplus_rmp_create_macro_key_info_name(index);
	rmp_macro_key_info = roccat_key_file_get_binary(key_file, koneplus_rmp_group_name, key, sizeof(KoneplusRmpMacroKeyInfo), &local_error);

	if (g_error_matches(local_error, G_KEY_FILE_ERROR, G_KEY_FILE_ERROR_INVALID_VALUE)) {
		g_clear_error(&local_error);
		rmp_macro_key_info_v1 = roccat_key_file_get_binary(key_file, koneplus_rmp_group_name, key, sizeof(KoneplusRmpMacroKeyInfoV1), &local_error);
		if (local_error) {
			g_warning(_("Could not get value for key '%s': %s"), key, local_error->message);
			g_clear_error(&local_error);
		} else {
			rmp_macro_key_info = koneplus_rmp_macro_key_info_v1_to_koneplus_rmp_macro_key_info(rmp_macro_key_info_v1);
			g_free(rmp_macro_key_info_v1);
		}
	}

	g_free(key);
	return rmp_macro_key_info;
}

KoneplusRmpMacroKeyInfo *koneplus_rmp_get_macro_key_info(KoneplusRmp *rmp, guint index) {
	KoneplusRmpMacroKeyInfo *rmp_macro_key_info;

	rmp_macro_key_info = macro_key_info_with_fallback_without_default(rmp->key_file, index);
	if (rmp_macro_key_info)
		return rmp_macro_key_info;

	rmp_macro_key_info = macro_key_info_with_fallback_without_default(rmp->default_key_file, index);
	return rmp_macro_key_info;
}

void koneplus_rmp_set_profile_name(KoneplusRmp *rmp, gchar const *string) {
	gchar *actual_string;
	actual_string = koneplus_rmp_get_profile_name(rmp);
	if (strcmp(string, actual_string)) {
		g_key_file_set_string(rmp->key_file, koneplus_rmp_group_name, koneplus_rmp_profile_name_name, string);
		rmp->modified_rmp = TRUE;
	}
	g_free(actual_string);
}

gchar *koneplus_rmp_get_profile_name(KoneplusRmp *rmp) {
	GError *error = NULL;
	gchar *result;

	result = g_key_file_get_string(rmp->key_file, koneplus_rmp_group_name, koneplus_rmp_profile_name_name, &error);
	if (error) {
		g_clear_error(&error);
		result = g_key_file_get_string(rmp->default_key_file, koneplus_rmp_group_name, koneplus_rmp_profile_name_name, &error);
		if (error)
			g_error(_("Could not get default value for key '%s': %s"), koneplus_rmp_profile_name_name, error->message);
	}
	return result;
}

void koneplus_rmp_set_xy_synchronous(KoneplusRmp *rmp, guint value) {
 	koneplus_rmp_set_value(rmp, koneplus_rmp_xy_sync_name, value);
}

guint koneplus_rmp_get_xy_synchronous(KoneplusRmp *rmp) {
	return koneplus_rmp_get_value(rmp, koneplus_rmp_xy_sync_name);
}

void koneplus_rmp_set_sensitivity_x(KoneplusRmp *rmp, guint value) {
 	koneplus_rmp_set_value(rmp, koneplus_rmp_sensitivity_x_name, value);
}

guint koneplus_rmp_get_sensitivity_x(KoneplusRmp *rmp) {
	return koneplus_rmp_get_value(rmp, koneplus_rmp_sensitivity_x_name);
}

void koneplus_rmp_set_sensitivity_y(KoneplusRmp *rmp, guint value) {
 	koneplus_rmp_set_value(rmp, koneplus_rmp_sensitivity_y_name, value);
}

guint koneplus_rmp_get_sensitivity_y(KoneplusRmp *rmp) {
	return koneplus_rmp_get_value(rmp, koneplus_rmp_sensitivity_y_name);
}

void koneplus_rmp_set_sensitivity(KoneplusRmp *rmp, guint value) {
 	koneplus_rmp_set_value(rmp, koneplus_rmp_sensitivity_name, value);
}

guint koneplus_rmp_get_sensitivity(KoneplusRmp *rmp) {
	return koneplus_rmp_get_value(rmp, koneplus_rmp_sensitivity_name);
}

void koneplus_rmp_set_cpi(KoneplusRmp *rmp, guint bit, guint value) {
	gchar *key = NULL;
	g_assert(bit < KONEPLUS_PROFILE_SETTING_CPI_LEVELS_NUM);
	g_assert(value == KONEPLUS_PROFILE_SETTINGS_CPI_ACTIVE || value == KONEPLUS_PROFILE_SETTINGS_CPI_INACTIVE);
	if (koneplus_rmp_get_cpi(rmp, bit) != value) {
		key = koneplus_rmp_create_cpi_name(bit);
		g_key_file_set_integer(rmp->key_file, koneplus_rmp_group_name, key, value);
		g_free(key);
		rmp->modified_settings = TRUE;
	}
}

void koneplus_rmp_set_cpi_all(KoneplusRmp *rmp, guint value) {
	guint i;
	g_assert(!(value & 0xc0));
	for (i = 0; i < KONEPLUS_PROFILE_SETTING_CPI_LEVELS_NUM; ++i)
		koneplus_rmp_set_cpi(rmp, i, roccat_get_bit8(value, i));
}

guint koneplus_rmp_get_cpi(KoneplusRmp *rmp, guint bit) {
	gchar *key = NULL;
	guint result;
	g_assert(bit < KONEPLUS_PROFILE_SETTING_CPI_LEVELS_NUM);
	key = koneplus_rmp_create_cpi_name(bit);
	result = koneplus_rmp_get_value(rmp, key);
	g_free(key);
	return result;
}

guint8 koneplus_rmp_get_cpi_all(KoneplusRmp *rmp) {
	guint i;
	guint8 result = 0;
	for (i = 0; i < KONEPLUS_PROFILE_SETTING_CPI_LEVELS_NUM; ++i) {
		roccat_set_bit8(&result, i, koneplus_rmp_get_cpi(rmp, i));
	}
	return result;
}

KoneplusRmpLightInfo *koneplus_rmp_get_light_info_value(KoneplusRmp *rmp, gchar const *key) {
	KoneplusRmpLightInfo *rmp_light_info;
	GError *error = NULL;

	rmp_light_info = roccat_key_file_get_binary(rmp->key_file, koneplus_rmp_group_name, key, sizeof(KoneplusRmpLightInfo), &error);
	if (error) {
		g_clear_error(&error);
		rmp_light_info = roccat_key_file_get_binary(rmp->default_key_file, koneplus_rmp_group_name, key, sizeof(KoneplusRmpLightInfo), &error);
		if (error)
			g_error(_("Could not get default value for key '%s': %s"), key, error->message);
	}

	return rmp_light_info;
}

KoneplusRmpLightInfo *koneplus_rmp_get_rmp_light_info(KoneplusRmp *rmp, guint index) {
	KoneplusRmpLightInfo *rmp_light_info;
	gchar *key = NULL;

	g_assert(index < KONEPLUS_PROFILE_SETTING_LIGHTS_NUM);
	key = koneplus_rmp_create_light_info_name(index);
	rmp_light_info = koneplus_rmp_get_light_info_value(rmp, key);
	g_free(key);
	return rmp_light_info;
}

void koneplus_rmp_set_light_info_value(KoneplusRmp *rmp, gchar const *key, KoneplusRmpLightInfo *rmp_light_info) {
	KoneplusRmpLightInfo *actual_light_info;

	actual_light_info = koneplus_rmp_get_light_info_value(rmp, key);
	if (!koneplus_rmp_light_info_equal(rmp_light_info, actual_light_info)) {
		koneplus_rmp_light_info_finalize(rmp_light_info);
		roccat_key_file_set_binary(rmp->key_file, koneplus_rmp_group_name, key, (gconstpointer)rmp_light_info, sizeof(KoneplusRmpLightInfo));
		rmp->modified_settings = TRUE;
	}

	g_free(actual_light_info);
}

void koneplus_rmp_set_rmp_light_info(KoneplusRmp *rmp, guint index, KoneplusRmpLightInfo *rmp_light_info) {
	gchar *key = NULL;

	g_assert(index < KONEPLUS_PROFILE_SETTING_LIGHTS_NUM);
	key = koneplus_rmp_create_light_info_name(index);
	koneplus_rmp_set_light_info_value(rmp, key, rmp_light_info);
	g_free(key);
}

void koneplus_rmp_set_polling_rate(KoneplusRmp *rmp, guint value) {
 	koneplus_rmp_set_value(rmp, koneplus_rmp_polling_rate_name, value);
}

guint koneplus_rmp_get_polling_rate(KoneplusRmp *rmp) {
	return koneplus_rmp_get_value(rmp, koneplus_rmp_polling_rate_name);
}

void koneplus_rmp_set_light_effect_type(KoneplusRmp *rmp, guint value) {
 	g_assert(value >= KONEPLUS_PROFILE_SETTINGS_LIGHT_EFFECT_TYPE_ALL_OFF && value <= KONEPLUS_PROFILE_SETTINGS_LIGHT_EFFECT_TYPE_ALL_HEARTBEAT);
 	koneplus_rmp_set_value(rmp, koneplus_rmp_light_effect_type_name, value);
}

guint koneplus_rmp_get_light_effect_type(KoneplusRmp *rmp) {
	return koneplus_rmp_get_value(rmp, koneplus_rmp_light_effect_type_name);
}

void koneplus_rmp_set_light_effect_speed(KoneplusRmp *rmp, guint value) {
 	g_assert(value >= KONEPLUS_PROFILE_SETTINGS_LIGHT_EFFECT_SPEED_MIN && value <= KONEPLUS_PROFILE_SETTINGS_LIGHT_EFFECT_SPEED_MAX);
 	koneplus_rmp_set_value(rmp, koneplus_rmp_light_effect_speed_name, value);
}

guint koneplus_rmp_get_light_effect_speed(KoneplusRmp *rmp) {
	return koneplus_rmp_get_value(rmp, koneplus_rmp_light_effect_speed_name);
}

void koneplus_rmp_set_light_effect_mode(KoneplusRmp *rmp, guint value) {
 	koneplus_rmp_set_value(rmp, koneplus_rmp_light_effect_mode_name, value);
}

guint koneplus_rmp_get_light_effect_mode(KoneplusRmp *rmp) {
	return koneplus_rmp_get_value(rmp, koneplus_rmp_light_effect_mode_name);
}

void koneplus_rmp_set_light_color_flow(KoneplusRmp *rmp, guint value) {
 	g_assert(value >= KONEPLUS_PROFILE_SETTINGS_COLOR_FLOW_EFFECT_OFF && value <= KONEPLUS_PROFILE_SETTINGS_COLOR_FLOW_EFFECT_RIGHT);
 	koneplus_rmp_set_value(rmp, koneplus_rmp_light_color_flow_name, value);
}

guint koneplus_rmp_get_light_color_flow(KoneplusRmp *rmp) {
	return koneplus_rmp_get_value(rmp, koneplus_rmp_light_color_flow_name);
}

guint koneplus_rmp_get_cpi_level_x(KoneplusRmp *rmp, guint index) {
	gchar *key = NULL;
	guint result;

	g_assert(index < KONEPLUS_PROFILE_SETTING_CPI_LEVELS_NUM);
	key = koneplus_rmp_create_cpi_level_x_name(index);
	result = koneplus_rmp_get_value(rmp, key);
	g_free(key);
	return result;
}

void koneplus_rmp_set_cpi_level_x(KoneplusRmp *rmp, guint index, guint value) {
	gchar *key = NULL;
	g_assert(index < KONEPLUS_PROFILE_SETTING_CPI_LEVELS_NUM);
	if (koneplus_rmp_get_cpi_level_x(rmp, index) != value) {
		key = koneplus_rmp_create_cpi_level_x_name(index);
		g_key_file_set_integer(rmp->key_file, koneplus_rmp_group_name, key, value);
		g_free(key);
		rmp->modified_settings = TRUE;
	}
}

guint koneplus_rmp_get_cpi_level_y(KoneplusRmp *rmp, guint index) {
	gchar *key = NULL;
	guint result;

	g_assert(index < KONEPLUS_PROFILE_SETTING_CPI_LEVELS_NUM);
	key = koneplus_rmp_create_cpi_level_y_name(index);
	result = koneplus_rmp_get_value(rmp, key);
	g_free(key);
	return result;
}

void koneplus_rmp_set_cpi_level_y(KoneplusRmp *rmp, guint index, guint value) {
	gchar *key = NULL;
	g_assert(index < KONEPLUS_PROFILE_SETTING_CPI_LEVELS_NUM);
	if (koneplus_rmp_get_cpi_level_y(rmp, index) != value) {
		key = koneplus_rmp_create_cpi_level_y_name(index);
		g_key_file_set_integer(rmp->key_file, koneplus_rmp_group_name, key, value);
		g_free(key);
		rmp->modified_settings = TRUE;
	}
}

void koneplus_rmp_set_cpi_x(KoneplusRmp *rmp, guint value) {
 	g_assert(value < KONEPLUS_PROFILE_SETTING_CPI_LEVELS_NUM);
 	koneplus_rmp_set_value(rmp, koneplus_rmp_cpi_x_name, value);
}

guint koneplus_rmp_get_cpi_x(KoneplusRmp *rmp) {
	return koneplus_rmp_get_value(rmp, koneplus_rmp_cpi_x_name);
}

void koneplus_rmp_set_cpi_y(KoneplusRmp *rmp, guint value) {
 	g_assert(value < KONEPLUS_PROFILE_SETTING_CPI_LEVELS_NUM);
 	koneplus_rmp_set_value(rmp, koneplus_rmp_cpi_y_name, value);
}

guint koneplus_rmp_get_cpi_y(KoneplusRmp *rmp) {
	return koneplus_rmp_get_value(rmp, koneplus_rmp_cpi_y_name);
}

void koneplus_rmp_set_talkfx(KoneplusRmp *rmp, guint value) {
	koneplus_rmp_set_value(rmp, koneplus_rmp_talkfx_name, value);
}

guint koneplus_rmp_get_talkfx(KoneplusRmp *rmp) {
	return koneplus_rmp_get_value(rmp, koneplus_rmp_talkfx_name);
}

void koneplus_rmp_set_game_file_name(KoneplusRmp *rmp, guint index, gchar const *string) {
	gchar *key;
	gchar *actual_string;

	key = koneplus_rmp_create_game_file_name(index);
	actual_string = koneplus_rmp_get_game_file_name(rmp, index);
	if (strcmp(string, actual_string)) {
		g_key_file_set_string(rmp->key_file, koneplus_rmp_group_name, key, string);
		rmp->modified_rmp = TRUE;
	}
	g_free(actual_string);
	g_free(key);
}

gchar *koneplus_rmp_get_game_file_name(KoneplusRmp *rmp, guint index) {
	GError *error = NULL;
	gchar *result;
	gchar *key;

	key = koneplus_rmp_create_game_file_name(index);
	result = g_key_file_get_string(rmp->key_file, koneplus_rmp_group_name, key, &error);
	if (error) {
		g_clear_error(&error);
		result = g_key_file_get_string(rmp->default_key_file, koneplus_rmp_group_name, key, &error);
		if (error)
			g_error(_("Could not get default value for key '%s': %s"), key, error->message);
	}
	g_free(key);
	return result;
}
