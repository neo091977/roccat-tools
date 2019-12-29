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
#include "skeltr_capslock_selector.h"
#include "gtk_roccat_helper.h"

#define SKELTR_CAPSLOCK_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), SKELTR_CAPSLOCK_SELECTOR_TYPE, SkeltrCapslockSelectorClass))
#define IS_SKELTR_CAPSLOCK_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), SKELTR_CAPSLOCK_SELECTOR_TYPE))
#define SKELTR_CAPSLOCK_SELECTOR_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), SKELTR_CAPSLOCK_SELECTOR_TYPE, SkeltrCapslockSelectorClass))
#define SKELTR_CAPSLOCK_SELECTOR_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), SKELTR_CAPSLOCK_SELECTOR_TYPE, SkeltrCapslockSelectorPrivate))

typedef struct _SkeltrCapslockSelectorClass SkeltrCapslockSelectorClass;
typedef struct _SkeltrCapslockSelectorPrivate SkeltrCapslockSelectorPrivate;

struct _SkeltrCapslockSelector {
	GtkVBox parent;
	SkeltrCapslockSelectorPrivate *priv;
};

struct _SkeltrCapslockSelectorClass {
	GtkVBoxClass parent_class;
};

struct _SkeltrCapslockSelectorPrivate {
	SkeltrKeyComboBoxData *basepointer;
	SkeltrKeyComboBox *combo_box;
	gint index;
};

G_DEFINE_TYPE(SkeltrCapslockSelector, skeltr_capslock_selector, GTK_TYPE_VBOX);

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

GtkWidget *skeltr_capslock_selector_new(SkeltrKeyComboBoxData *basepointer) {
	SkeltrCapslockSelector *capslock_selector;
	SkeltrCapslockSelectorPrivate *priv;

	capslock_selector = SKELTR_CAPSLOCK_SELECTOR(g_object_new(SKELTR_CAPSLOCK_SELECTOR_TYPE, NULL));
	priv = capslock_selector->priv;
	priv->basepointer = basepointer;
	return GTK_WIDGET(capslock_selector);
}

static void key_changed_cb(GtkComboBox *combo, gpointer user_data) {
	SkeltrCapslockSelector *capslock_selector = SKELTR_CAPSLOCK_SELECTOR(user_data);
	g_signal_emit((gpointer)capslock_selector, signals[KEY_CHANGED], 0, capslock_selector->priv->index);
}

static void skeltr_capslock_selector_init(SkeltrCapslockSelector *capslock_selector) {
	SkeltrCapslockSelectorPrivate *priv = SKELTR_CAPSLOCK_SELECTOR_GET_PRIVATE(capslock_selector);

	capslock_selector->priv = priv;

	priv->combo_box = SKELTR_KEY_COMBO_BOX(skeltr_key_combo_box_new(0, 0));
	gtk_box_pack_start(GTK_BOX(capslock_selector), GTK_WIDGET(priv->combo_box), TRUE, FALSE, 0);
	g_signal_connect(G_OBJECT(priv->combo_box), "key-changed", G_CALLBACK(key_changed_cb), capslock_selector);
}

static void skeltr_capslock_selector_class_init(SkeltrCapslockSelectorClass *klass) {
	g_type_class_add_private(klass, sizeof(SkeltrCapslockSelectorPrivate));

	signals[KEY_CHANGED] = g_signal_new("key-changed",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__INT, G_TYPE_NONE,
			1, G_TYPE_INT);
}

void skeltr_capslock_selector_set_index(SkeltrCapslockSelector *selector, gint index, guint mask, guint standard) {
	SkeltrCapslockSelectorPrivate *priv = selector->priv;
	selector->priv->index = index;
	roccat_key_combo_box_set_exclude_mask(ROCCAT_KEY_COMBO_BOX(priv->combo_box), mask);
	skeltr_key_combo_box_set_data_pointer(priv->combo_box, &priv->basepointer[index]);
	skeltr_key_combo_box_set_standard(priv->combo_box, standard);
}

gint skeltr_capslock_selector_get_index(SkeltrCapslockSelector *selector) {
	return selector->priv->index;
}

gchar *skeltr_capslock_selector_get_text_for_data(SkeltrCapslockSelector *selector, SkeltrKeyComboBoxData const *data) {
	return skeltr_key_combo_box_get_text_for_data(selector->priv->combo_box, data);
}

void skeltr_capslock_selector_update(SkeltrCapslockSelector *selector) {
	skeltr_key_combo_box_update(selector->priv->combo_box);
}
