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

#include "ryosconfig_effect_selector.h"
#include "ryos_stored_lights.h"
#include "gtk_roccat_helper.h"
#include "i18n.h"

#define RYOSCONFIG_EFFECT_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), RYOSCONFIG_EFFECT_SELECTOR_TYPE, RyosconfigEffectSelectorClass))
#define IS_RYOSCONFIG_EFFECT_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), RYOSCONFIG_EFFECT_SELECTOR_TYPE))
#define RYOSCONFIG_EFFECT_SELECTOR_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), RYOSCONFIG_EFFECT_SELECTOR_TYPE, RyosconfigEffectSelectorClass))
#define RYOSCONFIG_EFFECT_SELECTOR_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), RYOSCONFIG_EFFECT_SELECTOR_TYPE, RyosconfigEffectSelectorPrivate))

typedef struct _RyosconfigEffectSelectorClass RyosconfigEffectSelectorClass;
typedef struct _RyosconfigEffectSelectorPrivate RyosconfigEffectSelectorPrivate;

struct _RyosconfigEffectSelector {
	GtkHBox parent;
	RyosconfigEffectSelectorPrivate *priv;
};

struct _RyosconfigEffectSelectorClass {
	GtkHBoxClass parent_class;
};

struct _RyosconfigEffectSelectorPrivate {
	GtkToggleButton *off;
	GtkToggleButton *fade;
	GtkToggleButton *ripple;
	GtkSpinButton *delay;
	gulong delay_handler_id;
};

G_DEFINE_TYPE(RyosconfigEffectSelector, ryosconfig_effect_selector, GTK_TYPE_HBOX);

enum {
	CHANGED,
	LAST_SIGNAL,
};

static guint signals[LAST_SIGNAL] = { 0 };

GtkWidget *ryosconfig_effect_selector_new(void) {
	return GTK_WIDGET(g_object_new(RYOSCONFIG_EFFECT_SELECTOR_TYPE, NULL));
}

static void effect_cb(GtkToggleButton *togglebutton, gpointer user_data) {
	RyosconfigEffectSelector *effect_selector = RYOSCONFIG_EFFECT_SELECTOR(user_data);
	if (gtk_toggle_button_get_active(togglebutton))
		g_signal_emit((gpointer)effect_selector, signals[CHANGED], 0);
}

static void delay_cb(GtkSpinButton *spinbutton, gpointer user_data) {
	RyosconfigEffectSelector *effect_selector = RYOSCONFIG_EFFECT_SELECTOR(user_data);
	g_signal_emit((gpointer)effect_selector, signals[CHANGED], 0);
}

static void ryosconfig_effect_selector_init(RyosconfigEffectSelector *effect_selector) {
	RyosconfigEffectSelectorPrivate *priv = RYOSCONFIG_EFFECT_SELECTOR_GET_PRIVATE(effect_selector);

	effect_selector->priv = priv;

	priv->off = GTK_TOGGLE_BUTTON(gtk_radio_button_new_with_label(NULL, _("Off")));
	priv->fade = GTK_TOGGLE_BUTTON(gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(priv->off), _("Fade FX")));
	priv->ripple = GTK_TOGGLE_BUTTON(gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(priv->fade), _("Script FX")));
	priv->delay = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(0.0, 255.0, 1.0));

	g_signal_connect(G_OBJECT(priv->off), "toggled", G_CALLBACK(effect_cb), effect_selector);
	g_signal_connect(G_OBJECT(priv->fade), "toggled", G_CALLBACK(effect_cb), effect_selector);
	g_signal_connect(G_OBJECT(priv->ripple), "toggled", G_CALLBACK(effect_cb), effect_selector);
	priv->delay_handler_id = g_signal_connect(G_OBJECT(priv->delay), "value-changed", G_CALLBACK(delay_cb), effect_selector);

	gtk_box_pack_start(GTK_BOX(effect_selector), GTK_WIDGET(priv->off), TRUE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(effect_selector), gtk_vseparator_new(), FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(effect_selector), GTK_WIDGET(priv->fade), TRUE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(effect_selector), gtk_label_new(_("Delay")), FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(effect_selector), GTK_WIDGET(priv->delay), TRUE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(effect_selector), gtk_vseparator_new(), FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(effect_selector), GTK_WIDGET(priv->ripple), TRUE, FALSE, 0);
}

static void ryosconfig_effect_selector_class_init(RyosconfigEffectSelectorClass *klass) {
	g_type_class_add_private(klass, sizeof(RyosconfigEffectSelectorPrivate));

	signals[CHANGED] = g_signal_new("changed",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);
}

void ryosconfig_effect_selector_set_effect_blocked(RyosconfigEffectSelector *effect_selector, guint effect) {
	RyosconfigEffectSelectorPrivate *priv = effect_selector->priv;

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

guint ryosconfig_effect_selector_get_effect(RyosconfigEffectSelector *effect_selector) {
	RyosconfigEffectSelectorPrivate *priv = effect_selector->priv;

	if (gtk_toggle_button_get_active(priv->ripple))
		return RYOS_LIGHT_LAYER_EFFECT_RIPPLE;
	else if (gtk_toggle_button_get_active(priv->fade))
		return RYOS_LIGHT_LAYER_EFFECT_FADE;
	else
		return RYOS_LIGHT_LAYER_EFFECT_OFF;
}

void ryosconfig_effect_selector_set_delay_blocked(RyosconfigEffectSelector *effect_selector, guint delay) {
	RyosconfigEffectSelectorPrivate *priv = effect_selector->priv;
	g_signal_handler_block(G_OBJECT(priv->delay), priv->delay_handler_id);
	gtk_spin_button_set_value(priv->delay, (gdouble)delay);
	g_signal_handler_unblock(G_OBJECT(priv->delay), priv->delay_handler_id);
}

guint ryosconfig_effect_selector_get_delay(RyosconfigEffectSelector *effect_selector) {
	return (guint)(gtk_spin_button_get_value(effect_selector->priv->delay) + 0.5);
}
