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

#include "kone_internal.h"
#include "roccat_helper.h"
#include "g_roccat_helper.h"
#include "i18n-lib.h"
#include <string.h>

static gchar const * const kone_rmp_group_name = "Setting";
static gchar const * const kone_rmp_main_sensitivity_name = "MainSensitivity";
static gchar const * const kone_rmp_advanced_sensitivity_name = "XYSensitivityEnabled";
static gchar const * const kone_rmp_x_sensitivity_name = "XSensitivity";
static gchar const * const kone_rmp_y_sensitivity_name = "YSensitivity";
static gchar const * const kone_rmp_startup_dpi_name = "dpiValue";
static gchar const * const kone_rmp_polling_rate_name = "PollingRate";
static gchar const * const kone_rmp_dcu_flag_name = "DCUFlag";
static gchar const * const kone_rmp_light_effect_1_name = "LightEffect1";
static gchar const * const kone_rmp_light_effect_2_name = "LightEffect2";
static gchar const * const kone_rmp_light_effect_3_name = "LightEffect3";
static gchar const * const kone_rmp_light_effect_speed_name = "iLTEffectSpd";
static gchar const * const kone_rmp_game_file_name = "GameFile";
static gchar const * const kone_rmp_profile_name_name = "nsProfileName";

static gchar *kone_rmp_create_light_info_key(guint index) {
	return g_strdup_printf("LightInfo%i", index);
}

static gchar *kone_rmp_create_button_info_key(guint index) {
	return g_strdup_printf("ButtonInfo%i", index);
}

static gchar *kone_rmp_create_dpi_key(guint bit) {
	return g_strdup_printf("dpi%i", bit);
}

/* number is in range of 1 to 8 */
static void kone_rmp_button_info_finalize(KoneRMPButtonInfo *rmp_button_info, guint number) {
	rmp_button_info->number = number;
	kone_rmp_button_info_set_checksum(rmp_button_info);
}

void kone_rmp_set_rmp_button_info(KoneRMP *rmp, guint index, KoneRMPButtonInfo *rmp_button_info) {
	gchar *key = NULL;
	KoneRMPButtonInfo *actual_button_info;

	g_assert(index < KONE_BUTTON_INFO_NUM);

	actual_button_info = kone_rmp_get_rmp_button_info(rmp, index);
	if (!kone_rmp_button_info_equal(rmp_button_info, actual_button_info)) {
		key = kone_rmp_create_button_info_key(index);
		kone_rmp_button_info_finalize(rmp_button_info, index + 1);
		roccat_key_file_set_binary(rmp->key_file, kone_rmp_group_name, key, (gconstpointer)rmp_button_info, sizeof(KoneRMPButtonInfo));
		g_free(key);
		rmp->modified_profile = TRUE;
	}
	g_free(actual_button_info);
}

/* number is in range 1 to 5 */
static void kone_rmp_light_info_finalize(KoneRMPLightInfo *rmp_light_info, guint number) {
	rmp_light_info->number = number;
	kone_rmp_light_info_set_checksum(rmp_light_info);
}

void kone_rmp_set_rmp_light_info(KoneRMP *rmp, guint index, KoneRMPLightInfo *rmp_light_info) {
	gchar *key = NULL;
	KoneRMPLightInfo *actual_light_info;

	g_assert(index < KONE_LIGHT_INFO_NUM);

	actual_light_info = kone_rmp_get_rmp_light_info(rmp, index);
	if (!kone_rmp_light_info_equal(rmp_light_info, actual_light_info)) {
		key = kone_rmp_create_light_info_key(index);
		kone_rmp_light_info_finalize(rmp_light_info, index + 1);
		roccat_key_file_set_binary(rmp->key_file, kone_rmp_group_name, key, (gconstpointer)rmp_light_info, sizeof(KoneRMPLightInfo));
		g_free(key);
		rmp->modified_profile = TRUE;
	}
	g_free(actual_light_info);
}

/*
 * May terminate application if error occurs
 */
static guint kone_rmp_get_default_value(gchar const *key) {
	KoneRMP const *default_rmp = kone_default_rmp();
	GError *error = NULL;
	guint result;
	result = g_key_file_get_integer(default_rmp->key_file, kone_rmp_group_name, key, &error);
	if (error)
		g_error(_("Could not get default value for key '%s': %s"), key, error->message);
	return result;
}

static guint kone_rmp_get_value(KoneRMP *rmp, gchar const *key) {
	GError *error = NULL;
	guint result = g_key_file_get_integer(rmp->key_file, kone_rmp_group_name, key, &error);
	if (error) {
		g_clear_error(&error);
		result = kone_rmp_get_default_value(key);
	}
	return result;
}

static void kone_rmp_set_value(KoneRMP *rmp, gchar const *key, guint value) {
	if (kone_rmp_get_value(rmp, key) != value) {
		g_key_file_set_integer(rmp->key_file, kone_rmp_group_name, key, value);
		rmp->modified_profile = TRUE;
	}
}

void kone_rmp_set_main_sensitivity(KoneRMP *rmp, guint value) {
	g_assert(value >= KONE_MAIN_SENSITIVITY_MIN && value <= KONE_MAIN_SENSITIVITY_MAX);
	kone_rmp_set_value(rmp, kone_rmp_main_sensitivity_name, value);
}

void kone_rmp_set_advanced_sensitivity(KoneRMP *rmp, guint value) {
	g_assert(value == KONE_ADVANCED_SENSITIVITY_ON || value == KONE_ADVANCED_SENSITIVITY_OFF);
	kone_rmp_set_value(rmp, kone_rmp_advanced_sensitivity_name, value);
}

void kone_rmp_set_x_sensitivity(KoneRMP *rmp, guint value) {
	g_assert(value >= KONE_X_SENSITIVITY_MIN && value <= KONE_X_SENSITIVITY_MAX);
	kone_rmp_set_value(rmp, kone_rmp_x_sensitivity_name, value);
}

void kone_rmp_set_y_sensitivity(KoneRMP *rmp, guint value) {
	g_assert(value >= KONE_Y_SENSITIVITY_MIN && value <= KONE_Y_SENSITIVITY_MAX);
	kone_rmp_set_value(rmp, kone_rmp_y_sensitivity_name, value);
}

void kone_rmp_set_dpi(KoneRMP *rmp, guint bit, guint value) {
	gchar *key = NULL;
	g_assert(bit < KONE_DPI_NUM);
	g_assert(value == KONE_DPI_ACTIVE || value == KONE_DPI_INACTIVE);
	key = kone_rmp_create_dpi_key(bit);
	kone_rmp_set_value(rmp, key, value);
	g_free(key);
}

void kone_rmp_set_dpi_all(KoneRMP *rmp, guint value) {
	guint i;
	g_assert(!(value & 0xc0));
	for (i = 0; i < KONE_DPI_NUM; ++i)
		kone_rmp_set_dpi(rmp, i, roccat_get_bit8(value, i));
}

void kone_rmp_set_startup_dpi(KoneRMP *rmp, guint value) {
	g_assert(value >= KONE_DPI_800 && value <= KONE_DPI_3200);
	kone_rmp_set_value(rmp, kone_rmp_startup_dpi_name, value);
}

void kone_rmp_set_polling_rate(KoneRMP *rmp, guint value) {
	g_assert(value >= KONE_POLLING_RATE_125 && value <= KONE_POLLING_RATE_1000);
	kone_rmp_set_value(rmp, kone_rmp_polling_rate_name, value);
}

void kone_rmp_set_dcu_flag(KoneRMP *rmp, guint value) {
	g_assert(value == KONE_DCU_ON || value == KONE_DCU_OFF);
	kone_rmp_set_value(rmp, kone_rmp_dcu_flag_name, value);
}

void kone_rmp_set_light_effect_1(KoneRMP *rmp, guint value) {
	g_assert(value >= KONE_LIGHT_EFFECT_1_SELECTED_COLOR && value <= KONE_LIGHT_EFFECT_1_RANDOM_MULTI);
	kone_rmp_set_value(rmp, kone_rmp_light_effect_1_name, value);
}

void kone_rmp_set_light_effect_2(KoneRMP *rmp, guint value) {
	g_assert(value >= KONE_LIGHT_EFFECT_2_FIXED_COLOR && value <= KONE_LIGHT_EFFECT_2_MOVE_HORIZONTAL);
	kone_rmp_set_value(rmp, kone_rmp_light_effect_2_name, value);
}

void kone_rmp_set_light_effect_3(KoneRMP *rmp, guint value) {
	g_assert(value >= KONE_LIGHT_EFFECT_3_FULL_LIGHTENED && value <= KONE_LIGHT_EFFECT_3_HEARTBEAT);
	kone_rmp_set_value(rmp, kone_rmp_light_effect_3_name, value);
}

void kone_rmp_set_light_effect_speed(KoneRMP *rmp, guint value) {
	g_assert(value >= KONE_LIGHT_EFFECT_SPEED_MIN && value <= KONE_LIGHT_EFFECT_SPEED_MAX);
	kone_rmp_set_value(rmp, kone_rmp_light_effect_speed_name, value);
}

guint kone_rmp_get_main_sensitivity(KoneRMP *rmp) {
	return kone_rmp_get_value(rmp, kone_rmp_main_sensitivity_name);
}

guint kone_rmp_get_advanced_sensitivity(KoneRMP *rmp) {
	return kone_rmp_get_value(rmp, kone_rmp_advanced_sensitivity_name);
}

guint kone_rmp_get_x_sensitivity(KoneRMP *rmp) {
	return kone_rmp_get_value(rmp, kone_rmp_x_sensitivity_name);
}

guint kone_rmp_get_y_sensitivity(KoneRMP *rmp) {
	return kone_rmp_get_value(rmp, kone_rmp_y_sensitivity_name);
}

guint kone_rmp_get_startup_dpi(KoneRMP *rmp) {
	return kone_rmp_get_value(rmp, kone_rmp_startup_dpi_name);
}

guint kone_rmp_get_polling_rate(KoneRMP *rmp) {
	return kone_rmp_get_value(rmp, kone_rmp_polling_rate_name);
}

guint kone_rmp_get_dcu_flag(KoneRMP *rmp) {
	return kone_rmp_get_value(rmp, kone_rmp_dcu_flag_name);
}

guint kone_rmp_get_light_effect_1(KoneRMP *rmp) {
	return kone_rmp_get_value(rmp, kone_rmp_light_effect_1_name);
}

guint kone_rmp_get_light_effect_2(KoneRMP *rmp) {
	return kone_rmp_get_value(rmp, kone_rmp_light_effect_2_name);
}

guint kone_rmp_get_light_effect_3(KoneRMP *rmp) {
	return kone_rmp_get_value(rmp, kone_rmp_light_effect_3_name);
}

guint kone_rmp_get_light_effect_speed(KoneRMP *rmp) {
	return kone_rmp_get_value(rmp, kone_rmp_light_effect_speed_name);
}

guint kone_rmp_get_dpi(KoneRMP *rmp, guint bit) {
	gchar *key = NULL;
	guint result;
	g_assert(bit < KONE_DPI_NUM);
	key = kone_rmp_create_dpi_key(bit);
	result = kone_rmp_get_value(rmp, key);
	g_free(key);
	return result;
}

guint8 kone_rmp_get_dpi_all(KoneRMP *rmp) {
	guint i;
	guint8 result = 0;
	for (i = 0; i < KONE_DPI_NUM; ++i)
		roccat_set_bit8(&result, i, kone_rmp_get_dpi(rmp, i));
	return result;
}

KoneRMPButtonInfo *kone_rmp_get_rmp_button_info(KoneRMP *rmp, guint index) {
	KoneRMP const *default_rmp;
	GError *error = NULL;
	gchar *key = NULL;
	KoneRMPButtonInfo *rmp_button_info;

	g_assert(index < KONE_BUTTON_INFO_NUM);
	key = kone_rmp_create_button_info_key(index);
	rmp_button_info = roccat_key_file_get_binary(rmp->key_file, kone_rmp_group_name, key, sizeof(KoneRMPButtonInfo), &error);
	if (error) {
		g_clear_error(&error);
		default_rmp = kone_default_rmp();
		rmp_button_info = roccat_key_file_get_binary(default_rmp->key_file, kone_rmp_group_name, key, sizeof(KoneRMPButtonInfo), &error);
		if (error)
			g_error(_("Could not get default value for key '%s': %s"), key, error->message);
	}

	g_free(key);
	return rmp_button_info;
}

KoneRMPLightInfo *kone_rmp_get_rmp_light_info(KoneRMP *rmp, guint index) {
	KoneRMP const *default_rmp;
	GError *error = NULL;
	gchar *key = NULL;
	KoneRMPLightInfo *rmp_light_info;

	g_assert(index < KONE_LIGHT_INFO_NUM);
	key = kone_rmp_create_light_info_key(index);
	rmp_light_info = roccat_key_file_get_binary(rmp->key_file, kone_rmp_group_name, key, sizeof(KoneRMPLightInfo), &error);
	if (error) {
		g_clear_error(&error);
		default_rmp = kone_default_rmp();
		rmp_light_info = roccat_key_file_get_binary(default_rmp->key_file, kone_rmp_group_name, key, sizeof(KoneRMPLightInfo), &error);
		if (error)
			g_error(_("Could not get default value for key '%s': %s"), key, error->message);
	}

	g_free(key);
	return rmp_light_info;
}

void kone_rmp_set_game_file_name(KoneRMP *rmp, gchar const *string) {
	gchar *actual_string;

	actual_string = kone_rmp_get_game_file_name(rmp);
	if (strcmp(string, actual_string)) {
		g_key_file_set_string(rmp->key_file, kone_rmp_group_name, kone_rmp_game_file_name, string);
		rmp->modified_rmp = TRUE;
	}
	g_free(actual_string);
}

gchar *kone_rmp_get_game_file_name(KoneRMP *rmp) {
	KoneRMP const *default_rmp;
	GError *error = NULL;
	gchar *result;

	result = g_key_file_get_string(rmp->key_file, kone_rmp_group_name, kone_rmp_game_file_name, &error);
	if (error) {
		g_clear_error(&error);
		default_rmp = kone_default_rmp();
		result = g_key_file_get_string(default_rmp->key_file, kone_rmp_group_name, kone_rmp_game_file_name, &error);
		if (error)
			g_error(_("Could not get default value for key '%s': %s"), kone_rmp_game_file_name, error->message);
	}
	return result;
}

void kone_rmp_remove_profile_name(KoneRMP *rmp) {
	g_key_file_remove_key(rmp->key_file, kone_rmp_group_name, kone_rmp_profile_name_name, NULL);
}

void kone_rmp_set_profile_name(KoneRMP *rmp, gchar const *string) {
	gchar *actual_string;

	actual_string = kone_rmp_get_profile_name(rmp);
	if (strcmp(string, actual_string)) {
		g_key_file_set_string(rmp->key_file, kone_rmp_group_name, kone_rmp_profile_name_name, string);
		rmp->modified_rmp = TRUE;
	}
	g_free(actual_string);
}

gchar *kone_rmp_get_profile_name(KoneRMP *rmp) {
	GError *error = NULL;
	gchar *result;

	result = g_key_file_get_string(rmp->key_file, kone_rmp_group_name, kone_rmp_profile_name_name, &error);
	if (error) {
		g_clear_error(&error);
		return g_strdup("Default");
	}
	return result;
}
