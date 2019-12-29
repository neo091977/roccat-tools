#ifndef __ROCCAT_SUORA_CONFIG_H__
#define __ROCCAT_SUORA_CONFIG_H__

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

G_BEGIN_DECLS

RoccatKeyFile *suora_configuration_load(void);
gboolean suora_configuration_save(RoccatKeyFile *config, GError **error);
void suora_configuration_free(RoccatKeyFile *config);
gchar *suora_configuration_get_rkp_path(RoccatKeyFile *config);
void suora_configuration_set_rkp_path(RoccatKeyFile *config, gchar const *path);
gchar *suora_configuration_get_layout(RoccatKeyFile *config);
void suora_configuration_set_layout(RoccatKeyFile *config, gchar const *layout);
gchar *suora_configuration_get_default_profile_name(RoccatKeyFile *config);
void suora_configuration_set_default_profile_name(RoccatKeyFile *config, gchar const *name);

G_END_DECLS

#endif
