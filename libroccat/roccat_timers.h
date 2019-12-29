#ifndef __ROCCAT_TIMERS_H__
#define __ROCCAT_TIMERS_H__

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

#include "roccat_timer.h"

G_BEGIN_DECLS

typedef struct _RoccatTimers RoccatTimers;

RoccatTimers *roccat_timers_new(void);
void roccat_timers_free(RoccatTimers *timers);
RoccatTimers *roccat_timers_load(GError **error);
gboolean roccat_timers_save(RoccatTimers *timers, GError **error);
RoccatTimer *roccat_timers_get(RoccatTimers *timers, gchar const *timer_name, GError **error);
void roccat_timers_set(RoccatTimers *timers, RoccatTimer const *timer);
gchar **roccat_timers_get_timer_names(RoccatTimers *timers, gsize *length, GError **error);

G_END_DECLS

#endif
