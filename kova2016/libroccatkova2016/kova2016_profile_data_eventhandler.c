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
#include "kova2016_profile_data_eventhandler.h"
#include "i18n-lib.h"

static gchar *kova2016_profile_dir(void) {
	gchar *base = roccat_profile_dir();
	gchar *dir = g_build_path("/", base, "kova2016", NULL);
	g_free(base);
	return dir;
}

static gboolean kova2016_profile_dir_create_if_needed(GError **error) {
	gchar *dir;
	gboolean retval;

	dir = kova2016_profile_dir();
	retval = roccat_profile_dir_create_if_needed(dir, error);
	g_free(dir);
	return retval;
}

static gchar *kova2016_build_profile_path(guint profile_index) {
	gchar *dir = kova2016_profile_dir();
	gchar *filename = g_strdup_printf("actual%i", profile_index);
	gchar *path = g_build_path("/", dir, filename, NULL);
	g_free(filename);
	g_free(dir);
	return path;
}

void kova2016_profile_data_eventhandler_set_profile_name(Kova2016ProfileDataEventhandler *profile_data, gchar const *new_name) {
	if (strncmp(profile_data->profile_name, new_name, ROCCAT_SWARM_RMP_PROFILE_NAME_LENGTH)) {
		g_strlcpy(profile_data->profile_name, new_name, ROCCAT_SWARM_RMP_PROFILE_NAME_LENGTH + 1);
		profile_data->modified = TRUE;
	}
}

void kova2016_profile_data_eventhandler_set_gamefile_name(Kova2016ProfileDataEventhandler *profile_data, guint index, gchar const *new_name) {
	if (strncmp(profile_data->gamefile_names[index], new_name, ROCCAT_SWARM_RMP_GAMEFILE_LENGTH)) {
		g_strlcpy(profile_data->gamefile_names[index], new_name, ROCCAT_SWARM_RMP_GAMEFILE_LENGTH);
		profile_data->modified = TRUE;
	}
}

void kova2016_profile_data_eventhandler_set_timer(Kova2016ProfileDataEventhandler *profile_data, guint index, RoccatTimer const *timer) {
	if (!roccat_timer_equal(&profile_data->timers[index], timer)) {
		roccat_timer_copy(&profile_data->timers[index], timer);
		profile_data->modified = TRUE;
	}
}

void kova2016_profile_data_eventhandler_set_opener(Kova2016ProfileDataEventhandler *profile_data, guint index, gchar const *new_opener) {
	if (strncmp(profile_data->openers[index], new_opener, ROCCAT_SWARM_RMP_OPENER_LENGTH)) {
		g_strlcpy(profile_data->openers[index], new_opener, ROCCAT_SWARM_RMP_OPENER_LENGTH);
		profile_data->modified = TRUE;
	}
}

void kova2016_profile_data_eventhandler_set_talk_target(Kova2016ProfileDataEventhandler *profile_data, guint index, guint new_target) {
	if (profile_data->talk_targets[index] != new_target) {
		profile_data->talk_targets[index] = new_target;
		profile_data->modified = TRUE;
	}
}

static Kova2016ProfileDataEventhandler *kova2016_profile_data_eventhandler_read_with_path(gchar const *path, GError **error) {
	Kova2016ProfileDataEventhandler *profile_data;
	gsize length;
	gboolean result;

	result = g_file_get_contents(path, (gchar **)&profile_data, &length, error);

	if (!result)
		return NULL;

	if (length != sizeof(Kova2016ProfileDataEventhandler)) {
		g_set_error(error, G_FILE_ERROR, G_FILE_ERROR_INVAL, _("Eventhandler datafile has wrong size: %zu instead of %zu"),
				(gsize)length, (gsize)sizeof(Kova2016ProfileDataEventhandler));
		g_free(profile_data);
		return NULL;
	}

	return profile_data;
}

static gboolean kova2016_profile_data_eventhandler_write_with_path(gchar const *path, Kova2016ProfileDataEventhandler const *profile_data, GError **error) {
	return roccat_profile_write_with_path(path, (gchar const *)profile_data, sizeof(Kova2016ProfileDataEventhandler), error);
}

gboolean kova2016_profile_data_eventhandler_save(Kova2016ProfileDataEventhandler *profile_data, guint profile_index, GError **error) {
	gchar *config_path;
	gboolean retval;

	if (!kova2016_profile_dir_create_if_needed(error))
		return FALSE;

	config_path = kova2016_build_profile_path(profile_index);
	retval = kova2016_profile_data_eventhandler_write_with_path(config_path, profile_data, error);
	if (retval)
		kova2016_profile_data_eventhandler_set_unmodified(profile_data);
	g_free(config_path);
	return retval;
}

void kova2016_profile_data_eventhandler_update_with_default(Kova2016ProfileDataEventhandler *profile_data) {
	profile_data->timer_notification_type = ROCCAT_NOTIFICATION_TYPE_OSD;
	profile_data->profile_notification_type = ROCCAT_NOTIFICATION_TYPE_OSD;
	profile_data->cpi_notification_type = ROCCAT_NOTIFICATION_TYPE_OSD;
	profile_data->notification_volume = 0.0;
	kova2016_profile_data_eventhandler_set_modified(profile_data);
}

gboolean kova2016_profile_data_eventhandler_update(Kova2016ProfileDataEventhandler *profile_data, guint profile_index, GError **error) {
	Kova2016ProfileDataEventhandler *temp;
	gchar *config_path;

	config_path = kova2016_build_profile_path(profile_index);
	temp = kova2016_profile_data_eventhandler_read_with_path(config_path, error);
	g_free(config_path);

	if (!temp)
		return FALSE;

	memcpy(profile_data, temp, sizeof(Kova2016ProfileDataEventhandler));
	g_free(temp);

	kova2016_profile_data_eventhandler_set_unmodified(profile_data);

	return TRUE;
}

void kova2016_profile_data_eventhandler_set_modified(Kova2016ProfileDataEventhandler *profile_data) {
	profile_data->modified = TRUE;
}

void kova2016_profile_data_eventhandler_set_unmodified(Kova2016ProfileDataEventhandler *profile_data) {
	profile_data->modified = FALSE;
}

gboolean kova2016_profile_data_eventhandler_get_modified(Kova2016ProfileDataEventhandler const *profile_data) {
	return profile_data->modified;
}
