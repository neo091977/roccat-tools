#ifndef __ROCCAT_GTK_HELPER_H__
#define __ROCCAT_GTK_HELPER_H__

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

#include <gtk/gtk.h>

G_BEGIN_DECLS

/* recursive search */
gboolean gtk_roccat_tree_model_iter_find_string(GtkTreeModel *model, gint key, gchar const *value, GtkTreeIter *result, GtkTreeIter *first);

/* recursive search */
gboolean gtk_roccat_tree_model_iter_find_int(GtkTreeModel *model, gint key, gint value, GtkTreeIter *result, GtkTreeIter *first);

void gtk_roccat_tree_store_append_with_values(GtkTreeStore *tree_store, GtkTreeIter *iter, GtkTreeIter *parent, ...);

void gtk_roccat_list_store_append_with_values(GtkListStore *list_store, GtkTreeIter *iter, ...);

gulong gtk_roccat_combo_box_lookup_changed_handler(GtkComboBox *combo);

gulong gtk_roccat_spin_button_lookup_value_changed_handler(GtkSpinButton *spin);

GtkWidget *gtk_roccat_radio_button_group_get_active(GSList *group);

GtkWindow *gtk_roccat_widget_get_gtk_window(GtkWidget *widget);

/* case insensitive search */
gboolean gtk_roccat_tree_view_search_string_case_equal_func(GtkTreeModel *model, gint column, gchar const *key, GtkTreeIter *iter, gpointer dummy);

/* case insensitive sort */
gint gtk_roccat_tree_iter_string_case_compare_func(GtkTreeModel *model, GtkTreeIter *a, GtkTreeIter *b, gpointer pcolumn);

gint gtk_roccat_tree_iter_int_compare_func(GtkTreeModel *model, GtkTreeIter *a, GtkTreeIter *b, gpointer pcolumn);

void gtk_roccat_radio_buttons_block_toggled(GSList *radios, gboolean block);

/*
 * Runs iterations of main loop until all pending events are processed.
 * Returns TRUE if gtk_main_quit() has been called for the innermost mainloop.
 */
gboolean gtk_roccat_main_iterate_pending(void);

GtkWidget *gtk_roccat_label_new_left_aligned(gchar const *str);

/*
 * Busywait but does not block UI
 */
void gtk_roccat_wait(guint seconds);

gint gtk_roccat_container_get_n_children(GtkContainer *container);

void gtk_roccat_toggle_button_toggle(GtkToggleButton *toggle_button);

void gtk_roccat_table_clear(GtkTable *table);

G_END_DECLS

#endif
