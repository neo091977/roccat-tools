#ifndef __ROCCAT_TYON_CONFIG_H__
#define __ROCCAT_TYON_CONFIG_H__

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

RoccatKeyFile *tyon_configuration_load(void);
gboolean tyon_configuration_save(RoccatKeyFile *config, GError **error);
void tyon_configuration_free(RoccatKeyFile *config);
gchar *tyon_configuration_get_rmp_save_path(RoccatKeyFile *config);
void tyon_configuration_set_rmp_save_path(RoccatKeyFile *config, gchar const *path);
gint tyon_configuration_get_timer_notification_type(RoccatKeyFile *config);
void tyon_configuration_set_timer_notification_type(RoccatKeyFile *config, gint new_value);
gint tyon_configuration_get_profile_notification_type(RoccatKeyFile *config);
void tyon_configuration_set_profile_notification_type(RoccatKeyFile *config, gint new_value);
gint tyon_configuration_get_sensitivity_notification_type(RoccatKeyFile *config);
void tyon_configuration_set_sensitivity_notification_type(RoccatKeyFile *config, gint new_value);
gint tyon_configuration_get_cpi_notification_type(RoccatKeyFile *config);
void tyon_configuration_set_cpi_notification_type(RoccatKeyFile *config, gint new_value);
gdouble tyon_configuration_get_notification_volume(RoccatKeyFile *config);
void tyon_configuration_set_notification_volume(RoccatKeyFile *config, gdouble new_value);
gint tyon_configuration_get_default_profile_number(RoccatKeyFile *config);
void tyon_configuration_set_default_profile_number(RoccatKeyFile *config, gint new_value);

G_END_DECLS

#endif
