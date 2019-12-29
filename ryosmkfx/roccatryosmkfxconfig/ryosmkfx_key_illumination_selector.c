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

#include "ryosmkfx_key_illumination_selector.h"
#include "ryosmkfx_light_layer.h"
#include "ryos_stored_lights.h"
#include "roccat_swarm_color_selection_button.h"
#include "roccat_helper.h"
#include "i18n.h"

#define RYOSMKFX_KEY_ILLUMINATION_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), RYOSMKFX_KEY_ILLUMINATION_SELECTOR_TYPE, RyosmkfxKeyIlluminationSelectorClass))
#define IS_RYOSMKFX_KEY_ILLUMINATION_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), RYOSMKFX_KEY_ILLUMINATION_SELECTOR_TYPE))
#define RYOSMKFX_KEY_ILLUMINATION_SELECTOR_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), RYOSMKFX_KEY_ILLUMINATION_SELECTOR_TYPE, RyosmkfxKeyIlluminationSelectorPrivate))

typedef struct _RyosmkfxKeyIlluminationSelectorClass RyosmkfxKeyIlluminationSelectorClass;
typedef struct _RyosmkfxKeyIlluminationSelectorPrivate RyosmkfxKeyIlluminationSelectorPrivate;

struct _RyosmkfxKeyIlluminationSelector {
	GtkHBox parent;
	RyosmkfxKeyIlluminationSelectorPrivate *priv;
};

struct _RyosmkfxKeyIlluminationSelectorClass {
	GtkHBoxClass parent_class;
};

struct _RyosmkfxKeyIlluminationSelectorPrivate {
	GtkToggleButton *on;
	RoccatColorSelectionButton *color;
};

G_DEFINE_TYPE(RyosmkfxKeyIlluminationSelector, ryosmkfx_key_illumination_selector, GTK_TYPE_HBOX);

enum {
	CHANGED,
	LAST_SIGNAL,
};

static guint signals[LAST_SIGNAL] = { 0 };

GtkWidget *ryosmkfx_key_illumination_selector_new(void) {
	return GTK_WIDGET(g_object_new(RYOSMKFX_KEY_ILLUMINATION_SELECTOR_TYPE, NULL));
}

static void on_cb(GtkToggleButton *togglebutton, gpointer user_data) {
	g_signal_emit(user_data, signals[CHANGED], 0);
}

static void color_changed_cb(RoccatColorSelectionButton *button, gpointer user_data) {
	g_signal_emit(user_data, signals[CHANGED], 0);
}

static void ryosmkfx_key_illumination_selector_init(RyosmkfxKeyIlluminationSelector *illumination_selector) {
	RyosmkfxKeyIlluminationSelectorPrivate *priv = RYOSMKFX_KEY_ILLUMINATION_SELECTOR_GET_PRIVATE(illumination_selector);
	illumination_selector->priv = priv;

	priv->on = GTK_TOGGLE_BUTTON(gtk_check_button_new_with_label(_("On/Off")));
	g_signal_connect(G_OBJECT(priv->on), "toggled", G_CALLBACK(on_cb), illumination_selector);

	priv->color = ROCCAT_COLOR_SELECTION_BUTTON(roccat_swarm_color_selection_button_new());
	g_signal_connect(G_OBJECT(priv->color), "value-changed", G_CALLBACK(color_changed_cb), illumination_selector);

	gtk_box_pack_start(GTK_BOX(illumination_selector), GTK_WIDGET(priv->on), TRUE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(illumination_selector), gtk_vseparator_new(), FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(illumination_selector), GTK_WIDGET(priv->color), TRUE, FALSE, 0);
}

static void ryosmkfx_key_illumination_selector_class_init(RyosmkfxKeyIlluminationSelectorClass *klass) {
	g_type_class_add_private(klass, sizeof(RyosmkfxKeyIlluminationSelectorPrivate));

	signals[CHANGED] = g_signal_new("changed",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);
}

void ryosmkfx_key_illumination_selector_set_state(RyosmkfxKeyIlluminationSelector *illumination_selector, gboolean state) {
	gtk_toggle_button_set_active(illumination_selector->priv->on, state);
}

gboolean ryosmkfx_key_illumination_selector_get_state(RyosmkfxKeyIlluminationSelector *illumination_selector) {
	return gtk_toggle_button_get_active(illumination_selector->priv->on);
}

void ryosmkfx_key_illumination_selector_set_color(RyosmkfxKeyIlluminationSelector *illumination_selector, GdkColor const *color) {
	roccat_color_selection_button_set_custom_color(illumination_selector->priv->color, color);
}

void ryosmkfx_key_illumination_selector_get_color(RyosmkfxKeyIlluminationSelector *illumination_selector, GdkColor *color) {
	roccat_color_selection_button_get_color(illumination_selector->priv->color, color);
}
