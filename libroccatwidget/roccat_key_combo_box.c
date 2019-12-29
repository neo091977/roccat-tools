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

#include "roccat_key_combo_box.h"
#include "gtk_roccat_helper.h"
#include "string.h"
#include "i18n-lib.h"

#define ROCCAT_KEY_COMBO_BOX_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), ROCCAT_KEY_COMBO_BOX_TYPE, RoccatKeyComboBoxPrivate))

#if (GTK_CHECK_VERSION(2,24,0))
G_DEFINE_TYPE(RoccatKeyComboBox, roccat_key_combo_box, GTK_TYPE_COMBO_BOX);
#else
G_DEFINE_TYPE(RoccatKeyComboBox, roccat_key_combo_box, GTK_TYPE_COMBO_BOX_ENTRY);
#endif

struct _RoccatKeyComboBoxPrivate {
	guint exclude_mask;
};

enum {
	TITLE_COLUMN,
	TYPE_COLUMN,
	GROUP_COLUMN,
	N_COLUMNS
};

enum {
	PROP_0,
	PROP_EXCLUDE_MASK,
};

static gboolean roccat_key_combo_box_model_find_type(GtkTreeModel *model, gint type, GtkTreeIter *iter) {
	return gtk_roccat_tree_model_iter_find_int(model, TYPE_COLUMN, type, iter, NULL);
}

static gchar *roccat_key_combo_box_model_get_title(GtkTreeModel *model, GtkTreeIter *iter) {
	gchar *title;
	gtk_tree_model_get(model, iter, TITLE_COLUMN, &title, -1);
	return title;
}

static guint roccat_key_combo_box_model_get_group(GtkTreeModel *model, GtkTreeIter *iter) {
	guint group;
	gtk_tree_model_get(model, iter, GROUP_COLUMN, &group, -1);
	return group;
}

static gint roccat_key_combo_box_model_get_type(GtkTreeModel *model, GtkTreeIter *iter) {
	gint type;
	gtk_tree_model_get(model, iter, TYPE_COLUMN, &type, -1);
	return type;
}

static void cell_data_func(GtkCellLayout *cell_layout, GtkCellRenderer *cell, GtkTreeModel *model, GtkTreeIter *iter, gpointer data) {
	gboolean sensitive = TRUE;

	if (gtk_tree_model_iter_has_child(model, iter))
		sensitive = FALSE;
	else if (roccat_key_combo_box_model_get_type(model, iter) == ROCCAT_KEY_COMBO_BOX_TYPE_NONE)
		sensitive = FALSE;

	g_object_set(cell, "sensitive", sensitive, NULL);
}

static gboolean visible_func(GtkTreeModel *model, GtkTreeIter *iter, gpointer user_data) {
	RoccatKeyComboBoxPrivate *priv = ROCCAT_KEY_COMBO_BOX(user_data)->priv;
	guint group;
	guint effective;

	/* TODO hide toplevel elements if no children or children are hidden
	 * If toplevel element is hidden its children can't be restored
	 * after showing it again.
	 */
	if (gtk_tree_model_iter_has_child(model, iter))
		return TRUE;

	group = roccat_key_combo_box_model_get_group(model, iter);
	effective = priv->exclude_mask & group;
	return effective ? FALSE : TRUE;
}

static void roccat_key_combo_box_init(RoccatKeyComboBox *key_combo_box) {
	RoccatKeyComboBoxPrivate *priv = ROCCAT_KEY_COMBO_BOX_GET_PRIVATE(key_combo_box);
	key_combo_box->priv = priv;
}

static GObject *constructor(GType gtype, guint n_properties, GObjectConstructParam *properties) {
	GObject *object;
	GtkCellRenderer *cell;
	GList *cells;

#if (GTK_CHECK_VERSION(2,24,0))
	guint i;
	gchar const *name;
	GObjectConstructParam *property;

	for (i = 0, property = properties; i < n_properties; ++i, ++property) {
		name = g_param_spec_get_name(property->pspec);
		if (!strcmp(name, "has-entry")) // is G_PARAM_CONSTRUCT_ONLY
			g_value_set_boolean(property->value, TRUE);
	}
#endif

	object = G_OBJECT_CLASS(roccat_key_combo_box_parent_class)->constructor(gtype, n_properties, properties);

#if (GTK_CHECK_VERSION(2,24,0))
	g_object_set(object, "entry-text-column", TITLE_COLUMN, NULL);
#else
	g_object_set(object, "text-column", TITLE_COLUMN, NULL);
#endif

	cells = gtk_cell_layout_get_cells(GTK_CELL_LAYOUT(object));
	cell = GTK_CELL_RENDERER(cells->data);
	gtk_cell_layout_set_cell_data_func(GTK_CELL_LAYOUT(object), cell, cell_data_func, NULL, NULL);
	g_list_free(cells);

	gtk_editable_set_editable(GTK_EDITABLE(gtk_bin_get_child(GTK_BIN(object))), FALSE);

	return object;
}

void roccat_key_combo_box_set_exclude_mask(RoccatKeyComboBox *combo_box, guint exclude_mask) {
	combo_box->priv->exclude_mask = exclude_mask;
	GtkTreeModel *model;
	model = gtk_combo_box_get_model(GTK_COMBO_BOX(combo_box));
	if (model)
		gtk_tree_model_filter_refilter(GTK_TREE_MODEL_FILTER(model));
}

guint roccat_key_combo_box_get_exclude_mask(RoccatKeyComboBox *combo_box) {
	return combo_box->priv->exclude_mask;
}

static void set_property(GObject *object, guint prop_id, GValue const *value, GParamSpec *pspec) {
	RoccatKeyComboBox *combo_box = ROCCAT_KEY_COMBO_BOX(object);
	switch(prop_id) {
	case PROP_EXCLUDE_MASK:
		roccat_key_combo_box_set_exclude_mask(combo_box, g_value_get_uint(value));
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
		break;
	}
}

static void get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec) {
	RoccatKeyComboBox *combo_box = ROCCAT_KEY_COMBO_BOX(object);
	switch(prop_id) {
	case PROP_EXCLUDE_MASK:
		g_value_set_uint(value, roccat_key_combo_box_get_exclude_mask(combo_box));
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
		break;
	}
}

static void roccat_key_combo_box_class_init(RoccatKeyComboBoxClass *klass) {
	GObjectClass *gobject_class = (GObjectClass*)klass;

	gobject_class->constructor = constructor;
	gobject_class->get_property = get_property;
	gobject_class->set_property = set_property;

	g_type_class_add_private(klass, sizeof(RoccatKeyComboBoxPrivate));

	g_object_class_install_property(gobject_class, PROP_EXCLUDE_MASK,
			g_param_spec_uint("exclude-mask",
					"exclude-mask",
					"exclude-mask",
					0, G_MAXUINT, 0,
					G_PARAM_READABLE | G_PARAM_WRITABLE | G_PARAM_CONSTRUCT));

}

void roccat_key_combo_box_set_model(RoccatKeyComboBox *key_combo_box, GtkTreeModel *model) {
	GtkTreeModel *filter;

	filter = gtk_tree_model_filter_new(model, NULL);
	gtk_tree_model_filter_set_visible_func(GTK_TREE_MODEL_FILTER(filter), visible_func, key_combo_box, NULL);
	gtk_combo_box_set_model(GTK_COMBO_BOX(key_combo_box), filter);
}

void roccat_key_combo_box_set_text(RoccatKeyComboBox *key_combo_box, gchar const *text) {
	GtkEntry *entry;
	entry = GTK_ENTRY(gtk_bin_get_child(GTK_BIN(key_combo_box)));

	gtk_combo_box_set_active_iter(GTK_COMBO_BOX(key_combo_box), NULL);
	gtk_entry_set_text(entry, text);
	gtk_entry_set_width_chars(entry, g_utf8_strlen(text, -1));
}

gint roccat_key_combo_box_get_active_type(RoccatKeyComboBox *key_combo_box) {
	GtkTreeIter iter;
	gboolean valid;

	valid = gtk_combo_box_get_active_iter(GTK_COMBO_BOX(key_combo_box), &iter);
	if (!valid)
		return ROCCAT_KEY_COMBO_BOX_TYPE_NONE;

	return roccat_key_combo_box_model_get_type(gtk_combo_box_get_model(GTK_COMBO_BOX(key_combo_box)), &iter);
}

gchar *roccat_key_combo_box_model_get_title_for_type_explicit(GtkTreeModel *model, gint type) {
	GtkTreeIter iter;
	gboolean valid;

	valid = roccat_key_combo_box_model_find_type(model, type, &iter);
	if (valid)
		return roccat_key_combo_box_model_get_title(model, &iter);
	else
		return NULL;
}

gchar *roccat_key_combo_box_model_get_title_for_type(GtkTreeModel *model, gint type) {
	gchar *title;

	title = roccat_key_combo_box_model_get_title_for_type_explicit(model, type);
	if (!title)
		title = g_strdup_printf(_("FIXME (0x%02x)"), type);
	return title;
}

GtkTreeStore *roccat_key_combo_box_store_new(void) {
	return gtk_tree_store_new(N_COLUMNS, G_TYPE_STRING, G_TYPE_INT, G_TYPE_UINT);
}

void roccat_key_combo_box_store_append(GtkTreeStore *store, GtkTreeIter *iter, GtkTreeIter *parent,
		gchar const *title, gint type, gint group) {
	gtk_roccat_tree_store_append_with_values(store, iter, parent,
			TITLE_COLUMN, title,
			TYPE_COLUMN, type,
			GROUP_COLUMN, group, -1);
}
