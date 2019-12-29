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

#include "arvo_rkp.h"
#include "roccat_helper.h"
#include "g_roccat_helper.h"
#include "i18n-lib.h"
#include <string.h>

static gchar const * const group_name = "Setting";
static gchar const * const mode_name = "Mode";
static gchar const * const profile_name_name = "ProFileName";

static gchar *create_button_info_name(guint index) {
	return g_strdup_printf("ButtonInfo%i", index);
}

static gchar *create_key_name(guint index) {
	return g_strdup_printf("key%i", index);
}

static gchar *create_game_file_name(guint index) {
	return g_strdup_printf("GameFile%i", index);
}

static void arvo_rkp_button_info_finalize(ArvoRkpButtonInfo *rkp_button_info, guint index) {
	rkp_button_info->number = index + 1;
}

void arvo_rkp_set_button_info(ArvoRkp *rkp, guint index, ArvoRkpButtonInfo *rkp_button_info) {
	gchar *key;
	ArvoRkpButtonInfo *actual_button_info;

	g_assert(index < ARVO_BUTTON_NUM);

	actual_button_info = arvo_rkp_get_button_info(rkp, index);
	if (!arvo_rkp_button_info_equal(rkp_button_info, actual_button_info)) {
		key = create_button_info_name(index);
		arvo_rkp_button_info_finalize(rkp_button_info, index);
		roccat_key_file_set_binary(rkp->key_file, group_name, key, (gconstpointer)rkp_button_info, sizeof(ArvoRkpButtonInfo));
		g_free(key);
		rkp->modified_buttons[index] = TRUE;
	}
	arvo_rkp_button_info_free(actual_button_info);
}

ArvoRkpButtonInfo *arvo_rkp_get_button_info(ArvoRkp *rkp, guint index) {
	ArvoRkp const *default_rkp;
	GError *error = NULL;
	gchar *key;
	ArvoRkpButtonInfo *rkp_button_info;

	g_assert(index < ARVO_BUTTON_NUM);

	key = create_button_info_name(index);
	rkp_button_info = roccat_key_file_get_binary(rkp->key_file, group_name, key, sizeof(ArvoRkpButtonInfo), &error);
	if (error) {
		g_clear_error(&error);
		default_rkp = arvo_default_rkp();
		rkp_button_info = roccat_key_file_get_binary(default_rkp->key_file, group_name, key, sizeof(ArvoRkpButtonInfo), &error);
		if (error)
			g_error(_("Could not get default value for key '%s': %s"), key, error->message);
	}
	g_free(key);
	return rkp_button_info;
}

void arvo_rkp_set_mode_key(ArvoRkp *rkp, guint value) {
	if (arvo_rkp_get_mode_key(rkp) != value)  {
		g_key_file_set_integer(rkp->key_file, group_name, mode_name, value);
		rkp->modified_mode_key = TRUE;
	}
}

guint arvo_rkp_get_mode_key(ArvoRkp *rkp) {
	ArvoRkp const *default_rkp;
	GError *error = NULL;
	guint result;
	result = g_key_file_get_integer(rkp->key_file, group_name, mode_name, &error);
	if (error) {
		g_clear_error(&error);
		default_rkp = arvo_default_rkp();
		result = g_key_file_get_integer(default_rkp->key_file, group_name, mode_name, &error);
		if (error)
			g_error(_("Could not get default value for key '%s': %s"), mode_name, error->message);
	}
	return result;
}

void arvo_rkp_set_key(ArvoRkp *rkp, guint index, guint value) {
	gchar *key;

	g_assert(index < ARVO_KEY_NUM);

	if (arvo_rkp_get_key(rkp, index) != value) {
		key = create_key_name(index);
		g_key_file_set_integer(rkp->key_file, group_name, key, value);
		g_free(key);
		rkp->modified_key_mask = TRUE;
	}
}

guint arvo_rkp_get_key(ArvoRkp *rkp, guint index) {
	ArvoRkp const *default_rkp;
	GError *error = NULL;
	guint result;
	gchar *key;

	g_assert(index < ARVO_KEY_NUM);

	key = create_key_name(index);
	result = g_key_file_get_integer(rkp->key_file, group_name, key, &error);
	if (error) {
		g_clear_error(&error);
		default_rkp = arvo_default_rkp();
		result = g_key_file_get_integer(default_rkp->key_file, group_name, key, &error);
		if (error)
			g_error(_("Could not get default value for key '%s': %s"), key, error->message);
	}
	g_free(key);
	return result;
}

guint8 arvo_rkp_get_key_mask(ArvoRkp *rkp) {
	guint8 result = 0;
	guint i;

	for (i = 0; i < ARVO_KEY_NUM; ++i)
		roccat_set_bit8(&result, i, arvo_rkp_get_key(rkp, i));
	return result;
}

void arvo_rkp_set_profile_name(ArvoRkp *rkp, gchar const *string) {
	gchar *actual_string;
	actual_string = arvo_rkp_get_profile_name(rkp);
	if (strcmp(string, actual_string)) {
		g_key_file_set_string(rkp->key_file, group_name, profile_name_name, string);
		rkp->modified_rkp = TRUE;
	}
	g_free(actual_string);
}

gchar *arvo_rkp_get_profile_name(ArvoRkp *rkp) {
	ArvoRkp const *default_rkp;
	GError *error = NULL;
	gchar *result;

	result = g_key_file_get_string(rkp->key_file, group_name, profile_name_name, &error);
	if (error) {
		g_clear_error(&error);
		default_rkp = arvo_default_rkp();
		result = g_key_file_get_string(default_rkp->key_file, group_name, profile_name_name, &error);
		if (error)
			g_error(_("Could not get default value for key '%s': %s"), profile_name_name, error->message);
	}
	return result;
}

void arvo_rkp_set_game_file_name(ArvoRkp *rkp, guint index, gchar const *string) {
	gchar *key;
	gchar *actual_string;

	key = create_game_file_name(index);
	actual_string = arvo_rkp_get_game_file_name(rkp, index);
	if (strcmp(string, actual_string)) {
		g_key_file_set_string(rkp->key_file, group_name, key, string);
		rkp->modified_rkp = TRUE;
	}
	g_free(actual_string);
	g_free(key);
}

gchar *arvo_rkp_get_game_file_name(ArvoRkp *rkp, guint index) {
	ArvoRkp const *default_rkp;
	GError *error = NULL;
	gchar *result;
	gchar *key;

	key = create_game_file_name(index);
	result = g_key_file_get_string(rkp->key_file, group_name, key, &error);
	if (error) {
		g_clear_error(&error);
		default_rkp = arvo_default_rkp();
		result = g_key_file_get_string(default_rkp->key_file, group_name, key, &error);
		if (error)
			g_error(_("Could not get default value for key '%s': %s"), key, error->message);
	}
	g_free(key);
	return result;
}
