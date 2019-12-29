#ifndef __ROCCAT_PROCESS_HELPER_H__
#define __ROCCAT_PROCESS_HELPER_H__

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

gboolean roccat_double_fork(gchar const *command);
gboolean roccat_double_fork_with_arg(gchar const *command, gchar const *arg);
gboolean roccat_open_url(gchar const *url);
gboolean roccat_open_default_folder(gchar const *type);

G_END_DECLS

#endif
