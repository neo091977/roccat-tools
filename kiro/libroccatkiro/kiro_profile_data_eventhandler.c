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

#include "roccat_helper.h"
#include "roccat_notificator.h"
#include "kiro_profile_data_eventhandler.h"
#include "i18n-lib.h"

static gchar *kiro_profile_dir(void) {
	gchar *base = roccat_profile_dir();
	gchar *dir = g_build_path("/", base, "kiro", NULL);
	g_free(base);
	return dir;
}

static gboolean kiro_profile_dir_create_if_needed(GError **error) {
	gchar *dir;
	gboolean retval;

	dir = kiro_profile_dir();
	retval = roccat_profile_dir_create_if_needed(dir, error);
	g_free(dir);
	return retval;
}

static gchar *kiro_build_profile_path(void) {
	gchar *dir = kiro_profile_dir();
	gchar *path = g_build_path("/", dir, "actual", NULL);
	g_free(dir);
	return path;
}

void kiro_profile_data_eventhandler_set_profile_name(KiroProfileDataEventhandler *profile_data, gchar const *new_name) {
	if (strncmp(profile_data->profile_name, new_name, ROCCAT_SWARM_RMP_PROFILE_NAME_LENGTH)) {
		g_strlcpy(profile_data->profile_name, new_name, ROCCAT_SWARM_RMP_PROFILE_NAME_LENGTH + 1);
		profile_data->modified = TRUE;
	}
}

void kiro_profile_data_eventhandler_set_opener(KiroProfileDataEventhandler *profile_data, guint index, gchar const *new_opener) {
	if (strncmp(profile_data->openers[index], new_opener, ROCCAT_SWARM_RMP_OPENER_LENGTH)) {
		g_strlcpy(profile_data->openers[index], new_opener, ROCCAT_SWARM_RMP_OPENER_LENGTH);
		profile_data->modified = TRUE;
	}
}

static KiroProfileDataEventhandler *kiro_profile_data_eventhandler_read_with_path(gchar const *path, GError **error) {
	KiroProfileDataEventhandler *profile_data;
	gsize length;
	gboolean result;

	result = g_file_get_contents(path, (gchar **)&profile_data, &length, error);

	if (!result)
		return NULL;

	if (length != sizeof(KiroProfileDataEventhandler)) {
		g_set_error(error, G_FILE_ERROR, G_FILE_ERROR_INVAL, _("Eventhandler datafile has wrong size: %zu instead of %zu"),
				(gsize)length, (gsize)sizeof(KiroProfileDataEventhandler));
		g_free(profile_data);
		return NULL;
	}

	return profile_data;
}

static gboolean kiro_profile_data_eventhandler_write_with_path(gchar const *path, KiroProfileDataEventhandler const *profile_data, GError **error) {
	return roccat_profile_write_with_path(path, (gchar const *)profile_data, sizeof(KiroProfileDataEventhandler), error);
}

gboolean kiro_profile_data_eventhandler_save(KiroProfileDataEventhandler *profile_data, GError **error) {
	gchar *config_path;
	gboolean retval;

	if (!kiro_profile_dir_create_if_needed(error))
		return FALSE;

	config_path = kiro_build_profile_path();
	retval = kiro_profile_data_eventhandler_write_with_path(config_path, profile_data, error);
	if (retval)
		kiro_profile_data_eventhandler_set_unmodified(profile_data);
	g_free(config_path);
	return retval;
}

void kiro_profile_data_eventhandler_update_with_default(KiroProfileDataEventhandler *profile_data) {
	profile_data->sensitivity_notification_type = ROCCAT_NOTIFICATION_TYPE_OSD;
	profile_data->cpi_notification_type = ROCCAT_NOTIFICATION_TYPE_OSD;
	profile_data->notification_volume = 0.0;
	kiro_profile_data_eventhandler_set_modified(profile_data);
}

gboolean kiro_profile_data_eventhandler_update(KiroProfileDataEventhandler *profile_data, GError **error) {
	KiroProfileDataEventhandler *temp;
	gchar *config_path;

	config_path = kiro_build_profile_path();
	temp = kiro_profile_data_eventhandler_read_with_path(config_path, error);
	g_free(config_path);

	if (!temp)
		return FALSE;

	memcpy(profile_data, temp, sizeof(KiroProfileDataEventhandler));
	g_free(temp);

	kiro_profile_data_eventhandler_set_unmodified(profile_data);

	return TRUE;
}

void kiro_profile_data_eventhandler_set_modified(KiroProfileDataEventhandler *profile_data) {
	profile_data->modified = TRUE;
}

void kiro_profile_data_eventhandler_set_unmodified(KiroProfileDataEventhandler *profile_data) {
	profile_data->modified = FALSE;
}

gboolean kiro_profile_data_eventhandler_get_modified(KiroProfileDataEventhandler const *profile_data) {
	return profile_data->modified;
}
