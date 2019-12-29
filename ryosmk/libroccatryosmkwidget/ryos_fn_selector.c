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

#include <gaminggear/hid_uid.h>
#include "ryos_fn_selector.h"
#include "ryos.h"
#include "gtk_roccat_helper.h"
#include "i18n-lib.h"

#define RYOS_FN_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), RYOS_FN_SELECTOR_TYPE, RyosFnSelectorClass))
#define IS_RYOS_FN_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), RYOS_FN_SELECTOR_TYPE))
#define RYOS_FN_SELECTOR_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), RYOS_FN_SELECTOR_TYPE, RyosFnSelectorClass))
#define RYOS_FN_SELECTOR_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), RYOS_FN_SELECTOR_TYPE, RyosFnSelectorPrivate))

typedef struct _RyosFnSelectorClass RyosFnSelectorClass;
typedef struct _RyosFnSelectorPrivate RyosFnSelectorPrivate;

struct _RyosFnSelector {
	GtkVBox parent;
	RyosFnSelectorPrivate *priv;
};

struct _RyosFnSelectorClass {
	GtkVBoxClass parent_class;
	GtkListStore *store;
};

struct _RyosFnSelectorPrivate {
	GtkComboBox *combo_box;
	gulong changed_handler_id;
};

G_DEFINE_TYPE(RyosFnSelector, ryos_fn_selector, GTK_TYPE_VBOX);

enum {
	TITLE_COLUMN,
	VALUE_COLUMN,
	N_COLUMNS,
};

enum {
	KEY_CHANGED,
	LAST_SIGNAL,
};

static guint signals[LAST_SIGNAL] = { 0 };

GtkWidget *ryos_fn_selector_new(void) {
	return GTK_WIDGET(g_object_new(RYOS_FN_SELECTOR_TYPE, NULL));
}

static void changed_cb(GtkComboBox *combo, gpointer user_data) {
	RyosFnSelector *fn_selector = RYOS_FN_SELECTOR(user_data);
	g_signal_emit((gpointer)fn_selector, signals[KEY_CHANGED], 0);
}

static void ryos_fn_selector_init(RyosFnSelector *fn_selector) {
	RyosFnSelectorPrivate *priv = RYOS_FN_SELECTOR_GET_PRIVATE(fn_selector);
	RyosFnSelectorClass *klass = RYOS_FN_SELECTOR_GET_CLASS(fn_selector);
	GtkCellRenderer *renderer;

	fn_selector->priv = priv;

	priv->combo_box = GTK_COMBO_BOX(gtk_combo_box_new_with_model(GTK_TREE_MODEL(klass->store)));
	renderer = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(priv->combo_box), renderer, TRUE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(priv->combo_box), renderer, "text", TITLE_COLUMN, NULL);
	gtk_box_pack_start(GTK_BOX(fn_selector), GTK_WIDGET(priv->combo_box), TRUE, FALSE, 0);
	priv->changed_handler_id = g_signal_connect(G_OBJECT(priv->combo_box), "changed", G_CALLBACK(changed_cb), fn_selector);
}

static GtkListStore *store_new(void) {
	GtkListStore *store;

	store = gtk_list_store_new(N_COLUMNS, G_TYPE_STRING, G_TYPE_UINT);
	gtk_roccat_list_store_append_with_values(store, NULL, TITLE_COLUMN, _("FN"), VALUE_COLUMN, RYOS_KEY_TYPE_FN, -1);
	gtk_roccat_list_store_append_with_values(store, NULL, TITLE_COLUMN, _("Right Win key"), VALUE_COLUMN, HID_UID_KB_RIGHT_GUI, -1);

	return store;
}

static void ryos_fn_selector_class_init(RyosFnSelectorClass *klass) {
	klass->store = store_new();

	g_type_class_add_private(klass, sizeof(RyosFnSelectorPrivate));

	signals[KEY_CHANGED] = g_signal_new("key-changed",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);
}

void ryos_fn_selector_set_value(RyosFnSelector *fn_selector, guint value) {
	RyosFnSelectorPrivate *priv = fn_selector->priv;
	GtkTreeIter iter;
	gboolean valid;

	valid = gtk_roccat_tree_model_iter_find_int(gtk_combo_box_get_model(priv->combo_box), VALUE_COLUMN, value, &iter, NULL);
	if (valid)
		gtk_combo_box_set_active_iter(priv->combo_box, &iter);
}

guint ryos_fn_selector_get_value(RyosFnSelector *fn_selector) {
	RyosFnSelectorPrivate *priv = fn_selector->priv;
	GtkTreeIter iter;
	gboolean valid;
	guint value;

	valid = gtk_combo_box_get_active_iter(priv->combo_box, &iter);
	if (!valid)
		return 0;

	gtk_tree_model_get(gtk_combo_box_get_model(priv->combo_box), &iter, VALUE_COLUMN, &value, -1);
	return value;
}

gchar *ryos_fn_selector_get_text_for_value(RyosFnSelector *fn_selector, guint value) {
	GtkTreeIter iter;
	gboolean valid;
	GtkTreeModel *model;
	gchar *text;

	model = gtk_combo_box_get_model(fn_selector->priv->combo_box);

	valid = gtk_roccat_tree_model_iter_find_int(model, VALUE_COLUMN, value, &iter, NULL);
	if (!valid)
		return NULL;

	gtk_tree_model_get(model, &iter, TITLE_COLUMN, &text, -1);
	return text;
}
