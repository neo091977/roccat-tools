#ifndef __ROCCAT_RYOS_RKP_QUICKLAUNCH_H__
#define __ROCCAT_RYOS_RKP_QUICKLAUNCH_H__

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

#include "ryos.h"

G_BEGIN_DECLS

typedef struct _RyosRkpQuicklaunch RyosRkpQuicklaunch;

struct _RyosRkpQuicklaunch {
	guint16 path[RYOS_RKP_QUICKLAUNCH_PATH_LENGTH];
	guint16 name[RYOS_RKP_QUICKLAUNCH_NAME_LENGTH];
} __attribute__ ((packed));

gchar *ryos_rkp_quicklaunch_get_path(RyosRkpQuicklaunch const *quicklaunch);
gchar *ryos_rkp_quicklaunch_get_name(RyosRkpQuicklaunch const *quicklaunch);
void ryos_rkp_quicklaunch_set(RyosRkpQuicklaunch *quicklaunch, gchar const *path);
gboolean ryos_rkp_quicklaunch_equal(RyosRkpQuicklaunch const *left, RyosRkpQuicklaunch const *right);
void ryos_rkp_quicklaunch_copy(RyosRkpQuicklaunch *destination, RyosRkpQuicklaunch const *source);
RyosRkpQuicklaunch *ryos_rkp_quicklaunch_dup(RyosRkpQuicklaunch const *source);

G_END_DECLS

#endif
