#ifndef __ROCCAT_KEY_FILE_H__
#define __ROCCAT_KEY_FILE_H__

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

#include <glib.h>

G_BEGIN_DECLS

typedef struct _RoccatKeyFile RoccatKeyFile;

RoccatKeyFile *roccat_key_file_load(gchar const *filename);

/*
 * Does not create dir
 */
gboolean roccat_key_file_save(RoccatKeyFile *key_file, GError **error);

gboolean roccat_key_file_save_as(RoccatKeyFile *key_file, gchar const *filename, GError **error);
void roccat_key_file_free(RoccatKeyFile *key_file);

gchar *roccat_key_file_get_string_with_default(RoccatKeyFile *key_file, char const *group, gchar const *key, gchar const *defval);
void roccat_key_file_set_string(RoccatKeyFile *key_file, char const *group, gchar const *key, gchar const *new_value);

gint roccat_key_file_get_integer_with_default(RoccatKeyFile *key_file, char const *group, gchar const *key, gint defval);
void roccat_key_file_set_integer(RoccatKeyFile *key_file, char const *group, gchar const *key, gint new_value);

gboolean roccat_key_file_get_boolean_with_default(RoccatKeyFile *key_file, char const *group, gchar const *key, gboolean defval);
void roccat_key_file_set_boolean(RoccatKeyFile *key_file, char const *group, gchar const *key, gboolean new_value);

gdouble roccat_key_file_get_double_with_default(RoccatKeyFile *key_file, char const *group, gchar const *key, gdouble defval);
void roccat_key_file_set_double(RoccatKeyFile *key_file, char const *group, gchar const *key, gdouble new_value);

G_END_DECLS

#endif
