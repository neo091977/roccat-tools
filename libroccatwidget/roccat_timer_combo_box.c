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

#include "roccat_timer_combo_box.h"
#include "roccat_timer_list_store.h"
#include "gtk_roccat_helper.h"

GtkWidget *roccat_timer_combo_box_new(void) {
	GtkCellRenderer *cell;
	GtkWidget *combo_box;
	RoccatTimerListStore *store;

	store = roccat_timer_list_store_new();
	combo_box = gtk_combo_box_new_with_model(GTK_TREE_MODEL(store));
	g_object_unref(store);

	cell = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(combo_box), cell, TRUE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(combo_box), cell, "text",
			roccat_timer_list_store_name_column(), NULL);
	return combo_box;
}

void roccat_timer_combo_box_set_timers(GtkComboBox *combo_box, RoccatTimers *timers) {
	RoccatTimerListStore *store;

	store = ROCCAT_TIMER_LIST_STORE(gtk_combo_box_get_model(combo_box));
	roccat_timer_list_store_add_timers(store, timers);
}

RoccatTimer *roccat_timer_combo_box_get_value(GtkComboBox *combo_box) {
	GtkTreeIter iter;
	RoccatTimerListStore *store;
	gboolean valid;

	store = ROCCAT_TIMER_LIST_STORE(gtk_combo_box_get_model(combo_box));

	valid = gtk_combo_box_get_active_iter(combo_box, &iter);
	if (!valid)
		valid = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(store), &iter);

	if (valid)
		return roccat_timer_list_store_get_timer(store, &iter);
	else
		return NULL;
}
