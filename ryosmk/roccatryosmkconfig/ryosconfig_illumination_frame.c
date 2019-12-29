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

#include "ryosconfig_illumination_frame.h"
#include "ryos_layer_mode_selector.h"
#include "ryos_layer_selector.h"
#include "ryosconfig_effect_selector.h"
#include "roccat_multiwidget.h"
#include "gtk_roccat_helper.h"
#include "g_cclosure_roccat_marshaller.h"
#include "i18n.h"

#define RYOSCONFIG_ILLUMINATION_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), RYOSCONFIG_ILLUMINATION_FRAME_TYPE, RyosconfigIlluminationFrameClass))
#define IS_RYOSCONFIG_ILLUMINATION_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), RYOSCONFIG_ILLUMINATION_FRAME_TYPE))
#define RYOSCONFIG_ILLUMINATION_FRAME_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), RYOSCONFIG_ILLUMINATION_FRAME_TYPE, RyosconfigIlluminationFramePrivate))

typedef struct _RyosconfigIlluminationFrameClass RyosconfigIlluminationFrameClass;
typedef struct _RyosconfigIlluminationFramePrivate RyosconfigIlluminationFramePrivate;

struct _RyosconfigIlluminationFrame {
	GtkFrame parent;
	RyosconfigIlluminationFramePrivate *priv;
};

struct _RyosconfigIlluminationFrameClass {
	GtkFrameClass parent_class;
};

struct _RyosconfigIlluminationFramePrivate {
	RoccatMultiwidget *multiwidget;
	GtkLabel *nothing;
	GtkTable *table;
	RyosLayerModeSelector *mode;
	RyosLayerSelector *layer;
	RyosconfigEffectSelector *effect;
	gulong mode_changed_handler_id;
	gulong layer_changed_handler_id;
	gulong effect_changed_handler_id;
	RyosLightLayer *pointer;
};

G_DEFINE_TYPE(RyosconfigIlluminationFrame, ryosconfig_illumination_frame, GTK_TYPE_FRAME);

enum {
	MODE_CHANGED,
	LAYER_CHANGED,
	LAYER_EDITOR,
	LAST_SIGNAL,
};

static guint signals[LAST_SIGNAL] = { 0 };

GtkWidget *ryosconfig_illumination_frame_new(void) {
	return GTK_WIDGET(g_object_new(RYOSCONFIG_ILLUMINATION_FRAME_TYPE, NULL));
}

static void mode_changed_cb(GtkComboBox *combo, gpointer user_data) {
	RyosconfigIlluminationFrame *illumination_frame = RYOSCONFIG_ILLUMINATION_FRAME(user_data);
	RyosconfigIlluminationFramePrivate *priv = illumination_frame->priv;
	guint mode = ryos_layer_mode_selector_get_value(priv->mode);
	ryos_layer_selector_set_mode(priv->layer, mode);
	g_signal_emit((gpointer)illumination_frame, signals[MODE_CHANGED], 0);
}

static void layer_changed_cb(GtkComboBox *combo, gpointer user_data) {
	RyosconfigIlluminationFrame *illumination_frame = RYOSCONFIG_ILLUMINATION_FRAME(user_data);
	g_signal_emit((gpointer)illumination_frame, signals[LAYER_CHANGED], 0);
}

static void effect_changed_cb(RyosconfigEffectSelector *effect_selector, gpointer user_data) {
	RyosconfigIlluminationFrame *illumination_frame = RYOSCONFIG_ILLUMINATION_FRAME(user_data);
	RyosconfigIlluminationFramePrivate *priv = illumination_frame->priv;

	g_return_if_fail(priv->pointer != NULL);

	priv->pointer->effect = ryosconfig_effect_selector_get_effect(priv->effect);
	priv->pointer->fade_delay = ryosconfig_effect_selector_get_delay(priv->effect);
}

static void layer_illumination_dialog_cb(GtkButton *button, gpointer user_data) {
	RyosconfigIlluminationFrame *illumination_frame = RYOSCONFIG_ILLUMINATION_FRAME(user_data);
	gboolean changed;
	g_signal_emit((gpointer)illumination_frame, signals[LAYER_EDITOR], 0, illumination_frame->priv->pointer, &changed);
	if (changed)
		g_signal_emit((gpointer)illumination_frame, signals[LAYER_CHANGED], 0);
}

static void ryosconfig_illumination_frame_init(RyosconfigIlluminationFrame *illumination_frame) {
	RyosconfigIlluminationFramePrivate *priv = RYOSCONFIG_ILLUMINATION_FRAME_GET_PRIVATE(illumination_frame);
	illumination_frame->priv = priv;
	GtkBox *hbox;
	GtkButton *layer_illumination_button;

	priv->pointer = NULL;

	hbox = GTK_BOX(gtk_hbox_new(FALSE, 0));
	layer_illumination_button = GTK_BUTTON(gtk_button_new_with_label(_("Editor")));
	priv->multiwidget = ROCCAT_MULTIWIDGET(roccat_multiwidget_new());
	priv->table = GTK_TABLE(gtk_table_new(3, 2, FALSE));
	priv->nothing = GTK_LABEL(gtk_label_new(_("This model does not support illumination settings")));
	priv->mode = RYOS_LAYER_MODE_SELECTOR(ryos_layer_mode_selector_new());
	priv->layer = RYOS_LAYER_SELECTOR(ryos_layer_selector_new());
	priv->effect = RYOSCONFIG_EFFECT_SELECTOR(ryosconfig_effect_selector_new());

	gtk_label_set_line_wrap(GTK_LABEL(priv->nothing), TRUE);

	g_signal_connect(G_OBJECT(layer_illumination_button), "clicked", G_CALLBACK(layer_illumination_dialog_cb), illumination_frame);
	priv->mode_changed_handler_id = g_signal_connect(G_OBJECT(priv->mode), "changed", G_CALLBACK(mode_changed_cb), illumination_frame);
	priv->layer_changed_handler_id = g_signal_connect(G_OBJECT(priv->layer), "changed", G_CALLBACK(layer_changed_cb), illumination_frame);
	priv->effect_changed_handler_id = g_signal_connect(G_OBJECT(priv->effect), "changed", G_CALLBACK(effect_changed_cb), illumination_frame);

	gtk_box_pack_start(hbox, GTK_WIDGET(priv->layer), TRUE, TRUE, 0);
	gtk_box_pack_start(hbox, GTK_WIDGET(layer_illumination_button), FALSE, FALSE, 0);

	gtk_table_attach(priv->table, gtk_label_new(_("Mode")), 0, 1, 0, 1, 0, GTK_EXPAND, 0, 0);
	gtk_table_attach(priv->table, GTK_WIDGET(priv->mode), 1, 2, 0, 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 0, 0);
	gtk_table_attach(priv->table, gtk_label_new(_("Layer")), 0, 1, 1, 2, 0, GTK_EXPAND, 0, 0);
	gtk_table_attach(priv->table, GTK_WIDGET(hbox), 1, 2, 1, 2, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 0, 0);
	gtk_table_attach(priv->table, gtk_label_new(_("Effect")), 0, 1, 2, 3, 0, GTK_EXPAND, 0, 0);
	gtk_table_attach(priv->table, GTK_WIDGET(priv->effect), 1, 2, 2, 3, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 0, 0);

	roccat_multiwidget_add(priv->multiwidget, GTK_WIDGET(priv->table));
	roccat_multiwidget_add(priv->multiwidget, GTK_WIDGET(priv->nothing));

	gtk_container_add(GTK_CONTAINER(illumination_frame), GTK_WIDGET(priv->multiwidget));

	gtk_frame_set_label(GTK_FRAME(illumination_frame), _("Illumination"));
}

static void ryosconfig_illumination_frame_class_init(RyosconfigIlluminationFrameClass *klass) {
	g_type_class_add_private(klass, sizeof(RyosconfigIlluminationFramePrivate));

	signals[MODE_CHANGED] = g_signal_new("mode-changed",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);

	signals[LAYER_CHANGED] = g_signal_new("layer-changed",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);

	signals[LAYER_EDITOR] = g_signal_new("layer-editor",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_roccat_marshal_BOOLEAN__POINTER, G_TYPE_BOOLEAN, 1, G_TYPE_POINTER);
}

void ryosconfig_illumination_frame_set_device_type(RyosconfigIlluminationFrame *illumination_frame, RoccatDevice const *device) {
	RyosconfigIlluminationFramePrivate *priv = illumination_frame->priv;
	switch (gaminggear_device_get_product_id(GAMINGGEAR_DEVICE(device))) {
	case USB_DEVICE_ID_ROCCAT_RYOS_MK:
	case USB_DEVICE_ID_ROCCAT_RYOS_MK_GLOW:
		roccat_multiwidget_show(priv->multiwidget, GTK_WIDGET(priv->nothing));
		break;
	case USB_DEVICE_ID_ROCCAT_RYOS_MK_PRO:
		roccat_multiwidget_show(priv->multiwidget, GTK_WIDGET(priv->table));
		break;
	default:
		break;
	}
}

void ryosconfig_illumination_frame_set_mode_blocked(RyosconfigIlluminationFrame *illumination_frame, guint mode) {
	RyosconfigIlluminationFramePrivate *priv = illumination_frame->priv;

	g_signal_handler_block(G_OBJECT(priv->mode), priv->mode_changed_handler_id);
	g_signal_handler_block(G_OBJECT(priv->layer), priv->layer_changed_handler_id);

	ryos_layer_mode_selector_set_value(priv->mode, mode);
	ryos_layer_selector_set_mode(priv->layer, mode);

	g_signal_handler_unblock(G_OBJECT(priv->layer), priv->layer_changed_handler_id);
	g_signal_handler_unblock(G_OBJECT(priv->mode), priv->mode_changed_handler_id);
}

void ryosconfig_illumination_frame_set_pointer_blocked(RyosconfigIlluminationFrame *illumination_frame, RyosLightLayer *layer) {
	RyosconfigIlluminationFramePrivate *priv = illumination_frame->priv;

	priv->pointer = layer;

	ryosconfig_effect_selector_set_effect_blocked(priv->effect, layer->effect);
	ryosconfig_effect_selector_set_delay_blocked(priv->effect, layer->fade_delay);
}

guint ryosconfig_illumination_frame_get_mode(RyosconfigIlluminationFrame *illumination_frame) {
	return ryos_layer_mode_selector_get_value(illumination_frame->priv->mode);
}

guint ryosconfig_illumination_frame_get_layer(RyosconfigIlluminationFrame *illumination_frame) {
	return ryos_layer_selector_get_value(illumination_frame->priv->layer);
}
