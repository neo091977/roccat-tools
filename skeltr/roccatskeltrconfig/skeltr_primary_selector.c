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

#include "skeltr_primary_selector.h"
#include "gtk_roccat_helper.h"
#include "g_cclosure_roccat_marshaller.h"
#include "i18n.h"

#define SKELTR_PRIMARY_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), SKELTR_PRIMARY_SELECTOR_TYPE, SkeltrPrimarySelectorClass))
#define IS_SKELTR_PRIMARY_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), SKELTR_PRIMARY_SELECTOR_TYPE))
#define SKELTR_PRIMARY_SELECTOR_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), SKELTR_PRIMARY_SELECTOR_TYPE, SkeltrPrimarySelectorPrivate))

typedef struct _SkeltrPrimarySelectorClass SkeltrPrimarySelectorClass;
typedef struct _SkeltrPrimarySelectorPrivate SkeltrPrimarySelectorPrivate;

struct _SkeltrPrimarySelector {
	GtkVBox parent;
	SkeltrPrimarySelectorPrivate *priv;
};

struct _SkeltrPrimarySelectorClass {
	GtkVBoxClass parent_class;
};

struct _SkeltrPrimarySelectorPrivate {
	SkeltrKeyComboBoxData *basepointer;
	gint index;
	SkeltrKeyComboBox *standard;
};

G_DEFINE_TYPE(SkeltrPrimarySelector, skeltr_primary_selector, GTK_TYPE_VBOX);

enum {
	KEY_CHANGED,
	LAST_SIGNAL,
};

static guint signals[LAST_SIGNAL] = { 0 };

GtkWidget *skeltr_primary_selector_new(SkeltrKeyComboBoxData *basepointer) {
	SkeltrPrimarySelector *primary_selector;
	SkeltrPrimarySelectorPrivate *priv;

	primary_selector = SKELTR_PRIMARY_SELECTOR(g_object_new(SKELTR_PRIMARY_SELECTOR_TYPE,
			"homogeneous", TRUE,
			NULL));

	priv = primary_selector->priv;

	priv->basepointer = basepointer;

	return GTK_WIDGET(primary_selector);
}

static void key_changed_cb(SkeltrKeyComboBox *key_combo_box, gpointer user_data) {
	SkeltrPrimarySelector *primary_selector = SKELTR_PRIMARY_SELECTOR(user_data);
	g_signal_emit((gpointer)primary_selector, signals[KEY_CHANGED], 0, primary_selector->priv->index);
}

static void skeltr_primary_selector_init(SkeltrPrimarySelector *primary_selector) {
	SkeltrPrimarySelectorPrivate *priv = SKELTR_PRIMARY_SELECTOR_GET_PRIVATE(primary_selector);
	GtkWidget *table;

	primary_selector->priv = priv;

	priv->index = -1;

	table = gtk_table_new(1, 2, FALSE);

	priv->standard = SKELTR_KEY_COMBO_BOX(skeltr_key_combo_box_new(0, 0));

	gtk_table_attach(GTK_TABLE(table), gtk_label_new(_("Standard")), 0, 1, 0, 1, 0, GTK_EXPAND, 0, 0);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(priv->standard), 1, 2, 0, 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 0, 0);

	g_signal_connect(G_OBJECT(priv->standard), "key-changed", G_CALLBACK(key_changed_cb), primary_selector);

	gtk_box_pack_start(GTK_BOX(primary_selector), table, TRUE, TRUE, 0);
}

static void skeltr_primary_selector_class_init(SkeltrPrimarySelectorClass *klass) {
	g_type_class_add_private(klass, sizeof(SkeltrPrimarySelectorPrivate));

	signals[KEY_CHANGED] = g_signal_new("key-changed",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__INT, G_TYPE_NONE,
			1, G_TYPE_INT);
}

void skeltr_primary_selector_set_index(SkeltrPrimarySelector *selector, gint index, guint mask, guint standard) {
	SkeltrPrimarySelectorPrivate *priv = selector->priv;
	selector->priv->index = index;
	roccat_key_combo_box_set_exclude_mask(ROCCAT_KEY_COMBO_BOX(priv->standard), mask);
	skeltr_key_combo_box_set_data_pointer(priv->standard, &priv->basepointer[index]);
	skeltr_key_combo_box_set_standard(priv->standard, standard);
}

gint skeltr_primary_selector_get_index(SkeltrPrimarySelector *selector) {
	return selector->priv->index;
}

gchar *skeltr_primary_selector_get_text_for_data(SkeltrPrimarySelector *selector, SkeltrKeyComboBoxData const *data) {
	/* standard and secondary KeyComboBoxes are the same */
	return skeltr_key_combo_box_get_text_for_data(selector->priv->standard, data);
}

void skeltr_primary_selector_update(SkeltrPrimarySelector *selector) {
	skeltr_key_combo_box_update(selector->priv->standard);
}
