#ifndef __ROCCATEVENTHANDLER_CONFIG_H__
#define __ROCCATEVENTHANDLER_CONFIG_H__

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

gint roccateventhandler_configuration_get_scan_interval(RoccatKeyFile *config);
gboolean roccateventhandler_configuration_get_start_quiet(RoccatKeyFile *config);
gchar *roccateventhandler_configuration_get_sound_driver(RoccatKeyFile *config);
guint roccateventhandler_configuration_get_driver_state(RoccatKeyFile *config);

G_END_DECLS

#endif
