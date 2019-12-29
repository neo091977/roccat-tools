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

#include "ryosconfig_key_illumination_selector.h"
#include "ryos_stored_lights.h"
#include "roccat_helper.h"
#include "i18n.h"

#define RYOSCONFIG_KEY_ILLUMINATION_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), RYOSCONFIG_KEY_ILLUMINATION_SELECTOR_TYPE, RyosconfigKeyIlluminationSelectorClass))
#define IS_RYOSCONFIG_KEY_ILLUMINATION_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), RYOSCONFIG_KEY_ILLUMINATION_SELECTOR_TYPE))
#define RYOSCONFIG_KEY_ILLUMINATION_SELECTOR_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), RYOSCONFIG_KEY_ILLUMINATION_SELECTOR_TYPE, RyosconfigKeyIlluminationSelectorPrivate))

typedef struct _RyosconfigKeyIlluminationSelectorClass RyosconfigKeyIlluminationSelectorClass;
typedef struct _RyosconfigKeyIlluminationSelectorPrivate RyosconfigKeyIlluminationSelectorPrivate;

struct _RyosconfigKeyIlluminationSelector {
	GtkHBox parent;
	RyosconfigKeyIlluminationSelectorPrivate *priv;
};

struct _RyosconfigKeyIlluminationSelectorClass {
	GtkHBoxClass parent_class;
};

struct _RyosconfigKeyIlluminationSelectorPrivate {
	GtkToggleButton *on;
	GtkToggleButton *blink;
	gulong on_handler_id;
	gulong blink_handler_id;
};

G_DEFINE_TYPE(RyosconfigKeyIlluminationSelector, ryosconfig_key_illumination_selector, GTK_TYPE_HBOX);

enum {
	CHANGED,
	LAST_SIGNAL,
};

static guint signals[LAST_SIGNAL] = { 0 };

GtkWidget *ryosconfig_key_illumination_selector_new(void) {
	return GTK_WIDGET(g_object_new(RYOSCONFIG_KEY_ILLUMINATION_SELECTOR_TYPE, NULL));
}

static void on_cb(GtkToggleButton *togglebutton, gpointer user_data) {
	RyosconfigKeyIlluminationSelector *illumination_selector = RYOSCONFIG_KEY_ILLUMINATION_SELECTOR(user_data);

	g_signal_emit((gpointer)illumination_selector, signals[CHANGED], 0);
}

static void blink_cb(GtkToggleButton *togglebutton, gpointer user_data) {
	RyosconfigKeyIlluminationSelector *illumination_selector = RYOSCONFIG_KEY_ILLUMINATION_SELECTOR(user_data);
	RyosconfigKeyIlluminationSelectorPrivate *priv = illumination_selector->priv;

	if (gtk_toggle_button_get_active(togglebutton)) {
		g_signal_handler_block(G_OBJECT(priv->on), priv->on_handler_id);
		gtk_toggle_button_set_active(priv->on, TRUE);
		g_signal_handler_unblock(G_OBJECT(priv->on), priv->on_handler_id);
	}
	g_signal_emit((gpointer)illumination_selector, signals[CHANGED], 0);
}

static void ryosconfig_key_illumination_selector_init(RyosconfigKeyIlluminationSelector *illumination_selector) {
	RyosconfigKeyIlluminationSelectorPrivate *priv = RYOSCONFIG_KEY_ILLUMINATION_SELECTOR_GET_PRIVATE(illumination_selector);
	illumination_selector->priv = priv;

	priv->on = GTK_TOGGLE_BUTTON(gtk_check_button_new_with_label(_("On/Off")));
	priv->blink = GTK_TOGGLE_BUTTON(gtk_check_button_new_with_label(_("Blinking")));

	gtk_widget_set_tooltip_text(GTK_WIDGET(priv->blink), _("Blink only works when layer effect is off."));
			
	priv->on_handler_id = g_signal_connect(G_OBJECT(priv->on), "toggled", G_CALLBACK(on_cb), illumination_selector);
	priv->blink_handler_id = g_signal_connect(G_OBJECT(priv->blink), "toggled", G_CALLBACK(blink_cb), illumination_selector);

	gtk_box_pack_start(GTK_BOX(illumination_selector), GTK_WIDGET(priv->on), TRUE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(illumination_selector), gtk_vseparator_new(), FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(illumination_selector), GTK_WIDGET(priv->blink), TRUE, FALSE, 0);
}

static void ryosconfig_key_illumination_selector_class_init(RyosconfigKeyIlluminationSelectorClass *klass) {
	g_type_class_add_private(klass, sizeof(RyosconfigKeyIlluminationSelectorPrivate));

	signals[CHANGED] = g_signal_new("changed",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);
}

void ryosconfig_key_illumination_selector_set_value_blocked(RyosconfigKeyIlluminationSelector *illumination_selector, guint8 value) {
	RyosconfigKeyIlluminationSelectorPrivate *priv = illumination_selector->priv;

	g_signal_handler_block(G_OBJECT(priv->on), priv->on_handler_id);
	gtk_toggle_button_set_active(priv->on, roccat_get_bit8(value, RYOS_LIGHT_LAYER_KEY_BIT_ON));
	g_signal_handler_unblock(G_OBJECT(priv->on), priv->on_handler_id);

	g_signal_handler_block(G_OBJECT(priv->blink), priv->blink_handler_id);
	gtk_toggle_button_set_active(priv->blink, roccat_get_bit8(value, RYOS_LIGHT_LAYER_KEY_BIT_BLINK));
	g_signal_handler_unblock(G_OBJECT(priv->blink), priv->blink_handler_id);
}

guint8 ryosconfig_key_illumination_selector_get_value(RyosconfigKeyIlluminationSelector *illumination_selector) {
	RyosconfigKeyIlluminationSelectorPrivate *priv = illumination_selector->priv;
	guint8 result = 0;
	roccat_set_bit8(&result, RYOS_LIGHT_LAYER_KEY_BIT_ON, gtk_toggle_button_get_active(priv->on));
	roccat_set_bit8(&result, RYOS_LIGHT_LAYER_KEY_BIT_BLINK, gtk_toggle_button_get_active(priv->blink));
	return result;
}
