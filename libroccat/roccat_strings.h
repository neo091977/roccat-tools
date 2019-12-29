#ifndef __ROCCAT_STRINGS_H__
#define __ROCCAT_STRINGS_H__

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

gchar const *roccat_string_gamefile_frame_tooltip(void);
gchar const *roccat_string_default_profile_number_tooltip(void);
gchar const *roccat_string_firmware_liability(void);

gchar *roccat_string_old_firmware_version(guint has, guint needs);

G_END_DECLS

#endif
