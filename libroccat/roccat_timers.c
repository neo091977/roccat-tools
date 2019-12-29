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

#include "roccat_timers.h"
#include "roccat_helper.h"
#include "roccat_config.h"
#include "g_roccat_helper.h"

struct _RoccatTimers {
	GKeyFile *key_file;
};

static gchar const * const timers_set_name = "Timers";

static gchar *roccat_timers_path(void) {
	gchar *dir = roccat_configuration_dir();
	gchar *path = g_build_path("/", dir, "roccat_timers.ini", NULL);
	g_free(dir);
	return path;
}

RoccatTimers *roccat_timers_new(void) {
	RoccatTimers *timers;
	timers = g_malloc0(sizeof(RoccatTimers));
	timers->key_file = g_key_file_new();
	return timers;
}

void roccat_timers_free(RoccatTimers *timers) {
	if (timers) {
		g_key_file_free(timers->key_file);
		g_free(timers);
	}
}

RoccatTimers *roccat_timers_load(GError **error) {
	gchar *path;
	RoccatTimers *timers;

	timers = roccat_timers_new();
	path = roccat_timers_path();

	g_key_file_load_from_file(timers->key_file, path, G_KEY_FILE_KEEP_COMMENTS, error);

	if (*error) {
		/* file might not exist yet */
		if (g_error_matches(*error, G_FILE_ERROR, G_FILE_ERROR_NOENT))
			g_clear_error(error);
		else
			g_clear_pointer(&timers, roccat_timers_free);
	}

	g_free(path);
	return timers;
}

gboolean roccat_timers_save(RoccatTimers *timers, GError **error) {
	gchar *dir, *path, *data;
	gsize length;
	gboolean retval;

	dir = roccat_configuration_dir();
	if (!roccat_create_dir_if_needed(dir, error))
		return FALSE;
	g_free(dir);

	path = roccat_timers_path();
	data = g_key_file_to_data(timers->key_file, &length, error);

	retval = g_file_set_contents(path, data, length, error);

	g_free(data);
	g_free(path);

	return retval;
}

RoccatTimer *roccat_timers_get(RoccatTimers *timers, gchar const *timer_name, GError **error) {
	RoccatTimer *timer;
	gint seconds;

	seconds = g_key_file_get_integer(timers->key_file, timers_set_name, timer_name, error);
	if (error && *error)
		return NULL;

	timer = roccat_timer_new();
	roccat_timer_set_name(timer, timer_name);
	roccat_timer_set_seconds(timer, seconds);
	return timer;
}

void roccat_timers_set(RoccatTimers *timers, RoccatTimer const *timer) {
	g_key_file_set_integer(timers->key_file, timers_set_name, (gchar const *)timer->name, roccat_timer_get_seconds(timer));
}

gchar **roccat_timers_get_timer_names(RoccatTimers *timers, gsize *length, GError **error) {
	return g_key_file_get_keys(timers->key_file, timers_set_name, length, error);
}
