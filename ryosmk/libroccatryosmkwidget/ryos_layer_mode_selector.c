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

#include "ryos_layer_mode_selector.h"
#include "ryos.h"
#include "gtk_roccat_helper.h"
#include "i18n-lib.h"

#define RYOS_LAYER_MODE_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), RYOS_LAYER_MODE_SELECTOR_TYPE, RyosLayerModeSelectorClass))
#define IS_RYOS_LAYER_MODE_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), RYOS_LAYER_MODE_SELECTOR_TYPE))
#define RYOS_LAYER_MODE_SELECTOR_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), RYOS_LAYER_MODE_SELECTOR_TYPE, RyosLayerModeSelectorClass))

typedef struct _RyosLayerModeSelectorClass RyosLayerModeSelectorClass;

struct _RyosLayerModeSelector {
	GtkComboBox parent;
};

struct _RyosLayerModeSelectorClass {
	GtkComboBoxClass parent_class;
	GtkListStore *store;
};

enum {
	TITLE_COLUMN,
	MODE_COLUMN,
	N_COLUMNS
};

G_DEFINE_TYPE(RyosLayerModeSelector, ryos_layer_mode_selector, GTK_TYPE_COMBO_BOX);

static gboolean find_mode(GtkTreeModel *model, gint mode, GtkTreeIter *iter) {
	return gtk_roccat_tree_model_iter_find_int(model, MODE_COLUMN, mode, iter, NULL);
}

static gint get_mode(GtkTreeModel *model, GtkTreeIter *iter) {
	gint mode;
	gtk_tree_model_get(model, iter, MODE_COLUMN, &mode, -1);
	return mode;
}

GtkWidget *ryos_layer_mode_selector_new(void) {
	return GTK_WIDGET(g_object_new(RYOS_LAYER_MODE_SELECTOR_TYPE, NULL));
}

static void ryos_layer_mode_selector_init(RyosLayerModeSelector *mode_selector) {
	RyosLayerModeSelectorClass *klass = RYOS_LAYER_MODE_SELECTOR_GET_CLASS(mode_selector);
	GtkCellRenderer *renderer;

	gtk_combo_box_set_model(GTK_COMBO_BOX(mode_selector), GTK_TREE_MODEL(klass->store));

	renderer = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(mode_selector), renderer, TRUE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(mode_selector), renderer, "text", TITLE_COLUMN, NULL);
}

static void ryos_layer_mode_selector_class_init(RyosLayerModeSelectorClass *klass) {
	GtkTreeIter iter;

	klass->store = gtk_list_store_new(N_COLUMNS, G_TYPE_STRING, G_TYPE_UINT);

	gtk_roccat_list_store_append_with_values(klass->store, &iter, TITLE_COLUMN, _("Automatic"),
			MODE_COLUMN, RYOS_ILLUMINATION_MODE_AUTOMATIC, -1);
	gtk_roccat_list_store_append_with_values(klass->store, &iter, TITLE_COLUMN, _("Manual"),
			MODE_COLUMN, RYOS_ILLUMINATION_MODE_MANUAL, -1);
}

void ryos_layer_mode_selector_set_value(RyosLayerModeSelector *mode_selector, guint mode) {
	GtkComboBox *combo_box = GTK_COMBO_BOX(mode_selector);
	GtkTreeIter iter;
	gboolean valid;

	valid = find_mode(gtk_combo_box_get_model(combo_box), mode, &iter);
	if (!valid)
		valid = gtk_tree_model_get_iter_first(gtk_combo_box_get_model(combo_box), &iter);

	if (valid)
		gtk_combo_box_set_active_iter(combo_box, &iter);
}

guint ryos_layer_mode_selector_get_value(RyosLayerModeSelector *mode_selector) {
	GtkComboBox *combo_box = GTK_COMBO_BOX(mode_selector);
	GtkTreeIter iter;
	gboolean valid;

	valid = gtk_combo_box_get_active_iter(combo_box, &iter);
	if (!valid)
		return RYOS_ILLUMINATION_MODE_AUTOMATIC;

	return get_mode(gtk_combo_box_get_model(combo_box), &iter);
}
