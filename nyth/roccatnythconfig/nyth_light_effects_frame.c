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

#include "nyth_light_effects_frame.h"
#include "nyth_profile_settings.h"
#include "gtk_roccat_helper.h"
#include <gaminggear/gaminggear_hscale.h>
#include "i18n.h"

#define NYTH_LIGHT_EFFECTS_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), NYTH_LIGHT_EFFECTS_FRAME_TYPE, NythLightEffectsFrameClass))
#define IS_NYTH_LIGHT_EFFECTS_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), NYTH_LIGHT_EFFECTS_FRAME_TYPE))
#define NYTH_LIGHT_EFFECTS_FRAME_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), NYTH_LIGHT_EFFECTS_FRAME_TYPE, NythLightEffectsFramePrivate))

typedef struct _NythLightEffectsFrameClass NythLightEffectsFrameClass;
typedef struct _NythLightEffectsFramePrivate NythLightEffectsFramePrivate;

struct _NythLightEffectsFrame {
	GtkFrame parent;
	NythLightEffectsFramePrivate *priv;
};

struct _NythLightEffectsFrameClass {
	GtkFrameClass parent_class;
};

struct _NythLightEffectsFramePrivate {
	GSList *radios_color_effect;
	GSList *radios_light_effect;
	GaminggearHScale *speed;
};

G_DEFINE_TYPE(NythLightEffectsFrame, nyth_light_effects_frame, GTK_TYPE_FRAME);

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

void nyth_light_effects_frame_set_from_profile_data(NythLightEffectsFrame *frame, NythProfileData const *profile_data) {
	NythLightEffectsFramePrivate *priv = frame->priv;

	GtkRadioButton *color_effect = find_radio(priv->radios_color_effect, profile_data->hardware.profile_settings.color_effect);
	GtkRadioButton *light_effect = find_radio(priv->radios_light_effect, profile_data->hardware.profile_settings.light_effect);
	if (color_effect)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(color_effect), TRUE);
	if (light_effect)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(light_effect), TRUE);
	gaminggear_hscale_set_value(priv->speed, profile_data->hardware.profile_settings.effect_speed);
}

void nyth_light_effects_frame_update_profile_data(NythLightEffectsFrame *frame, NythProfileData *profile_data) {
	NythProfileDataHardware *hardware = &profile_data->hardware;
	NythLightEffectsFramePrivate *priv = frame->priv;
	GtkWidget *active;
	guint value;

	active = gtk_roccat_radio_button_group_get_active(priv->radios_color_effect);
	value = GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(active), value_key));
	if (hardware->profile_settings.color_effect != value) {
		hardware->profile_settings.color_effect = value;
		hardware->modified_profile_settings = TRUE;
	}
	active = gtk_roccat_radio_button_group_get_active(priv->radios_light_effect);
	value = GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(active), value_key));
	if (hardware->profile_settings.light_effect != value) {
		hardware->profile_settings.light_effect = value;
		hardware->modified_profile_settings = TRUE;
	}
	value = gaminggear_hscale_get_value(priv->speed);
	if (hardware->profile_settings.effect_speed != value) {
		hardware->profile_settings.effect_speed = value;
		hardware->modified_profile_settings = TRUE;
	}
}

GtkWidget *nyth_light_effects_frame_new(void) {
	NythLightEffectsFrame *frame;

	frame = NYTH_LIGHT_EFFECTS_FRAME(g_object_new(NYTH_LIGHT_EFFECTS_FRAME_TYPE,
			NULL));

	return GTK_WIDGET(frame);
}

static void add_radios_color_effect(GtkBox *parent, NythLightEffectsFrame *frame) {
	NythLightEffectsFramePrivate *priv = frame->priv;
	GtkWidget *radio;

	radio = gtk_radio_button_new_with_label(NULL, _("Flow direction down"));
	g_object_set_data(G_OBJECT(radio), value_key, GUINT_TO_POINTER(NYTH_PROFILE_SETTINGS_COLOR_EFFECT_DOWN));

	radio = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio), _("Flow direction up"));
	g_object_set_data(G_OBJECT(radio), value_key, GUINT_TO_POINTER(NYTH_PROFILE_SETTINGS_COLOR_EFFECT_UP));

	radio = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio), _("All lights simultaneously"));
	g_object_set_data(G_OBJECT(radio), value_key, GUINT_TO_POINTER(NYTH_PROFILE_SETTINGS_COLOR_EFFECT_SIMULTANEOUSLY));

	radio = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio), _("No color flow"));
	g_object_set_data(G_OBJECT(radio), value_key, GUINT_TO_POINTER(NYTH_PROFILE_SETTINGS_COLOR_EFFECT_OFF));

	priv->radios_color_effect = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radio));
	g_slist_foreach(priv->radios_color_effect, pack_radio_button, parent);
}

static void add_radios_light_effect(GtkBox *parent, NythLightEffectsFrame *frame) {
	NythLightEffectsFramePrivate *priv = frame->priv;
	GtkWidget *radio;

	radio = gtk_radio_button_new_with_label(NULL, _("Heartbeat"));
	g_object_set_data(G_OBJECT(radio), value_key, GUINT_TO_POINTER(NYTH_PROFILE_SETTINGS_LIGHT_EFFECT_HEARTBEAT));

	radio = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio), _("Breathing"));
	g_object_set_data(G_OBJECT(radio), value_key, GUINT_TO_POINTER(NYTH_PROFILE_SETTINGS_LIGHT_EFFECT_BREATHING));

	radio = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio), _("Blinking"));
	g_object_set_data(G_OBJECT(radio), value_key, GUINT_TO_POINTER(NYTH_PROFILE_SETTINGS_LIGHT_EFFECT_BLINKING));

	radio = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio), _("Fully lighted"));
	g_object_set_data(G_OBJECT(radio), value_key, GUINT_TO_POINTER(NYTH_PROFILE_SETTINGS_LIGHT_EFFECT_FULLY_LIGHTED));

	radio = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio), _("All lights off"));
	g_object_set_data(G_OBJECT(radio), value_key, GUINT_TO_POINTER(NYTH_PROFILE_SETTINGS_LIGHT_EFFECT_ALL_OFF));

	priv->radios_light_effect = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radio));
	g_slist_foreach(priv->radios_light_effect, pack_radio_button, parent);
}

static void add_speed(GtkBox *parent, NythLightEffectsFrame *frame) {
	NythLightEffectsFramePrivate *priv = frame->priv;
	GtkWidget *hbox;
	guint i;

	hbox = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(parent, hbox, TRUE, TRUE, 0);

	gtk_box_pack_start(GTK_BOX(hbox), gtk_label_new(_("Effect speed")), FALSE, FALSE, 0);

	priv->speed = GAMINGGEAR_HSCALE(gaminggear_hscale_new_with_range(NYTH_PROFILE_SETTINGS_EFFECT_SPEED_MIN, NYTH_PROFILE_SETTINGS_EFFECT_SPEED_MAX, 1.0));
	gaminggear_hscale_set_draw_spin(priv->speed, FALSE);

	for (i = NYTH_PROFILE_SETTINGS_EFFECT_SPEED_MIN; i <= NYTH_PROFILE_SETTINGS_EFFECT_SPEED_MAX; ++i)
		gaminggear_hscale_add_mark(priv->speed, i, GTK_POS_BOTTOM, NULL);

	gtk_box_pack_start(GTK_BOX(hbox), GTK_WIDGET(priv->speed), TRUE, TRUE, 0);
}

static void nyth_light_effects_frame_init(NythLightEffectsFrame *frame) {
	NythLightEffectsFramePrivate *priv = NYTH_LIGHT_EFFECTS_FRAME_GET_PRIVATE(frame);
	GtkBox *box;

	frame->priv = priv;

	box = GTK_BOX(gtk_vbox_new(FALSE, 0));

	add_radios_color_effect(box, frame);
	gtk_box_pack_start(box, gtk_hseparator_new(), TRUE, TRUE, 0);
	add_radios_light_effect(box, frame);
	gtk_box_pack_start(box, gtk_hseparator_new(), TRUE, TRUE, 0);
	add_speed(box, frame);

	gtk_container_add(GTK_CONTAINER(frame), GTK_WIDGET(box));

	gtk_frame_set_label(GTK_FRAME(frame), _("Light effects"));
}

static void nyth_light_effects_frame_class_init(NythLightEffectsFrameClass *klass) {
	g_type_class_add_private(klass, sizeof(NythLightEffectsFramePrivate));
}
