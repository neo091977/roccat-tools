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
#include "roccat.h"
#include "i18n-lib.h"
#include <string.h>

static gchar *kone_profile_dir(void) {
	gchar *base = roccat_profile_dir();
	gchar *dir = g_build_path("/", base, "kone", NULL);
	g_free(base);
	return dir;
}

static gboolean kone_profile_dir_create_if_needed(GError **error) {
	gchar *dir;
	gboolean retval;

	dir = kone_profile_dir();
	retval = roccat_profile_dir_create_if_needed(dir, error);
	g_free(dir);
	return retval;
}

/* returns TRUE if equal */
static gboolean kone_profile_check_equality(KoneProfile const *left, KoneProfile const *right) {
	/* Doing binary comparison since there's no padding. */
	if (memcmp(left, right, sizeof(KoneProfile)))
		return FALSE;

	return TRUE;
}

static gchar *kone_build_config_rmp_path(guint profile_number) {
	gchar *dir = kone_profile_dir();
	gchar *filename = g_strdup_printf("actual%i." ROCCAT_MOUSE_PROFILE_EXTENSION, profile_number);
	gchar *path = g_build_path("/", dir, filename, NULL);
	g_free(filename);
	g_free(dir);
	return path;
}

gboolean kone_rmp_get_modified(KoneRMP const *rmp) {
	if (rmp->modified_rmp || rmp->modified_profile)
		return TRUE;
	return FALSE;
}

static void kone_rmp_set_modified_state(KoneRMP *rmp, gboolean state) {
	rmp->modified_rmp = state;
	rmp->modified_profile = state;
}

void kone_rmp_set_modified(KoneRMP *rmp) {
	kone_rmp_set_modified_state(rmp, TRUE);
}

void kone_rmp_set_unmodified(KoneRMP *rmp) {
	kone_rmp_set_modified_state(rmp, FALSE);
}

KoneRMP *kone_rmp_load_actual(guint profile_number) {
	KoneRMP *rmp;
	gchar *config_path;
	config_path = kone_build_config_rmp_path(profile_number);
	rmp = kone_rmp_read_with_path(config_path, NULL);
	g_free(config_path);
	if (!rmp)
		rmp = kone_rmp_dup(kone_default_rmp());
	return rmp;
}

void kone_rmp_update_from_device(KoneRMP *rmp, RoccatDevice *kone, guint profile_number) {
	KoneProfile *hardware_profile;
	KoneProfile *rmp_profile;
	GError *error;

	hardware_profile = kone_profile_read(kone, profile_number, &error);
	if (!hardware_profile) {
		g_error_free(error);
		return;
	}

	rmp_profile = kone_rmp_to_profile(rmp);
	kone_profile_finalize(rmp_profile, profile_number);

	if (!kone_profile_check_equality(hardware_profile, rmp_profile)) {
		g_warning(_("Hardware data and rmp for profile %u differ"), profile_number);
		/* hardware profile should take precedence */
		kone_rmp_update_with_profile(rmp, hardware_profile);
	}
	g_free(rmp_profile);
	g_free(hardware_profile);

}

KoneRMP *kone_rmp_load(RoccatDevice *kone, guint profile_number, GError **error) {
	KoneRMP *rmp;

	rmp = kone_rmp_load_actual(profile_number);

	kone_rmp_update_from_device(rmp, kone, profile_number);

	return rmp;
}

gboolean kone_rmp_save_actual(KoneRMP *rmp, guint profile_number, GError **error) {
	gchar *config_path;
	gboolean retval;

	kone_profile_dir_create_if_needed(error);
	if (*error)
		return FALSE;

	config_path = kone_build_config_rmp_path(profile_number);
	retval = kone_rmp_write_with_path(config_path, rmp, error);
	g_free(config_path);
	return retval;
}

gboolean kone_rmp_save(RoccatDevice *kone, KoneRMP *rmp, guint profile_number, GError **error) {
	KoneProfile *profile;

	if (!kone_rmp_get_modified(rmp)) return TRUE;

	kone_rmp_save_actual(rmp, profile_number, error);
	if (*error)
		return FALSE;

	if (rmp->modified_profile) {
		profile = kone_rmp_to_profile(rmp);
		kone_profile_write(kone, profile_number, profile, error);
		g_free(profile);
		if (*error)
			return FALSE;
	}

	kone_rmp_set_unmodified(rmp);
	return TRUE;
}
