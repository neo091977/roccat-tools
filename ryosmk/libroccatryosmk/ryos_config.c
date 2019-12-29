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

#include "ryos_config.h"
#include "g_roccat_helper.h"
#include "roccat_notificator.h"
#include "config.h"

static gchar const * const group_name = "RyosGUI";
static gchar const * const rkp_path_key = "RkpFilePath";
static gchar const * const default_profile_number_key = "DefaultProfileNumber";
static gchar const * const layout_key = "Layout";
static gchar const * const script_path_key = "ScriptFilePath";

static gchar *configuration_path(void) {
	gchar *dir = roccat_configuration_dir();
	gchar *path = g_build_filename(dir, "ryosmk.ini", NULL);
	g_free(dir);
	return path;
}

RoccatKeyFile *ryos_configuration_load(void) {
	RoccatKeyFile *config;
	gchar *path;

	path = configuration_path();
	config = roccat_key_file_load(path);
	g_free(path);

	return config;
}

gchar *ryos_configuration_get_rkp_path(RoccatKeyFile *config) {
	return roccat_key_file_get_string_with_default(config, group_name, rkp_path_key, g_get_home_dir());
}

void ryos_configuration_set_rkp_path(RoccatKeyFile *config, gchar const *path) {
	roccat_key_file_set_string(config, group_name, rkp_path_key, path);
}

gchar *ryos_configuration_get_script_path(RoccatKeyFile *config) {
	return roccat_key_file_get_string_with_default(config, group_name, script_path_key, RYOS_EFFECT_MODULES_PATH);
}

void ryos_configuration_set_script_path(RoccatKeyFile *config, gchar const *path) {
	roccat_key_file_set_string(config, group_name, script_path_key, path);
}

gint ryos_configuration_get_default_profile_number(RoccatKeyFile *config) {
	return roccat_key_file_get_integer_with_default(config, group_name, default_profile_number_key, 0);
}

void ryos_configuration_set_default_profile_number(RoccatKeyFile *config, gint new_value) {
	roccat_key_file_set_integer(config, group_name, default_profile_number_key, new_value);
}

gchar *ryos_configuration_get_layout(RoccatKeyFile *config) {
	return roccat_key_file_get_string_with_default(config, group_name, layout_key, "");
}

void ryos_configuration_set_layout(RoccatKeyFile *config, gchar const *layout) {
	roccat_key_file_set_string(config, group_name, layout_key, layout);
}
