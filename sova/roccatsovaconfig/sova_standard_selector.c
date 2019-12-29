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

#include "sova_standard_selector.h"
#include "gtk_roccat_helper.h"
#include "g_cclosure_roccat_marshaller.h"
#include "i18n.h"

#define SOVA_STANDARD_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), SOVA_STANDARD_SELECTOR_TYPE, SovaStandardSelectorClass))
#define IS_SOVA_STANDARD_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), SOVA_STANDARD_SELECTOR_TYPE))
#define SOVA_STANDARD_SELECTOR_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), SOVA_STANDARD_SELECTOR_TYPE, SovaStandardSelectorPrivate))

typedef struct _SovaStandardSelectorClass SovaStandardSelectorClass;
typedef struct _SovaStandardSelectorPrivate SovaStandardSelectorPrivate;

struct _SovaStandardSelector {
	GtkVBox parent;
	SovaStandardSelectorPrivate *priv;
};

struct _SovaStandardSelectorClass {
	GtkVBoxClass parent_class;
};

struct _SovaStandardSelectorPrivate {
	SovaKeyComboBoxData *standard_basepointer;
	SovaKeyComboBoxData *secondary_basepointer;
	gint index;
	SovaKeyComboBox *standard;
	SovaKeyComboBox *secondary;
	GtkLabel *secondary_label;
};

G_DEFINE_TYPE(SovaStandardSelector, sova_standard_selector, GTK_TYPE_VBOX);

enum {
	KEY_CHANGED,
	LAST_SIGNAL,
};

static guint signals[LAST_SIGNAL] = { 0 };

GtkWidget *sova_standard_selector_new(SovaKeyComboBoxData *standard_basepointer, SovaKeyComboBoxData *secondary_basepointer) {
	SovaStandardSelector *standard_selector;
	SovaStandardSelectorPrivate *priv;

	standard_selector = SOVA_STANDARD_SELECTOR(g_object_new(SOVA_STANDARD_SELECTOR_TYPE,
			"homogeneous", TRUE,
			NULL));

	priv = standard_selector->priv;

	priv->standard_basepointer = standard_basepointer;
	priv->secondary_basepointer = secondary_basepointer;

	return GTK_WIDGET(standard_selector);
}

static void key_changed_cb(SovaKeyComboBox *key_combo_box, gpointer user_data) {
	SovaStandardSelector *standard_selector = SOVA_STANDARD_SELECTOR(user_data);
	g_signal_emit((gpointer)standard_selector, signals[KEY_CHANGED], 0, standard_selector->priv->index);
}

static void sova_standard_selector_init(SovaStandardSelector *standard_selector) {
	SovaStandardSelectorPrivate *priv = SOVA_STANDARD_SELECTOR_GET_PRIVATE(standard_selector);
	GtkWidget *table;

	standard_selector->priv = priv;

	priv->index = -1;

	table = gtk_table_new(2, 2, FALSE);

	priv->standard = SOVA_KEY_COMBO_BOX(sova_key_combo_box_new(0, 0));
	priv->secondary = SOVA_KEY_COMBO_BOX(sova_key_combo_box_new(0, 0));
	priv->secondary_label = GTK_LABEL(gtk_label_new(NULL));

	gtk_table_attach(GTK_TABLE(table), gtk_label_new(_("Standard")), 0, 1, 0, 1, 0, GTK_EXPAND, 0, 0);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(priv->standard), 1, 2, 0, 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 0, 0);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(priv->secondary_label), 0, 1, 1, 2, 0, GTK_EXPAND, 0, 0);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(priv->secondary), 1, 2, 1, 2, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 0, 0);

	g_signal_connect(G_OBJECT(priv->standard), "key-changed", G_CALLBACK(key_changed_cb), standard_selector);
	g_signal_connect(G_OBJECT(priv->secondary), "key-changed", G_CALLBACK(key_changed_cb), standard_selector);

	gtk_box_pack_start(GTK_BOX(standard_selector), table, TRUE, TRUE, 0);
}

static void sova_standard_selector_class_init(SovaStandardSelectorClass *klass) {
	g_type_class_add_private(klass, sizeof(SovaStandardSelectorPrivate));

	signals[KEY_CHANGED] = g_signal_new("key-changed",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__INT, G_TYPE_NONE,
			1, G_TYPE_INT);
}

void sova_standard_selector_set_index(SovaStandardSelector *selector, gint index, guint standard_mask, guint standard_standard, guint secondary_mask, guint secondary_standard) {
	SovaStandardSelectorPrivate *priv = selector->priv;
	selector->priv->index = index;
	roccat_key_combo_box_set_exclude_mask(ROCCAT_KEY_COMBO_BOX(priv->standard), standard_mask);
	roccat_key_combo_box_set_exclude_mask(ROCCAT_KEY_COMBO_BOX(priv->secondary), secondary_mask);
	sova_key_combo_box_set_data_pointer(priv->standard, &priv->standard_basepointer[index]);
	sova_key_combo_box_set_data_pointer(priv->secondary, &priv->secondary_basepointer[index]);
	sova_key_combo_box_set_standard(priv->standard, standard_standard);
	sova_key_combo_box_set_standard(priv->secondary, secondary_standard);
}

gint sova_standard_selector_get_index(SovaStandardSelector *selector) {
	return selector->priv->index;
}

void sova_standard_selector_set_secondary_text(SovaStandardSelector *selector, gchar const *text) {
	gtk_label_set_text(selector->priv->secondary_label, text);
}

gchar *sova_standard_selector_get_text_for_data(SovaStandardSelector *selector, SovaKeyComboBoxData const *data) {
	/* standard and secondary KeyComboBoxes are the same */
	return sova_key_combo_box_get_text_for_data(selector->priv->standard, data);
}

void sova_standard_selector_update(SovaStandardSelector *selector) {
	sova_key_combo_box_update(selector->priv->standard);
	sova_key_combo_box_update(selector->priv->secondary);
}
