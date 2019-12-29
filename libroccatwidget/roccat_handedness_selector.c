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

#include "roccat_handedness_selector.h"
#include "i18n-lib.h"

#define ROCCAT_HANDEDNESS_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), ROCCAT_HANDEDNESS_SELECTOR_TYPE, RoccatHandednessSelectorClass))
#define IS_ROCCAT_HANDEDNESS_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), ROCCAT_HANDEDNESS_SELECTOR_TYPE))
#define ROCCAT_HANDEDNESS_SELECTOR_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), ROCCAT_HANDEDNESS_SELECTOR_TYPE, RoccatHandednessSelectorPrivate))

struct _RoccatHandednessSelectorPrivate {
	GtkRadioButton *left;
	GtkRadioButton *right;
};

enum {
	TOGGLED,
	LAST_SIGNAL,
};

static guint signals[LAST_SIGNAL] = { 0 };

enum {
	PROP_0,
	PROP_VALUE,
};

G_DEFINE_TYPE(RoccatHandednessSelector, roccat_handedness_selector, GTK_TYPE_FRAME);

static void left_toggled_cb(GtkToggleButton *togglebutton, gpointer user_data) {
	g_signal_emit((gpointer)user_data, signals[TOGGLED], 0);
}

guint roccat_handedness_selector_get_value(RoccatHandednessSelector *selector) {
	RoccatHandednessSelectorPrivate *priv = selector->priv;

	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(priv->left)))
		return ROCCAT_HANDEDNESS_SELECTOR_LEFT;
	else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(priv->right)))
		return ROCCAT_HANDEDNESS_SELECTOR_RIGHT;
	else
		return ROCCAT_HANDEDNESS_SELECTOR_NONE;
}

void roccat_handedness_selector_set_value(RoccatHandednessSelector *selector, guint new_value) {
	RoccatHandednessSelectorPrivate *priv = selector->priv;

	switch (new_value) {
	case ROCCAT_HANDEDNESS_SELECTOR_LEFT:
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(priv->left), TRUE);
		break;
	case ROCCAT_HANDEDNESS_SELECTOR_RIGHT:
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(priv->right), TRUE);
		break;
	default:
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(priv->left), FALSE);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(priv->right), FALSE);
		break;
	}
}

void roccat_handedness_selector_set_value_blocked(RoccatHandednessSelector *selector, guint new_value) {
	g_signal_handlers_block_by_func(G_OBJECT(selector->priv->left), left_toggled_cb, selector);
	roccat_handedness_selector_set_value(selector, new_value);
	g_signal_handlers_unblock_by_func(G_OBJECT(selector->priv->left), left_toggled_cb, selector);
}

GtkWidget *roccat_handedness_selector_new(void) {
	RoccatHandednessSelector *selector;

	selector = g_object_new(ROCCAT_HANDEDNESS_SELECTOR_TYPE, NULL);

	return GTK_WIDGET(selector);
}

static void roccat_handedness_selector_set_property(GObject *object, guint prop_id, GValue const *value, GParamSpec *pspec) {
	RoccatHandednessSelector *selector = ROCCAT_HANDEDNESS_SELECTOR(object);

	switch(prop_id) {
	case PROP_VALUE:
		roccat_handedness_selector_set_value(selector, g_value_get_uint(value));
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
	}
}

static void roccat_handedness_selector_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec) {
	RoccatHandednessSelector *selector = ROCCAT_HANDEDNESS_SELECTOR(object);

	switch(prop_id) {
	case PROP_VALUE:
		g_value_set_uint(value, roccat_handedness_selector_get_value(selector));
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
	}
}

static void roccat_handedness_selector_init(RoccatHandednessSelector *selector) {
	RoccatHandednessSelectorPrivate *priv = ROCCAT_HANDEDNESS_SELECTOR_GET_PRIVATE(selector);
	GtkWidget *hbox;

	selector->priv = priv;

	hbox = gtk_hbox_new(TRUE, 0);
	gtk_container_add(GTK_CONTAINER(selector), hbox);

	priv->left = GTK_RADIO_BUTTON(gtk_radio_button_new_with_label(NULL, _("Left handed")));
	gtk_box_pack_start(GTK_BOX(hbox), GTK_WIDGET(priv->left), TRUE, TRUE, 0);
	g_signal_connect(G_OBJECT(priv->left), "toggled", G_CALLBACK(left_toggled_cb), selector);

	priv->right = GTK_RADIO_BUTTON(gtk_radio_button_new_with_label_from_widget(priv->left, _("Right handed")));
	gtk_box_pack_start(GTK_BOX(hbox), GTK_WIDGET(priv->right), TRUE, TRUE, 0);

	gtk_frame_set_label(GTK_FRAME(selector), _("Handedness"));
}

static void roccat_handedness_selector_class_init(RoccatHandednessSelectorClass *klass) {
	GObjectClass *gobject_class;

	gobject_class = (GObjectClass *)klass;

	gobject_class->set_property = roccat_handedness_selector_set_property;
	gobject_class->get_property = roccat_handedness_selector_get_property;

	g_type_class_add_private(klass, sizeof(RoccatHandednessSelectorPrivate));

	signals[TOGGLED] = g_signal_new("toggled",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);

	g_object_class_install_property(gobject_class, PROP_VALUE,
			g_param_spec_uint("value", "value", "Reads or sets value",
					ROCCAT_HANDEDNESS_SELECTOR_NONE, ROCCAT_HANDEDNESS_SELECTOR_RIGHT, ROCCAT_HANDEDNESS_SELECTOR_NONE,
					G_PARAM_READWRITE));
}
