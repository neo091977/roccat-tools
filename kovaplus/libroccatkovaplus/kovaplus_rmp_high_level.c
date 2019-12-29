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
#include "roccat.h"
#include "i18n-lib.h"
#include <string.h>

static gchar *kovaplus_profile_dir(void) {
	gchar *base = roccat_profile_dir();
	gchar *dir = g_build_path("/", base, "kovaplus", NULL);
	g_free(base);
	return dir;
}

static gboolean kovaplus_profile_dir_create_if_needed(GError **error) {
	gchar *dir;
	gboolean retval;

	dir = kovaplus_profile_dir();
	retval = roccat_profile_dir_create_if_needed(dir, error);
	g_free(dir);
	return retval;
}

static gboolean kovaplus_profile_settings_check_equality(KovaplusProfileSettings const *left, KovaplusProfileSettings const *right) {
	if (memcmp(left, right, sizeof(KovaplusProfileSettings)))
		return FALSE;
	return TRUE;
}

static gchar *build_config_rmp_path(guint profile_index) {
	gchar *dir = kovaplus_profile_dir();
	gchar *filename = g_strdup_printf("actual%i." ROCCAT_MOUSE_PROFILE_EXTENSION, profile_index + 1);
	gchar *path = g_build_path("/", dir, filename, NULL);
	g_free(filename);
	g_free(dir);
	return path;
}

gboolean kovaplus_rmp_get_modified(KovaplusRmp const *rmp) {
	return (rmp->modified_rmp || rmp->modified_profile_settings || rmp->modified_profile_buttons);
}

static void kovaplus_rmp_set_modified_state(KovaplusRmp *rmp, gboolean state) {
	rmp->modified_rmp = state;
	rmp->modified_profile_buttons = state;
	rmp->modified_profile_settings = state;
}

void kovaplus_rmp_set_modified(KovaplusRmp *rmp) {
	kovaplus_rmp_set_modified_state(rmp, TRUE);
}

void kovaplus_rmp_set_unmodified(KovaplusRmp *rmp) {
	kovaplus_rmp_set_modified_state(rmp, FALSE);
}

void kovaplus_rmp_update_from_device(KovaplusRmp *rmp, RoccatDevice *kovaplus, guint profile_index) {
	KovaplusProfileButtons *hardware_profile_buttons, *rmp_profile_buttons;
	KovaplusProfileSettings *hardware_profile_settings, *rmp_profile_settings;
	GError *error = NULL;

	gaminggear_device_lock(GAMINGGEAR_DEVICE(kovaplus));

	hardware_profile_settings = kovaplus_profile_settings_read(kovaplus, profile_index, &error);

	if (hardware_profile_settings) {
		rmp_profile_settings = kovaplus_rmp_to_profile_settings(rmp);
		kovaplus_profile_settings_finalize(rmp_profile_settings, profile_index);

		if (!kovaplus_profile_settings_check_equality(hardware_profile_settings, rmp_profile_settings)) {
			g_warning(_("Hardware data and rmp for profile %u differ"), profile_index + 1);
			kovaplus_rmp_update_with_profile_settings(rmp, hardware_profile_settings);
		}
		g_free(rmp_profile_settings);
		g_free(hardware_profile_settings);
	} else
		g_clear_error(&error);

	hardware_profile_buttons = kovaplus_profile_buttons_read(kovaplus, profile_index, &error);

	if (hardware_profile_buttons) {
		rmp_profile_buttons = kovaplus_rmp_to_profile_buttons(rmp);
		kovaplus_rmp_update_with_profile_buttons(rmp, hardware_profile_buttons);
		g_free(rmp_profile_buttons);
		g_free(hardware_profile_buttons);
	} else
		g_clear_error(&error);

	gaminggear_device_unlock(GAMINGGEAR_DEVICE(kovaplus));
}

KovaplusRmp *kovaplus_rmp_load_actual(guint profile_index) {
	KovaplusRmp *rmp;
	gchar *config_path;

	g_assert(profile_index < KOVAPLUS_PROFILE_NUM);

	config_path = build_config_rmp_path(profile_index);
	rmp = kovaplus_rmp_read_with_path(config_path, NULL);
	g_free(config_path);
	if (!rmp) {
		rmp = kovaplus_rmp_dup(kovaplus_default_rmp());
		rmp->modified_rmp = TRUE;
	}
	return rmp;
}

KovaplusRmp *kovaplus_rmp_load(RoccatDevice *kovaplus, guint profile_index, GError **error) {
	KovaplusRmp *rmp;

	rmp = kovaplus_rmp_load_actual(profile_index);
	kovaplus_rmp_update_from_device(rmp, kovaplus, profile_index);

	return rmp;
}

gboolean kovaplus_rmp_save_actual(KovaplusRmp *rmp, guint profile_index, GError **error) {
	gchar *config_path;
	gboolean retval;

	kovaplus_profile_dir_create_if_needed(error);
	if (*error)
		return FALSE;

	config_path = build_config_rmp_path(profile_index);
	retval = kovaplus_rmp_write_with_path(config_path, rmp, error);
	g_free(config_path);

	return retval;
}

static gboolean kovaplus_rmp_save_actual_when_needed(KovaplusRmp *rmp, guint profile_index, GError **error) {
	gboolean retval = TRUE;

	if (kovaplus_rmp_get_modified(rmp)) {
		retval = kovaplus_rmp_save_actual(rmp, profile_index, error);
		rmp->modified_rmp = FALSE;
	}

	return retval;
}

gboolean kovaplus_rmp_save(RoccatDevice *kovaplus, KovaplusRmp *rmp, guint profile_index, GError **error) {
	gboolean retval = FALSE;
	KovaplusProfileButtons *profile_buttons;
	KovaplusProfileSettings *profile_settings;

	g_assert(profile_index < KOVAPLUS_PROFILE_NUM);

	if (!kovaplus_rmp_save_actual_when_needed(rmp, profile_index, error)) return FALSE;

	gaminggear_device_lock(GAMINGGEAR_DEVICE(kovaplus));

	if (rmp->modified_profile_buttons) {
		profile_buttons = kovaplus_rmp_to_profile_buttons(rmp);
		kovaplus_profile_buttons_write(kovaplus, profile_index, profile_buttons, error);
		g_free(profile_buttons);
		if (*error)
			goto error;
	}

	if (rmp->modified_profile_settings) {
		profile_settings = kovaplus_rmp_to_profile_settings(rmp);
		kovaplus_profile_settings_write(kovaplus, profile_index, profile_settings, error);
		g_free(profile_settings);
		if (*error)
			goto error;
	}

	kovaplus_rmp_set_unmodified(rmp);
	retval = TRUE;
error:
	gaminggear_device_unlock(GAMINGGEAR_DEVICE(kovaplus));
	return retval;
}

KovaplusRmp *kovaplus_rmp_load_save_after_reset(RoccatDevice *kovaplus, guint profile_index, GError **error) {
	KovaplusRmp *rmp;

	rmp = kovaplus_rmp_dup(kovaplus_default_rmp());

	kovaplus_rmp_update_from_device(rmp, kovaplus, profile_index);
	kovaplus_rmp_set_unmodified(rmp);

	kovaplus_rmp_save_actual(rmp, profile_index, error);
	return rmp;
}
