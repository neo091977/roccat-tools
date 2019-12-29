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

#include "roccat_notification_type_combo_box.h"
#include "roccat_notificator.h"
#include "gtk_roccat_helper.h"
#include "i18n-lib.h"

enum {
	TITLE_COLUMN,
	TYPE_COLUMN,
	N_COLUMNS
};

GtkWidget *roccat_notification_type_combo_box_new(void) {
	GtkWidget *combo;
	GtkListStore *store;
	GtkTreeIter iter;
	GtkCellRenderer *renderer;

	store = gtk_list_store_new(N_COLUMNS, G_TYPE_STRING, G_TYPE_INT);

	gtk_roccat_list_store_append_with_values(store, &iter, TITLE_COLUMN, _("Off"), TYPE_COLUMN, ROCCAT_NOTIFICATION_TYPE_OFF, -1);
	gtk_roccat_list_store_append_with_values(store, &iter, TITLE_COLUMN, _("Audio"), TYPE_COLUMN, ROCCAT_NOTIFICATION_TYPE_AUDIO, -1);
	gtk_roccat_list_store_append_with_values(store, &iter, TITLE_COLUMN, _("OSD"), TYPE_COLUMN, ROCCAT_NOTIFICATION_TYPE_OSD, -1);

	combo = gtk_combo_box_new_with_model(GTK_TREE_MODEL(store));
	g_object_unref(store);

	renderer = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(combo), renderer, TRUE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(combo), renderer, "text", TITLE_COLUMN, NULL);

	return combo;
}

gint roccat_notification_type_combo_box_get_value(GtkComboBox *combo) {
	GtkTreeIter iter;
	GtkTreeModel *model;
	gboolean valid;
	gint type;

	valid = gtk_combo_box_get_active_iter(combo, &iter);
	if (!valid) {
		model = gtk_combo_box_get_model(combo);
		valid = gtk_tree_model_get_iter_first(model, &iter);
	}

	/* not testing for empty model, so this is always executed */
	gtk_tree_model_get(gtk_combo_box_get_model(combo), &iter, TYPE_COLUMN, &type, -1);
	return type;
}

void roccat_notification_type_combo_box_set_value(GtkComboBox *combo, gint value) {
	GtkTreeIter iter;
	GtkTreeModel *model;
	gboolean valid;

	model = gtk_combo_box_get_model(combo);
	valid = gtk_roccat_tree_model_iter_find_int(model, TYPE_COLUMN, value, &iter, NULL);
	if (valid)
		gtk_combo_box_set_active_iter(combo, &iter);
}
