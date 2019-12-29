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

#include "roccat_key_file.h"
#include "roccat_helper.h"
#include <string.h>

struct _RoccatKeyFile {
	GKeyFile *key_file;
	gboolean modified;
	gchar *filename;
};

static RoccatKeyFile *roccat_key_file_new(void) {
	RoccatKeyFile *key_file;

	key_file = (RoccatKeyFile *)g_malloc0(sizeof(RoccatKeyFile));

	key_file->key_file = g_key_file_new();
	key_file->modified = FALSE;

	return key_file;
}

RoccatKeyFile *roccat_key_file_load(gchar const *filename) {
	RoccatKeyFile *key_file;
	GError *error = NULL;

	key_file = roccat_key_file_new();
	key_file->filename = g_strdup(filename);

	if (!g_key_file_load_from_file(key_file->key_file, key_file->filename, G_KEY_FILE_NONE, &error))
		g_clear_error(&error);

	return key_file;
}

gboolean roccat_key_file_save(RoccatKeyFile *key_file, GError **error) {
	gchar *data;
	gsize length;
	gboolean retval;
	gchar *dir;

	if (key_file->modified == FALSE)
		return TRUE;

	dir = g_path_get_dirname(key_file->filename);
	retval = roccat_create_dir_if_needed(dir, error);
	g_free(dir);
	if (!retval)
		return FALSE;

	data = g_key_file_to_data(key_file->key_file, &length, error);
	retval = g_file_set_contents(key_file->filename, data, length, error);
	g_free(data);
	if (retval)
		key_file->modified = FALSE;

	return retval;
}

gboolean roccat_key_file_save_as(RoccatKeyFile *key_file, gchar const *filename, GError **error) {
	g_free(key_file->filename);
	key_file->filename = g_strdup(filename);
	key_file->modified = TRUE;
	return roccat_key_file_save(key_file, error);
}

void roccat_key_file_free(RoccatKeyFile *key_file) {
	g_key_file_free(key_file->key_file);
	g_free(key_file->filename);
	g_free(key_file);
}

static gchar *roccat_key_file_get_string(RoccatKeyFile *key_file, char const *group, gchar const *key, GError **error) {
	return g_key_file_get_string(key_file->key_file, group, key, error);
}

gchar *roccat_key_file_get_string_with_default(RoccatKeyFile *key_file, char const *group, gchar const *key, gchar const *defval) {
	gchar *result;
	GError *error = NULL;

	result = roccat_key_file_get_string(key_file, group, key, &error);
	if (!result) {
		g_clear_error(&error);
		return g_strdup(defval);
	}

	return result;
}

void roccat_key_file_set_string(RoccatKeyFile *key_file, char const *group, gchar const *key, gchar const *new_value) {
	gchar *orig;
	GError *error = NULL;

	orig = roccat_key_file_get_string(key_file, group, key, &error);
	if (error || strcmp(orig, new_value)) {
		g_key_file_set_string(key_file->key_file, group, key, new_value);
		key_file->modified = TRUE;
	}
	g_free(orig);
}

static gint roccat_key_file_get_integer(RoccatKeyFile *key_file, char const *group, gchar const *key, GError **error) {
	return g_key_file_get_integer(key_file->key_file, group, key, error);
}

gint roccat_key_file_get_integer_with_default(RoccatKeyFile *key_file, char const *group, gchar const *key, gint defval) {
	gint retval;
	GError *error = NULL;

	retval = roccat_key_file_get_integer(key_file, group, key, &error);
	if (error) {
		g_clear_error(&error);
		return defval;
	}

	return retval;
}

void roccat_key_file_set_integer(RoccatKeyFile *key_file, char const *group, gchar const *key, gint new_value) {
	gint orig;
	GError *error = NULL;

	orig = roccat_key_file_get_integer(key_file, group, key, &error);
	if (error || (orig != new_value)) {
		g_clear_error(&error);
		g_key_file_set_integer(key_file->key_file, group, key, new_value);
		key_file->modified = TRUE;
	}
}

static gdouble roccat_key_file_get_double(RoccatKeyFile *key_file, char const *group, gchar const *key, GError **error) {
	return g_key_file_get_double(key_file->key_file, group, key, error);
}

gdouble roccat_key_file_get_double_with_default(RoccatKeyFile *key_file, char const *group, gchar const *key, gdouble defval) {
	gint retval;
	GError *error = NULL;

	retval = roccat_key_file_get_double(key_file, group, key, &error);
	if (error) {
		g_clear_error(&error);
		return defval;
	}

	return retval;
}

void roccat_key_file_set_double(RoccatKeyFile *key_file, char const *group, gchar const *key, gdouble new_value) {
	gdouble orig;
	GError *error = NULL;

	orig = roccat_key_file_get_double(key_file, group, key, &error);
	if (error || (orig != new_value)) {
		g_clear_error(&error);
		g_key_file_set_double(key_file->key_file, group, key, new_value);
		key_file->modified = TRUE;
	}
}

static gboolean roccat_key_file_get_boolean(RoccatKeyFile *key_file, char const *group, gchar const *key, GError **error) {
	return g_key_file_get_boolean(key_file->key_file, group, key, error);
}

gboolean roccat_key_file_get_boolean_with_default(RoccatKeyFile *key_file, char const *group, gchar const *key, gboolean defval) {
	gboolean result;
	GError *error = NULL;

	result = roccat_key_file_get_boolean(key_file, group, key, &error);
	if (error) {
		g_clear_error(&error);
		return defval;
	}
	return result;
}

void roccat_key_file_set_boolean(RoccatKeyFile *key_file, char const *group, gchar const *key, gboolean new_value) {
	gboolean orig;
	GError *error = NULL;

	orig = roccat_key_file_get_boolean(key_file, group, key, &error);
	if (error || orig != new_value) {
		g_clear_error(&error);
		g_key_file_set_boolean(key_file->key_file, group, key, new_value);
		key_file->modified = TRUE;
	}
}
