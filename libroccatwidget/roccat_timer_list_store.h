#ifndef __ROCCAT_TIMER_LIST_STORE_H__
#define __ROCCAT_TIMER_LIST_STORE_H__

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

#include "roccat_timers.h"
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define ROCCAT_TIMER_LIST_STORE_TYPE (roccat_timer_list_store_get_type())
#define ROCCAT_TIMER_LIST_STORE(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), ROCCAT_TIMER_LIST_STORE_TYPE, RoccatTimerListStore))
#define IS_ROCCAT_TIMER_LIST_STORE(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), ROCCAT_TIMER_LIST_STORE_TYPE))

typedef struct _RoccatTimerListStore RoccatTimerListStore;
typedef struct _RoccatTimerListStorePrivate RoccatTimerListStorePrivate;

struct _RoccatTimerListStore {
	GtkListStore parent;
	RoccatTimerListStorePrivate *priv;
};

GType roccat_timer_list_store_get_type(void);

RoccatTimerListStore *roccat_timer_list_store_new(void);
guint roccat_timer_list_store_name_column(void);
guint roccat_timer_list_store_seconds_column(void);
void roccat_timer_list_store_set_modified(RoccatTimerListStore *timer_list_store, gboolean state);
gboolean roccat_timer_list_store_get_modified(RoccatTimerListStore *timer_list_store);
void roccat_timer_list_store_remove(RoccatTimerListStore *timer_list_store, GtkTreeIter *iter);
void roccat_timer_list_store_add_timer(RoccatTimerListStore *timer_list_store, GtkTreeIter *iter, gchar const *name, gint seconds);
void roccat_timer_list_store_add_timers(RoccatTimerListStore *timer_list_store, RoccatTimers *roccat_timers);
RoccatTimers *roccat_timer_list_store_get_timers(RoccatTimerListStore *timer_list_store);
RoccatTimer *roccat_timer_list_store_get_timer(RoccatTimerListStore *timer_list_store, GtkTreeIter *iter);
gboolean roccat_timer_list_store_is_name_unique(RoccatTimerListStore *timer_list_store, gchar const *name);
gchar *roccat_timer_list_store_generate_unique_name(RoccatTimerListStore *timer_list_store, gchar const *name);
void roccat_timer_list_store_clear(RoccatTimerListStore *timer_list_store);

/* returns TRUE if name was different */
gboolean roccat_timer_list_store_set_name(RoccatTimerListStore *timer_list_store, GtkTreeIter *iter, gchar const *new_name);
gboolean roccat_timer_list_store_set_seconds(RoccatTimerListStore *timer_list_store, GtkTreeIter *iter, gint new_seconds);

G_END_DECLS

#endif
