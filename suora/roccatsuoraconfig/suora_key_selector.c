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

#include "suora_key_selector.h"
#include "gtk_roccat_helper.h"
#include "g_cclosure_roccat_marshaller.h"
#include "i18n.h"

#define SUORA_KEY_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), SUORA_KEY_SELECTOR_TYPE, SuoraKeySelectorClass))
#define IS_SUORA_KEY_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), SUORA_KEY_SELECTOR_TYPE))
#define SUORA_KEY_SELECTOR_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), SUORA_KEY_SELECTOR_TYPE, SuoraKeySelectorPrivate))

typedef struct _SuoraKeySelectorClass SuoraKeySelectorClass;
typedef struct _SuoraKeySelectorPrivate SuoraKeySelectorPrivate;

struct _SuoraKeySelector {
	GtkVBox parent;
	SuoraKeySelectorPrivate *priv;
};

struct _SuoraKeySelectorClass {
	GtkVBoxClass parent_class;
};

struct _SuoraKeySelectorPrivate {
	SuoraKeyComboBoxData *basepointer;
	gint index;
	SuoraKeyComboBox *standard;
};

G_DEFINE_TYPE(SuoraKeySelector, suora_key_selector, GTK_TYPE_VBOX);

enum {
	KEY_CHANGED,
	LAST_SIGNAL,
};

static guint signals[LAST_SIGNAL] = { 0 };

GtkWidget *suora_key_selector_new(SuoraKeyComboBoxData *basepointer) {
	SuoraKeySelector *key_selector;
	SuoraKeySelectorPrivate *priv;

	key_selector = SUORA_KEY_SELECTOR(g_object_new(SUORA_KEY_SELECTOR_TYPE,
			"homogeneous", TRUE,
			NULL));

	priv = key_selector->priv;

	priv->basepointer = basepointer;

	return GTK_WIDGET(key_selector);
}

static void key_changed_cb(SuoraKeyComboBox *key_combo_box, gpointer user_data) {
	SuoraKeySelector *key_selector = SUORA_KEY_SELECTOR(user_data);
	g_signal_emit((gpointer)key_selector, signals[KEY_CHANGED], 0, key_selector->priv->index);
}

static void suora_key_selector_init(SuoraKeySelector *key_selector) {
	SuoraKeySelectorPrivate *priv = SUORA_KEY_SELECTOR_GET_PRIVATE(key_selector);

	key_selector->priv = priv;

	priv->index = -1;
	priv->standard = SUORA_KEY_COMBO_BOX(suora_key_combo_box_new(0, 0));

	g_signal_connect(G_OBJECT(priv->standard), "key-changed", G_CALLBACK(key_changed_cb), key_selector);

	gtk_box_pack_start(GTK_BOX(key_selector), GTK_WIDGET(priv->standard), TRUE, TRUE, 0);
}

static void suora_key_selector_class_init(SuoraKeySelectorClass *klass) {
	g_type_class_add_private(klass, sizeof(SuoraKeySelectorPrivate));

	signals[KEY_CHANGED] = g_signal_new("key-changed",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__INT, G_TYPE_NONE,
			1, G_TYPE_INT);
}

void suora_key_selector_set_index(SuoraKeySelector *selector, gint index, guint mask, guint standard) {
	SuoraKeySelectorPrivate *priv = selector->priv;
	selector->priv->index = index;
	roccat_key_combo_box_set_exclude_mask(ROCCAT_KEY_COMBO_BOX(priv->standard), mask);
	suora_key_combo_box_set_data_pointer(priv->standard, &priv->basepointer[index]);
	suora_key_combo_box_set_standard(priv->standard, standard);
}

gint suora_key_selector_get_index(SuoraKeySelector *selector) {
	return selector->priv->index;
}

gchar *suora_key_selector_get_text_for_data(SuoraKeySelector *selector, SuoraKeyComboBoxData const *data) {
	/* standard and secondary KeyComboBoxes are the same */
	return suora_key_combo_box_get_text_for_data(selector->priv->standard, data);
}

void suora_key_selector_update(SuoraKeySelector *selector) {
	suora_key_combo_box_update(selector->priv->standard);
}
