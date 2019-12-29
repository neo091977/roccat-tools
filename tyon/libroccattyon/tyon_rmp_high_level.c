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

#include "tyon.h"
#include "roccat_helper.h"
#include "tyon_rmp.h"
#include "roccat.h"
#include "i18n-lib.h"
#include <string.h>

static gchar *tyon_profile_dir(void) {
	gchar *base = roccat_profile_dir();
	gchar *dir = g_build_path("/", base, "tyon", NULL);
	g_free(base);
	return dir;
}

static gboolean tyon_profile_dir_create_if_needed(GError **error) {
	gchar *dir;
	gboolean retval;

	dir = tyon_profile_dir();
	retval = roccat_profile_dir_create_if_needed(dir, error);
	g_free(dir);
	return retval;
}

static gchar *tyon_build_config_rmp_path(guint profile_number) {
	gchar *dir = tyon_profile_dir();
	gchar *filename = g_strdup_printf("actual%i." ROCCAT_MOUSE_PROFILE_EXTENSION, profile_number + 1);
	gchar *path = g_build_path("/", dir, filename, NULL);
	g_free(filename);
	g_free(dir);
	return path;
}

static gboolean tyon_rmp_profile_buttons_modified(TyonRmp const *rmp) {
	guint i;

	for (i = 0; i < TYON_PROFILE_BUTTON_NUM; ++i) {
		if (rmp->modified_macros[i])
			return TRUE;
	}
	return FALSE;
}

gboolean tyon_rmp_get_modified(TyonRmp const *rmp) {
	if (rmp->modified_rmp)
		return TRUE;
	if (rmp->modified_settings)
		return TRUE;
	if (tyon_rmp_profile_buttons_modified(rmp))
		return TRUE;
	return FALSE;
}

static void tyon_rmp_set_modified_state(TyonRmp *rmp, gboolean state) {
	guint i;
	rmp->modified_rmp = state;
	rmp->modified_settings = state;
	for (i = 0; i < TYON_PROFILE_BUTTON_NUM; ++i)
		rmp->modified_macros[i] = state;
}

void tyon_rmp_set_modified(TyonRmp *rmp) {
	tyon_rmp_set_modified_state(rmp, TRUE);
}

void tyon_rmp_set_unmodified(TyonRmp *rmp) {
	tyon_rmp_set_modified_state(rmp, FALSE);
}

TyonRmp *tyon_rmp_load_actual(guint profile_index) {
	TyonRmp *rmp;
	gchar *config_path;

	config_path = tyon_build_config_rmp_path(profile_index);
	rmp = tyon_rmp_read_with_path(config_path, tyon_rmp_defaults(), NULL);
	g_free(config_path);
	if (!rmp) {
		rmp = tyon_default_rmp();
		rmp->modified_rmp = TRUE;
	}
	return rmp;
}

static void tyon_rmp_update_with_hardware_macro(TyonRmp *rmp, RoccatDevice *device, guint profile_index, guint profile_buttons_index) {
	GError *error = NULL;
	TyonMacro *hardware_macro;
	hardware_macro = tyon_macro_read(device, profile_index, profile_buttons_index, &error);
	if (error) {
		g_clear_error(&error);
	} else {
		tyon_rmp_update_with_macro(rmp, profile_buttons_index, hardware_macro);
		g_free(hardware_macro);
	}
}

void tyon_rmp_update_from_device(TyonRmp *rmp, RoccatDevice *tyon, guint profile_index) {
	TyonProfileButtons *hardware_profile_buttons, *rmp_profile_buttons;
	TyonProfileSettings *hardware_profile_settings, *rmp_profile_settings;
	GError *error = NULL;
	guint i;

	gaminggear_device_lock(GAMINGGEAR_DEVICE(tyon));

	/* hardware takes precedence */
	hardware_profile_settings = tyon_profile_settings_read(tyon, profile_index, &error);

	if (hardware_profile_settings) {
		rmp_profile_settings = tyon_rmp_to_profile_settings(rmp);
		tyon_profile_settings_finalize(rmp_profile_settings, profile_index);

		if (!tyon_profile_settings_equal(hardware_profile_settings, rmp_profile_settings)) {
			g_warning(_("Hardware data and rmp for profile %u differ"), profile_index + 1);
			tyon_rmp_update_with_profile_settings(rmp, hardware_profile_settings);
		}
		g_free(rmp_profile_settings);
		g_free(hardware_profile_settings);
	} else
		g_clear_error(&error);

	hardware_profile_buttons = tyon_profile_buttons_read(tyon, profile_index, &error);

	if (hardware_profile_buttons) {
		rmp_profile_buttons = tyon_rmp_to_profile_buttons(rmp);

		if (!tyon_profile_buttons_equal(hardware_profile_buttons, rmp_profile_buttons)) {
			g_warning(_("Hardware data and rmp for profile %u differ"), profile_index + 1);
			tyon_rmp_update_with_profile_buttons(rmp, hardware_profile_buttons);
		}

		g_free(rmp_profile_buttons);

		for (i = 0; i < TYON_PROFILE_BUTTON_NUM; ++i) {
			if (hardware_profile_buttons->buttons[i].type == TYON_BUTTON_TYPE_MACRO)
				tyon_rmp_update_with_hardware_macro(rmp, tyon, profile_index, i);
		}

		g_free(hardware_profile_buttons);
	} else
		g_clear_error(&error);

	gaminggear_device_unlock(GAMINGGEAR_DEVICE(tyon));
}

TyonRmp *tyon_rmp_load(RoccatDevice *tyon, guint profile_index, GError **error) {
	TyonRmp *rmp;

	g_assert(profile_index < TYON_PROFILE_NUM);

	rmp = tyon_rmp_load_actual(profile_index);

	tyon_rmp_update_from_device(rmp, tyon, profile_index);

	return rmp;
}

gboolean tyon_rmp_save_actual(TyonRmp *rmp, guint profile_index, GError **error) {
	gchar *config_path;
	gboolean retval;

	tyon_profile_dir_create_if_needed(error);
	if (*error)
		return FALSE;

	config_path = tyon_build_config_rmp_path(profile_index);
	retval = tyon_rmp_write_with_path(config_path, rmp, error);
	g_free(config_path);

	return retval;
}

static gboolean tyon_rmp_save_profile_settings(RoccatDevice *tyon, TyonRmp *rmp, guint profile_index, GError **error) {
	TyonProfileSettings *profile_settings;
	gboolean retval;

	profile_settings = tyon_rmp_to_profile_settings(rmp);
	retval = tyon_profile_settings_write(tyon, profile_index, profile_settings, error);
	g_free(profile_settings);

	return retval;
}

static gboolean tyon_rmp_save_profile_buttons(RoccatDevice *tyon, TyonRmp *rmp, guint profile_index, GError **error) {
	TyonProfileButtons *profile_buttons;
	gboolean retval;

	profile_buttons = tyon_rmp_to_profile_buttons(rmp);
	retval = tyon_profile_buttons_write(tyon, profile_index, profile_buttons, error);
	g_free(profile_buttons);

	return retval;
}

static gboolean tyon_rmp_save_macro(RoccatDevice *tyon, TyonRmp *rmp, guint profile_index, guint key_index, GError **error) {
	TyonRmpMacroKeyInfo *macro_key_info;
	TyonMacro *macro;
	guint retval = TRUE;

	macro_key_info = tyon_rmp_get_macro_key_info(rmp, key_index);
	macro = tyon_rmp_macro_key_info_to_macro(macro_key_info);
	g_free(macro_key_info);

	if (macro) {
		retval = tyon_macro_write(tyon, profile_index, key_index, macro, error);
		g_free(macro);
	}

	return retval;
}

static gboolean tyon_rmp_save_actual_when_needed(TyonRmp *rmp, guint profile_index, GError **error) {
	gboolean retval = TRUE;

	if (tyon_rmp_get_modified(rmp)) {
		retval = tyon_rmp_save_actual(rmp, profile_index, error);
		rmp->modified_rmp = FALSE;
	}

	return retval;
}

static gboolean tyon_rmp_save_profile_settings_when_needed(RoccatDevice *tyon, TyonRmp *rmp, guint profile_index, GError **error) {
	gboolean retval = TRUE;

	if (rmp->modified_settings) {
		retval = tyon_rmp_save_profile_settings(tyon, rmp, profile_index, error);
		rmp->modified_settings = FALSE;
	}

	return retval;
}

static gboolean tyon_rmp_save_macro_when_needed(RoccatDevice *tyon, TyonRmp *rmp, guint profile_index, guint key_index, GError **error) {
	gboolean retval = TRUE;

	if (rmp->modified_macros[key_index]) {
		retval = tyon_rmp_save_macro(tyon, rmp, profile_index, key_index, error);
		rmp->modified_macros[key_index] = FALSE;
	}

	return retval;
}

static gboolean tyon_rmp_save_profile_buttons_when_needed(RoccatDevice *tyon, TyonRmp *rmp, guint profile_index, GError **error) {
	gboolean retval = TRUE;
	guint i;

	if (tyon_rmp_profile_buttons_modified(rmp)) {
		retval = tyon_rmp_save_profile_buttons(tyon, rmp, profile_index, error);
		if (!retval)
			return FALSE;

		for (i = 0; i < TYON_PROFILE_BUTTON_NUM; ++i) {
			if (!tyon_rmp_save_macro_when_needed(tyon, rmp, profile_index, i, error))
				return FALSE;
		}
	}
	return retval;
}

gboolean tyon_rmp_save(RoccatDevice *tyon, TyonRmp *rmp, guint profile_index, GError **error) {
	gboolean retval = FALSE;

	g_assert(profile_index < TYON_PROFILE_NUM);

	if (!tyon_rmp_save_actual_when_needed(rmp, profile_index, error)) return FALSE;

	gaminggear_device_lock(GAMINGGEAR_DEVICE(tyon));

	if (!tyon_rmp_save_profile_buttons_when_needed(tyon, rmp, profile_index, error)) goto error;
	if (!tyon_rmp_save_profile_settings_when_needed(tyon, rmp, profile_index, error)) goto error;

	retval = TRUE;
error:
	gaminggear_device_unlock(GAMINGGEAR_DEVICE(tyon));
	return retval;
}

TyonRmp *tyon_rmp_load_save_after_reset(RoccatDevice *tyon, guint profile_index, GError **error) {
	TyonRmp *rmp;

	rmp = tyon_default_rmp();

	/* Hardware profiles differ in light-color and -effects */
	tyon_rmp_update_from_device(rmp, tyon, profile_index);
	tyon_rmp_set_unmodified(rmp);

	tyon_rmp_save_actual(rmp, profile_index, error);
	return rmp;
}
