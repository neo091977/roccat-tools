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

#include <gaminggear/gaminggear_macro_dialog.h>
#include "ryosconfig_pro_dimness_selector.h"
#include "gtk_roccat_helper.h"
#include "roccat_warning_dialog.h"
#include "i18n.h"
#include <string.h>

#define RYOSCONFIG_PRO_DIMNESS_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), RYOSCONFIG_PRO_DIMNESS_SELECTOR_TYPE, RyosconfigProDimnessSelectorClass))
#define IS_RYOSCONFIG_PRO_DIMNESS_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), RYOSCONFIG_PRO_DIMNESS_SELECTOR_TYPE))
#define RYOSCONFIG_PRO_DIMNESS_SELECTOR_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), RYOSCONFIG_PRO_DIMNESS_SELECTOR_TYPE, RyosconfigProDimnessSelectorClass))
#define RYOSCONFIG_PRO_DIMNESS_SELECTOR_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), RYOSCONFIG_PRO_DIMNESS_SELECTOR_TYPE, RyosconfigProDimnessSelectorPrivate))

typedef struct _RyosconfigProDimnessSelectorClass RyosconfigProDimnessSelectorClass;
typedef struct _RyosconfigProDimnessSelectorPrivate RyosconfigProDimnessSelectorPrivate;

struct _RyosconfigProDimnessSelector {
#if (GTK_CHECK_VERSION(2,24,0))
	GtkComboBox parent;
#else
	GtkComboBoxEntry parent;
#endif
	RyosconfigProDimnessSelectorPrivate *priv;
};

struct _RyosconfigProDimnessSelectorClass {
#if (GTK_CHECK_VERSION(2,24,0))
	GtkComboBoxClass parent_class;
#else
	GtkComboBoxEntryClass parent_class;
#endif
	GtkListStore *store;
};

struct _RyosconfigProDimnessSelectorPrivate {
	gboolean is_pro;
	guint dimness_type;
	gulong changed_handler_id;
	RyosMacro *light_macro;
};

enum {
	TITLE_COLUMN,
	TYPE_COLUMN,
	GROUP_COLUMN,
	N_COLUMNS
};

enum {
	GROUP_ALL,
	GROUP_PRO,
};

#if (GTK_CHECK_VERSION(2,24,0))
G_DEFINE_TYPE(RyosconfigProDimnessSelector, ryosconfig_pro_dimness_selector, GTK_TYPE_COMBO_BOX);
#else
G_DEFINE_TYPE(RyosconfigProDimnessSelector, ryosconfig_pro_dimness_selector, GTK_TYPE_COMBO_BOX_ENTRY);
#endif

static gboolean find_type(GtkTreeModel *model, gint type, GtkTreeIter *iter) {
	return gtk_roccat_tree_model_iter_find_int(model, TYPE_COLUMN, type, iter, NULL);
}

static gchar *get_title(GtkTreeModel *model, GtkTreeIter *iter) {
	gchar *title;
	gtk_tree_model_get(model, iter, TITLE_COLUMN, &title, -1);
	return title;
}

static guint get_group(GtkTreeModel *model, GtkTreeIter *iter) {
	guint group;
	gtk_tree_model_get(model, iter, GROUP_COLUMN, &group, -1);
	return group;
}

static gint get_type(GtkTreeModel *model, GtkTreeIter *iter) {
	gint type;
	gtk_tree_model_get(model, iter, TYPE_COLUMN, &type, -1);
	return type;
}

static gint get_active_type(RyosconfigProDimnessSelector *dimness_selector) {
	GtkComboBox *combo_box;
	GtkTreeIter iter;
	gboolean valid;

	combo_box = GTK_COMBO_BOX(dimness_selector);
	valid = gtk_combo_box_get_active_iter(combo_box, &iter);
	if (!valid)
		return RYOS_LIGHT_DIMNESS_TYPE_VALUE;

	return get_type(gtk_combo_box_get_model(combo_box), &iter);
}

static gchar *get_text_for_data(RyosconfigProDimnessSelector *dimness_selector) {
	RyosconfigProDimnessSelectorPrivate *priv = dimness_selector->priv;
	GtkTreeIter iter;
	gboolean valid;
	guint type;
	GtkTreeModel *model;
	gchar *text;

	type = priv->dimness_type;

	switch (type) {
	case RYOS_LIGHT_DIMNESS_TYPE_MACRO:
		text = g_strdup_printf(_("Macro: %s/%s"), priv->light_macro->macroset_name, priv->light_macro->macro_name);
		break;
	default:
		model = gtk_combo_box_get_model(GTK_COMBO_BOX(dimness_selector));
		valid = find_type(model, type, &iter);
		if (valid)
			text = get_title(model, &iter);
		else
			text = g_strdup_printf(_("FIXME (0x%02x)"), type);
		break;
	}

	return text;
}

static void set_text(RyosconfigProDimnessSelector *dimness_selector, gchar const *text) {
	GtkEntry *entry;
	entry = GTK_ENTRY(gtk_bin_get_child(GTK_BIN(dimness_selector)));

	gtk_combo_box_set_active_iter(GTK_COMBO_BOX(dimness_selector), NULL);
	gtk_entry_set_text(entry, text);
	gtk_entry_set_width_chars(entry, g_utf8_strlen(text, -1));
}

static void update(RyosconfigProDimnessSelector *dimness_selector) {
	RyosconfigProDimnessSelectorPrivate *priv = dimness_selector->priv;
	gchar *text;

	text = get_text_for_data(dimness_selector);

	g_signal_handler_block(G_OBJECT(dimness_selector), priv->changed_handler_id);
	set_text(dimness_selector, text);
	g_signal_handler_unblock(G_OBJECT(dimness_selector), priv->changed_handler_id);

	g_free(text);

}

static guint macro(RyosconfigProDimnessSelector *dimness_selector) {
	RyosconfigProDimnessSelectorPrivate *priv = dimness_selector->priv;
	GError *local_error = NULL;
	RyosMacro *ryos_macro;
	GaminggearMacro *gaminggear_macro;
	GtkWindow *parent;
	guint type;
	gboolean result;

	/* save previous type for cancel action */
	type = priv->dimness_type;
	parent = gtk_roccat_widget_get_gtk_window(GTK_WIDGET(dimness_selector));

	gaminggear_macro = gaminggear_macro_dialog(parent);
	if (!gaminggear_macro)
		return type;

	ryos_macro = (RyosMacro *)g_malloc0(sizeof(RyosMacro));
	result = gaminggear_macro_to_ryos_macro(gaminggear_macro, ryos_macro, &local_error);
	gaminggear_macro_free(gaminggear_macro);
	if (!result) {
		roccat_handle_error_dialog(parent, _("Could not convert macro"), &local_error);
		g_free(ryos_macro);
		return type;
	}

	g_free(priv->light_macro);
	priv->light_macro = ryos_macro;
	type = RYOS_LIGHT_DIMNESS_TYPE_MACRO;

	return type;
}

static void changed_cb(GtkComboBox *combo, gpointer user_data) {
	RyosconfigProDimnessSelector *dimness_selector = RYOSCONFIG_PRO_DIMNESS_SELECTOR(combo);
	RyosconfigProDimnessSelectorPrivate *priv = dimness_selector->priv;
	guint type;

	type = get_active_type(dimness_selector);

	switch (type) {
	case RYOS_LIGHT_DIMNESS_TYPE_MACRO:
		type = macro(dimness_selector);
		break;
	default:
		break;
	}

	priv->dimness_type = type;
	update(dimness_selector);
}

GtkWidget *ryosconfig_pro_dimness_selector_new(void) {
	return GTK_WIDGET(g_object_new(RYOSCONFIG_PRO_DIMNESS_SELECTOR_TYPE, NULL));
}

static gboolean visible_func(GtkTreeModel *model, GtkTreeIter *iter, gpointer user_data) {
	RyosconfigProDimnessSelector *dimness_selector = RYOSCONFIG_PRO_DIMNESS_SELECTOR(user_data);
	RyosconfigProDimnessSelectorPrivate *priv = dimness_selector->priv;
	guint group;

	if (priv->is_pro)
		return TRUE;

	group = get_group(model, iter);
	if (group == GROUP_PRO)
		return FALSE;
	else
		return TRUE;
}

static void ryosconfig_pro_dimness_selector_init(RyosconfigProDimnessSelector *dimness_selector) {
	RyosconfigProDimnessSelectorPrivate *priv = RYOSCONFIG_PRO_DIMNESS_SELECTOR_GET_PRIVATE(dimness_selector);
	RyosconfigProDimnessSelectorClass *klass = RYOSCONFIG_PRO_DIMNESS_SELECTOR_GET_CLASS(dimness_selector);
	GtkTreeModel *filter;

	dimness_selector->priv = priv;

	filter = gtk_tree_model_filter_new(GTK_TREE_MODEL(klass->store), NULL);
	gtk_tree_model_filter_set_visible_func(GTK_TREE_MODEL_FILTER(filter), visible_func, dimness_selector, NULL);
	gtk_combo_box_set_model(GTK_COMBO_BOX(dimness_selector), filter);

	priv->changed_handler_id = g_signal_connect(G_OBJECT(dimness_selector), "changed", G_CALLBACK(changed_cb), NULL);
	priv->is_pro = TRUE; /* max features on startup */
	priv->light_macro = NULL;
}

static GObject *constructor(GType gtype, guint n_properties, GObjectConstructParam *properties) {
	GObject *object;

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

	object = G_OBJECT_CLASS(ryosconfig_pro_dimness_selector_parent_class)->constructor(gtype, n_properties, properties);

#if (GTK_CHECK_VERSION(2,24,0))
	g_object_set(object, "entry-text-column", TITLE_COLUMN, NULL);
#else
	g_object_set(object, "text-column", TITLE_COLUMN, NULL);
#endif

	gtk_editable_set_editable(GTK_EDITABLE(gtk_bin_get_child(GTK_BIN(object))), FALSE);

	return object;
}

static void finalize(GObject *object) {
	RyosconfigProDimnessSelectorPrivate *priv = RYOSCONFIG_PRO_DIMNESS_SELECTOR(object)->priv;
	g_free(priv->light_macro);
	G_OBJECT_CLASS(ryosconfig_pro_dimness_selector_parent_class)->finalize(object);
}

static void ryosconfig_pro_dimness_selector_class_init(RyosconfigProDimnessSelectorClass *klass) {
	GObjectClass *gobject_class;
	GtkTreeIter iter;

	gobject_class = G_OBJECT_CLASS(klass);

	gobject_class->constructor = constructor;
	gobject_class->finalize = finalize;

	g_type_class_add_private(klass, sizeof(RyosconfigProDimnessSelectorPrivate));

	klass->store = gtk_list_store_new(N_COLUMNS, G_TYPE_STRING, G_TYPE_UINT, G_TYPE_UINT);

	gtk_roccat_list_store_append_with_values(klass->store, &iter,
				TITLE_COLUMN, _("Dimness"),
				TYPE_COLUMN, RYOS_LIGHT_DIMNESS_TYPE_VALUE,
				GROUP_COLUMN, GROUP_ALL, -1);
	gtk_roccat_list_store_append_with_values(klass->store, &iter,
				TITLE_COLUMN, _("Starlit sky"),
				TYPE_COLUMN, RYOS_LIGHT_DIMNESS_TYPE_STARLIT_SKY,
				GROUP_COLUMN, GROUP_PRO, -1);
	gtk_roccat_list_store_append_with_values(klass->store, &iter,
				TITLE_COLUMN, _("Fall asleep"),
				TYPE_COLUMN, RYOS_LIGHT_DIMNESS_TYPE_FALL_ASLEEP,
				GROUP_COLUMN, GROUP_PRO, -1);
	gtk_roccat_list_store_append_with_values(klass->store, &iter,
				TITLE_COLUMN, _("Macro"),
				TYPE_COLUMN, RYOS_LIGHT_DIMNESS_TYPE_MACRO,
				GROUP_COLUMN, GROUP_PRO, -1);
}

void ryosconfig_pro_dimness_selector_set_device_type(RyosconfigProDimnessSelector *dimness_selector, RoccatDevice const *device) {
	RyosconfigProDimnessSelectorPrivate *priv = dimness_selector->priv;
	GtkTreeModel *model;

	switch (gaminggear_device_get_product_id(GAMINGGEAR_DEVICE(device))) {
	case USB_DEVICE_ID_ROCCAT_RYOS_MK_PRO:
		priv->is_pro = TRUE;
		break;
	case USB_DEVICE_ID_ROCCAT_RYOS_MK:
	case USB_DEVICE_ID_ROCCAT_RYOS_MK_GLOW:
	default:
		priv->is_pro = FALSE;
		break;
	}

	model = gtk_combo_box_get_model(GTK_COMBO_BOX(dimness_selector));
	gtk_tree_model_filter_refilter(GTK_TREE_MODEL_FILTER(model));
}

void ryosconfig_pro_dimness_selector_set_from_profile_data(RyosconfigProDimnessSelector *dimness_selector, RyosProfileData const *profile_data) {
	RyosconfigProDimnessSelectorPrivate *priv = dimness_selector->priv;

	priv->dimness_type = profile_data->hardware.light.dimness_type;

	g_free(priv->light_macro);
	priv->light_macro = ryos_macro_dup(&profile_data->hardware.light_macro);

	update(dimness_selector);
}

void ryosconfig_pro_dimness_selector_update_profile_data(RyosconfigProDimnessSelector *dimness_selector, RyosProfileData *profile_data) {
	RyosconfigProDimnessSelectorPrivate *priv = dimness_selector->priv;

	if (profile_data->hardware.light.dimness_type != priv->dimness_type) {
		profile_data->hardware.light.dimness_type = priv->dimness_type;
		profile_data->hardware.modified_light = TRUE;
	}
	ryos_profile_data_hardware_set_light_macro(&profile_data->hardware, priv->light_macro);
}
