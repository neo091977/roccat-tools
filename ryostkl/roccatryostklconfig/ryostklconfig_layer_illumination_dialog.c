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

#include "ryostklconfig_layer_illumination_dialog.h"
#include "ryostklconfig_blink.h"
#include "ryos_custom_lights.h"
#include "ryos_light_control.h"
#include "ryos_key_relations.h"
#include "roccat_warning_dialog.h"
#include "roccat_helper.h"
#include "gtk_roccat_helper.h"
#include <gaminggear/key_translations.h>
#include "i18n.h"

#define RYOSTKLCONFIG_LAYER_ILLUMINATION_DIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), RYOSTKLCONFIG_LAYER_ILLUMINATION_DIALOG_TYPE, RyostklconfigLayerIlluminationDialogClass))
#define IS_RYOSTKLCONFIG_LAYER_ILLUMINATION_DIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), RYOSTKLCONFIG_LAYER_ILLUMINATION_DIALOG_TYPE))
#define RYOSTKLCONFIG_LAYER_ILLUMINATION_DIALOG_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), RYOSTKLCONFIG_LAYER_ILLUMINATION_DIALOG_TYPE, RyostklconfigLayerIlluminationDialogPrivate))

typedef struct _RyostklconfigLayerIlluminationDialogClass RyostklconfigLayerIlluminationDialogClass;

struct _RyostklconfigLayerIlluminationDialogClass {
	GtkDialogClass parent_class;
};

struct _RyostklconfigLayerIlluminationDialogPrivate {
	RoccatDevice *ryos;
	RyostklLightLayer *light_layer;
	RyostklconfigBlink *blink;
	GtkToggleButton *off_button;
	GtkToggleButton *blink_button;
	GtkToggleButton *breath_button;
	GtkSpinButton *blink_time;
	GtkSpinButton *breath_time;
};

G_DEFINE_TYPE(RyostklconfigLayerIlluminationDialog, ryostklconfig_layer_illumination_dialog, GTK_TYPE_DIALOG);

static gchar const * const array_key = "array";

static guint16 key_state(RyostklconfigLayerIlluminationDialog *dialog, gboolean state) {
	RyostklconfigLayerIlluminationDialogPrivate *priv = dialog->priv;
	guint effect;
	guint blink_speed;
	guint breath_speed;

	if (gtk_toggle_button_get_active(priv->blink_button))
		effect = RYOSTKL_LIGHT_LAYER_KEY_EFFECT_BLINK;
	else if (gtk_toggle_button_get_active(priv->breath_button))
		effect = RYOSTKL_LIGHT_LAYER_KEY_EFFECT_BREATH;
	else
		effect = RYOSTKL_LIGHT_LAYER_KEY_EFFECT_OFF;

	blink_speed = (guint)(gtk_spin_button_get_value(priv->blink_time) + 0.5);
	breath_speed = (guint)(gtk_spin_button_get_value(priv->breath_time) + 0.5);

	return ryostkl_light_layer_key_create(state, effect, blink_speed, breath_speed);
}

static void ryos_custom_lights_set_from_array(RyostklLightLayer *layer, guint8 const *array, guint16 state) {
	while (*array != RYOS_LIGHT_LAYER_INDEX_INVALID)
		layer->keys[*array++] = state;
}

void ryostklconfig_layer_illumination_dialog_get_layer(RyostklconfigLayerIlluminationDialog *dialog, RyostklLightLayer *light_layer) {
	ryostkl_light_layer_copy(light_layer, dialog->priv->light_layer);
}

void ryostklconfig_layer_illumination_dialog_set_layer(RyostklconfigLayerIlluminationDialog *dialog, RyostklLightLayer const *light_layer) {
	ryostkl_light_layer_copy(dialog->priv->light_layer, light_layer);
}

GtkWidget *ryostklconfig_layer_illumination_dialog_new(GtkWindow *parent, RoccatDevice *ryos) {
	RyostklconfigLayerIlluminationDialog *layer_illumination_dialog;

	layer_illumination_dialog = g_object_new(RYOSTKLCONFIG_LAYER_ILLUMINATION_DIALOG_TYPE, NULL);

	layer_illumination_dialog->priv->ryos = ryos;

	gtk_dialog_add_buttons(GTK_DIALOG(layer_illumination_dialog),
			GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			NULL);
	gtk_window_set_title(GTK_WINDOW(layer_illumination_dialog), _("Edit layer illumination"));
	gtk_window_set_transient_for(GTK_WINDOW(layer_illumination_dialog), parent);
	gtk_window_set_modal(GTK_WINDOW(layer_illumination_dialog), TRUE);
	gtk_window_set_destroy_with_parent(GTK_WINDOW(layer_illumination_dialog), TRUE);
	gtk_window_set_type_hint(GTK_WINDOW(layer_illumination_dialog), GDK_WINDOW_TYPE_HINT_NORMAL);

	return GTK_WIDGET(layer_illumination_dialog);
}

static void array_button_toggled_cb(GtkToggleButton *togglebutton, gpointer user_data) {
	RyostklconfigLayerIlluminationDialog *dialog = RYOSTKLCONFIG_LAYER_ILLUMINATION_DIALOG(user_data);
	RyostklconfigLayerIlluminationDialogPrivate *priv = dialog->priv;
	guint16 value;
	guint8 const *data;

	if (gtk_toggle_button_get_inconsistent(togglebutton)) {
		gtk_toggle_button_set_inconsistent(togglebutton, FALSE);
		gtk_toggle_button_set_active(togglebutton, TRUE);
	}

	value = key_state(dialog, gtk_toggle_button_get_active(togglebutton));
	data = g_object_get_data(G_OBJECT(togglebutton), array_key);
	ryos_custom_lights_set_from_array(priv->light_layer, data, value);
}

static void add_array_button(RyostklconfigLayerIlluminationDialog *layer_illumination_dialog,
		GtkTable *table, gchar const *title, guint8 const *data, guint left_attach, guint top_attach) {
	GtkWidget *button;
	button = gtk_check_button_new_with_label(title);
	gtk_toggle_button_set_inconsistent(GTK_TOGGLE_BUTTON(button), TRUE);
	g_object_set_data(G_OBJECT(button), array_key, (gpointer)data);
	g_signal_connect(G_OBJECT(button), "toggled", G_CALLBACK(array_button_toggled_cb), layer_illumination_dialog);
	gtk_table_attach(table, button, left_attach, left_attach + 1, top_attach, top_attach + 1, GTK_EXPAND, GTK_EXPAND, 0, 0);
}

static gboolean key_press_event_cb(GtkWidget *dialog, GdkEventKey *event, gpointer user_data) {
	RyostklconfigLayerIlluminationDialog *layer_illumination_dialog = RYOSTKLCONFIG_LAYER_ILLUMINATION_DIALOG(dialog);
	RyostklconfigLayerIlluminationDialogPrivate *priv = layer_illumination_dialog->priv;
	guint8 hid;
	guint8 index;
	gboolean on;

	// FIXME support remap

	hid = gaminggear_xkeycode_to_hid(event->hardware_keycode);
	index = ryos_hid_to_layer_index[hid];
	
	on = priv->light_layer->keys[index] != 0;
	priv->light_layer->keys[index] = key_state(layer_illumination_dialog, !on);

	/* don't further process the event (disables return key to exit dialog) */
	return TRUE;
}

static void timeout_cb(RyostklconfigBlink *blink, gpointer user_data) {
	RyostklconfigLayerIlluminationDialog *dialog = RYOSTKLCONFIG_LAYER_ILLUMINATION_DIALOG(user_data);
	RyostklconfigLayerIlluminationDialogPrivate *priv = dialog->priv;
	GError *local_error = NULL;
	RyosCustomLights custom_lights;

	ryostklconfig_blink_light_layer_to_custom_lights(blink, priv->light_layer, &custom_lights);
	ryos_custom_lights_write(priv->ryos, &custom_lights, &local_error);
	if (local_error) {
		g_warning(_("Could not write custom lights: %s"), local_error->message);
		g_clear_error(&local_error);
	}
}

static void ryostklconfig_layer_illumination_dialog_init(RyostklconfigLayerIlluminationDialog *layer_illumination_dialog) {
	RyostklconfigLayerIlluminationDialogPrivate *priv = RYOSTKLCONFIG_LAYER_ILLUMINATION_DIALOG_GET_PRIVATE(layer_illumination_dialog);
	GtkVBox *content_area;
	GtkWidget *blink_box;
	GtkWidget *breath_box;
	GtkWidget *effect_box;
	GtkWidget *info_frame;
	GtkWidget *info_text;
	GtkWidget *effect_frame;
	GtkWidget *preset_frame;
	GtkTable *table;

	layer_illumination_dialog->priv = priv;
	priv->light_layer = g_malloc0(sizeof(RyostklLightLayer));

	priv->blink = ryostklconfig_blink_new();
	g_signal_connect(G_OBJECT(priv->blink), "timeout", G_CALLBACK(timeout_cb), layer_illumination_dialog);

	content_area = GTK_VBOX(gtk_dialog_get_content_area(GTK_DIALOG(layer_illumination_dialog)));

	info_frame = gtk_frame_new(_("Info"));
	info_text = gtk_label_new(
			_("Press the keys you want to toggle.\n"
			"The keyboard reflects the settings, except the thumbster keys.\n"
			"Remapping and special keys are not supported."));

	gtk_container_add(GTK_CONTAINER(info_frame), GTK_WIDGET(info_text));

	effect_frame = gtk_frame_new(_("Key effect"));
	blink_box = gtk_hbox_new(FALSE, 0);
	breath_box = gtk_hbox_new(FALSE, 0);
	effect_box = gtk_hbox_new(FALSE, 0);
	priv->off_button = GTK_TOGGLE_BUTTON(gtk_radio_button_new_with_label(NULL, _("Off")));
	priv->blink_button = GTK_TOGGLE_BUTTON(gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(priv->off_button), _("Blinking")));
	priv->breath_button = GTK_TOGGLE_BUTTON(gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(priv->blink_button), _("Breathing")));
	priv->blink_time = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(RYOSTKL_LIGHT_LAYER_KEY_EFFECT_SPEED_MIN, RYOSTKL_LIGHT_LAYER_KEY_EFFECT_SPEED_MAX, 1.0));
	priv->breath_time = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(RYOSTKL_LIGHT_LAYER_KEY_EFFECT_SPEED_MIN, RYOSTKL_LIGHT_LAYER_KEY_EFFECT_SPEED_MAX, 1.0));

	gtk_box_pack_start(GTK_BOX(blink_box), GTK_WIDGET(priv->blink_button), TRUE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(blink_box), GTK_WIDGET(priv->blink_time), TRUE, FALSE, 0);

	gtk_box_pack_start(GTK_BOX(breath_box), GTK_WIDGET(priv->breath_button), TRUE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(breath_box), GTK_WIDGET(priv->breath_time), TRUE, FALSE, 0);

	gtk_box_pack_start(GTK_BOX(effect_box), GTK_WIDGET(priv->off_button), TRUE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(effect_box), blink_box, TRUE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(effect_box), breath_box, TRUE, FALSE, 0);

	gtk_container_add(GTK_CONTAINER(effect_frame), effect_box);

	preset_frame = gtk_frame_new(_("Presets"));
	table = GTK_TABLE(gtk_table_new(1, 1, TRUE));

	add_array_button(layer_illumination_dialog, table, _("WASD"), ryos_layer_index_preset_wasd, 0, 0);
	add_array_button(layer_illumination_dialog, table, _("QWER"), ryos_layer_index_preset_qwer, 0, 1);
	add_array_button(layer_illumination_dialog, table, _("Arrows"), ryos_layer_index_preset_arrows, 0, 2);

	add_array_button(layer_illumination_dialog, table, _("F keys"), ryos_layer_index_preset_f_keys, 1, 0);
	add_array_button(layer_illumination_dialog, table, _("T keys"), ryos_layer_index_preset_t_keys, 1, 2);

	add_array_button(layer_illumination_dialog, table, _("Main Block"), ryos_layer_index_preset_main_keys, 2, 0);
	add_array_button(layer_illumination_dialog, table, _("All keys"), ryos_layer_index_preset_all_keys, 2, 2);

	gtk_container_add(GTK_CONTAINER(preset_frame), GTK_WIDGET(table));

	gtk_box_pack_start(GTK_BOX(content_area), info_frame, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(content_area), effect_frame, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(content_area), preset_frame, TRUE, TRUE, 0);

	g_signal_connect(G_OBJECT(layer_illumination_dialog), "key-press-event", G_CALLBACK(key_press_event_cb), NULL);

	gtk_widget_show_all(GTK_WIDGET(content_area));
}

static void finalize(GObject *object) {
	RyostklconfigLayerIlluminationDialogPrivate *priv = RYOSTKLCONFIG_LAYER_ILLUMINATION_DIALOG(object)->priv;
	g_free(priv->light_layer);
	G_OBJECT_CLASS(ryostklconfig_layer_illumination_dialog_parent_class)->finalize(object);
}

static void ryostklconfig_layer_illumination_dialog_class_init(RyostklconfigLayerIlluminationDialogClass *klass) {
	GObjectClass *gobject_class;

	gobject_class = (GObjectClass*)klass;
	gobject_class->finalize = finalize;

	g_type_class_add_private(klass, sizeof(RyostklconfigLayerIlluminationDialogPrivate));
}

static void ryostklconfig_layer_illumination_dialog_start(RyostklconfigLayerIlluminationDialog *dialog) {
	RyostklconfigLayerIlluminationDialogPrivate *priv = dialog->priv;
	GError *local_error = NULL;

	ryos_light_control_custom(priv->ryos, TRUE, &local_error);
	if (local_error)
		(void)roccat_handle_error_dialog(gtk_roccat_widget_get_gtk_window(GTK_WIDGET(dialog)), _("Could not init custom mode"), &local_error);

	ryostklconfig_blink_start(priv->blink);
}

static void ryostklconfig_layer_illumination_dialog_stop(RyostklconfigLayerIlluminationDialog *dialog) {
	RyostklconfigLayerIlluminationDialogPrivate *priv = dialog->priv;
	GError *local_error = NULL;

	ryostklconfig_blink_stop(priv->blink);

	ryos_light_control_custom(priv->ryos, FALSE, &local_error);
	if (local_error) {
		g_warning(_("Could not deinit custom mode: %s"), local_error->message);
		g_clear_error(&local_error);
	}
}

gboolean ryostklconfig_layer_illumination_dialog_run(GtkWindow *parent, RoccatDevice *ryos, RyostklLightLayer *light_layer) {
	RyostklconfigLayerIlluminationDialog *dialog;
	gboolean retval = FALSE;


	dialog = RYOSTKLCONFIG_LAYER_ILLUMINATION_DIALOG(ryostklconfig_layer_illumination_dialog_new(parent, ryos));
	ryostklconfig_layer_illumination_dialog_set_layer(dialog, light_layer);

	ryostklconfig_layer_illumination_dialog_start(dialog);

	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
		ryostklconfig_layer_illumination_dialog_get_layer(dialog, light_layer);
		retval = TRUE;
	}

	ryostklconfig_layer_illumination_dialog_stop(dialog);

	gtk_widget_destroy(GTK_WIDGET(dialog));

	return retval;
}
