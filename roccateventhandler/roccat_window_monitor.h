#ifndef __ROCCAT_WINDOW_MONITOR_H__
#define __ROCCAT_WINDOW_MONITOR_H__

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

#include <glib-object.h>

G_BEGIN_DECLS

#define ROCCAT_WINDOW_MONITOR_TYPE (roccat_window_monitor_get_type())
#define ROCCAT_WINDOW_MONITOR(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), ROCCAT_WINDOW_MONITOR_TYPE, RoccatWindowMonitor))
#define IS_ROCCAT_WINDOW_MONITOR(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), ROCCAT_WINDOW_MONITOR_TYPE))

typedef struct _RoccatWindowMonitor RoccatWindowMonitor;

GType roccat_window_monitor_get_type(void);
RoccatWindowMonitor *roccat_window_monitor_new(void);

void roccat_window_monitor_set_interval(RoccatWindowMonitor *monitor, gint interval);

G_END_DECLS

#endif
