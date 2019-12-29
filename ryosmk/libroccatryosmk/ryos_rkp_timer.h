#ifndef __ROCCAT_RYOS_RKP_TIMER_H__
#define __ROCCAT_RYOS_RKP_TIMER_H__

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
#include "roccat_timer.h"

G_BEGIN_DECLS

typedef struct _RyosRkpTimer RyosRkpTimer;

struct _RyosRkpTimer {
	guint8 name[RYOS_RKP_TIMER_NAME_LENGTH];
	guint8 duration;
	guint8 unknown[12];
} __attribute__ ((packed));

void ryos_rkp_timer_set_name(RyosRkpTimer *timer, gchar const *new_name);
void roccat_timer_to_ryos_rkp_timer(RoccatTimer const *roccat_timer, RyosRkpTimer *ryos_timer);
gboolean ryos_rkp_timer_equal(RyosRkpTimer const *left, RyosRkpTimer const *right);
void ryos_rkp_timer_copy(RyosRkpTimer *destination, RyosRkpTimer const *source);
RyosRkpTimer *ryos_rkp_timer_dup(RyosRkpTimer const *source);

G_END_DECLS

#endif
