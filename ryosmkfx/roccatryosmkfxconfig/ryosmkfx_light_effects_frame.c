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

#include "ryosmkfx_light_effects_frame.h"
#include "ryosmkfx_light.h"
#include "gtk_roccat_helper.h"
#include "i18n.h"
#include <gaminggear/gaminggear_hscale.h>
#include <string.h>

#define RYOSMKFX_LIGHT_EFFECTS_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), RYOSMKFX_LIGHT_EFFECTS_FRAME_TYPE, RyosmkfxLightEffectsFrameClass))
#define IS_RYOSMKFX_LIGHT_EFFECTS_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), RYOSMKFX_LIGHT_EFFECTS_FRAME_TYPE))
#define RYOSMKFX_LIGHT_EFFECTS_FRAME_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), RYOSMKFX_LIGHT_EFFECTS_FRAME_TYPE, RyosmkfxLightEffectsFramePrivate))

typedef struct _RyosmkfxLightEffectsFrameClass RyosmkfxLightEffectsFrameClass;
typedef struct _RyosmkfxLightEffectsFramePrivate RyosmkfxLightEffectsFramePrivate;

struct _RyosmkfxLightEffectsFrame {
	GtkFrame parent;
	RyosmkfxLightEffectsFramePrivate *priv;
};

struct _RyosmkfxLightEffectsFrameClass {
	GtkFrameClass parent_class;
};

struct _RyosmkfxLightEffectsFramePrivate {
	GSList *radios_effect;
	GtkFileChooser *script;
	GaminggearHScale *speed;
};

enum {
	EFFECT_CHANGED,
	SCRIPT_CHANGED,
	SPEED_CHANGED,
	LAST_SIGNAL,
};

static guint signals[LAST_SIGNAL] = { 0 };

G_DEFINE_TYPE(RyosmkfxLightEffectsFrame, ryosmkfx_light_effects_frame, GTK_TYPE_FRAME);

static gchar const * const value_key = "value";

static void pack_radio_button(gpointer button, gpointer parent) {
	gtk_box_pack_start(parent, button, TRUE, TRUE, 0);
}

static GtkRadioButton *find_radio(GSList *list, guint wanted) {
	GSList *child;
	guint value;

	for (child = list; child; child = g_slist_next(child)) {
		value = GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(child->data), value_key));
		if (value == wanted)
			return child->data;
	}
	return NULL;
}

static void effect_toggled_cb(GtkToggleButton *radio, gpointer user_data) {
	if (gtk_toggle_button_get_active(radio))
		g_signal_emit(user_data, signals[EFFECT_CHANGED], 0);
}

static void script_changed_cb(GtkFileChooser *chooser, gpointer user_data) {
	g_signal_emit(user_data, signals[SCRIPT_CHANGED], 0);
}

static void speed_changed_cb(GaminggearHScale *hscale, gpointer user_data) {
	g_signal_emit(user_data, signals[SPEED_CHANGED], 0);
}

void ryosmkfx_light_effects_frame_set_from_profile_data(RyosmkfxLightEffectsFrame *frame, RyosmkfxProfileData const *profile_data, RoccatKeyFile *config) {
	RyosmkfxLightEffectsFramePrivate *priv = frame->priv;
	gchar *path;
	GSList *child;
	
	for (child = priv->radios_effect; child; child = g_slist_next(child)) {
		g_signal_handlers_block_by_func(G_OBJECT(child->data), effect_toggled_cb, frame);
	}
	g_signal_handlers_block_by_func(G_OBJECT(priv->speed), speed_changed_cb, frame);
	g_signal_handlers_block_by_func(G_OBJECT(priv->script), script_changed_cb, frame);

	GtkRadioButton *effect = find_radio(priv->radios_effect, profile_data->hardware.light.effect);
	if (effect)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(effect), TRUE);

	if (strlen(profile_data->eventhandler.effect_script) > 0)
		gtk_file_chooser_set_filename(priv->script, profile_data->eventhandler.effect_script);
	else if (config) {
		path = ryosmkfx_configuration_get_script_path(config);
		gtk_file_chooser_set_current_folder(priv->script, path);
		g_free(path);
	}

	gaminggear_hscale_set_value(priv->speed, profile_data->hardware.light.effect_speed);

	for (child = priv->radios_effect; child; child = g_slist_next(child)) {
		g_signal_handlers_unblock_by_func(G_OBJECT(child->data), effect_toggled_cb, frame);
	}
	g_signal_handlers_unblock_by_func(G_OBJECT(priv->speed), speed_changed_cb, frame);
	g_signal_handlers_unblock_by_func(G_OBJECT(priv->script), script_changed_cb, frame);
}

gchar *ryosmkfx_light_effects_frame_get_script_path(RyosmkfxLightEffectsFrame *frame) {
	return gtk_file_chooser_get_filename(frame->priv->script);
}

guint ryosmkfx_light_effects_frame_get_effect(RyosmkfxLightEffectsFrame *frame) {
	GtkWidget *active = gtk_roccat_radio_button_group_get_active(frame->priv->radios_effect);
	return GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(active), value_key));
}

guint ryosmkfx_light_effects_frame_get_speed(RyosmkfxLightEffectsFrame *frame) {
	return gaminggear_hscale_get_value(frame->priv->speed);
}
	
void ryosmkfx_light_effects_frame_update_profile_data(RyosmkfxLightEffectsFrame *frame, RyosmkfxProfileData *profile_data, RoccatKeyFile *config) {
	RyosmkfxProfileDataHardware *hardware = &profile_data->hardware;
	guint value;
	gchar *filename;
	gchar *path;
	
	value = ryosmkfx_light_effects_frame_get_effect(frame);
	if (hardware->light.effect != value) {
		hardware->light.effect = value;
		hardware->modified_light = TRUE;
	}

	filename = ryosmkfx_light_effects_frame_get_script_path(frame);
	if (filename) {
		if (config) {
			path = g_path_get_dirname(filename);
			ryosmkfx_configuration_set_script_path(config, path);
			g_free(path);
		}
		ryosmkfx_profile_data_eventhandler_set_effect_script_name(&profile_data->eventhandler, filename);
		g_free(filename);
	}
	
	value = ryosmkfx_light_effects_frame_get_speed(frame);
	if (hardware->light.effect_speed != value) {
		hardware->light.effect_speed = value;
		hardware->modified_light = TRUE;
	}
}

GtkWidget *ryosmkfx_light_effects_frame_new(void) {
	RyosmkfxLightEffectsFrame *frame;

	frame = RYOSMKFX_LIGHT_EFFECTS_FRAME(g_object_new(RYOSMKFX_LIGHT_EFFECTS_FRAME_TYPE, NULL));

	return GTK_WIDGET(frame);
}

static void add_radios_effect(GtkBox *parent, RyosmkfxLightEffectsFrame *frame) {
	RyosmkfxLightEffectsFramePrivate *priv = frame->priv;
	GtkWidget *radio;

	radio = gtk_radio_button_new_with_label(NULL, _("Script FX"));
	g_object_set_data(G_OBJECT(radio), value_key, GUINT_TO_POINTER(RYOSMKFX_LIGHT_EFFECT_RIPPLE_FX));
	g_signal_connect(G_OBJECT(radio), "toggled", G_CALLBACK(effect_toggled_cb), frame);

	radio = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio), _("Heartbeat"));
	g_object_set_data(G_OBJECT(radio), value_key, GUINT_TO_POINTER(RYOSMKFX_LIGHT_EFFECT_HEARTBEAT));
	g_signal_connect(G_OBJECT(radio), "toggled", G_CALLBACK(effect_toggled_cb), frame);

	radio = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio), _("Breathing"));
	g_object_set_data(G_OBJECT(radio), value_key, GUINT_TO_POINTER(RYOSMKFX_LIGHT_EFFECT_BREATHING));
	g_signal_connect(G_OBJECT(radio), "toggled", G_CALLBACK(effect_toggled_cb), frame);

	radio = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio), _("Blinking"));
	g_object_set_data(G_OBJECT(radio), value_key, GUINT_TO_POINTER(RYOSMKFX_LIGHT_EFFECT_BLINKING));
	g_signal_connect(G_OBJECT(radio), "toggled", G_CALLBACK(effect_toggled_cb), frame);

	radio = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio), _("Fully lighted"));
	g_object_set_data(G_OBJECT(radio), value_key, GUINT_TO_POINTER(RYOSMKFX_LIGHT_EFFECT_FULLY_LIT));
	g_signal_connect(G_OBJECT(radio), "toggled", G_CALLBACK(effect_toggled_cb), frame);

	radio = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio), _("All lights off"));
	g_object_set_data(G_OBJECT(radio), value_key, GUINT_TO_POINTER(RYOSMKFX_LIGHT_EFFECT_OFF));
	g_signal_connect(G_OBJECT(radio), "toggled", G_CALLBACK(effect_toggled_cb), frame);

	priv->radios_effect = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radio));
	g_slist_foreach(priv->radios_effect, pack_radio_button, parent);
}

static void add_file_filter_to_file_chooser(GtkFileChooser *chooser) {
	GtkFileFilter *filter;
	filter = gtk_file_filter_new();
	gtk_file_filter_set_name(filter, _("Lua script file"));
	gtk_file_filter_add_pattern(filter, "*.lua");
	gtk_file_filter_add_pattern(filter, "*.lc");
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(chooser), filter);

	filter = gtk_file_filter_new();
	gtk_file_filter_set_name(filter, _("All files"));
	gtk_file_filter_add_pattern(filter, "*");
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(chooser), filter);
}

static void add_script_effect(GtkBox *parent, RyosmkfxLightEffectsFrame *frame) {
	RyosmkfxLightEffectsFramePrivate *priv = frame->priv;
	priv->script = GTK_FILE_CHOOSER(gtk_file_chooser_button_new(_("Select script"), GTK_FILE_CHOOSER_ACTION_OPEN));
	add_file_filter_to_file_chooser(priv->script);
	gtk_box_pack_start(parent, GTK_WIDGET(priv->script), TRUE, TRUE, 0);
	g_signal_connect(G_OBJECT(priv->script), "file-set", G_CALLBACK(script_changed_cb), frame);
}

static void add_speed(GtkBox *parent, RyosmkfxLightEffectsFrame *frame) {
	RyosmkfxLightEffectsFramePrivate *priv = frame->priv;
	GtkWidget *hbox;
	guint i;

	hbox = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(parent, hbox, TRUE, TRUE, 0);

	gtk_box_pack_start(GTK_BOX(hbox), gtk_label_new(_("Effect speed")), FALSE, FALSE, 0);

	priv->speed = GAMINGGEAR_HSCALE(gaminggear_hscale_new_with_range(RYOSMKFX_LIGHT_EFFECT_SPEED_MIN, RYOSMKFX_LIGHT_EFFECT_SPEED_MAX, 1.0));
	gaminggear_hscale_set_draw_spin(priv->speed, FALSE);

	for (i = RYOSMKFX_LIGHT_EFFECT_SPEED_MIN; i <= RYOSMKFX_LIGHT_EFFECT_SPEED_MAX; ++i)
		gaminggear_hscale_add_mark(priv->speed, i, GTK_POS_BOTTOM, NULL);

	gtk_box_pack_start(GTK_BOX(hbox), GTK_WIDGET(priv->speed), TRUE, TRUE, 0);
	g_signal_connect(G_OBJECT(priv->speed), "value-changed", G_CALLBACK(speed_changed_cb), frame);
}

static void ryosmkfx_light_effects_frame_init(RyosmkfxLightEffectsFrame *frame) {
	RyosmkfxLightEffectsFramePrivate *priv = RYOSMKFX_LIGHT_EFFECTS_FRAME_GET_PRIVATE(frame);
	GtkBox *box;

	frame->priv = priv;

	box = GTK_BOX(gtk_vbox_new(FALSE, 0));

	add_radios_effect(box, frame);
	add_script_effect(box, frame);
	gtk_box_pack_start(box, gtk_hseparator_new(), TRUE, TRUE, 0);
	add_speed(box, frame);

	gtk_container_add(GTK_CONTAINER(frame), GTK_WIDGET(box));

	gtk_frame_set_label(GTK_FRAME(frame), _("Light effects"));
}

static void ryosmkfx_light_effects_frame_class_init(RyosmkfxLightEffectsFrameClass *klass) {
	g_type_class_add_private(klass, sizeof(RyosmkfxLightEffectsFramePrivate));

	signals[EFFECT_CHANGED] = g_signal_new("effect-changed",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);

	signals[SCRIPT_CHANGED] = g_signal_new("script-changed",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);

	signals[SPEED_CHANGED] = g_signal_new("speed-changed",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);
}
