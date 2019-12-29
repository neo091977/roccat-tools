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

#include "ryosmkfx_dimness_selector.h"
#include "gtk_roccat_helper.h"
#include "i18n.h"
#include <string.h>

#define RYOSMKFX_DIMNESS_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), RYOSMKFX_DIMNESS_SELECTOR_TYPE, RyosmkfxDimnessSelectorClass))
#define IS_RYOSMKFX_DIMNESS_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), RYOSMKFX_DIMNESS_SELECTOR_TYPE))
#define RYOSMKFX_DIMNESS_SELECTOR_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), RYOSMKFX_DIMNESS_SELECTOR_TYPE, RyosmkfxDimnessSelectorClass))
#define RYOSMKFX_DIMNESS_SELECTOR_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), RYOSMKFX_DIMNESS_SELECTOR_TYPE, RyosmkfxDimnessSelectorPrivate))

typedef struct _RyosmkfxDimnessSelectorClass RyosmkfxDimnessSelectorClass;

struct _RyosmkfxDimnessSelector {
	GtkComboBox parent;
};

struct _RyosmkfxDimnessSelectorClass {
	GtkComboBoxClass parent_class;
	GtkListStore *store;
};

enum {
	TITLE_COLUMN,
	TYPE_COLUMN,
	N_COLUMNS
};

G_DEFINE_TYPE(RyosmkfxDimnessSelector, ryosmkfx_dimness_selector, GTK_TYPE_COMBO_BOX);

GtkWidget *ryosmkfx_dimness_selector_new(void) {
	return GTK_WIDGET(g_object_new(RYOSMKFX_DIMNESS_SELECTOR_TYPE, NULL));
}

static void ryosmkfx_dimness_selector_init(RyosmkfxDimnessSelector *dimness_selector) {
	RyosmkfxDimnessSelectorClass *klass = RYOSMKFX_DIMNESS_SELECTOR_GET_CLASS(dimness_selector);
	GtkCellRenderer *renderer;

	gtk_combo_box_set_model(GTK_COMBO_BOX(dimness_selector), GTK_TREE_MODEL(klass->store));

	renderer = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(dimness_selector), renderer, TRUE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(dimness_selector), renderer, "text", TITLE_COLUMN, NULL);
}

static void ryosmkfx_dimness_selector_class_init(RyosmkfxDimnessSelectorClass *klass) {
	GtkTreeIter iter;

	klass->store = gtk_list_store_new(N_COLUMNS, G_TYPE_STRING, G_TYPE_UINT);

	gtk_roccat_list_store_append_with_values(klass->store, &iter,
				TITLE_COLUMN, _("Dimness"),
				TYPE_COLUMN, RYOSMKFX_LIGHT_DIMNESS_TYPE_OFF,
				-1);
	gtk_roccat_list_store_append_with_values(klass->store, &iter,
				TITLE_COLUMN, _("Starlit sky"),
				TYPE_COLUMN, RYOSMKFX_LIGHT_DIMNESS_TYPE_STARLIT_SKY,
				-1);
	gtk_roccat_list_store_append_with_values(klass->store, &iter,
				TITLE_COLUMN, _("Fall asleep"),
				TYPE_COLUMN, RYOSMKFX_LIGHT_DIMNESS_TYPE_FALL_ASLEEP,
				-1);
}

void ryosmkfx_dimness_selector_set_from_profile_data(RyosmkfxDimnessSelector *dimness_selector, RyosmkfxProfileData const *profile_data) {
	GtkTreeIter iter;
	gboolean valid;
	RyosmkfxLight const *light;

	light = &profile_data->hardware.light;
	valid = gtk_roccat_tree_model_iter_find_int(gtk_combo_box_get_model(GTK_COMBO_BOX(dimness_selector)), TYPE_COLUMN, light->dimness_type, &iter, NULL);
	if (valid)
		gtk_combo_box_set_active_iter(GTK_COMBO_BOX(dimness_selector), &iter);
}

void ryosmkfx_dimness_selector_update_profile_data(RyosmkfxDimnessSelector *dimness_selector, RyosmkfxProfileData *profile_data) {
	gboolean valid;
	GtkTreeIter iter;
	gint type;
	RyosmkfxLight *light;

	valid = gtk_combo_box_get_active_iter(GTK_COMBO_BOX(dimness_selector), &iter);

	if (!valid)
		return;

	gtk_tree_model_get(gtk_combo_box_get_model(GTK_COMBO_BOX(dimness_selector)), &iter, TYPE_COLUMN, &type, -1);

	light = &profile_data->hardware.light;

	if (light->dimness_type != type) {
		light->dimness_type = type;
		profile_data->hardware.modified_light = TRUE;
	}
}
