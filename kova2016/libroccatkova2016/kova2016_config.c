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

#include "kova2016_config.h"
#include "g_roccat_helper.h"
#include "roccat_notificator.h"
#include "roccat_config.h"

static gchar const * const group_name = "Kova2016GUI";
static gchar const * const rmp_path_key = "RmpFilePath";
static gchar const * const default_profile_number_key = "DefaultProfileNumber";

static gchar *configuration_path(void) {
	gchar *dir = roccat_configuration_dir();
	gchar *path = g_build_filename(dir, "kova2016.ini", NULL);
	g_free(dir);
	return path;
}

RoccatKeyFile *kova2016_configuration_load(void) {
	RoccatKeyFile *config;
	gchar *path;

	path = configuration_path();
	config = roccat_key_file_load(path);
	g_free(path);

	return config;
}

void kova2016_configuration_free(RoccatKeyFile *config) {
	if (config == NULL)
		return;

	roccat_key_file_free(config);
}

gboolean kova2016_configuration_save(RoccatKeyFile *config, GError **error) {
	if (config == NULL)
		return TRUE;

	return roccat_key_file_save(config, error);
}

gchar *kova2016_configuration_get_rmp_path(RoccatKeyFile *config) {
	return roccat_key_file_get_string_with_default(config, group_name, rmp_path_key, g_get_home_dir());
}

void kova2016_configuration_set_rmp_path(RoccatKeyFile *config, gchar const *path) {
	roccat_key_file_set_string(config, group_name, rmp_path_key, path);
}

gint kova2016_configuration_get_default_profile_number(RoccatKeyFile *config) {
	return roccat_key_file_get_integer_with_default(config, group_name, default_profile_number_key, 0);
}

void kova2016_configuration_set_default_profile_number(RoccatKeyFile *config, gint new_value) {
	roccat_key_file_set_integer(config, group_name, default_profile_number_key, new_value);
}
