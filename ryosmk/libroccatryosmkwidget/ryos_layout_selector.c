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

#include "ryos_layout_selector.h"
#include "gtk_roccat_helper.h"
#include <string.h>

#define RYOS_LAYOUT_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), RYOS_LAYOUT_SELECTOR_TYPE, RyosLayoutSelectorClass))
#define IS_RYOS_LAYOUT_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), RYOS_LAYOUT_SELECTOR_TYPE))
#define RYOS_LAYOUT_SELECTOR_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), RYOS_LAYOUT_SELECTOR_TYPE, RyosLayoutSelectorClass))

typedef struct _RyosLayoutSelectorClass RyosLayoutSelectorClass;

struct _RyosLayoutSelector {
	GtkComboBox parent;
};

struct _RyosLayoutSelectorClass {
	GtkComboBoxClass parent_class;
	GtkListStore *store;
};

enum {
	TEXT_COLUMN,
	N_COLUMNS
};

G_DEFINE_TYPE(RyosLayoutSelector, ryos_layout_selector, GTK_TYPE_COMBO_BOX);

GtkWidget *ryos_layout_selector_new(void) {
	return GTK_WIDGET(g_object_new(RYOS_LAYOUT_SELECTOR_TYPE, NULL));
}

static void ryos_layout_selector_init(RyosLayoutSelector *layout_selector) {
	RyosLayoutSelectorClass *klass = RYOS_LAYOUT_SELECTOR_GET_CLASS(layout_selector);
	GtkCellRenderer *renderer;

	gtk_combo_box_set_model(GTK_COMBO_BOX(layout_selector), GTK_TREE_MODEL(klass->store));

	renderer = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(layout_selector), renderer, TRUE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(layout_selector), renderer, "text", TEXT_COLUMN, NULL);
}

static void ryos_layout_selector_class_init(RyosLayoutSelectorClass *klass) {
	GtkTreeIter iter;
	klass->store = gtk_list_store_new(N_COLUMNS, G_TYPE_STRING);
	gtk_roccat_list_store_append_with_values(klass->store, &iter, TEXT_COLUMN, "Generic", -1);
	gtk_roccat_list_store_append_with_values(klass->store, &iter, TEXT_COLUMN, "BE", -1);
	gtk_roccat_list_store_append_with_values(klass->store, &iter, TEXT_COLUMN, "CH", -1);
	gtk_roccat_list_store_append_with_values(klass->store, &iter, TEXT_COLUMN, "DE", -1);
	gtk_roccat_list_store_append_with_values(klass->store, &iter, TEXT_COLUMN, "ES", -1);
	gtk_roccat_list_store_append_with_values(klass->store, &iter, TEXT_COLUMN, "FR", -1);
	gtk_roccat_list_store_append_with_values(klass->store, &iter, TEXT_COLUMN, "HU", -1);
	gtk_roccat_list_store_append_with_values(klass->store, &iter, TEXT_COLUMN, "IT", -1);
	gtk_roccat_list_store_append_with_values(klass->store, &iter, TEXT_COLUMN, "JP", -1);
	gtk_roccat_list_store_append_with_values(klass->store, &iter, TEXT_COLUMN, "KR", -1);
	gtk_roccat_list_store_append_with_values(klass->store, &iter, TEXT_COLUMN, "LA", -1);
	gtk_roccat_list_store_append_with_values(klass->store, &iter, TEXT_COLUMN, "NO", -1);
	gtk_roccat_list_store_append_with_values(klass->store, &iter, TEXT_COLUMN, "PT", -1);
	gtk_roccat_list_store_append_with_values(klass->store, &iter, TEXT_COLUMN, "RU", -1);
	gtk_roccat_list_store_append_with_values(klass->store, &iter, TEXT_COLUMN, "TR", -1);
	gtk_roccat_list_store_append_with_values(klass->store, &iter, TEXT_COLUMN, "UK", -1);
	gtk_roccat_list_store_append_with_values(klass->store, &iter, TEXT_COLUMN, "UK-SK-CZ", -1);
	gtk_roccat_list_store_append_with_values(klass->store, &iter, TEXT_COLUMN, "US", -1);
}

void ryos_layout_selector_set_value(RyosLayoutSelector *layout_selector, gchar const *layout) {
	GtkComboBox *combo_box = GTK_COMBO_BOX(layout_selector);
	GtkTreeIter iter;
	gboolean valid;

	valid = gtk_roccat_tree_model_iter_find_string(gtk_combo_box_get_model(combo_box), TEXT_COLUMN, layout, &iter, NULL);
	if (valid)
		gtk_combo_box_set_active_iter(combo_box, &iter);
}

gchar *ryos_layout_selector_get_value(RyosLayoutSelector *layout_selector) {
	GtkComboBox *combo_box = GTK_COMBO_BOX(layout_selector);
	GtkTreeModel *model;
	GtkTreeIter iter;
	gboolean valid;
	gchar *string;

	model = gtk_combo_box_get_model(combo_box);

	valid = gtk_combo_box_get_active_iter(combo_box, &iter);
	if (!valid)
		gtk_tree_model_get_iter_first(model, &iter);

	gtk_tree_model_get(model, &iter, TEXT_COLUMN, &string, -1);
	return string;
}
