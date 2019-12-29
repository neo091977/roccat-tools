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

#include "sova_primary_selector.h"
#include "gtk_roccat_helper.h"
#include "g_cclosure_roccat_marshaller.h"
#include "i18n.h"

#define SOVA_PRIMARY_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), SOVA_PRIMARY_SELECTOR_TYPE, SovaPrimarySelectorClass))
#define IS_SOVA_PRIMARY_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), SOVA_PRIMARY_SELECTOR_TYPE))
#define SOVA_PRIMARY_SELECTOR_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), SOVA_PRIMARY_SELECTOR_TYPE, SovaPrimarySelectorPrivate))

typedef struct _SovaPrimarySelectorClass SovaPrimarySelectorClass;
typedef struct _SovaPrimarySelectorPrivate SovaPrimarySelectorPrivate;

struct _SovaPrimarySelector {
	GtkVBox parent;
	SovaPrimarySelectorPrivate *priv;
};

struct _SovaPrimarySelectorClass {
	GtkVBoxClass parent_class;
};

struct _SovaPrimarySelectorPrivate {
	SovaKeyComboBoxData *basepointer;
	gint index;
	SovaKeyComboBox *standard;
};

G_DEFINE_TYPE(SovaPrimarySelector, sova_primary_selector, GTK_TYPE_VBOX);

enum {
	KEY_CHANGED,
	LAST_SIGNAL,
};

static guint signals[LAST_SIGNAL] = { 0 };

GtkWidget *sova_primary_selector_new(SovaKeyComboBoxData *basepointer) {
	SovaPrimarySelector *primary_selector;
	SovaPrimarySelectorPrivate *priv;

	primary_selector = SOVA_PRIMARY_SELECTOR(g_object_new(SOVA_PRIMARY_SELECTOR_TYPE,
			"homogeneous", TRUE,
			NULL));

	priv = primary_selector->priv;

	priv->basepointer = basepointer;

	return GTK_WIDGET(primary_selector);
}

static void key_changed_cb(SovaKeyComboBox *key_combo_box, gpointer user_data) {
	SovaPrimarySelector *primary_selector = SOVA_PRIMARY_SELECTOR(user_data);
	g_signal_emit((gpointer)primary_selector, signals[KEY_CHANGED], 0, primary_selector->priv->index);
}

static void sova_primary_selector_init(SovaPrimarySelector *primary_selector) {
	SovaPrimarySelectorPrivate *priv = SOVA_PRIMARY_SELECTOR_GET_PRIVATE(primary_selector);
	GtkWidget *table;

	primary_selector->priv = priv;

	priv->index = -1;

	table = gtk_table_new(1, 2, FALSE);

	priv->standard = SOVA_KEY_COMBO_BOX(sova_key_combo_box_new(0, 0));

	gtk_table_attach(GTK_TABLE(table), gtk_label_new(_("Standard")), 0, 1, 0, 1, 0, GTK_EXPAND, 0, 0);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(priv->standard), 1, 2, 0, 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 0, 0);

	g_signal_connect(G_OBJECT(priv->standard), "key-changed", G_CALLBACK(key_changed_cb), primary_selector);

	gtk_box_pack_start(GTK_BOX(primary_selector), table, TRUE, TRUE, 0);
}

static void sova_primary_selector_class_init(SovaPrimarySelectorClass *klass) {
	g_type_class_add_private(klass, sizeof(SovaPrimarySelectorPrivate));

	signals[KEY_CHANGED] = g_signal_new("key-changed",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__INT, G_TYPE_NONE,
			1, G_TYPE_INT);
}

void sova_primary_selector_set_index(SovaPrimarySelector *selector, gint index, guint mask, guint standard) {
	SovaPrimarySelectorPrivate *priv = selector->priv;
	selector->priv->index = index;
	roccat_key_combo_box_set_exclude_mask(ROCCAT_KEY_COMBO_BOX(priv->standard), mask);
	sova_key_combo_box_set_data_pointer(priv->standard, &priv->basepointer[index]);
	sova_key_combo_box_set_standard(priv->standard, standard);
}

gint sova_primary_selector_get_index(SovaPrimarySelector *selector) {
	return selector->priv->index;
}

gchar *sova_primary_selector_get_text_for_data(SovaPrimarySelector *selector, SovaKeyComboBoxData const *data) {
	/* standard and secondary KeyComboBoxes are the same */
	return sova_key_combo_box_get_text_for_data(selector->priv->standard, data);
}

void sova_primary_selector_update(SovaPrimarySelector *selector) {
	sova_key_combo_box_update(selector->priv->standard);
}
