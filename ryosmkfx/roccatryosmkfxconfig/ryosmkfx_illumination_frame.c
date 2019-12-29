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

#include "ryosmkfx_illumination_frame.h"
#include "ryosmkfx_light_layer.h"
#include "roccat_swarm_color_selection_button.h"
#include "i18n.h"

#define RYOSMKFX_ILLUMINATION_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), RYOSMKFX_ILLUMINATION_FRAME_TYPE, RyosmkfxIlluminationFrameClass))
#define IS_RYOSMKFX_ILLUMINATION_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), RYOSMKFX_ILLUMINATION_FRAME_TYPE))
#define RYOSMKFX_ILLUMINATION_FRAME_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), RYOSMKFX_ILLUMINATION_FRAME_TYPE, RyosmkfxIlluminationFramePrivate))

typedef struct _RyosmkfxIlluminationFrameClass RyosmkfxIlluminationFrameClass;
typedef struct _RyosmkfxIlluminationFramePrivate RyosmkfxIlluminationFramePrivate;

struct _RyosmkfxIlluminationFrame {
	GtkFrame parent;
	RyosmkfxIlluminationFramePrivate *priv;
};

struct _RyosmkfxIlluminationFrameClass {
	GtkFrameClass parent_class;
};

struct _RyosmkfxIlluminationFramePrivate {
	GtkToggleButton *plain_radio;
	GtkToggleButton *layer_radio;
	RoccatColorSelectionButton *plain_button;
};

G_DEFINE_TYPE(RyosmkfxIlluminationFrame, ryosmkfx_illumination_frame, GTK_TYPE_FRAME);

enum {
	PLAIN_COLOR_CHANGED,
	MODE_CHANGED,
	LAYER_EDITOR,
	LAST_SIGNAL,
};

static guint signals[LAST_SIGNAL] = { 0 };

GtkWidget *ryosmkfx_illumination_frame_new(void) {
	return GTK_WIDGET(g_object_new(RYOSMKFX_ILLUMINATION_FRAME_TYPE, NULL));
}

static void plain_button_changed_cb(RoccatColorSelectionButton *button, gpointer user_data) {
	if (gtk_toggle_button_get_active(RYOSMKFX_ILLUMINATION_FRAME(user_data)->priv->plain_radio))
		g_signal_emit(user_data, signals[PLAIN_COLOR_CHANGED], 0);
}

static void radio_changed_cb(GtkToggleButton *toggle, gpointer user_data) {
	if (gtk_toggle_button_get_active(toggle))
		g_signal_emit(user_data, signals[MODE_CHANGED], 0);
}

static void layer_editor_dialog_cb(GtkButton *button, gpointer user_data) {
	g_signal_emit(user_data, signals[LAYER_EDITOR], 0);
}

static void ryosmkfx_illumination_frame_init(RyosmkfxIlluminationFrame *illumination_frame) {
	RyosmkfxIlluminationFramePrivate *priv = RYOSMKFX_ILLUMINATION_FRAME_GET_PRIVATE(illumination_frame);
	illumination_frame->priv = priv;
	GtkButton *layer_editor_button;
	GtkTable *table;

	table = GTK_TABLE(gtk_table_new(2, 3, FALSE));

	layer_editor_button = GTK_BUTTON(gtk_button_new_with_label(_("Editor")));
	g_signal_connect(G_OBJECT(layer_editor_button), "clicked", G_CALLBACK(layer_editor_dialog_cb), illumination_frame);

	priv->plain_radio = GTK_TOGGLE_BUTTON(gtk_radio_button_new_with_label(NULL, _("Plain")));
	priv->layer_radio = GTK_TOGGLE_BUTTON(gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(priv->plain_radio), _("Layer")));
	g_signal_connect(G_OBJECT(priv->plain_radio), "toggled", G_CALLBACK(radio_changed_cb), illumination_frame);
	g_signal_connect(G_OBJECT(priv->layer_radio), "toggled", G_CALLBACK(radio_changed_cb), illumination_frame);

	priv->plain_button = ROCCAT_COLOR_SELECTION_BUTTON(roccat_swarm_color_selection_button_new());
	g_signal_connect(G_OBJECT(priv->plain_button), "value-changed", G_CALLBACK(plain_button_changed_cb), illumination_frame);

	gtk_table_attach(table, GTK_WIDGET(priv->plain_radio), 0, 1, 0, 1, GTK_EXPAND, GTK_EXPAND, 0, 0);
	gtk_table_attach(table, GTK_WIDGET(priv->plain_button), 1, 2, 0, 1, GTK_EXPAND, GTK_EXPAND, 0, 0);
	gtk_table_attach(table, GTK_WIDGET(priv->layer_radio), 0, 1, 1, 2, GTK_EXPAND, GTK_EXPAND, 0, 0);
	gtk_table_attach(table, GTK_WIDGET(layer_editor_button), 2, 3, 1, 2, GTK_EXPAND, GTK_EXPAND, 0, 0);

	gtk_container_add(GTK_CONTAINER(illumination_frame), GTK_WIDGET(table));
	gtk_frame_set_label(GTK_FRAME(illumination_frame), _("Illumination"));
}

static void ryosmkfx_illumination_frame_class_init(RyosmkfxIlluminationFrameClass *klass) {
	g_type_class_add_private(klass, sizeof(RyosmkfxIlluminationFramePrivate));

	signals[MODE_CHANGED] = g_signal_new("mode-changed",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);

	signals[PLAIN_COLOR_CHANGED] = g_signal_new("plain-color-changed",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);

	signals[LAYER_EDITOR] = g_signal_new("layer-editor",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);
}

void ryosmkfx_illumination_frame_set_plain_color(RyosmkfxIlluminationFrame *illumination_frame, GdkColor const *color) {
	roccat_color_selection_button_set_custom_color(illumination_frame->priv->plain_button, color);
}

void ryosmkfx_illumination_frame_get_plain_color(RyosmkfxIlluminationFrame *illumination_frame, GdkColor *color) {
	roccat_color_selection_button_get_color(illumination_frame->priv->plain_button, color);
}

void ryosmkfx_illumination_frame_set_mode(RyosmkfxIlluminationFrame *illumination_frame, RyosmkfxLightMode mode) {
	RyosmkfxIlluminationFramePrivate *priv = illumination_frame->priv;
	gtk_toggle_button_set_active(mode == RYOSMKFX_LIGHT_MODE_PLAIN ? priv->plain_radio : priv->layer_radio, TRUE);
}

RyosmkfxLightMode ryosmkfx_illumination_frame_get_mode(RyosmkfxIlluminationFrame *illumination_frame) {
	return gtk_toggle_button_get_active(illumination_frame->priv->plain_radio) ? RYOSMKFX_LIGHT_MODE_PLAIN : RYOSMKFX_LIGHT_MODE_LAYER;
}
