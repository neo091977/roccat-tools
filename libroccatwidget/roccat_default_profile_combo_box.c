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

#include "roccat_default_profile_combo_box.h"
#include "gtk_roccat_helper.h"
#include "i18n-lib.h"

#define ROCCAT_DEFAULT_PROFILE_COMBO_BOX_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), ROCCAT_DEFAULT_PROFILE_COMBO_BOX_TYPE, RoccatDefaultProfileComboBoxClass))
#define IS_ROCCAT_DEFAULT_PROFILE_COMBO_BOX_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), ROCCAT_DEFAULT_PROFILE_COMBO_BOX_TYPE))
#define ROCCAT_DEFAULT_PROFILE_COMBO_BOX_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), ROCCAT_DEFAULT_PROFILE_COMBO_BOX_TYPE, RoccatDefaultProfileComboBoxPrivate))

typedef struct _RoccatDefaultProfileComboBoxClass RoccatDefaultProfileComboBoxClass;
typedef struct _RoccatDefaultProfileComboBoxPrivate RoccatDefaultProfileComboBoxPrivate;

struct _RoccatDefaultProfileComboBox {
	GtkComboBox parent;
	RoccatDefaultProfileComboBoxPrivate *priv;
};

struct _RoccatDefaultProfileComboBoxClass {
	GtkComboBoxClass parent_class;
};

G_DEFINE_TYPE(RoccatDefaultProfileComboBox, roccat_default_profile_combo_box, GTK_TYPE_COMBO_BOX);

enum {
	TITLE_COLUMN,
	VALUE_COLUMN,
	N_COLUMNS
};

guint roccat_default_profile_combo_box_get_value(RoccatDefaultProfileComboBox *combo_box) {
	GtkTreeIter iter;
	GtkTreeModel *model;
	gboolean valid;
	guint value;

	valid = gtk_combo_box_get_active_iter(GTK_COMBO_BOX(combo_box), &iter);
	if (!valid) {
		model = gtk_combo_box_get_model(GTK_COMBO_BOX(combo_box));
		valid = gtk_tree_model_get_iter_first(model, &iter);
	}

	gtk_tree_model_get(gtk_combo_box_get_model(GTK_COMBO_BOX(combo_box)), &iter, VALUE_COLUMN, &value, -1);
	return value;
}

void roccat_default_profile_combo_box_set_value(RoccatDefaultProfileComboBox *combo_box, guint new_value) {
	GtkTreeIter iter;
	GtkTreeModel *model;
	gboolean valid;

	model = gtk_combo_box_get_model(GTK_COMBO_BOX(combo_box));
	valid = gtk_roccat_tree_model_iter_find_int(model, VALUE_COLUMN, new_value, &iter, NULL);
	if (valid)
		gtk_combo_box_set_active_iter(GTK_COMBO_BOX(combo_box), &iter);
}

static void set_model(GtkComboBox *combo_box, guint max_profile_number) {
	GtkListStore *store;
	guint i;
	GtkTreeIter iter;
	gchar *string;

	store = gtk_list_store_new(N_COLUMNS, G_TYPE_STRING, G_TYPE_INT);

	gtk_roccat_list_store_append_with_values(store, &iter, TITLE_COLUMN, _("Off"), VALUE_COLUMN, 0, -1);

	for (i = 1; i <= max_profile_number; ++i) {
		string = g_strdup_printf("%u", i);
		gtk_roccat_list_store_append_with_values(store, &iter, TITLE_COLUMN, string, VALUE_COLUMN, i, -1);
		g_free(string);
	}

	gtk_combo_box_set_model(combo_box, GTK_TREE_MODEL(store));
	g_object_unref(store);
}

static void set_renderer(GtkComboBox *combo_box) {
	GtkCellRenderer *renderer;
	renderer = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(combo_box), renderer, TRUE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(combo_box), renderer, "text", TITLE_COLUMN, NULL);
}

GtkWidget *roccat_default_profile_combo_box_new(guint max_profile_number) {
	GtkComboBox *combo_box;

	combo_box = GTK_COMBO_BOX(g_object_new(ROCCAT_DEFAULT_PROFILE_COMBO_BOX_TYPE,
			NULL));

	set_model(combo_box, max_profile_number);
	set_renderer(combo_box);

	return GTK_WIDGET(combo_box);
}

static void roccat_default_profile_combo_box_init(RoccatDefaultProfileComboBox *default_profile_combo_box) {
}

static void roccat_default_profile_combo_box_class_init(RoccatDefaultProfileComboBoxClass *klass) {
}
