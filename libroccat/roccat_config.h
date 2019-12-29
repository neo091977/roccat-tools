#ifndef __ROCCAT_CONFIGURATION_H__
#define __ROCCAT_CONFIGURATION_H__

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

G_BEGIN_DECLS

gchar *roccat_configuration_dir(void);
RoccatKeyFile *roccat_configuration_load(void);
gboolean roccat_configuration_save(RoccatKeyFile *config, GError **error);
void roccat_configuration_free(RoccatKeyFile *config);
gchar *roccat_configuration_get_timer_save_path(RoccatKeyFile *config);
void roccat_configuration_set_timer_save_path(RoccatKeyFile *config, gchar const *path);
gchar *roccat_configuration_get_macro_save_path(RoccatKeyFile *config);
void roccat_configuration_set_macro_save_path(RoccatKeyFile *config, gchar const *path);

G_END_DECLS

#endif
