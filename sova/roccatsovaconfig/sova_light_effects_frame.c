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

#include "sova_light_effects_frame.h"
#include "sova_light.h"
#include "gtk_roccat_helper.h"
#include <gaminggear/gaminggear_hscale.h>
#include "i18n.h"

#define SOVA_LIGHT_EFFECTS_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), SOVA_LIGHT_EFFECTS_FRAME_TYPE, SovaLightEffectsFrameClass))
#define IS_SOVA_LIGHT_EFFECTS_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), SOVA_LIGHT_EFFECTS_FRAME_TYPE))
#define SOVA_LIGHT_EFFECTS_FRAME_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), SOVA_LIGHT_EFFECTS_FRAME_TYPE, SovaLightEffectsFramePrivate))

typedef struct _SovaLightEffectsFrameClass SovaLightEffectsFrameClass;
typedef struct _SovaLightEffectsFramePrivate SovaLightEffectsFramePrivate;

struct _SovaLightEffectsFrame {
	GtkFrame parent;
	SovaLightEffectsFramePrivate *priv;
};

struct _SovaLightEffectsFrameClass {
	GtkFrameClass parent_class;
};

struct _SovaLightEffectsFramePrivate {
	GSList *radios_effect_type;
	GaminggearHScale *speed;
	GaminggearHScale *brightness;
};

G_DEFINE_TYPE(SovaLightEffectsFrame, sova_light_effects_frame, GTK_TYPE_FRAME);

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

void sova_light_effects_frame_set_brightness(SovaLightEffectsFrame *frame, guint new_brightness) {
	gaminggear_hscale_set_value(frame->priv->brightness, new_brightness);
}

void sova_light_effects_frame_set_from_profile_data(SovaLightEffectsFrame *frame, SovaProfileData const *profile_data) {
	SovaLightEffectsFramePrivate *priv = frame->priv;
	SovaLight const *light = sova_profile_data_get_light(profile_data);
	GtkRadioButton *radio;

	radio = find_radio(priv->radios_effect_type, light->preset);
	if (radio)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio), TRUE);
	
	gaminggear_hscale_set_value(priv->speed, light->effect_speed);
	sova_light_effects_frame_set_brightness(frame, light->brightness);
}

void sova_light_effects_frame_update_profile_data(SovaLightEffectsFrame *frame, SovaProfileData *profile_data) {
	SovaLightEffectsFramePrivate *priv = frame->priv;
	SovaLight light;
	GtkWidget *active;
	
	sova_light_copy(&light, sova_profile_data_get_light(profile_data));
	
	active = gtk_roccat_radio_button_group_get_active(priv->radios_effect_type);
	light.preset = GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(active), value_key));

	light.effect_speed = gaminggear_hscale_get_value(priv->speed);
	light.brightness = gaminggear_hscale_get_value(priv->brightness);
	
	sova_profile_data_set_light(profile_data, &light);
}

GtkWidget *sova_light_effects_frame_new(void) {
	return GTK_WIDGET(g_object_new(SOVA_LIGHT_EFFECTS_FRAME_TYPE, NULL));
}

static void add_radios_effect_type(GtkBox *parent, SovaLightEffectsFrame *frame) {
	SovaLightEffectsFramePrivate *priv = frame->priv;
	GtkWidget *radio;

	radio = gtk_radio_button_new_with_label(NULL, _("Heatmap"));
	g_object_set_data(G_OBJECT(radio), value_key, GUINT_TO_POINTER(SOVA_LIGHT_PRESET_HEATMAP));

	radio = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio), _("Impulse"));
	g_object_set_data(G_OBJECT(radio), value_key, GUINT_TO_POINTER(SOVA_LIGHT_PRESET_IMPULSE));

	radio = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio), _("Heartbeat"));
	g_object_set_data(G_OBJECT(radio), value_key, GUINT_TO_POINTER(SOVA_LIGHT_PRESET_HEARTBEAT));

	radio = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio), _("Breathing"));
	g_object_set_data(G_OBJECT(radio), value_key, GUINT_TO_POINTER(SOVA_LIGHT_PRESET_BREATHING));

	radio = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio), _("Blinking"));
	g_object_set_data(G_OBJECT(radio), value_key, GUINT_TO_POINTER(SOVA_LIGHT_PRESET_BLINKING));

	radio = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio), _("Fully lighted"));
	g_object_set_data(G_OBJECT(radio), value_key, GUINT_TO_POINTER(SOVA_LIGHT_PRESET_FULLY_LIT));

	radio = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio), _("Off"));
	g_object_set_data(G_OBJECT(radio), value_key, GUINT_TO_POINTER(SOVA_LIGHT_PRESET_OFF));

	priv->radios_effect_type = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radio));
	g_slist_foreach(priv->radios_effect_type, pack_radio_button, parent);
}

static void add_speed(GtkBox *parent, SovaLightEffectsFrame *frame) {
	SovaLightEffectsFramePrivate *priv = frame->priv;
	GtkWidget *hbox;
	guint i;

	hbox = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(parent, hbox, TRUE, TRUE, 0);

	gtk_box_pack_start(GTK_BOX(hbox), gtk_label_new(_("Effect speed")), FALSE, FALSE, 0);

	priv->speed = GAMINGGEAR_HSCALE(gaminggear_hscale_new_with_range(SOVA_LIGHT_EFFECT_SPEED_MIN, SOVA_LIGHT_EFFECT_SPEED_MAX, 1.0));
	gaminggear_hscale_set_draw_spin(priv->speed, FALSE);

	for (i = SOVA_LIGHT_EFFECT_SPEED_MIN; i <= SOVA_LIGHT_EFFECT_SPEED_MAX; ++i)
		gaminggear_hscale_add_mark(priv->speed, i, GTK_POS_BOTTOM, NULL);

	gtk_box_pack_start(GTK_BOX(hbox), GTK_WIDGET(priv->speed), TRUE, TRUE, 0);
}

static void add_brightness(GtkBox *parent, SovaLightEffectsFrame *frame) {
	SovaLightEffectsFramePrivate *priv = frame->priv;
	GtkWidget *hbox;
	guint i;

	hbox = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(parent, hbox, TRUE, TRUE, 0);

	gtk_box_pack_start(GTK_BOX(hbox), gtk_label_new(_("Brightness")), FALSE, FALSE, 0);

	priv->brightness = GAMINGGEAR_HSCALE(gaminggear_hscale_new_with_range(SOVA_LIGHT_BRIGHTNESS_MIN, SOVA_LIGHT_BRIGHTNESS_MAX, 1.0));
	gaminggear_hscale_set_draw_spin(priv->brightness, FALSE);

	for (i = SOVA_LIGHT_BRIGHTNESS_MIN; i <= SOVA_LIGHT_BRIGHTNESS_MAX; ++i)
		gaminggear_hscale_add_mark(priv->brightness, i, GTK_POS_BOTTOM, NULL);

	gtk_box_pack_start(GTK_BOX(hbox), GTK_WIDGET(priv->brightness), TRUE, TRUE, 0);
}

static void sova_light_effects_frame_init(SovaLightEffectsFrame *frame) {
	SovaLightEffectsFramePrivate *priv = SOVA_LIGHT_EFFECTS_FRAME_GET_PRIVATE(frame);
	GtkBox *box;

	frame->priv = priv;

	box = GTK_BOX(gtk_vbox_new(FALSE, 0));

	add_radios_effect_type(box, frame);
	gtk_box_pack_start(box, gtk_hseparator_new(), TRUE, TRUE, 0);
	add_speed(box, frame);
	gtk_box_pack_start(box, gtk_hseparator_new(), TRUE, TRUE, 0);
	add_brightness(box, frame);

	gtk_container_add(GTK_CONTAINER(frame), GTK_WIDGET(box));

	gtk_frame_set_label(GTK_FRAME(frame), _("Light effects"));
}

static void sova_light_effects_frame_class_init(SovaLightEffectsFrameClass *klass) {
	g_type_class_add_private(klass, sizeof(SovaLightEffectsFramePrivate));
}
