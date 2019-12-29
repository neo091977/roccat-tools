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

#include "gtk_roccat_helper.h"
#include <string.h>
#include <time.h>

void gtk_roccat_tree_store_append_with_values(GtkTreeStore *tree_store, GtkTreeIter *iter, GtkTreeIter *parent, ...) {
	va_list var_args;
	GtkTreeIter tmp_iter;

	if (!iter)
		iter = &tmp_iter;

	gtk_tree_store_append(tree_store, iter, parent);
	va_start(var_args, parent);
	gtk_tree_store_set_valist(tree_store, iter, var_args);
	va_end(var_args);
}

void gtk_roccat_list_store_append_with_values(GtkListStore *list_store, GtkTreeIter *iter, ...) {
	va_list var_args;
	GtkTreeIter tmp_iter;

	if (!iter)
		iter = &tmp_iter;

	gtk_list_store_append(list_store, iter);
	va_start(var_args, iter);
	gtk_list_store_set_valist(list_store, iter, var_args);
	va_end(var_args);
}

gboolean gtk_roccat_tree_model_iter_find_string(GtkTreeModel *model, gint key, gchar const *value, GtkTreeIter *result, GtkTreeIter *first) {
	GtkTreeIter iter, first_child;
	gboolean valid, has_child, test;
	gchar *data;

	if (first) {
		iter = *first;
		valid = TRUE;
	} else {
		valid = gtk_tree_model_get_iter_first(model, &iter);
	}

	while (valid) {
		gtk_tree_model_get(model, &iter, key, &data, -1);
		test = strcmp(data, value) == 0;
		g_free(data);
		if (test) {
			if (result != NULL)
				*result = iter;
			return TRUE;
		}
		has_child = gtk_tree_model_iter_children(model, &first_child, &iter);
		if (has_child) {
			if (gtk_roccat_tree_model_iter_find_string(model, key, value, result, &first_child)) {
				/* result is already set */
				return TRUE;
			}
		}
		valid = gtk_tree_model_iter_next(model, &iter);
	}
	return FALSE;
}

gboolean gtk_roccat_tree_model_iter_find_int(GtkTreeModel *model, gint key, gint value, GtkTreeIter *result, GtkTreeIter *first) {
	GtkTreeIter iter, first_child;
	gboolean valid, has_child;
	gint data;

	if (first) {
		iter = *first;
		valid = TRUE;
	} else {
		valid = gtk_tree_model_get_iter_first(model, &iter);
	}

	while (valid) {
		gtk_tree_model_get(model, &iter, key, &data, -1);
		if (data == value) {
			if (result != NULL)
				*result = iter;
			return TRUE;
		}
		has_child = gtk_tree_model_iter_children(model, &first_child, &iter);
		if (has_child) {
			if (gtk_roccat_tree_model_iter_find_int(model, key, value, result, &first_child)) {
				/* result is already set */
				return TRUE;
			}
		}
		valid = gtk_tree_model_iter_next(model, &iter);
	}
	return FALSE;
}

static gulong g_roccat_signal_handler_lookup(GObject *object, GType type, gchar const *signal_name) {
	guint signal_id = g_signal_lookup(signal_name, type);
	return g_signal_handler_find(object, G_SIGNAL_MATCH_ID, signal_id, 0, 0, 0, 0);
}

gulong gtk_roccat_combo_box_lookup_changed_handler(GtkComboBox *combo) {
	return g_roccat_signal_handler_lookup(G_OBJECT(combo), GTK_TYPE_COMBO_BOX, "changed");
}

gulong gtk_roccat_spin_button_lookup_value_changed_handler(GtkSpinButton *spin) {
	return g_roccat_signal_handler_lookup(G_OBJECT(spin), GTK_TYPE_SPIN_BUTTON, "value-changed");
}

GtkWidget *gtk_roccat_radio_button_group_get_active(GSList *group) {
	GSList *child;

	for (child = group; child; child = g_slist_next(child))
		if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(child->data)))
			return GTK_WIDGET(child->data);

	return NULL;
}

GtkWindow *gtk_roccat_widget_get_gtk_window(GtkWidget *widget) {
	GtkWidget *toplevel = gtk_widget_get_toplevel(widget);
	return GTK_IS_WINDOW(toplevel) ? GTK_WINDOW(toplevel) : NULL;
}

gboolean gtk_roccat_tree_view_search_string_case_equal_func(GtkTreeModel *model, gint column, gchar const *key, GtkTreeIter *iter, gpointer dummy) {
	gchar *string;
	gchar *folded_string, *folded_key;
	gboolean result;
	guint byte_size;

	gtk_tree_model_get(model, iter, column, &string, -1);
	byte_size = strlen(key);

	folded_string = g_utf8_casefold(string, byte_size);
	folded_key = g_utf8_casefold(key, byte_size);

	result = strncmp(folded_string, folded_key, byte_size) == 0 ? FALSE : TRUE;

	g_free(folded_string);
	g_free(folded_key);
	g_free(string);

	return result;
}

gint gtk_roccat_tree_iter_string_case_compare_func(GtkTreeModel *model, GtkTreeIter *a, GtkTreeIter *b, gpointer pcolumn) {
	gchar *a_string;
	gchar *b_string;
	gint retval;
	gint column = GPOINTER_TO_INT(pcolumn);

	gtk_tree_model_get(model, a, column, &a_string, -1);
	gtk_tree_model_get(model, b, column, &b_string, -1);

	if (a_string == NULL && b_string == NULL)
		retval = 0;
	else if (a_string == NULL)
		retval = 1;
	else if (b_string == NULL)
		retval = -1;
	else
		retval = g_ascii_strcasecmp(a_string, b_string);

	g_free(a_string);
	g_free(b_string);

	return retval;
}

gint gtk_roccat_tree_iter_int_compare_func(GtkTreeModel *model, GtkTreeIter *a, GtkTreeIter *b, gpointer pcolumn) {
	gint a_int, b_int;
	gint column = GPOINTER_TO_INT(pcolumn);

	gtk_tree_model_get(model, a, column, &a_int, -1);
	gtk_tree_model_get(model, b, column, &b_int, -1);

	if (a_int < b_int)
		return -1;
	else if (a_int > b_int)
		return 1;
	else
		return 0;
}

void gtk_roccat_radio_buttons_block_toggled(GSList *radios, gboolean block) {
	GSList *radio;
	guint signal_id;
	gulong signal_handler;

	signal_id = g_signal_lookup("toggled", GTK_TYPE_RADIO_BUTTON);
	radio = radios;
	while (radio) {
		signal_handler = g_signal_handler_find(G_OBJECT(radio->data), G_SIGNAL_MATCH_ID, signal_id, 0, 0, 0, 0);
		if (block)
			g_signal_handler_block(G_OBJECT(radio->data), signal_handler);
		else
			g_signal_handler_unblock(G_OBJECT(radio->data), signal_handler);
		radio = g_slist_next(radio);
	}
}

gboolean gtk_roccat_main_iterate_pending(void) {
	do {
		if (gtk_main_iteration_do(FALSE))
			return TRUE;
	} while (gtk_events_pending());
	return FALSE;
}

GtkWidget *gtk_roccat_label_new_left_aligned(gchar const *str) {
	GtkWidget *label;

	label = gtk_label_new(str);
	gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);

	return label;
}

void gtk_roccat_wait(guint seconds) {
	clock_t end;

	end = clock() + seconds * CLOCKS_PER_SEC;

	while (end > clock())
		gtk_main_iteration_do(FALSE);
}

gint gtk_roccat_container_get_n_children(GtkContainer *container) {
	GList *children;
	gint result;
	children = gtk_container_get_children(container);
	result = g_list_length(children);
	g_list_free(children);
	return result;
}

void gtk_roccat_toggle_button_toggle(GtkToggleButton *toggle_button) {
	gtk_toggle_button_set_active(toggle_button, !gtk_toggle_button_get_active(toggle_button));
}

void gtk_roccat_table_clear(GtkTable *table) {
	GList *list, *children;

	children = gtk_container_get_children(GTK_CONTAINER(table));

	for (list = children; list; list = g_list_next(list))
		gtk_container_remove(GTK_CONTAINER(table), list->data);

	g_list_free(children);

	gtk_table_resize(table, 1, 1);
}
