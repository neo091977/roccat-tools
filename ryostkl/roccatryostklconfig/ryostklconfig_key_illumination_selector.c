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

#include "ryostklconfig_key_illumination_selector.h"
#include "ryostkl_light_layer.h"
#include "roccat_helper.h"
#include "i18n.h"

#define RYOSTKLCONFIG_KEY_ILLUMINATION_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), RYOSTKLCONFIG_KEY_ILLUMINATION_SELECTOR_TYPE, RyostklconfigKeyIlluminationSelectorClass))
#define IS_RYOSTKLCONFIG_KEY_ILLUMINATION_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), RYOSTKLCONFIG_KEY_ILLUMINATION_SELECTOR_TYPE))
#define RYOSTKLCONFIG_KEY_ILLUMINATION_SELECTOR_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), RYOSTKLCONFIG_KEY_ILLUMINATION_SELECTOR_TYPE, RyostklconfigKeyIlluminationSelectorPrivate))

typedef struct _RyostklconfigKeyIlluminationSelectorClass RyostklconfigKeyIlluminationSelectorClass;
typedef struct _RyostklconfigKeyIlluminationSelectorPrivate RyostklconfigKeyIlluminationSelectorPrivate;

struct _RyostklconfigKeyIlluminationSelector {
	GtkHBox parent;
	RyostklconfigKeyIlluminationSelectorPrivate *priv;
};

struct _RyostklconfigKeyIlluminationSelectorClass {
	GtkHBoxClass parent_class;
};

struct _RyostklconfigKeyIlluminationSelectorPrivate {
	gboolean limited;
	GtkWidget *blink_box;
	GtkWidget *breath_box;
	GtkToggleButton *off_button;
	GtkToggleButton *on_button;
	GtkToggleButton *blink_button;
	GtkToggleButton *breath_button;
	GtkSpinButton *blink_time;
	GtkSpinButton *breath_time;
	gulong off_handler_id;
	gulong on_handler_id;
	gulong blink_handler_id;
	gulong breath_handler_id;
	gulong blink_time_handler_id;
	gulong breath_time_handler_id;
};

G_DEFINE_TYPE(RyostklconfigKeyIlluminationSelector, ryostklconfig_key_illumination_selector, GTK_TYPE_HBOX);

enum {
	CHANGED,
	LAST_SIGNAL,
};

static guint signals[LAST_SIGNAL] = { 0 };

GtkWidget *ryostklconfig_key_illumination_selector_new(void) {
	return GTK_WIDGET(g_object_new(RYOSTKLCONFIG_KEY_ILLUMINATION_SELECTOR_TYPE, NULL));
}

static void effect_cb(GtkToggleButton *togglebutton, gpointer user_data) {
	RyostklconfigKeyIlluminationSelector *illumination_selector = RYOSTKLCONFIG_KEY_ILLUMINATION_SELECTOR(user_data);
	g_signal_emit((gpointer)illumination_selector, signals[CHANGED], 0);
}

static void value_cb(GtkSpinButton *spinbutton, gpointer user_data) {
	RyostklconfigKeyIlluminationSelector *illumination_selector = RYOSTKLCONFIG_KEY_ILLUMINATION_SELECTOR(user_data);
	g_signal_emit((gpointer)illumination_selector, signals[CHANGED], 0);
}

static void ryostklconfig_key_illumination_selector_init(RyostklconfigKeyIlluminationSelector *illumination_selector) {
	RyostklconfigKeyIlluminationSelectorPrivate *priv = RYOSTKLCONFIG_KEY_ILLUMINATION_SELECTOR_GET_PRIVATE(illumination_selector);
	illumination_selector->priv = priv;

	priv->blink_box = gtk_hbox_new(FALSE, 0);
	priv->breath_box = gtk_hbox_new(FALSE, 0);
	priv->off_button = GTK_TOGGLE_BUTTON(gtk_radio_button_new_with_label(NULL, _("Off")));
	priv->on_button = GTK_TOGGLE_BUTTON(gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(priv->off_button), _("On")));
	priv->blink_button = GTK_TOGGLE_BUTTON(gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(priv->off_button), _("Blinking")));
	priv->breath_button = GTK_TOGGLE_BUTTON(gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(priv->blink_button), _("Breathing")));
	priv->blink_time = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(RYOSTKL_LIGHT_LAYER_KEY_EFFECT_SPEED_MIN, RYOSTKL_LIGHT_LAYER_KEY_EFFECT_SPEED_MAX, 1.0));
	priv->breath_time = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(RYOSTKL_LIGHT_LAYER_KEY_EFFECT_SPEED_MIN, RYOSTKL_LIGHT_LAYER_KEY_EFFECT_SPEED_MAX, 1.0));

	g_object_ref(G_OBJECT(priv->blink_box));
	g_object_ref(G_OBJECT(priv->breath_box));

	priv->off_handler_id = g_signal_connect(G_OBJECT(priv->off_button), "toggled", G_CALLBACK(effect_cb), illumination_selector);
	priv->on_handler_id = g_signal_connect(G_OBJECT(priv->on_button), "toggled", G_CALLBACK(effect_cb), illumination_selector);
	priv->blink_handler_id = g_signal_connect(G_OBJECT(priv->blink_button), "toggled", G_CALLBACK(effect_cb), illumination_selector);
	priv->breath_handler_id = g_signal_connect(G_OBJECT(priv->breath_button), "toggled", G_CALLBACK(effect_cb), illumination_selector);
	priv->blink_time_handler_id = g_signal_connect(G_OBJECT(priv->blink_time), "value-changed", G_CALLBACK(value_cb), illumination_selector);
	priv->breath_time_handler_id = g_signal_connect(G_OBJECT(priv->breath_time), "value-changed", G_CALLBACK(value_cb), illumination_selector);

	gtk_box_pack_start(GTK_BOX(priv->blink_box), GTK_WIDGET(priv->blink_button), TRUE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(priv->blink_box), GTK_WIDGET(priv->blink_time), TRUE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(priv->breath_box), GTK_WIDGET(priv->breath_button), TRUE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(priv->breath_box), GTK_WIDGET(priv->breath_time), TRUE, FALSE, 0);

	gtk_box_pack_start(GTK_BOX(illumination_selector), GTK_WIDGET(priv->off_button), TRUE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(illumination_selector), GTK_WIDGET(priv->on_button), TRUE, FALSE, 0);

	gtk_widget_set_tooltip_text(GTK_WIDGET(illumination_selector), _("Key effect only works when layer effect is off."));

	priv->limited = TRUE;
}

static void finalize(GObject *object) {
	RyostklconfigKeyIlluminationSelectorPrivate *priv = RYOSTKLCONFIG_KEY_ILLUMINATION_SELECTOR(object)->priv;
	g_object_unref(G_OBJECT(priv->blink_box));
	g_object_unref(G_OBJECT(priv->breath_box));
	G_OBJECT_CLASS(ryostklconfig_key_illumination_selector_parent_class)->finalize(object);
}

static void ryostklconfig_key_illumination_selector_class_init(RyostklconfigKeyIlluminationSelectorClass *klass) {
	GObjectClass *gobject_class;

	gobject_class = G_OBJECT_CLASS(klass);

	gobject_class->finalize = finalize;

	g_type_class_add_private(klass, sizeof(RyostklconfigKeyIlluminationSelectorPrivate));

	signals[CHANGED] = g_signal_new("changed",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);
}

void ryostklconfig_key_illumination_selector_set_limited(RyostklconfigKeyIlluminationSelector *illumination_selector, gboolean limited) {
	RyostklconfigKeyIlluminationSelectorPrivate *priv = illumination_selector->priv;

	if (limited && !priv->limited) {
		gtk_container_remove(GTK_CONTAINER(illumination_selector), priv->blink_box);
		gtk_container_remove(GTK_CONTAINER(illumination_selector), priv->breath_box);
	} else if (!limited && priv->limited) {
		gtk_box_pack_start(GTK_BOX(illumination_selector), priv->blink_box, TRUE, FALSE, 0);
		gtk_box_pack_start(GTK_BOX(illumination_selector), priv->breath_box, TRUE, FALSE, 0);
	}
	priv->limited = limited;
}

void ryostklconfig_key_illumination_selector_set_value_blocked(RyostklconfigKeyIlluminationSelector *illumination_selector, guint16 value) {
	RyostklconfigKeyIlluminationSelectorPrivate *priv = illumination_selector->priv;

	g_signal_handler_block(G_OBJECT(priv->on_button), priv->on_handler_id);
	g_signal_handler_block(G_OBJECT(priv->off_button), priv->off_handler_id);
	g_signal_handler_block(G_OBJECT(priv->blink_button), priv->blink_handler_id);
	g_signal_handler_block(G_OBJECT(priv->breath_button), priv->breath_handler_id);
	g_signal_handler_block(G_OBJECT(priv->blink_time), priv->blink_time_handler_id);
	g_signal_handler_block(G_OBJECT(priv->breath_time), priv->breath_time_handler_id);

	gtk_spin_button_set_value(priv->blink_time, (gdouble)ryostkl_light_layer_key_get_blink_speed(&value));
	gtk_spin_button_set_value(priv->breath_time, (gdouble)ryostkl_light_layer_key_get_breath_speed(&value));

	if (ryostkl_light_layer_key_get_state(&value)) {
		if (ryostkl_light_layer_key_get_effect(&value) == RYOSTKL_LIGHT_LAYER_KEY_EFFECT_BLINK)
			gtk_toggle_button_set_active(priv->blink_button, TRUE);
		else if (ryostkl_light_layer_key_get_effect(&value) == RYOSTKL_LIGHT_LAYER_KEY_EFFECT_BREATH)
			gtk_toggle_button_set_active(priv->breath_button, TRUE);
		else
			gtk_toggle_button_set_active(priv->on_button, TRUE);
	} else
		gtk_toggle_button_set_active(priv->off_button, TRUE);

	g_signal_handler_unblock(G_OBJECT(priv->on_button), priv->on_handler_id);
	g_signal_handler_unblock(G_OBJECT(priv->off_button), priv->off_handler_id);
	g_signal_handler_unblock(G_OBJECT(priv->blink_button), priv->blink_handler_id);
	g_signal_handler_unblock(G_OBJECT(priv->breath_button), priv->breath_handler_id);
	g_signal_handler_unblock(G_OBJECT(priv->blink_time), priv->blink_time_handler_id);
	g_signal_handler_unblock(G_OBJECT(priv->breath_time), priv->breath_time_handler_id);
}

guint16 ryostklconfig_key_illumination_selector_get_value(RyostklconfigKeyIlluminationSelector *illumination_selector) {
	RyostklconfigKeyIlluminationSelectorPrivate *priv = illumination_selector->priv;
	gboolean state;
	guint effect;
	guint blink_speed;
	guint breath_speed;

	state = !gtk_toggle_button_get_active(priv->off_button);
	blink_speed = (guint)(gtk_spin_button_get_value(priv->blink_time) + 0.5);
	breath_speed = (guint)(gtk_spin_button_get_value(priv->breath_time) + 0.5);

	if (gtk_toggle_button_get_active(priv->blink_button))
		effect = RYOSTKL_LIGHT_LAYER_KEY_EFFECT_BLINK;
	else if (gtk_toggle_button_get_active(priv->breath_button))
		effect = RYOSTKL_LIGHT_LAYER_KEY_EFFECT_BREATH;
	else
		effect = RYOSTKL_LIGHT_LAYER_KEY_EFFECT_OFF;

	return ryostkl_light_layer_key_create(state, effect, blink_speed, breath_speed);
}
