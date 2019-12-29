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

#include "ryos_layer_selector.h"

#include "ryos_stored_lights.h"
#include "gtk_roccat_helper.h"
#include "i18n-lib.h"

#define RYOS_LAYER_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), RYOS_LAYER_SELECTOR_TYPE, RyosLayerSelectorClass))
#define IS_RYOS_LAYER_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), RYOS_LAYER_SELECTOR_TYPE))
#define RYOS_LAYER_SELECTOR_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), RYOS_LAYER_SELECTOR_TYPE, RyosLayerSelectorClass))
#define RYOS_LAYER_SELECTOR_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), RYOS_LAYER_SELECTOR_TYPE, RyosLayerSelectorPrivate))

typedef struct _RyosLayerSelectorClass RyosLayerSelectorClass;
typedef struct _RyosLayerSelectorPrivate RyosLayerSelectorPrivate;

struct _RyosLayerSelector {
	GtkComboBox parent;
	RyosLayerSelectorPrivate *priv;
};

struct _RyosLayerSelectorClass {
	GtkComboBoxClass parent_class;
	GtkListStore *store;
};

struct _RyosLayerSelectorPrivate {
	guint mode;
};

enum {
	TITLE_COLUMN,
	TYPE_COLUMN,
	GROUP_COLUMN,
	N_COLUMNS
};

enum {
	GROUP_BOTH,
	GROUP_ONLY_MANUAL,
};

G_DEFINE_TYPE(RyosLayerSelector, ryos_layer_selector, GTK_TYPE_COMBO_BOX);

static gint get_type(GtkTreeModel *model, GtkTreeIter *iter) {
	gint type;
	gtk_tree_model_get(model, iter, TYPE_COLUMN, &type, -1);
	return type;
}

static guint get_group(GtkTreeModel *model, GtkTreeIter *iter) {
	guint group;
	gtk_tree_model_get(model, iter, GROUP_COLUMN, &group, -1);
	return group;
}

GtkWidget *ryos_layer_selector_new(void) {
	return GTK_WIDGET(g_object_new(RYOS_LAYER_SELECTOR_TYPE, NULL));
}

static gboolean visible_func(GtkTreeModel *model, GtkTreeIter *iter, gpointer user_data) {
	RyosLayerSelector *layer_selector = RYOS_LAYER_SELECTOR(user_data);
	RyosLayerSelectorPrivate *priv = layer_selector->priv;
	guint group;

	if (priv->mode == RYOS_ILLUMINATION_MODE_MANUAL)
		return TRUE;

	group = get_group(model, iter);
	if (group == GROUP_ONLY_MANUAL)
		return FALSE;
	else
		return TRUE;
}

static void activate_first_iter(GtkComboBox *combo_box) {
	GtkTreeIter iter;
	gboolean valid;

	valid = gtk_tree_model_get_iter_first(gtk_combo_box_get_model(combo_box), &iter);
	if (valid)
		gtk_combo_box_set_active_iter(combo_box, &iter);
}

static void ryos_layer_selector_init(RyosLayerSelector *layer_selector) {
	RyosLayerSelectorPrivate *priv = RYOS_LAYER_SELECTOR_GET_PRIVATE(layer_selector);
	RyosLayerSelectorClass *klass = RYOS_LAYER_SELECTOR_GET_CLASS(layer_selector);
	GtkTreeModel *filter;
	GtkCellRenderer *renderer;

	layer_selector->priv = priv;

	priv->mode = RYOS_ILLUMINATION_MODE_MANUAL;

	filter = gtk_tree_model_filter_new(GTK_TREE_MODEL(klass->store), NULL);
	gtk_tree_model_filter_set_visible_func(GTK_TREE_MODEL_FILTER(filter), visible_func, layer_selector, NULL);
	gtk_combo_box_set_model(GTK_COMBO_BOX(layer_selector), filter);

	renderer = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(layer_selector), renderer, TRUE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(layer_selector), renderer, "text", TITLE_COLUMN, NULL);

	activate_first_iter(GTK_COMBO_BOX(layer_selector));
}

static void ryos_layer_selector_class_init(RyosLayerSelectorClass *klass) {
	GtkTreeIter iter;

	g_type_class_add_private(klass, sizeof(RyosLayerSelectorPrivate));

	klass->store = gtk_list_store_new(N_COLUMNS, G_TYPE_STRING, G_TYPE_UINT, G_TYPE_UINT);

	gtk_roccat_list_store_append_with_values(klass->store, &iter, TITLE_COLUMN, _("Normal"),
			TYPE_COLUMN, RYOS_STORED_LIGHTS_LAYER_NORMAL,
			GROUP_COLUMN, GROUP_BOTH, -1);
	gtk_roccat_list_store_append_with_values(klass->store, &iter, TITLE_COLUMN, _("Left shift"),
			TYPE_COLUMN, RYOS_STORED_LIGHTS_LAYER_LEFT_SHIFT,
			GROUP_COLUMN, GROUP_ONLY_MANUAL, -1);
	gtk_roccat_list_store_append_with_values(klass->store, &iter, TITLE_COLUMN, _("Right shift"),
			TYPE_COLUMN, RYOS_STORED_LIGHTS_LAYER_RIGHT_SHIFT,
			GROUP_COLUMN, GROUP_ONLY_MANUAL, -1);
	gtk_roccat_list_store_append_with_values(klass->store, &iter, TITLE_COLUMN, _("Left ctrl"),
			TYPE_COLUMN, RYOS_STORED_LIGHTS_LAYER_LEFT_CTRL,
			GROUP_COLUMN, GROUP_ONLY_MANUAL, -1);
	gtk_roccat_list_store_append_with_values(klass->store, &iter, TITLE_COLUMN, _("Right ctrl"),
			TYPE_COLUMN, RYOS_STORED_LIGHTS_LAYER_RIGHT_CTRL,
			GROUP_COLUMN, GROUP_ONLY_MANUAL, -1);
	gtk_roccat_list_store_append_with_values(klass->store, &iter, TITLE_COLUMN, _("Left alt"),
			TYPE_COLUMN, RYOS_STORED_LIGHTS_LAYER_LEFT_ALT,
			GROUP_COLUMN, GROUP_ONLY_MANUAL, -1);
	gtk_roccat_list_store_append_with_values(klass->store, &iter, TITLE_COLUMN, _("Right alt"),
			TYPE_COLUMN, RYOS_STORED_LIGHTS_LAYER_RIGHT_ALT,
			GROUP_COLUMN, GROUP_ONLY_MANUAL, -1);
	gtk_roccat_list_store_append_with_values(klass->store, &iter, TITLE_COLUMN, _("Left win"),
			TYPE_COLUMN, RYOS_STORED_LIGHTS_LAYER_LEFT_WIN,
			GROUP_COLUMN, GROUP_ONLY_MANUAL, -1);
	gtk_roccat_list_store_append_with_values(klass->store, &iter, TITLE_COLUMN, _("Right win"),
			TYPE_COLUMN, RYOS_STORED_LIGHTS_LAYER_RIGHT_WIN,
			GROUP_COLUMN, GROUP_ONLY_MANUAL, -1);
	gtk_roccat_list_store_append_with_values(klass->store, &iter, TITLE_COLUMN, _("Easyshift"),
			TYPE_COLUMN, RYOS_STORED_LIGHTS_LAYER_EASY_SHIFT,
			GROUP_COLUMN, GROUP_BOTH, -1);
	gtk_roccat_list_store_append_with_values(klass->store, &iter, TITLE_COLUMN, _("FN"),
			TYPE_COLUMN, RYOS_STORED_LIGHTS_LAYER_FN,
			GROUP_COLUMN, GROUP_BOTH, -1);
}

void ryos_layer_selector_set_mode(RyosLayerSelector *layer_selector, guint mode) {
	GtkTreeModel *model;

	layer_selector->priv->mode = mode;

	model = gtk_combo_box_get_model(GTK_COMBO_BOX(layer_selector));
	gtk_tree_model_filter_refilter(GTK_TREE_MODEL_FILTER(model));

	activate_first_iter(GTK_COMBO_BOX(layer_selector));
}

guint ryos_layer_selector_get_value(RyosLayerSelector *layer_selector) {
	GtkComboBox *combo_box = GTK_COMBO_BOX(layer_selector);
	GtkTreeIter iter;
	gboolean valid;

	valid = gtk_combo_box_get_active_iter(combo_box, &iter);
	if (!valid)
		return RYOS_ILLUMINATION_MODE_AUTOMATIC;

	return get_type(gtk_combo_box_get_model(combo_box), &iter);
}
