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

#include "isku_rkp.h"
#include "g_roccat_helper.h"
#include "i18n-lib.h"
#include <string.h>

static gchar const * const isku_rkp_group_name = "Setting";

static gchar const * const isku_rkp_profile_name_name = "ProFileName";
static gchar const * const isku_rkp_light_brightness_name = "LtBrightness";
static gchar const * const isku_rkp_light_timeout_name = "LtTimeout";
static gchar const * const isku_rkp_light_dimness_name = "DimmerOption";

static gchar *isku_rkp_create_macro_key_info_name(guint index) {
	return g_strdup_printf("MacroKeyInfo%i", index);
}

static gchar *isku_rkp_create_modify_key_name(guint index) {
	return g_strdup_printf("ModifyKey%i", index);
}

static gchar *isku_rkp_create_game_file_name(guint index) {
	return g_strdup_printf("GameFile%i", index);
}

/*
 * May terminate application if error occurs
 */
static guint isku_rkp_get_default_value(gchar const *key) {
	IskuRkp const *default_rkp = isku_default_rkp();
	GError *error = NULL;
	guint result;
	result = g_key_file_get_integer(default_rkp->key_file, isku_rkp_group_name, key, &error);
	if (error)
		g_error(_("Could not get default value for key '%s': %s"), key, error->message);
	return result;
}

guint isku_rkp_get_value(IskuRkp *rkp, gchar const *key) {
	GError *error = NULL;
	guint result = g_key_file_get_integer(rkp->key_file, isku_rkp_group_name, key, &error);
	if (error) {
		g_clear_error(&error);
		result = isku_rkp_get_default_value(key);
	}
	return result;
}

void isku_rkp_set_value(IskuRkp *rkp, gchar const *key, guint value) {
	g_key_file_set_integer(rkp->key_file, isku_rkp_group_name, key, value);
}

void isku_rkp_set_profile_name(IskuRkp *rkp, gchar const *string) {
	gchar *actual_string;
	actual_string = isku_rkp_get_profile_name(rkp);
	if (strcmp(string, actual_string)) {
		g_key_file_set_string(rkp->key_file, isku_rkp_group_name, isku_rkp_profile_name_name, string);
		rkp->modified_rkp = TRUE;
	}
	g_free(actual_string);
}

gchar *isku_rkp_get_profile_name(IskuRkp *rkp) {
	IskuRkp const *default_rkp;
	GError *error = NULL;
	gchar *result;

	result = g_key_file_get_string(rkp->key_file, isku_rkp_group_name, isku_rkp_profile_name_name, &error);
	if (error) {
		g_clear_error(&error);
		default_rkp = isku_default_rkp();
		result = g_key_file_get_string(default_rkp->key_file, isku_rkp_group_name, isku_rkp_profile_name_name, &error);
		if (error)
			g_error(_("Could not get default value for key '%s': %s"), isku_rkp_profile_name_name, error->message);
	}
	return result;
}

void isku_rkp_set_light_brightness_without_modified(IskuRkp *rkp, guint value) {
	g_assert(value >= ISKU_LIGHT_BRIGHTNESS_MIN && value <= ISKU_LIGHT_BRIGHTNESS_MAX);
	isku_rkp_set_value(rkp, isku_rkp_light_brightness_name, value);
}

void isku_rkp_set_light_brightness(IskuRkp *rkp, guint value) {
	if (isku_rkp_get_light_brightness(rkp) != value) {
		isku_rkp_set_light_brightness_without_modified(rkp, value);
		rkp->modified_light = TRUE;
	}
}

guint isku_rkp_get_light_brightness(IskuRkp *rkp) {
	return isku_rkp_get_value(rkp, isku_rkp_light_brightness_name);
}

void isku_rkp_set_light_dimness(IskuRkp *rkp, guint value) {
	g_assert(value >= ISKU_LIGHT_BRIGHTNESS_MIN && value <= ISKU_LIGHT_BRIGHTNESS_MAX);
	if (isku_rkp_get_light_dimness(rkp) != value) {
		isku_rkp_set_value(rkp, isku_rkp_light_dimness_name, value);
		rkp->modified_light = TRUE;
	}
}

guint isku_rkp_get_light_dimness(IskuRkp *rkp) {
	return isku_rkp_get_value(rkp, isku_rkp_light_dimness_name);
}

void isku_rkp_set_light_timeout(IskuRkp *rkp, guint value) {
	g_assert(value < G_MAXUINT8);
	if (isku_rkp_get_light_timeout(rkp) != value) {
		isku_rkp_set_value(rkp, isku_rkp_light_timeout_name, value);
		rkp->modified_light = TRUE;
	}
}

guint isku_rkp_get_light_timeout(IskuRkp *rkp) {
	return isku_rkp_get_value(rkp, isku_rkp_light_timeout_name);
}

static void isku_rkp_macro_key_info_finalize(IskuRkpMacroKeyInfo *key_info, guint index) {
	key_info->index = index;
	isku_rkp_macro_key_info_set_checksum(key_info);
}

void isku_rkp_set_macro_key_info_without_modified(IskuRkp *rkp, guint index, IskuRkpMacroKeyInfo *key_info) {
	gchar *key;

	g_assert(index < ISKU_KEY_INDEX_NUM);

	key = isku_rkp_create_macro_key_info_name(index);
	isku_rkp_macro_key_info_finalize(key_info, index);
	roccat_key_file_set_binary(rkp->key_file, isku_rkp_group_name, key, (gconstpointer)key_info, sizeof(IskuRkpMacroKeyInfo));
	g_free(key);
}

void isku_rkp_set_macro_key_info(IskuRkp *rkp, guint index, IskuRkpMacroKeyInfo *key_info) {
	IskuRkpMacroKeyInfo *actual_key_info;

	actual_key_info = isku_rkp_get_macro_key_info(rkp, index);
	if (!isku_rkp_macro_key_info_equal(key_info, actual_key_info)) {
		isku_rkp_set_macro_key_info_without_modified(rkp, index, key_info);
		rkp->modified_keys[index] = TRUE;
	}

	isku_rkp_macro_key_info_free(actual_key_info);
}

IskuRkpMacroKeyInfo *isku_rkp_get_macro_key_info(IskuRkp*rkp, guint index) {
	IskuRkp const *default_rkp;
	GError *error = NULL;
	gchar *key;
	IskuRkpMacroKeyInfo *key_info;

	g_assert(index < ISKU_KEY_INDEX_NUM);

	key = isku_rkp_create_macro_key_info_name(index);
	key_info = roccat_key_file_get_binary(rkp->key_file, isku_rkp_group_name, key, sizeof(IskuRkpMacroKeyInfo), &error);
	if (error) {
		g_clear_error(&error);
		default_rkp = isku_default_rkp();
		key_info = roccat_key_file_get_binary(default_rkp->key_file, isku_rkp_group_name, key, sizeof(IskuRkpMacroKeyInfo), &error);
		if (error)
			g_error(_("Could not get default value for key '%s': %s"), key, error->message);
	}
	g_free(key);
	return key_info;
}

gboolean isku_rkp_macro_key_info_equal_type_actual(IskuRkp *rkp, guint key_index, guint type) {
	IskuRkpMacroKeyInfo *actual_key_info;
	gboolean retval;
	actual_key_info = isku_rkp_get_macro_key_info(rkp, key_index);
	retval = (type == actual_key_info->type);
	isku_rkp_macro_key_info_free(actual_key_info);
	return retval;
}

void isku_rkp_set_modify_key(IskuRkp *rkp, guint index, gboolean value) {
	gchar *key;

	g_assert(index < ISKU_KEY_MASK_BIT_NUM);

	if (isku_rkp_get_modify_key(rkp, index) != value) {
		key = isku_rkp_create_modify_key_name(index);
		g_key_file_set_integer(rkp->key_file, isku_rkp_group_name, key, value);
		g_free(key);
		rkp->modified_key_mask = TRUE;
	}
}

gboolean isku_rkp_get_modify_key(IskuRkp *rkp, guint index) {
	IskuRkp const *default_rkp;
	GError *error = NULL;
	gboolean result;
	gchar *key;

	g_assert(index < ISKU_KEY_MASK_BIT_NUM);

	key = isku_rkp_create_modify_key_name(index);
	result = g_key_file_get_integer(rkp->key_file, isku_rkp_group_name, key, &error);
	if (error) {
		g_clear_error(&error);
		default_rkp = isku_default_rkp();
		result = g_key_file_get_integer(default_rkp->key_file, isku_rkp_group_name, key, &error);
		if (error)
			g_error(_("Could not get default value for key '%s': %s"), key, error->message);
	}
	g_free(key);
	return result;
}

void isku_rkp_set_game_file_name(IskuRkp *rkp, guint index, gchar const *string) {
	gchar *key;
	gchar *actual_string;

	key = isku_rkp_create_game_file_name(index);
	actual_string = isku_rkp_get_game_file_name(rkp, index);
	if (strcmp(string, actual_string)) {
		g_key_file_set_string(rkp->key_file, isku_rkp_group_name, key, string);
		rkp->modified_rkp = TRUE;
	}
	g_free(actual_string);
	g_free(key);
}

gchar *isku_rkp_get_game_file_name(IskuRkp *rkp, guint index) {
	IskuRkp const *default_rkp;
	GError *error = NULL;
	gchar *result;
	gchar *key;

	key = isku_rkp_create_game_file_name(index);
	result = g_key_file_get_string(rkp->key_file, isku_rkp_group_name, key, &error);
	if (error) {
		g_clear_error(&error);
		default_rkp = isku_default_rkp();
		result = g_key_file_get_string(default_rkp->key_file, isku_rkp_group_name, key, &error);
		if (error)
			g_error(_("Could not get default value for key '%s': %s"), key, error->message);
	}
	g_free(key);
	return result;
}
