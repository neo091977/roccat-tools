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

#include <gaminggear/key_translations.h>
#include "ryosmkfx_layer_editor_dialog.h"
#include "ryosmkfx_custom_lights.h"
#include "ryos_stored_lights.h"
#include "ryos_light_control.h"
#include "ryos_key_relations.h"
#include "roccat_swarm_color_selection_button.h"
#include "roccat_warning_dialog.h"
#include "roccat_helper.h"
#include "roccat.h"
#include "gtk_roccat_helper.h"
#include "i18n.h"

#define RYOSMKFX_LAYER_EDITOR_DIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), RYOSMKFX_LAYER_EDITOR_DIALOG_TYPE, RyosmkfxLayerEditorDialogClass))
#define IS_RYOSMKFX_LAYER_EDITOR_DIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), RYOSMKFX_LAYER_EDITOR_DIALOG_TYPE))
#define RYOSMKFX_LAYER_EDITOR_DIALOG_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), RYOSMKFX_LAYER_EDITOR_DIALOG_TYPE, RyosmkfxLayerEditorDialogPrivate))

typedef struct _RyosmkfxLayerEditorDialogClass RyosmkfxLayerEditorDialogClass;
typedef struct _RyosmkfxLayerEditorDialogPrivateData RyosmkfxLayerEditorDialogPrivateData;

struct _RyosmkfxLayerEditorDialogClass {
	GtkDialogClass parent_class;
};

struct _RyosmkfxLayerEditorDialogPrivate {
	RoccatDevice *device;
	RoccatColorSelectionButton *color;
	RyosmkfxLayerEditorDialogPrivateData *data;
};

struct _RyosmkfxLayerEditorDialogPrivateData {
	RyosmkfxLightLayerData light_layer_data;
	RyosmkfxCustomLights custom_lights;
};

G_DEFINE_TYPE(RyosmkfxLayerEditorDialog, ryosmkfx_layer_editor_dialog, GTK_TYPE_DIALOG);

static gchar const * const array_key = "array";

static void ryos_custom_lights_set_from_array(RyosmkfxLightLayerData *light_layer_data, guint8 const *array, gboolean state, GdkColor *color) {
	guint key_index;

	while (*array != RYOS_LIGHT_LAYER_INDEX_INVALID) {
		key_index = *array++;
		light_layer_data->keys[key_index].state = state;
		light_layer_data->keys[key_index].red = color->red;
		light_layer_data->keys[key_index].green = color->green;
		light_layer_data->keys[key_index].blue = color->blue;
	}
}

static void send_custom_lights(RyosmkfxLayerEditorDialog *dialog) {
	RyosmkfxLayerEditorDialogPrivate *priv = dialog->priv;
	GError *local_error = NULL;

	ryosmkfx_light_layer_set_data(&priv->data->custom_lights.light_layer, &priv->data->light_layer_data);
	ryosmkfx_custom_lights_write(priv->device, &priv->data->custom_lights, &local_error);
	if (local_error) {
		g_warning(_("Could not write custom lights: %s"), local_error->message);
		g_clear_error(&local_error);
	}
}

void ryosmkfx_layer_editor_dialog_get_layer_datas(RyosmkfxLayerEditorDialog *dialog, RyosmkfxLightLayerData *light_layer_data) {
	memcpy(light_layer_data, &dialog->priv->data->light_layer_data, sizeof(RyosmkfxLightLayerData));
}

void ryosmkfx_layer_editor_dialog_set_layer_datas(RyosmkfxLayerEditorDialog *dialog, RyosmkfxLightLayerData const *light_layer_data) {
	memcpy(&dialog->priv->data->light_layer_data, light_layer_data, sizeof(RyosmkfxLightLayerData));
}

GtkWidget *ryosmkfx_layer_editor_dialog_new(GtkWindow *parent, RoccatDevice *device) {
	RyosmkfxLayerEditorDialog *layer_illumination_dialog;

	layer_illumination_dialog = g_object_new(RYOSMKFX_LAYER_EDITOR_DIALOG_TYPE, NULL);

	layer_illumination_dialog->priv->device = device;

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
	RyosmkfxLayerEditorDialog *dialog = RYOSMKFX_LAYER_EDITOR_DIALOG(user_data);
	RyosmkfxLayerEditorDialogPrivate *priv = dialog->priv;
	gboolean state;
	guint8 const *data;
	GdkColor color;

	if (gtk_toggle_button_get_inconsistent(togglebutton)) {
		gtk_toggle_button_set_inconsistent(togglebutton, FALSE);
		gtk_toggle_button_set_active(togglebutton, TRUE);
	}

	state = gtk_toggle_button_get_active(togglebutton);
	data = g_object_get_data(G_OBJECT(togglebutton), array_key);
	roccat_color_selection_button_get_color(priv->color, &color);

	ryos_custom_lights_set_from_array(&priv->data->light_layer_data, data, state, &color);

	send_custom_lights(dialog);
}

static void add_array_button(RyosmkfxLayerEditorDialog *layer_illumination_dialog,
		GtkTable *table, gchar const *title, guint8 const *data, guint left_attach, guint top_attach) {
	GtkWidget *button;
	button = gtk_check_button_new_with_label(title);
	gtk_toggle_button_set_inconsistent(GTK_TOGGLE_BUTTON(button), TRUE);
	g_object_set_data(G_OBJECT(button), array_key, (gpointer)data);
	g_signal_connect(G_OBJECT(button), "toggled", G_CALLBACK(array_button_toggled_cb), layer_illumination_dialog);
	gtk_table_attach(table, button, left_attach, left_attach + 1, top_attach, top_attach + 1, GTK_EXPAND, GTK_EXPAND, 0, 0);
}

static gboolean key_press_event_cb(GtkWidget *dialog, GdkEventKey *event, gpointer user_data) {
	RyosmkfxLayerEditorDialog *layer_illumination_dialog = RYOSMKFX_LAYER_EDITOR_DIALOG(dialog);
	RyosmkfxLayerEditorDialogPrivate *priv = layer_illumination_dialog->priv;
	guint8 hid;
	guint8 index;
	gboolean state;
	GdkColor color;

	// FIXME support remap

	hid = gaminggear_xkeycode_to_hid(event->hardware_keycode);
	index = ryos_hid_to_layer_index[hid];
	state = priv->data->light_layer_data.keys[index].state;
	roccat_color_selection_button_get_color(priv->color, &color);

	priv->data->light_layer_data.keys[index].state = !state;
	priv->data->light_layer_data.keys[index].red = color.red;
	priv->data->light_layer_data.keys[index].green = color.green;
	priv->data->light_layer_data.keys[index].blue = color.blue;

	send_custom_lights(layer_illumination_dialog);

	/* don't further process the event (disables return key to exit dialog) */
	return TRUE;
}

static void ryosmkfx_layer_editor_dialog_init(RyosmkfxLayerEditorDialog *layer_illumination_dialog) {
	RyosmkfxLayerEditorDialogPrivate *priv = RYOSMKFX_LAYER_EDITOR_DIALOG_GET_PRIVATE(layer_illumination_dialog);
	GtkVBox *content_area;
	GtkWidget *info_frame;
	GtkWidget *info_text;
	GtkWidget *effect_frame;
	GtkWidget *preset_frame;
	GtkTable *table;
	GdkColor default_color;

	layer_illumination_dialog->priv = priv;
	priv->data = (RyosmkfxLayerEditorDialogPrivateData *)g_malloc0(sizeof(RyosmkfxLayerEditorDialogPrivateData));

	content_area = GTK_VBOX(gtk_dialog_get_content_area(GTK_DIALOG(layer_illumination_dialog)));

	info_frame = gtk_frame_new(_("Info"));
	info_text = gtk_label_new(
			_("Press the keys you want to toggle.\n"
			"The keyboard reflects the settings, except the thumbster keys.\n"
			"Remapping and special keys are not supported."));

	gtk_container_add(GTK_CONTAINER(info_frame), GTK_WIDGET(info_text));

	effect_frame = gtk_frame_new(_("Key effect"));

	priv->color = ROCCAT_COLOR_SELECTION_BUTTON(roccat_swarm_color_selection_button_new());
	gdk_color_parse(ROCCAT_BLUE, &default_color);
	roccat_color_selection_button_set_custom_color(priv->color, &default_color);
	gtk_container_add(GTK_CONTAINER(effect_frame), GTK_WIDGET(priv->color));

	preset_frame = gtk_frame_new(_("Presets"));
	table = GTK_TABLE(gtk_table_new(1, 1, TRUE));

	add_array_button(layer_illumination_dialog, table, _("WASD"), ryos_layer_index_preset_wasd, 0, 0);
	add_array_button(layer_illumination_dialog, table, _("QWER"), ryos_layer_index_preset_qwer, 0, 1);
	add_array_button(layer_illumination_dialog, table, _("Arrows"), ryos_layer_index_preset_arrows, 0, 2);

	add_array_button(layer_illumination_dialog, table, _("F keys"), ryos_layer_index_preset_f_keys, 1, 0);
	add_array_button(layer_illumination_dialog, table, _("M keys"), ryos_layer_index_preset_m_keys, 1, 1);

	add_array_button(layer_illumination_dialog, table, _("Main Block"), ryos_layer_index_preset_main_keys, 2, 0);
	add_array_button(layer_illumination_dialog, table, _("Numpad"), ryos_layer_index_preset_numpad, 2, 1);
	add_array_button(layer_illumination_dialog, table, _("All keys"), ryos_layer_index_preset_all_keys, 2, 2);

	gtk_container_add(GTK_CONTAINER(preset_frame), GTK_WIDGET(table));

	gtk_box_pack_start(GTK_BOX(content_area), info_frame, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(content_area), effect_frame, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(content_area), preset_frame, TRUE, TRUE, 0);

	g_signal_connect(G_OBJECT(layer_illumination_dialog), "key-press-event", G_CALLBACK(key_press_event_cb), NULL);

	gtk_widget_show_all(GTK_WIDGET(content_area));
}

static void finalize(GObject *object) {
	RyosmkfxLayerEditorDialogPrivate *priv = RYOSMKFX_LAYER_EDITOR_DIALOG(object)->priv;
	g_free(priv->data);
	G_OBJECT_CLASS(ryosmkfx_layer_editor_dialog_parent_class)->finalize(object);
}

static void ryosmkfx_layer_editor_dialog_class_init(RyosmkfxLayerEditorDialogClass *klass) {
	GObjectClass *gobject_class;

	gobject_class = (GObjectClass*)klass;
	gobject_class->finalize = finalize;

	g_type_class_add_private(klass, sizeof(RyosmkfxLayerEditorDialogPrivate));
}

static void ryosmkfx_layer_editor_dialog_start(RyosmkfxLayerEditorDialog *dialog) {
	RyosmkfxLayerEditorDialogPrivate *priv = dialog->priv;
	GError *local_error = NULL;

	ryos_light_control_custom(priv->device, TRUE, &local_error);
	if (local_error) {
		(void)roccat_handle_error_dialog(gtk_roccat_widget_get_gtk_window(GTK_WIDGET(dialog)), _("Could not init custom mode"), &local_error);
		return;
	}
	send_custom_lights(dialog);
}

static void ryosmkfx_layer_editor_dialog_stop(RyosmkfxLayerEditorDialog *dialog) {
	RyosmkfxLayerEditorDialogPrivate *priv = dialog->priv;
	GError *local_error = NULL;

	ryos_light_control_custom(priv->device, FALSE, &local_error);
	if (local_error) {
		g_warning(_("Could not deinit custom mode: %s"), local_error->message);
		g_clear_error(&local_error);
	}
}

gboolean ryosmkfx_layer_editor_dialog_run(GtkWindow *parent, RoccatDevice *device, RyosmkfxLightLayerData *light_layer_data) {
	RyosmkfxLayerEditorDialog *dialog;
	gboolean retval = FALSE;


	dialog = RYOSMKFX_LAYER_EDITOR_DIALOG(ryosmkfx_layer_editor_dialog_new(parent, device));
	ryosmkfx_layer_editor_dialog_set_layer_datas(dialog, light_layer_data);

	ryosmkfx_layer_editor_dialog_start(dialog);

	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
		ryosmkfx_layer_editor_dialog_get_layer_datas(dialog, light_layer_data);
		retval = TRUE;
	}

	ryosmkfx_layer_editor_dialog_stop(dialog);

	gtk_widget_destroy(GTK_WIDGET(dialog));

	return retval;
}
