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
#include "gtk_roccat_helper.h"
#include "i18n-lib.h"
#include <string.h>

#define ROCCAT_TIMER_LIST_STORE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), ROCCAT_TIMER_LIST_STORE_TYPE, RoccatTimerListStoreClass))
#define IS_ROCCAT_TIMER_LIST_STORE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), ROCCAT_TIMER_LIST_STORE_TYPE))
#define ROCCAT_TIMER_LIST_STORE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), ROCCAT_TIMER_LIST_STORE_TYPE, RoccatTimerListStorePrivate))

typedef struct _RoccatTimerListStoreClass RoccatTimerListStoreClass;

struct _RoccatTimerListStoreClass {
	GtkListStoreClass parent_class;
	void (*modified)(RoccatTimerListStore *timer_list_store);
};

struct _RoccatTimerListStorePrivate {
	gboolean modified;
};

G_DEFINE_TYPE(RoccatTimerListStore, roccat_timer_list_store, GTK_TYPE_LIST_STORE);

enum {
	MODIFIED,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

enum {
	ROCCAT_TIMER_LIST_STORE_NAME_COLUMN,
	ROCCAT_TIMER_LIST_STORE_SECONDS_COLUMN,
	ROCCAT_TIMER_LIST_STORE_N_COLUMNS
};

void roccat_timer_list_store_set_modified(RoccatTimerListStore *timer_list_store, gboolean state) {
	timer_list_store->priv->modified = state;
	if (state)
		g_signal_emit((gpointer)timer_list_store, signals[MODIFIED], 0);
}

gboolean roccat_timer_list_store_get_modified(RoccatTimerListStore *timer_list_store) {
	return timer_list_store->priv->modified;
}

RoccatTimerListStore *roccat_timer_list_store_new(void) {
	RoccatTimerListStore *timer_list_store;

	timer_list_store = g_object_new(ROCCAT_TIMER_LIST_STORE_TYPE, NULL);

	return timer_list_store;
}

static void roccat_timer_list_store_init(RoccatTimerListStore *timer_list_store) {
	RoccatTimerListStorePrivate *priv = ROCCAT_TIMER_LIST_STORE_GET_PRIVATE(timer_list_store);
	GtkTreeSortable *sortable = GTK_TREE_SORTABLE(timer_list_store);
	GType types[ROCCAT_TIMER_LIST_STORE_N_COLUMNS] = {
		G_TYPE_STRING,
		G_TYPE_INT
	};

	timer_list_store->priv = priv;

	priv->modified = FALSE;

	gtk_list_store_set_column_types(GTK_LIST_STORE(timer_list_store),
			ROCCAT_TIMER_LIST_STORE_N_COLUMNS, types);

	gtk_tree_sortable_set_sort_func(sortable,
			ROCCAT_TIMER_LIST_STORE_NAME_COLUMN,
			gtk_roccat_tree_iter_string_case_compare_func,
			GINT_TO_POINTER(ROCCAT_TIMER_LIST_STORE_NAME_COLUMN),
			NULL);

	gtk_tree_sortable_set_sort_func(sortable,
			ROCCAT_TIMER_LIST_STORE_SECONDS_COLUMN,
			gtk_roccat_tree_iter_int_compare_func,
			GINT_TO_POINTER(ROCCAT_TIMER_LIST_STORE_SECONDS_COLUMN),
			NULL);

	/* initial sorting */
	gtk_tree_sortable_set_sort_column_id(sortable, ROCCAT_TIMER_LIST_STORE_NAME_COLUMN, GTK_SORT_ASCENDING);

	priv->modified = FALSE;
}

static void roccat_timer_list_store_class_init(RoccatTimerListStoreClass *klass) {
	g_type_class_add_private(klass, sizeof(RoccatTimerListStorePrivate));

	signals[MODIFIED] = g_signal_new("modified",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			G_STRUCT_OFFSET(RoccatTimerListStoreClass, modified),
			NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);
}

guint roccat_timer_list_store_name_column(void) {
	return ROCCAT_TIMER_LIST_STORE_NAME_COLUMN;
}

guint roccat_timer_list_store_seconds_column(void) {
	return ROCCAT_TIMER_LIST_STORE_SECONDS_COLUMN;
}

gboolean roccat_timer_list_store_set_name(RoccatTimerListStore *timer_list_store, GtkTreeIter *iter, gchar const *new_name) {
	gchar *actual_name;
	gboolean retval = FALSE;

	gtk_tree_model_get(GTK_TREE_MODEL(timer_list_store), iter, ROCCAT_TIMER_LIST_STORE_NAME_COLUMN, &actual_name, -1);

	if (strcmp(actual_name, new_name)) {
		gtk_list_store_set(GTK_LIST_STORE(timer_list_store), iter, ROCCAT_TIMER_LIST_STORE_NAME_COLUMN, new_name, -1);
		roccat_timer_list_store_set_modified(timer_list_store, TRUE);
		retval = TRUE;
	}

	g_free(actual_name);

	return retval;
}

gboolean roccat_timer_list_store_set_seconds(RoccatTimerListStore *timer_list_store, GtkTreeIter *iter, gint new_seconds) {
	gint actual_seconds;
	gboolean retval = FALSE;

	gtk_tree_model_get(GTK_TREE_MODEL(timer_list_store), iter, ROCCAT_TIMER_LIST_STORE_SECONDS_COLUMN, &actual_seconds, -1);

	if (actual_seconds != new_seconds) {
		gtk_list_store_set(GTK_LIST_STORE(timer_list_store), iter, ROCCAT_TIMER_LIST_STORE_SECONDS_COLUMN, new_seconds, -1);
		roccat_timer_list_store_set_modified(timer_list_store, TRUE);
		retval = TRUE;
	}

	return retval;
}

/*
 * Design decision: Not importing timers with already used names.
 * Alternative would be to generate unique names.
 */
static void add_timer(RoccatTimerListStore *timer_list_store, GtkTreeIter *iter, gchar const *name, gint seconds) {
	if (!roccat_timer_list_store_is_name_unique(timer_list_store, name)) {
		g_message(_("Ignoring already existing timer %s"), name);
		return;
	}

	gtk_roccat_list_store_append_with_values(GTK_LIST_STORE(timer_list_store), iter,
			ROCCAT_TIMER_LIST_STORE_NAME_COLUMN, name,
			ROCCAT_TIMER_LIST_STORE_SECONDS_COLUMN, seconds,
			-1);
}

void roccat_timer_list_store_remove(RoccatTimerListStore *timer_list_store, GtkTreeIter *iter) {
	gtk_list_store_remove(GTK_LIST_STORE(timer_list_store), iter);
	roccat_timer_list_store_set_modified(timer_list_store, TRUE);
}

void roccat_timer_list_store_add_timer(RoccatTimerListStore *timer_list_store, GtkTreeIter *iter, gchar const *name, gint seconds) {
	add_timer(timer_list_store, iter, name, seconds);
	roccat_timer_list_store_set_modified(timer_list_store, TRUE);
}

void roccat_timer_list_store_add_timers(RoccatTimerListStore *timer_list_store, RoccatTimers *roccat_timers) {
	RoccatTimer *roccat_timer;
	gchar **timer_names, **timer_name;
	GError *error = NULL;

	timer_names = roccat_timers_get_timer_names(roccat_timers, NULL, &error);
	if (error) {
		g_warning(_("Could not get timer names: %s"), error->message);
		g_error_free(error);
		return;
	}

	for (timer_name = timer_names; *timer_name; ++timer_name) {
		roccat_timer = roccat_timers_get(roccat_timers, *timer_name, &error);
		add_timer(timer_list_store, NULL, (gchar const *)roccat_timer->name, roccat_timer_get_seconds(roccat_timer));
		roccat_timer_free(roccat_timer);
	}

	g_strfreev(timer_names);
	roccat_timer_list_store_set_modified(timer_list_store, TRUE);
}

RoccatTimer *roccat_timer_list_store_get_timer(RoccatTimerListStore *timer_list_store, GtkTreeIter *iter) {
	RoccatTimer *roccat_timer;
	gchar *name;
	gint seconds;

	gtk_tree_model_get(GTK_TREE_MODEL(timer_list_store), iter,
			ROCCAT_TIMER_LIST_STORE_NAME_COLUMN, &name,
			ROCCAT_TIMER_LIST_STORE_SECONDS_COLUMN, &seconds,
			-1);

	roccat_timer = roccat_timer_new();
	roccat_timer_set_name(roccat_timer, name);
	roccat_timer_set_seconds(roccat_timer, seconds);

	g_free(name);

	return roccat_timer;
}

RoccatTimers *roccat_timer_list_store_get_timers(RoccatTimerListStore *timer_list_store) {
	RoccatTimers *roccat_timers;
	GtkTreeIter iter;
	RoccatTimer *roccat_timer;
	GtkTreeModel *model = GTK_TREE_MODEL(timer_list_store);

	roccat_timers = roccat_timers_new();

	if (gtk_tree_model_get_iter_first(model, &iter)) {

		do {
			roccat_timer = roccat_timer_list_store_get_timer(timer_list_store, &iter);
			roccat_timers_set(roccat_timers, roccat_timer);
			roccat_timer_free(roccat_timer);
		} while (gtk_tree_model_iter_next(model, &iter));

	}

	return roccat_timers;
}

gboolean roccat_timer_list_store_is_name_unique(RoccatTimerListStore *timer_list_store, gchar const *name) {
	return !gtk_roccat_tree_model_iter_find_string(GTK_TREE_MODEL(timer_list_store), ROCCAT_TIMER_LIST_STORE_NAME_COLUMN, name, NULL, NULL);
}

gchar *roccat_timer_list_store_generate_unique_name(RoccatTimerListStore *timer_list_store, gchar const *name) {
	gchar *new_name;
	guint count;
	count = 1;

	if (roccat_timer_list_store_is_name_unique(timer_list_store, name))
		return g_strdup(name);

	for (count = 1; count < G_MAXUINT8; ++count) {
		new_name = g_strdup_printf("%s_%03i", name, count);
		if (roccat_timer_list_store_is_name_unique(timer_list_store, new_name))
			return new_name;
		g_free(new_name);
	}
	return NULL;
}

void roccat_timer_list_store_clear(RoccatTimerListStore *timer_list_store) {
	gtk_list_store_clear(GTK_LIST_STORE(timer_list_store));
	roccat_timer_list_store_set_modified(timer_list_store, TRUE);
}
