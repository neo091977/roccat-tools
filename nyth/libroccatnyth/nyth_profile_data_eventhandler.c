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
#include "nyth_profile_data_eventhandler.h"
#include "i18n-lib.h"

static gchar *nyth_profile_dir(void) {
	gchar *base = roccat_profile_dir();
	gchar *dir = g_build_path("/", base, "nyth", NULL);
	g_free(base);
	return dir;
}

static gboolean nyth_profile_dir_create_if_needed(GError **error) {
	gchar *dir;
	gboolean retval;

	dir = nyth_profile_dir();
	retval = roccat_profile_dir_create_if_needed(dir, error);
	g_free(dir);
	return retval;
}

static gchar *nyth_build_profile_path(guint profile_index) {
	gchar *dir = nyth_profile_dir();
	gchar *filename = g_strdup_printf("actual%i", profile_index);
	gchar *path = g_build_path("/", dir, filename, NULL);
	g_free(filename);
	g_free(dir);
	return path;
}

void nyth_profile_data_eventhandler_set_profile_name(NythProfileDataEventhandler *profile_data, gchar const *new_name) {
	if (strncmp(profile_data->profile_name, new_name, ROCCAT_SWARM_RMP_PROFILE_NAME_LENGTH)) {
		g_strlcpy(profile_data->profile_name, new_name, ROCCAT_SWARM_RMP_PROFILE_NAME_LENGTH + 1);
		profile_data->modified = TRUE;
	}
}

void nyth_profile_data_eventhandler_set_gamefile_name(NythProfileDataEventhandler *profile_data, guint index, gchar const *new_name) {
	if (strncmp(profile_data->gamefile_names[index], new_name, ROCCAT_SWARM_RMP_GAMEFILE_LENGTH)) {
		g_strlcpy(profile_data->gamefile_names[index], new_name, ROCCAT_SWARM_RMP_GAMEFILE_LENGTH);
		profile_data->modified = TRUE;
	}
}

void nyth_profile_data_eventhandler_set_timer(NythProfileDataEventhandler *profile_data, guint index, RoccatTimer const *timer) {
	if (!roccat_timer_equal(&profile_data->timers[index], timer)) {
		roccat_timer_copy(&profile_data->timers[index], timer);
		profile_data->modified = TRUE;
	}
}

void nyth_profile_data_eventhandler_set_opener(NythProfileDataEventhandler *profile_data, guint index, gchar const *new_opener) {
	if (strncmp(profile_data->openers[index], new_opener, ROCCAT_SWARM_RMP_OPENER_LENGTH)) {
		g_strlcpy(profile_data->openers[index], new_opener, ROCCAT_SWARM_RMP_OPENER_LENGTH);
		profile_data->modified = TRUE;
	}
}

void nyth_profile_data_eventhandler_set_talk_target(NythProfileDataEventhandler *profile_data, guint index, guint new_target) {
	if (profile_data->talk_targets[index] != new_target) {
		profile_data->talk_targets[index] = new_target;
		profile_data->modified = TRUE;
	}
}

static NythProfileDataEventhandler *nyth_profile_data_eventhandler_read_with_path(gchar const *path, GError **error) {
	NythProfileDataEventhandler *profile_data;
	gsize length;
	gboolean result;

	result = g_file_get_contents(path, (gchar **)&profile_data, &length, error);

	if (!result)
		return NULL;

	if (length != sizeof(NythProfileDataEventhandler)) {
		g_set_error(error, G_FILE_ERROR, G_FILE_ERROR_INVAL, _("Eventhandler datafile has wrong size: %zu instead of %zu"),
				(gsize)length, (gsize)sizeof(NythProfileDataEventhandler));
		g_free(profile_data);
		return NULL;
	}

	return profile_data;
}

static gboolean nyth_profile_data_eventhandler_write_with_path(gchar const *path, NythProfileDataEventhandler const *profile_data, GError **error) {
	return roccat_profile_write_with_path(path, (gchar const *)profile_data, sizeof(NythProfileDataEventhandler), error);
}

gboolean nyth_profile_data_eventhandler_save(NythProfileDataEventhandler *profile_data, guint profile_index, GError **error) {
	gchar *config_path;
	gboolean retval;

	if (!nyth_profile_dir_create_if_needed(error))
		return FALSE;

	config_path = nyth_build_profile_path(profile_index);
	retval = nyth_profile_data_eventhandler_write_with_path(config_path, profile_data, error);
	if (retval)
		nyth_profile_data_eventhandler_set_unmodified(profile_data);
	g_free(config_path);
	return retval;
}

void nyth_profile_data_eventhandler_update_with_default(NythProfileDataEventhandler *profile_data) {
	profile_data->timer_notification_type = ROCCAT_NOTIFICATION_TYPE_OSD;
	profile_data->profile_notification_type = ROCCAT_NOTIFICATION_TYPE_OSD;
	profile_data->sensitivity_notification_type = ROCCAT_NOTIFICATION_TYPE_OSD;
	profile_data->cpi_notification_type = ROCCAT_NOTIFICATION_TYPE_OSD;
	profile_data->notification_volume = 0.0;
	nyth_profile_data_eventhandler_set_modified(profile_data);
}

gboolean nyth_profile_data_eventhandler_update(NythProfileDataEventhandler *profile_data, guint profile_index, GError **error) {
	NythProfileDataEventhandler *temp;
	gchar *config_path;

	config_path = nyth_build_profile_path(profile_index);
	temp = nyth_profile_data_eventhandler_read_with_path(config_path, error);
	g_free(config_path);

	if (!temp)
		return FALSE;

	memcpy(profile_data, temp, sizeof(NythProfileDataEventhandler));
	g_free(temp);

	nyth_profile_data_eventhandler_set_unmodified(profile_data);

	return TRUE;
}

void nyth_profile_data_eventhandler_set_modified(NythProfileDataEventhandler *profile_data) {
	profile_data->modified = TRUE;
}

void nyth_profile_data_eventhandler_set_unmodified(NythProfileDataEventhandler *profile_data) {
	profile_data->modified = FALSE;
}

gboolean nyth_profile_data_eventhandler_get_modified(NythProfileDataEventhandler const *profile_data) {
	return profile_data->modified;
}
