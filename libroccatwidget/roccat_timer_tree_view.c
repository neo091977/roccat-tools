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

#include "roccat_timer_tree_view.h"
#include "gtk_roccat_helper.h"
#include "i18n-lib.h"
#include <string.h>
#include <stdlib.h>

#define ROCCAT_TIMER_TREE_VIEW_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), ROCCAT_TIMER_TREE_VIEW_TYPE, RoccatTimerTreeViewClass))
#define IS_ROCCAT_TIMER_TREE_VIEW_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), ROCCAT_TIMER_TREE_VIEW_TYPE))
#define ROCCAT_TIMER_TREE_VIEW_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), ROCCAT_TIMER_TREE_VIEW_TYPE, RoccatTimerTreeViewPrivate))

typedef struct _RoccatTimerTreeViewClass RoccatTimerTreeViewClass;

struct _RoccatTimerTreeViewClass {
	GtkTreeViewClass parent_class;
};

G_DEFINE_TYPE(RoccatTimerTreeView, roccat_timer_tree_view, GTK_TYPE_TREE_VIEW);

static gboolean name_is_valid(gchar const *name) {
	gchar *stripped;
	gboolean retval;

	if (name == NULL)
		return FALSE;

	stripped = g_strstrip(g_strdup(name));
	retval = !(g_ascii_strcasecmp(stripped, "") == 0);
	g_free(stripped);
	return retval;
}

static void name_edited_cb(GtkCellRendererText *renderer, gchar *path_string, gchar *new_name, gpointer user_data) {
	RoccatTimerTreeView *timer_tree_view = ROCCAT_TIMER_TREE_VIEW(user_data);
	GtkTreeIter iter;
	RoccatTimerListStore *store;

	if (!name_is_valid(new_name))
		return;

	store = roccat_timer_tree_view_get_store(timer_tree_view);

	if (!gtk_tree_model_get_iter_from_string(GTK_TREE_MODEL(store), &iter, path_string))
		return;

	if (roccat_timer_list_store_is_name_unique(store, new_name))
		roccat_timer_list_store_set_name(store, &iter, new_name);
}

static void seconds_edited_cb(GtkCellRendererText *renderer, gchar *path_string, gchar *text, gpointer user_data) {
	RoccatTimerTreeView *timer_tree_view = ROCCAT_TIMER_TREE_VIEW(user_data);
	GtkTreeIter iter;
	gulong new_seconds;
	RoccatTimerListStore *store;

	new_seconds = strtoul(text, NULL, 10);

	if (new_seconds < 1 || new_seconds > G_MAXINT)
		return;

	store = roccat_timer_tree_view_get_store(timer_tree_view);

	if (!gtk_tree_model_get_iter_from_string(GTK_TREE_MODEL(store), &iter, path_string))
		return;

	roccat_timer_list_store_set_seconds(store, &iter, new_seconds);
}

static void treeview_focus_name(GtkTreeView *treeview, GtkTreePath *row_sorted) {
	GtkTreeViewColumn *column;

	column = gtk_tree_view_get_column(treeview, roccat_timer_list_store_name_column());
	gtk_tree_view_scroll_to_cell(treeview, NULL, column, TRUE, 0, 0);
	gtk_tree_view_set_cursor(treeview, row_sorted, column, TRUE);
}

void roccat_timer_tree_view_add(RoccatTimerTreeView *timer_tree_view) {
	RoccatTimerListStore *store;
	GtkTreePath *row;
	GtkTreeIter iter;
	gchar *name;

	store = roccat_timer_tree_view_get_store(timer_tree_view);

	name = roccat_timer_list_store_generate_unique_name(store, _("New timer"));
	roccat_timer_list_store_add_timer(store, &iter, name, 30);
	g_free(name);

	row = gtk_tree_model_get_path(GTK_TREE_MODEL(store), &iter);
	treeview_focus_name(GTK_TREE_VIEW(timer_tree_view), row);
	gtk_tree_path_free(row);
}

void roccat_timer_tree_view_remove(RoccatTimerTreeView *timer_tree_view) {
	GtkTreeModel *model;
	GtkTreeIter iter;
	GtkTreeSelection *selection;

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(timer_tree_view));
	if (!gtk_tree_selection_get_selected(selection, &model, &iter))
		return;

	roccat_timer_list_store_remove(ROCCAT_TIMER_LIST_STORE(model), &iter);
}

GtkWidget *roccat_timer_tree_view_new(RoccatTimerListStore *store) {
	RoccatTimerTreeView *timer_tree_view;

	timer_tree_view = g_object_new(ROCCAT_TIMER_TREE_VIEW_TYPE, NULL);

	gtk_tree_view_set_model(GTK_TREE_VIEW(timer_tree_view), GTK_TREE_MODEL(store));

	return GTK_WIDGET(timer_tree_view);
}

static void roccat_timer_tree_view_init(RoccatTimerTreeView *timer_tree_view) {
	GtkCellRenderer *text_renderer, *spin_renderer;
	GtkTreeViewColumn *column;
	GtkAdjustment *adjustment;

	text_renderer = gtk_cell_renderer_text_new();
	g_object_set(text_renderer, "editable", TRUE, "editable-set", TRUE, NULL);
	g_signal_connect(G_OBJECT(text_renderer), "edited", G_CALLBACK(name_edited_cb), timer_tree_view);
	column = gtk_tree_view_column_new_with_attributes(_("Name"), text_renderer, "text", roccat_timer_list_store_name_column(), NULL);
	gtk_tree_view_column_set_sort_column_id(column, roccat_timer_list_store_name_column());
	gtk_tree_view_append_column(GTK_TREE_VIEW(timer_tree_view), column);

	adjustment = (GtkAdjustment *)gtk_adjustment_new(0, 1, G_MAXINT, 1, 60, 0);
	spin_renderer = gtk_cell_renderer_spin_new();
	g_object_set(spin_renderer, "editable", TRUE, "editable-set", TRUE, "adjustment", adjustment, NULL);
	g_signal_connect(G_OBJECT(spin_renderer), "edited", G_CALLBACK(seconds_edited_cb), timer_tree_view);
	column = gtk_tree_view_column_new_with_attributes(_("Seconds"), spin_renderer, "text", roccat_timer_list_store_seconds_column(), NULL);
	gtk_tree_view_column_set_sort_column_id(column, roccat_timer_list_store_seconds_column());
	gtk_tree_view_append_column(GTK_TREE_VIEW(timer_tree_view), column);

	gtk_tree_view_set_enable_search(GTK_TREE_VIEW(timer_tree_view), TRUE);
	gtk_tree_view_set_search_column(GTK_TREE_VIEW(timer_tree_view), roccat_timer_list_store_name_column());
	gtk_tree_view_set_search_equal_func(GTK_TREE_VIEW(timer_tree_view), gtk_roccat_tree_view_search_string_case_equal_func, NULL, NULL);
}

static void roccat_timer_tree_view_finalize(GObject *object) {
	G_OBJECT_CLASS(roccat_timer_tree_view_parent_class)->finalize(object);
}

static void roccat_timer_tree_view_class_init(RoccatTimerTreeViewClass *klass) {
	GObjectClass *gobject_class;

	gobject_class = G_OBJECT_CLASS(klass);

	gobject_class->finalize = roccat_timer_tree_view_finalize;
}

RoccatTimers *roccat_timer_tree_view_export(RoccatTimerTreeView *timer_tree_view) {
	RoccatTimerListStore *store;

	store = roccat_timer_tree_view_get_store(timer_tree_view);
	return roccat_timer_list_store_get_timers(store);
}

void roccat_timer_tree_view_import(RoccatTimerTreeView *timer_tree_view, RoccatTimers *roccat_timers) {
	RoccatTimerListStore *store;

	store = roccat_timer_tree_view_get_store(timer_tree_view);

	roccat_timer_list_store_clear(store);
	roccat_timer_list_store_add_timers(store, roccat_timers);
	roccat_timer_list_store_set_modified(store, FALSE);
}

RoccatTimerListStore *roccat_timer_tree_view_get_store(RoccatTimerTreeView *timer_tree_view) {
	return ROCCAT_TIMER_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(timer_tree_view)));
}
