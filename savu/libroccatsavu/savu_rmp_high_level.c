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
#include "roccat_helper.h"
#include "roccat.h"
#include "i18n-lib.h"

static gchar *savu_profile_dir(void) {
	gchar *base = roccat_profile_dir();
	gchar *dir = g_build_path("/", base, "savu", NULL);
	g_free(base);
	return dir;
}

static gboolean savu_profile_dir_create_if_needed(GError **error) {
	gchar *dir;
	gboolean retval;

	dir = savu_profile_dir();
	retval = roccat_profile_dir_create_if_needed(dir, error);
	g_free(dir);
	return retval;
}

gboolean savu_rmp_get_modified(SavuRmp const *rmp) {
	guint i;

	if (rmp->modified_buttons ||
			rmp->modified_general ||
			rmp->modified_rmp)
		return TRUE;

	for (i = 0; i < SAVU_BUTTON_NUM; ++i)
		if (rmp->modified_macros[i])
			return TRUE;

	return FALSE;
}

static void savu_rmp_set_modified_state(SavuRmp *rmp, gboolean state) {
	guint i;
	rmp->modified_rmp = state;
	rmp->modified_buttons = state;
	rmp->modified_general = state;
	for (i = 0; i < SAVU_BUTTON_NUM; ++i)
		rmp->modified_macros[i] = state;
}

void savu_rmp_set_modified(SavuRmp *rmp) {
	savu_rmp_set_modified_state(rmp, TRUE);
}

void savu_rmp_set_unmodified(SavuRmp *rmp) {
	savu_rmp_set_modified_state(rmp, FALSE);
}

static gchar *savu_build_config_rmp_path(guint profile_index) {
	gchar *dir = savu_profile_dir();
	gchar *filename = g_strdup_printf("actual%i." ROCCAT_MOUSE_PROFILE_EXTENSION, profile_index);
	gchar *path = g_build_path("/", dir, filename, NULL);
	g_free(filename);
	g_free(dir);
	return path;
}

SavuRmp *savu_rmp_load_actual(guint profile_index) {
	SavuRmp *rmp;
	gchar *config_path;
	config_path = savu_build_config_rmp_path(profile_index);
	rmp = savu_rmp_read_with_path(config_path, NULL);
	g_free(config_path);
	if (!rmp)
		rmp = savu_rmp_dup(savu_default_rmp());
	return rmp;
}

static void savu_rmp_differ_warning(SavuRmp const *rmp, guint profile_index) {
	guint i;

	if (savu_rmp_get_modified(rmp))
		g_warning(_("Hardware data and rmp for profile %u differ"), profile_index + 1);

	if (rmp->modified_general)
		g_warning(_("general"));
	if (rmp->modified_buttons)
		g_warning(_("buttons"));

	for (i = 0; i < SAVU_BUTTON_NUM; ++i)
		if (rmp->modified_macros[i])
			g_warning(_("key 0x%02x"), i);
}

static void savu_rmp_update_with_hardware_macro(RoccatDevice *savu, SavuRmp *rmp, guint profile_index, guint button_index) {
	GError *error = NULL;
	SavuMacro *macro = savu_macro_read(savu, profile_index, button_index, &error);
	if (macro) {
		savu_rmp_update_with_macro(rmp, button_index, macro);
		g_free(macro);
	} else
		g_clear_error(&error);
}

static void savu_rmp_update_with_hardware_buttons(RoccatDevice *savu, SavuRmp *rmp, guint profile_index) {
	GError *error = NULL;
	guint i;
	SavuButtons *buttons = savu_buttons_read(savu, profile_index, &error);
	if (buttons) {
		savu_rmp_update_with_buttons(rmp, buttons);

		for (i = 0; i < SAVU_BUTTON_NUM; ++i) {
			if (buttons->buttons[i].type == SAVU_BUTTON_TYPE_MACRO)
				savu_rmp_update_with_hardware_macro(savu, rmp, profile_index, i);
		}

		g_free(buttons);
	} else
		g_clear_error(&error);
}

static void savu_rmp_update_with_hardware_general(RoccatDevice *savu, SavuRmp *rmp, guint profile_index) {
	GError *error = NULL;
	SavuGeneral *general = savu_general_read(savu, profile_index, &error);
	if (general) {
		savu_rmp_update_with_general(rmp, general);
		g_free(general);
	} else
		g_clear_error(&error);
}

void savu_rmp_update_from_device(RoccatDevice *savu, SavuRmp *rmp, guint profile_index) {
	gaminggear_device_lock(GAMINGGEAR_DEVICE(savu));
	savu_rmp_update_with_hardware_buttons(savu, rmp, profile_index);
	savu_rmp_update_with_hardware_general(savu, rmp, profile_index);
	gaminggear_device_unlock(GAMINGGEAR_DEVICE(savu));
}

SavuRmp *savu_rmp_load(RoccatDevice *savu, guint profile_index, GError **error) {
	SavuRmp *rmp;

	g_assert(profile_index < SAVU_PROFILE_NUM);

	rmp = savu_rmp_load_actual(profile_index);

	savu_rmp_update_from_device(savu, rmp, profile_index);

	savu_rmp_differ_warning(rmp, profile_index);

	return rmp;
}

gboolean savu_rmp_save_actual(SavuRmp *rmp, guint profile_index, GError **error) {
	gchar *config_path;
	gboolean retval;

	savu_profile_dir_create_if_needed(error);
	if (*error)
		return FALSE;

	config_path = savu_build_config_rmp_path(profile_index);
	retval = savu_rmp_write_with_path(config_path, rmp, error);
	g_free(config_path);
	return retval;
}

static gboolean savu_rmp_save_actual_when_needed(SavuRmp *rmp, guint profile_index, GError **error) {
	gboolean retval = TRUE;

	if (savu_rmp_get_modified(rmp)) {
		retval = savu_rmp_save_actual(rmp, profile_index, error);
		if (retval)
			rmp->modified_rmp = FALSE;
	}

	return retval;
}

static gboolean savu_rmp_save_general_when_needed(RoccatDevice *savu, SavuRmp *rmp, guint profile_index, GError **error) {
	SavuGeneral *general;
	gboolean retval = TRUE;
	if (rmp->modified_general) {
		general = savu_rmp_to_general(rmp);
		retval = savu_general_write(savu, profile_index, general, error);
		g_free(general);
		rmp->modified_general = FALSE;
	}
	return retval;
}

static gboolean savu_rmp_save_macro(RoccatDevice *savu, SavuRmp *rmp, guint profile_index, guint button_index, GError **error) {
	SavuMacro *macro;
	gboolean retval;
	macro = savu_rmp_to_macro(rmp, button_index);
	retval = savu_macro_write(savu, profile_index, button_index, macro, error);
	g_free(macro);
	return retval;
}

static gboolean savu_rmp_save_macros_when_needed(RoccatDevice *savu, SavuRmp *rmp, guint profile_index, GError **error) {
	SavuButtons *buttons;
	guint i;
	gboolean retval = TRUE;

	buttons = savu_rmp_to_buttons(rmp);

	for (i = 0; i < SAVU_BUTTON_NUM; ++i) {
		if (rmp->modified_macros[i]) {
			if (buttons->buttons[i].type == SAVU_BUTTON_TYPE_MACRO) {
				if (savu_rmp_save_macro(savu, rmp, profile_index, i, error) == FALSE) {
					retval = FALSE;
					break;
				}
			}
			rmp->modified_macros[i] = FALSE;
		}
	}

	g_free(buttons);
	return retval;
}

static gboolean savu_rmp_save_buttons_when_needed(RoccatDevice *savu, SavuRmp *rmp, guint profile_index, GError **error) {
	SavuButtons *buttons;
	gboolean retval = TRUE;

	if (rmp->modified_buttons) {
		buttons = savu_rmp_to_buttons(rmp);
		retval = savu_buttons_write(savu, profile_index, buttons, error);
		g_free(buttons);
		rmp->modified_buttons = FALSE;
	}

	return retval;
}

gboolean savu_rmp_save(RoccatDevice *savu, SavuRmp *rmp, guint profile_index, GError **error) {
	gboolean retval = FALSE;

	g_assert(profile_index < SAVU_PROFILE_NUM);

	if (!savu_rmp_get_modified(rmp)) return TRUE;

	if (!savu_rmp_save_actual_when_needed(rmp, profile_index, error)) return FALSE;

	gaminggear_device_lock(GAMINGGEAR_DEVICE(savu));

	if (!savu_rmp_save_general_when_needed(savu, rmp, profile_index, error)) goto error;
	if (!savu_rmp_save_buttons_when_needed(savu, rmp, profile_index, error)) goto error;
	if (!savu_rmp_save_macros_when_needed(savu, rmp, profile_index, error)) goto error;

	retval = TRUE;
error:
	gaminggear_device_unlock(GAMINGGEAR_DEVICE(savu));
	return retval;
}

SavuRmp *savu_rmp_load_save_after_reset(RoccatDevice *savu, guint profile_index, GError **error) {
	SavuRmp *rmp;

	rmp = savu_rmp_dup(savu_default_rmp());

	savu_rmp_update_from_device(savu, rmp, profile_index);
	savu_rmp_set_unmodified(rmp);

	savu_rmp_save_actual(rmp, profile_index, error);
	return rmp;
}
