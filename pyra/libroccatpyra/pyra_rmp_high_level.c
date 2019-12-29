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

#include "pyra_rmp.h"
#include "roccat_helper.h"
#include "roccat.h"
#include "i18n-lib.h"
#include <string.h>

static gchar *pyra_profile_dir(void) {
	gchar *base = roccat_profile_dir();
	gchar *dir = g_build_path("/", base, "pyra", NULL);
	g_free(base);
	return dir;
}

static gboolean pyra_profile_dir_create_if_needed(GError **error) {
	gchar *dir;
	gboolean retval;

	dir = pyra_profile_dir();
	retval = roccat_profile_dir_create_if_needed(dir, error);
	g_free(dir);
	return retval;
}

static gboolean pyra_profile_buttons_check_equality(PyraProfileButtons const *left, PyraProfileButtons const *right) {
	if (memcmp(left, right, sizeof(PyraProfileButtons)))
		return FALSE;
	return TRUE;
}

static gboolean pyra_profile_settings_check_equality(PyraProfileSettings const *left, PyraProfileSettings const *right) {
	if (memcmp(left, right, sizeof(PyraProfileSettings)))
		return FALSE;
	return TRUE;
}

static gchar *pyra_build_config_rmp_path(guint profile_index) {
	gchar *dir = pyra_profile_dir();
	gchar *filename = g_strdup_printf("actual%i." ROCCAT_MOUSE_PROFILE_EXTENSION, profile_index + 1);
	gchar *path = g_build_path("/", dir, filename, NULL);
	g_free(filename);
	g_free(dir);
	return path;
}

gboolean pyra_rmp_get_modified(PyraRMP const *rmp) {
	return (rmp->modified_rmp || rmp->modified_profile_buttons || rmp->modified_profile_settings);
}

static void pyra_rmp_set_modified_state(PyraRMP *rmp, gboolean state) {
	rmp->modified_rmp = state;
	rmp->modified_profile_buttons = state;
	rmp->modified_profile_settings = state;
}

void pyra_rmp_set_modified(PyraRMP *rmp) {
	pyra_rmp_set_modified_state(rmp, TRUE);
}

void pyra_rmp_set_unmodified(PyraRMP *rmp) {
	pyra_rmp_set_modified_state(rmp, FALSE);
}

void pyra_rmp_update_from_device(PyraRMP *rmp, RoccatDevice *pyra, guint profile_index) {
	PyraProfileButtons *hardware_profile_buttons, *rmp_profile_buttons;
	PyraProfileSettings *hardware_profile_settings, *rmp_profile_settings;
	GError *error = NULL;

	gaminggear_device_lock(GAMINGGEAR_DEVICE(pyra));

	hardware_profile_settings = pyra_profile_settings_read(pyra, profile_index, &error);

	if (hardware_profile_settings) {
		rmp_profile_settings = pyra_rmp_to_profile_settings(rmp);
		pyra_profile_settings_finalize(rmp_profile_settings, profile_index);

		if (!!pyra_profile_settings_check_equality(hardware_profile_settings, rmp_profile_settings)) {
			g_warning(_("Hardware data and rmp for profile %u differ"), profile_index + 1);
			pyra_rmp_update_with_profile_settings(rmp, hardware_profile_settings);
		}
		g_free(rmp_profile_settings);
		g_free(hardware_profile_settings);
	} else
		g_clear_error(&error);

	hardware_profile_buttons = pyra_profile_buttons_read(pyra, profile_index, &error);

	if (hardware_profile_buttons) {
		rmp_profile_buttons = pyra_rmp_to_profile_buttons(rmp);
		pyra_profile_buttons_finalize(rmp_profile_buttons, profile_index);

		if (!pyra_profile_buttons_check_equality(hardware_profile_buttons, rmp_profile_buttons)) {
			g_warning(_("Hardware data and rmp for profile %u differ"), profile_index + 1);
			pyra_rmp_update_with_profile_buttons(rmp, hardware_profile_buttons);
		}
		g_free(rmp_profile_buttons);
		g_free(hardware_profile_buttons);
	} else
		g_clear_error(&error);

	gaminggear_device_unlock(GAMINGGEAR_DEVICE(pyra));
}

PyraRMP *pyra_rmp_load_actual(guint profile_index) {
	PyraRMP *rmp;
	gchar *config_path;

	config_path = pyra_build_config_rmp_path(profile_index);
	rmp = pyra_rmp_read_with_path(config_path, NULL);
	g_free(config_path);
	if (!rmp)
		rmp = pyra_rmp_dup(pyra_default_rmp());
	return rmp;
}

PyraRMP *pyra_rmp_load(RoccatDevice *pyra, guint profile_index, GError **error) {
	PyraRMP *rmp;

	g_assert(profile_index < PYRA_PROFILE_NUM);

	rmp = pyra_rmp_load_actual(profile_index);

	pyra_rmp_update_from_device(rmp, pyra, profile_index);

	return rmp;
}

gboolean pyra_rmp_save_actual(PyraRMP *rmp, guint profile_index, GError **error) {
	gchar *config_path;
	gboolean retval;

	pyra_profile_dir_create_if_needed(error);
	if (*error)
		return FALSE;

	config_path = pyra_build_config_rmp_path(profile_index);
	retval = pyra_rmp_write_with_path(config_path, rmp, error);
	g_free(config_path);

	return retval;
}

static gboolean pyra_rmp_save_actual_when_needed(PyraRMP *rmp, guint profile_index, GError **error) {
	gboolean retval = TRUE;

	if (pyra_rmp_get_modified(rmp)) {
		retval = pyra_rmp_save_actual(rmp, profile_index, error);
		rmp->modified_rmp = FALSE;
	}

	return retval;
}

gboolean pyra_rmp_save(RoccatDevice *pyra, PyraRMP *rmp, guint profile_index, GError **error) {
	gboolean retval = FALSE;
	PyraProfileButtons *profile_buttons;
	PyraProfileSettings *profile_settings;

	g_assert(profile_index < PYRA_PROFILE_NUM);

	if (!pyra_rmp_save_actual_when_needed(rmp, profile_index, error)) return FALSE;

	gaminggear_device_lock(GAMINGGEAR_DEVICE(pyra));

	if (rmp->modified_profile_buttons) {
		profile_buttons = pyra_rmp_to_profile_buttons(rmp);
		pyra_profile_buttons_write(pyra, profile_index, profile_buttons, error);
		g_free(profile_buttons);
		if (*error)
			goto error;
	}

	if (rmp->modified_profile_settings) {
		profile_settings = pyra_rmp_to_profile_settings(rmp);
		pyra_profile_settings_write(pyra, profile_index, profile_settings, error);
		g_free(profile_settings);
		if (*error)
			goto error;
	}

	pyra_rmp_set_unmodified(rmp);
	retval = TRUE;
error:
	gaminggear_device_unlock(GAMINGGEAR_DEVICE(pyra));
	return retval;
}

PyraRMP *pyra_rmp_load_save_after_reset(RoccatDevice *pyra, guint profile_index, GError **error) {
	PyraRMP *rmp;

	rmp = pyra_rmp_dup(pyra_default_rmp());

	pyra_rmp_update_from_device(rmp, pyra, profile_index);
	pyra_rmp_set_unmodified(rmp);

	pyra_rmp_save_actual(rmp, profile_index, error);
	return rmp;
}
