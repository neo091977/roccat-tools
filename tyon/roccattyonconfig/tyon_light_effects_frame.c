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

#include "tyon_light_effects_frame.h"
#include "tyon_profile_settings.h"
#include "gtk_roccat_helper.h"
#include <gaminggear/gaminggear_hscale.h>
#include "i18n.h"

#define TYON_LIGHT_EFFECTS_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), TYON_LIGHT_EFFECTS_FRAME_TYPE, TyonLightEffectsFrameClass))
#define IS_TYON_LIGHT_EFFECTS_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), TYON_LIGHT_EFFECTS_FRAME_TYPE))
#define TYON_LIGHT_EFFECTS_FRAME_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), TYON_LIGHT_EFFECTS_FRAME_TYPE, TyonLightEffectsFramePrivate))

typedef struct _TyonLightEffectsFrameClass TyonLightEffectsFrameClass;
typedef struct _TyonLightEffectsFramePrivate TyonLightEffectsFramePrivate;

struct _TyonLightEffectsFrame {
	GtkFrame parent;
	TyonLightEffectsFramePrivate *priv;
};

struct _TyonLightEffectsFrameClass {
	GtkFrameClass parent_class;
};

struct _TyonLightEffectsFramePrivate {
	GSList *radios_color_flow;
	GSList *radios_effect_type;
	GaminggearHScale *speed;
};

G_DEFINE_TYPE(TyonLightEffectsFrame, tyon_light_effects_frame, GTK_TYPE_FRAME);

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

void tyon_light_effects_frame_set_from_rmp(TyonLightEffectsFrame *frame, TyonRmp *rmp) {
	TyonLightEffectsFramePrivate *priv = frame->priv;
	GtkRadioButton *radio;

	radio = find_radio(priv->radios_color_flow, tyon_rmp_get_light_color_flow(rmp));
	if (radio)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio), TRUE);

	radio = find_radio(priv->radios_effect_type, tyon_rmp_get_light_effect_type(rmp));
	if (radio)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio), TRUE);

	gaminggear_hscale_set_value(priv->speed, tyon_rmp_get_light_effect_speed(rmp));
}

void tyon_light_effects_frame_update_rmp(TyonLightEffectsFrame *frame, TyonRmp *rmp) {
	TyonLightEffectsFramePrivate *priv = frame->priv;
	GtkWidget *active;
	guint value;

	active = gtk_roccat_radio_button_group_get_active(priv->radios_color_flow);
	value = GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(active), value_key));
	tyon_rmp_set_light_color_flow(rmp, value);

	active = gtk_roccat_radio_button_group_get_active(priv->radios_effect_type);
	value = GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(active), value_key));
	tyon_rmp_set_light_effect_type(rmp, value);

	tyon_rmp_set_light_effect_speed(rmp, gaminggear_hscale_get_value(priv->speed));
}

GtkWidget *tyon_light_effects_frame_new(void) {
	TyonLightEffectsFrame *frame;

	frame = TYON_LIGHT_EFFECTS_FRAME(g_object_new(TYON_LIGHT_EFFECTS_FRAME_TYPE,
			NULL));

	return GTK_WIDGET(frame);
}

static void add_radios_color_flow(GtkBox *parent, TyonLightEffectsFrame *frame) {
	TyonLightEffectsFramePrivate *priv = frame->priv;
	GtkWidget *radio;

	radio = gtk_radio_button_new_with_label(NULL, _("Flow direction down"));
	g_object_set_data(G_OBJECT(radio), value_key, GUINT_TO_POINTER(TYON_PROFILE_SETTINGS_COLOR_FLOW_DOWN));

	radio = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio), _("Flow direction up"));
	g_object_set_data(G_OBJECT(radio), value_key, GUINT_TO_POINTER(TYON_PROFILE_SETTINGS_COLOR_FLOW_UP));

	radio = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio), _("All lights simultaneously"));
	g_object_set_data(G_OBJECT(radio), value_key, GUINT_TO_POINTER(TYON_PROFILE_SETTINGS_COLOR_FLOW_SIMULTANEOUSLY));

	radio = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio), _("No color flow"));
	g_object_set_data(G_OBJECT(radio), value_key, GUINT_TO_POINTER(TYON_PROFILE_SETTINGS_COLOR_FLOW_OFF));

	priv->radios_color_flow = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radio));
	g_slist_foreach(priv->radios_color_flow, pack_radio_button, parent);
}

static void add_radios_effect_type(GtkBox *parent, TyonLightEffectsFrame *frame) {
	TyonLightEffectsFramePrivate *priv = frame->priv;
	GtkWidget *radio;

	radio = gtk_radio_button_new_with_label(NULL, _("Heartbeat"));
	g_object_set_data(G_OBJECT(radio), value_key, GUINT_TO_POINTER(TYON_PROFILE_SETTINGS_LIGHT_EFFECT_HEARTBEAT));

	radio = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio), _("Breathing"));
	g_object_set_data(G_OBJECT(radio), value_key, GUINT_TO_POINTER(TYON_PROFILE_SETTINGS_LIGHT_EFFECT_BREATHING));

	radio = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio), _("Blinking"));
	g_object_set_data(G_OBJECT(radio), value_key, GUINT_TO_POINTER(TYON_PROFILE_SETTINGS_LIGHT_EFFECT_BLINKING));

	radio = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio), _("Fully lighted"));
	g_object_set_data(G_OBJECT(radio), value_key, GUINT_TO_POINTER(TYON_PROFILE_SETTINGS_LIGHT_EFFECT_FULLY_LIGHTED));

	radio = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio), _("All lights off"));
	g_object_set_data(G_OBJECT(radio), value_key, GUINT_TO_POINTER(TYON_PROFILE_SETTINGS_LIGHT_EFFECT_ALL_OFF));

	priv->radios_effect_type = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radio));
	g_slist_foreach(priv->radios_effect_type, pack_radio_button, parent);
}

static void add_speed(GtkBox *parent, TyonLightEffectsFrame *frame) {
	TyonLightEffectsFramePrivate *priv = frame->priv;
	GtkWidget *hbox;
	guint i;

	hbox = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(parent, hbox, TRUE, TRUE, 0);

	gtk_box_pack_start(GTK_BOX(hbox), gtk_label_new(_("Effect speed")), FALSE, FALSE, 0);

	priv->speed = GAMINGGEAR_HSCALE(gaminggear_hscale_new_with_range(TYON_PROFILE_SETTINGS_EFFECT_SPEED_MIN, TYON_PROFILE_SETTINGS_EFFECT_SPEED_MAX, 1.0));
	gaminggear_hscale_set_draw_spin(priv->speed, FALSE);

	for (i = TYON_PROFILE_SETTINGS_EFFECT_SPEED_MIN; i <= TYON_PROFILE_SETTINGS_EFFECT_SPEED_MAX; ++i)
		gaminggear_hscale_add_mark(priv->speed, i, GTK_POS_BOTTOM, NULL);

	gtk_box_pack_start(GTK_BOX(hbox), GTK_WIDGET(priv->speed), TRUE, TRUE, 0);
}

static void tyon_light_effects_frame_init(TyonLightEffectsFrame *frame) {
	TyonLightEffectsFramePrivate *priv = TYON_LIGHT_EFFECTS_FRAME_GET_PRIVATE(frame);
	GtkBox *box;

	frame->priv = priv;

	box = GTK_BOX(gtk_vbox_new(FALSE, 0));

	add_radios_color_flow(box, frame);
	gtk_box_pack_start(box, gtk_hseparator_new(), TRUE, TRUE, 0);
	add_radios_effect_type(box, frame);
	gtk_box_pack_start(box, gtk_hseparator_new(), TRUE, TRUE, 0);
	add_speed(box, frame);

	gtk_container_add(GTK_CONTAINER(frame), GTK_WIDGET(box));

	gtk_frame_set_label(GTK_FRAME(frame), _("Light effects"));
}

static void tyon_light_effects_frame_class_init(TyonLightEffectsFrameClass *klass) {
	g_type_class_add_private(klass, sizeof(TyonLightEffectsFramePrivate));
}
