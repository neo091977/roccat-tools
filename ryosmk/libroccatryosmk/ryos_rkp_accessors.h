#ifndef __ROCCAT_RYOS_RKP_ACCESSORS_H__
#define __ROCCAT_RYOS_RKP_ACCESSORS_H__

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

#include "ryos_rkp.h"

G_BEGIN_DECLS

gchar *ryos_rkp_get_profile_name(RyosRkp const *rkp);
gchar *ryos_rkp_get_gamefile_name(RyosRkp const *rkp, guint index);

void ryos_rkp_set_profile_name(RyosRkp *rkp, gchar const *name);
void ryos_rkp_set_gamefile_name(RyosRkp *rkp, guint index, gchar const *name);

G_END_DECLS

#endif
