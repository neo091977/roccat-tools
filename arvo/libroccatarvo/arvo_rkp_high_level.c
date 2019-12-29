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
#include "roccat.h"
#include <string.h>

static gchar *arvo_profile_dir(void) {
	gchar *base = roccat_profile_dir();
	gchar *dir = g_build_path("/", base, "arvo", NULL);
	g_free(base);
	return dir;
}

static gboolean arvo_profile_dir_create_if_needed(GError **error) {
	gchar *dir;
	gboolean retval;

	dir = arvo_profile_dir();
	retval = roccat_profile_dir_create_if_needed(dir, error);
	g_free(dir);
	return retval;
}

static gchar *arvo_build_config_rkp_path(guint profile_number) {
	gchar *dir = arvo_profile_dir();
	gchar *filename = g_strdup_printf("actual%i." ROCCAT_KEYBOARD_PROFILE_EXTENSION, profile_number);
	gchar *path = g_build_path("/", dir, filename, NULL);
	g_free(filename);
	g_free(dir);
	return path;
}

gboolean arvo_rkp_get_modified(ArvoRkp const *rkp) {
	guint i;

	if (rkp->modified_rkp || rkp->modified_key_mask || rkp->modified_mode_key)
		return TRUE;

	for (i = 0; i < ARVO_BUTTON_NUM; ++i)
		if (rkp->modified_buttons[i])
			return TRUE;

	return FALSE;
}

static void arvo_rkp_set_modified_state(ArvoRkp *rkp, gboolean state) {
	guint i;

	rkp->modified_rkp = state;
	rkp->modified_key_mask = state;
	rkp->modified_mode_key = state;
	for (i = 0; i < ARVO_BUTTON_NUM; ++i)
		rkp->modified_buttons[i] = state;
}

void arvo_rkp_set_modified(ArvoRkp *rkp) {
	arvo_rkp_set_modified_state(rkp, TRUE);
}

void arvo_rkp_set_unmodified(ArvoRkp *rkp) {
	arvo_rkp_set_modified_state(rkp, FALSE);
}

ArvoRkp *arvo_rkp_load_actual(guint profile_number) {
	ArvoRkp *rkp;
	gchar *config_path;
	config_path = arvo_build_config_rkp_path(profile_number);
	rkp = arvo_rkp_read_with_path(config_path, NULL);
	g_free(config_path);
	if (!rkp)
		rkp = arvo_rkp_dup(arvo_default_rkp());
	return rkp;
}

ArvoRkp *arvo_rkp_load(RoccatDevice const *arvo, guint profile_number, GError **error) {
	g_assert(profile_number >= 1 && profile_number <= ARVO_PROFILE_NUM);

	/*
	 * You can only get shortcuts out of the hardware, so I let it.
	 * Mode key and disabled keys only for active profile.
	 */
	return arvo_rkp_load_actual(profile_number);
}

gboolean arvo_rkp_save_actual(ArvoRkp *rkp, guint profile_number, GError **error) {
	gchar *config_path;
	gboolean retval;

	arvo_profile_dir_create_if_needed(error);
	if (*error)
		return FALSE;

	config_path = arvo_build_config_rkp_path(profile_number);
	retval = arvo_rkp_write_with_path(config_path, rkp, error);
	g_free(config_path);
	return retval;
}

gboolean arvo_rkp_save(RoccatDevice *arvo, ArvoRkp *rkp, guint profile_number, GError **error) {
	ArvoRkpButtonInfo *button_info;
	ArvoButton *button;
	guint i;
	gboolean result;

	g_assert(profile_number >= 1 && profile_number <= ARVO_PROFILE_NUM);

	if (!arvo_rkp_get_modified(rkp)) return TRUE;

	arvo_rkp_save_actual(rkp, profile_number, error);
	if (*error)
		return FALSE;

	gaminggear_device_lock(GAMINGGEAR_DEVICE(arvo));

	for (i = 0; i < ARVO_BUTTON_NUM; ++i) {
		if (!rkp->modified_buttons[i])
			continue;
		button_info = arvo_rkp_get_button_info(rkp, i);
		button = arvo_rkp_button_info_to_button(button_info);
		g_free(button_info);
		result = arvo_button_write(arvo, button, profile_number, i + 1, error);
		g_free(button);
		if (result)
			rkp->modified_buttons[i] = FALSE;
		else
			goto error;
	}

	g_usleep(500 * G_ROCCAT_USEC_PER_MSEC);
	if (profile_number == arvo_actual_profile_read(arvo, error)) {
		if (rkp->modified_mode_key) {
			result = arvo_mode_key_write(arvo, arvo_rkp_get_mode_key(rkp), error);
			if (result)
				rkp->modified_mode_key = FALSE;
			else
				goto error;

		}
		if (rkp->modified_key_mask) {
			result = arvo_key_mask_write(arvo, arvo_rkp_get_key_mask(rkp), error);
			if (result)
				rkp->modified_key_mask = FALSE;
			else
				goto error;

		}
	}

	gaminggear_device_unlock(GAMINGGEAR_DEVICE(arvo));
	return TRUE;
error:
	gaminggear_device_unlock(GAMINGGEAR_DEVICE(arvo));
	return FALSE;
}
