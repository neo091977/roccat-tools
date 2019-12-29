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

#include "roccat_config.h"

static gchar const * const group_name = "General";
static gchar const * const timer_save_path_key = "TimerFilePath";
static gchar const * const macro_save_path_key = "MacroFilePath";

gchar *roccat_configuration_dir(void) {
	return g_build_path("/", g_get_user_config_dir(), "roccat", NULL);
}

static gchar *roccat_configuration_path(void) {
	gchar *dir = roccat_configuration_dir();
	gchar *path = g_build_filename(dir, "roccat.ini", NULL);
	g_free(dir);
	return path;
}

RoccatKeyFile *roccat_configuration_load(void) {
	RoccatKeyFile *config;
	gchar *path;

	path = roccat_configuration_path();
	config = roccat_key_file_load(path);
	g_free(path);

	return config;
}

gboolean roccat_configuration_save(RoccatKeyFile *config, GError **error) {
	if (config == NULL)
		return TRUE;

	return roccat_key_file_save(config, error);
}

void roccat_configuration_free(RoccatKeyFile *config) {
	if (config == NULL)
		return;

	roccat_key_file_free(config);
}

gchar *roccat_configuration_get_timer_save_path(RoccatKeyFile *config) {
	return roccat_key_file_get_string_with_default(config, group_name, timer_save_path_key, g_get_home_dir());
}

void roccat_configuration_set_timer_save_path(RoccatKeyFile *config, gchar const *path) {
	roccat_key_file_set_string(config, group_name, timer_save_path_key, path);
}

gchar *roccat_configuration_get_macro_save_path(RoccatKeyFile *config) {
	return roccat_key_file_get_string_with_default(config, group_name, macro_save_path_key, g_get_home_dir());
}

void roccat_configuration_set_macro_save_path(RoccatKeyFile *config, gchar const *path) {
	roccat_key_file_set_string(config, group_name, macro_save_path_key, path);
}
