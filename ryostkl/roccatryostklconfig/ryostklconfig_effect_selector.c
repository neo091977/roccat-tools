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

#include "ryostklconfig_effect_selector.h"
#include "ryostkl_light_layer.h"
#include "gtk_roccat_helper.h"
#include "i18n.h"

#define RYOSTKLCONFIG_EFFECT_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), RYOSTKLCONFIG_EFFECT_SELECTOR_TYPE, RyostklconfigEffectSelectorClass))
#define IS_RYOSTKLCONFIG_EFFECT_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), RYOSTKLCONFIG_EFFECT_SELECTOR_TYPE))
#define RYOSTKLCONFIG_EFFECT_SELECTOR_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), RYOSTKLCONFIG_EFFECT_SELECTOR_TYPE, RyostklconfigEffectSelectorClass))
#define RYOSTKLCONFIG_EFFECT_SELECTOR_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), RYOSTKLCONFIG_EFFECT_SELECTOR_TYPE, RyostklconfigEffectSelectorPrivate))

typedef struct _RyostklconfigEffectSelectorClass RyostklconfigEffectSelectorClass;
typedef struct _RyostklconfigEffectSelectorPrivate RyostklconfigEffectSelectorPrivate;

struct _RyostklconfigEffectSelector {
	GtkHBox parent;
	RyostklconfigEffectSelectorPrivate *priv;
};

struct _RyostklconfigEffectSelectorClass {
	GtkHBoxClass parent_class;
};

struct _RyostklconfigEffectSelectorPrivate {
	GtkToggleButton *off;
	GtkToggleButton *fade;
	GtkToggleButton *ripple;
	GtkSpinButton *delay_time;
	GtkSpinButton *fade_time;
	gulong delay_handler_id;
	gulong fade_handler_id;
};

G_DEFINE_TYPE(RyostklconfigEffectSelector, ryostklconfig_effect_selector, GTK_TYPE_HBOX);

enum {
	CHANGED,
	LAST_SIGNAL,
};

static guint signals[LAST_SIGNAL] = { 0 };

GtkWidget *ryostklconfig_effect_selector_new(void) {
	return GTK_WIDGET(g_object_new(RYOSTKLCONFIG_EFFECT_SELECTOR_TYPE, NULL));
}

static void effect_cb(GtkToggleButton *togglebutton, gpointer user_data) {
	RyostklconfigEffectSelector *effect_selector = RYOSTKLCONFIG_EFFECT_SELECTOR(user_data);
	if (gtk_toggle_button_get_active(togglebutton))
		g_signal_emit((gpointer)effect_selector, signals[CHANGED], 0);
}

static void time_cb(GtkSpinButton *spinbutton, gpointer user_data) {
	RyostklconfigEffectSelector *effect_selector = RYOSTKLCONFIG_EFFECT_SELECTOR(user_data);
	g_signal_emit((gpointer)effect_selector, signals[CHANGED], 0);
}

static void ryostklconfig_effect_selector_init(RyostklconfigEffectSelector *effect_selector) {
	RyostklconfigEffectSelectorPrivate *priv = RYOSTKLCONFIG_EFFECT_SELECTOR_GET_PRIVATE(effect_selector);

	effect_selector->priv = priv;

	priv->off = GTK_TOGGLE_BUTTON(gtk_radio_button_new_with_label(NULL, _("Off")));
	priv->fade = GTK_TOGGLE_BUTTON(gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(priv->off), _("Fade FX")));
	priv->ripple = GTK_TOGGLE_BUTTON(gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(priv->fade), _("Script FX")));
	priv->delay_time = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(RYOSTKL_LIGHT_LAYER_EFFECT_TIME_MIN, RYOSTKL_LIGHT_LAYER_EFFECT_TIME_MAX, 1.0));
	priv->fade_time = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(RYOSTKL_LIGHT_LAYER_EFFECT_TIME_MIN, RYOSTKL_LIGHT_LAYER_EFFECT_TIME_MAX, 1.0));

	g_signal_connect(G_OBJECT(priv->off), "toggled", G_CALLBACK(effect_cb), effect_selector);
	g_signal_connect(G_OBJECT(priv->fade), "toggled", G_CALLBACK(effect_cb), effect_selector);
	g_signal_connect(G_OBJECT(priv->ripple), "toggled", G_CALLBACK(effect_cb), effect_selector);
	priv->delay_handler_id = g_signal_connect(G_OBJECT(priv->delay_time), "value-changed", G_CALLBACK(time_cb), effect_selector);
	priv->fade_handler_id = g_signal_connect(G_OBJECT(priv->fade_time), "value-changed", G_CALLBACK(time_cb), effect_selector);

	gtk_box_pack_start(GTK_BOX(effect_selector), GTK_WIDGET(priv->off), TRUE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(effect_selector), gtk_vseparator_new(), FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(effect_selector), GTK_WIDGET(priv->fade), TRUE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(effect_selector), gtk_label_new(_("Delay")), FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(effect_selector), GTK_WIDGET(priv->delay_time), TRUE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(effect_selector), gtk_label_new(_("Fade")), FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(effect_selector), GTK_WIDGET(priv->fade_time), TRUE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(effect_selector), gtk_vseparator_new(), FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(effect_selector), GTK_WIDGET(priv->ripple), TRUE, FALSE, 0);
}

static void ryostklconfig_effect_selector_class_init(RyostklconfigEffectSelectorClass *klass) {
	g_type_class_add_private(klass, sizeof(RyostklconfigEffectSelectorPrivate));

	signals[CHANGED] = g_signal_new("changed",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);
}

void ryostklconfig_effect_selector_set_effect_blocked(RyostklconfigEffectSelector *effect_selector, guint effect) {
	RyostklconfigEffectSelectorPrivate *priv = effect_selector->priv;

	gtk_roccat_radio_buttons_block_toggled(gtk_radio_button_get_group(GTK_RADIO_BUTTON(priv->off)), TRUE);

	switch (effect) {
	case RYOS_LIGHT_LAYER_EFFECT_FADE:
		gtk_toggle_button_set_active(priv->fade, TRUE);
		break;
	case RYOS_LIGHT_LAYER_EFFECT_RIPPLE:
		gtk_toggle_button_set_active(priv->ripple, TRUE);
		break;
	case RYOS_LIGHT_LAYER_EFFECT_OFF:
	default:
		gtk_toggle_button_set_active(priv->off, TRUE);
		break;
	}

	gtk_roccat_radio_buttons_block_toggled(gtk_radio_button_get_group(GTK_RADIO_BUTTON(priv->off)), FALSE);
}

guint ryostklconfig_effect_selector_get_effect(RyostklconfigEffectSelector *effect_selector) {
	RyostklconfigEffectSelectorPrivate *priv = effect_selector->priv;

	if (gtk_toggle_button_get_active(priv->ripple))
		return RYOS_LIGHT_LAYER_EFFECT_RIPPLE;
	else if (gtk_toggle_button_get_active(priv->fade))
		return RYOS_LIGHT_LAYER_EFFECT_FADE;
	else
		return RYOS_LIGHT_LAYER_EFFECT_OFF;
}

void ryostklconfig_effect_selector_set_delay_time_blocked(RyostklconfigEffectSelector *effect_selector, guint delay_time) {
	RyostklconfigEffectSelectorPrivate *priv = effect_selector->priv;
	g_signal_handler_block(G_OBJECT(priv->delay_time), priv->delay_handler_id);
	gtk_spin_button_set_value(priv->delay_time, (gdouble)delay_time);
	g_signal_handler_unblock(G_OBJECT(priv->delay_time), priv->delay_handler_id);
}

guint ryostklconfig_effect_selector_get_delay_time(RyostklconfigEffectSelector *effect_selector) {
	return (guint)(gtk_spin_button_get_value(effect_selector->priv->delay_time) + 0.5);
}

void ryostklconfig_effect_selector_set_fade_time_blocked(RyostklconfigEffectSelector *effect_selector, guint fade_time) {
	RyostklconfigEffectSelectorPrivate *priv = effect_selector->priv;
	g_signal_handler_block(G_OBJECT(priv->fade_time), priv->fade_handler_id);
	gtk_spin_button_set_value(priv->fade_time, (gdouble)fade_time);
	g_signal_handler_unblock(G_OBJECT(priv->fade_time), priv->fade_handler_id);
}

guint ryostklconfig_effect_selector_get_fade_time(RyostklconfigEffectSelector *effect_selector) {
	return (guint)(gtk_spin_button_get_value(effect_selector->priv->fade_time) + 0.5);
}

