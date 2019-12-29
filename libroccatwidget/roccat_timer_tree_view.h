#ifndef __ROCCAT_TIMER_TREE_VIEW_H__
#define __ROCCAT_TIMER_TREE_VIEW_H__

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

#include "roccat_timer_list_store.h"
#include "roccat_timers.h"
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define ROCCAT_TIMER_TREE_VIEW_TYPE (roccat_timer_tree_view_get_type())
#define ROCCAT_TIMER_TREE_VIEW(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), ROCCAT_TIMER_TREE_VIEW_TYPE, RoccatTimerTreeView))
#define IS_ROCCAT_TIMER_TREE_VIEW(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), ROCCAT_TIMER_TREE_VIEW_TYPE))

typedef struct _RoccatTimerTreeView RoccatTimerTreeView;

struct _RoccatTimerTreeView {
	GtkTreeView parent;
};

GType roccat_timer_tree_view_get_type(void);
GtkWidget *roccat_timer_tree_view_new(RoccatTimerListStore *store);

RoccatTimers *roccat_timer_tree_view_export(RoccatTimerTreeView *timer_tree_view);

/* clears store beforehand and sets modified to FALSE */
void roccat_timer_tree_view_import(RoccatTimerTreeView *timer_tree_view, RoccatTimers *roccat_timers);

RoccatTimerListStore *roccat_timer_tree_view_get_store(RoccatTimerTreeView *timer_tree_view);

void roccat_timer_tree_view_add(RoccatTimerTreeView *timer_tree_view);
void roccat_timer_tree_view_remove(RoccatTimerTreeView *timer_tree_view);

G_END_DECLS

#endif
