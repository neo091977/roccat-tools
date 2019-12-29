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

#include "suora_illumination_frame.h"
#include "suora_illumination.h"
#include "gtk_roccat_helper.h"
#include <gaminggear/gaminggear_hscale.h>
#include "i18n.h"

#define SUORA_ILLUMINATION_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), SUORA_ILLUMINATION_FRAME_TYPE, SuoraIlluminationFrameClass))
#define IS_SUORA_ILLUMINATION_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), SUORA_ILLUMINATION_FRAME_TYPE))
#define SUORA_ILLUMINATION_FRAME_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), SUORA_ILLUMINATION_FRAME_TYPE, SuoraIlluminationFramePrivate))

typedef struct _SuoraIlluminationFrameClass SuoraIlluminationFrameClass;
typedef struct _SuoraIlluminationFramePrivate SuoraIlluminationFramePrivate;

struct _SuoraIlluminationFrame {
	GtkFrame parent;
	SuoraIlluminationFramePrivate *priv;
};

struct _SuoraIlluminationFrameClass {
	GtkFrameClass parent_class;
};

struct _SuoraIlluminationFramePrivate {
	GSList *radios_light_effect;
	GaminggearHScale *brightness;
};

G_DEFINE_TYPE(SuoraIlluminationFrame, suora_illumination_frame, GTK_TYPE_FRAME);

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

void suora_illumination_frame_set_from_profile_data(SuoraIlluminationFrame *frame, SuoraProfileData const *profile_data) {
	SuoraIlluminationFramePrivate *priv = frame->priv;
	SuoraIllumination const *illumination = suora_profile_data_get_illumination(profile_data);

	GtkRadioButton *light_effect = find_radio(priv->radios_light_effect, illumination->effect);
	if (light_effect)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(light_effect), TRUE);
	gaminggear_hscale_set_value(priv->brightness, illumination->brightness);
}

void suora_illumination_frame_update_profile_data(SuoraIlluminationFrame *frame, SuoraProfileData *profile_data) {
	SuoraIlluminationFramePrivate *priv = frame->priv;
	SuoraIllumination *illumination;
	GtkWidget *active;

	illumination = suora_illumination_dup(suora_profile_data_get_illumination(profile_data));

	active = gtk_roccat_radio_button_group_get_active(priv->radios_light_effect);
	illumination->effect = GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(active), value_key));

	if (illumination->effect == SUORA_ILLUMINATION_EFFECT_BREATHING)
		illumination->brightness = SUORA_ILLUMINATION_BRIGHTNESS_BREATHING;
	else
		illumination->brightness = gaminggear_hscale_get_value(priv->brightness);

	suora_profile_data_set_illumination(profile_data, illumination);
	g_free(illumination);
}

GtkWidget *suora_illumination_frame_new(void) {
	SuoraIlluminationFrame *frame;

	frame = SUORA_ILLUMINATION_FRAME(g_object_new(SUORA_ILLUMINATION_FRAME_TYPE,
			NULL));

	return GTK_WIDGET(frame);
}

static void add_radios_light_effect(GtkBox *parent, SuoraIlluminationFrame *frame) {
	SuoraIlluminationFramePrivate *priv = frame->priv;
	GtkWidget *radio;

	radio = gtk_radio_button_new_with_label(NULL, _("Breathing"));
	g_object_set_data(G_OBJECT(radio), value_key, GUINT_TO_POINTER(SUORA_ILLUMINATION_EFFECT_BREATHING));

	radio = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio), _("Fully lighted"));
	g_object_set_data(G_OBJECT(radio), value_key, GUINT_TO_POINTER(SUORA_ILLUMINATION_EFFECT_FULLY_LIT));

	radio = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio), _("All lights off"));
	g_object_set_data(G_OBJECT(radio), value_key, GUINT_TO_POINTER(SUORA_ILLUMINATION_EFFECT_OFF));

	priv->radios_light_effect = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radio));
	g_slist_foreach(priv->radios_light_effect, pack_radio_button, parent);
}

static void add_brightness(GtkBox *parent, SuoraIlluminationFrame *frame) {
	SuoraIlluminationFramePrivate *priv = frame->priv;
	GtkWidget *hbox;
	guint i;

	hbox = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(parent, hbox, TRUE, TRUE, 0);

	gtk_box_pack_start(GTK_BOX(hbox), gtk_label_new(_("Brightness")), FALSE, FALSE, 0);

	priv->brightness = GAMINGGEAR_HSCALE(gaminggear_hscale_new_with_range(SUORA_ILLUMINATION_BRIGHTNESS_MIN, SUORA_ILLUMINATION_BRIGHTNESS_MAX, 1.0));
	gaminggear_hscale_set_draw_spin(priv->brightness, FALSE);

	for (i = SUORA_ILLUMINATION_BRIGHTNESS_MIN; i <= SUORA_ILLUMINATION_BRIGHTNESS_MAX; ++i)
		gaminggear_hscale_add_mark(priv->brightness, i, GTK_POS_BOTTOM, NULL);

	gtk_box_pack_start(GTK_BOX(hbox), GTK_WIDGET(priv->brightness), TRUE, TRUE, 0);
}

static void suora_illumination_frame_init(SuoraIlluminationFrame *frame) {
	SuoraIlluminationFramePrivate *priv = SUORA_ILLUMINATION_FRAME_GET_PRIVATE(frame);
	GtkBox *box;

	frame->priv = priv;

	box = GTK_BOX(gtk_vbox_new(FALSE, 0));

	add_radios_light_effect(box, frame);
	gtk_box_pack_start(box, gtk_hseparator_new(), TRUE, TRUE, 0);
	add_brightness(box, frame);

	gtk_container_add(GTK_CONTAINER(frame), GTK_WIDGET(box));

	gtk_frame_set_label(GTK_FRAME(frame), _("Light effects"));
}

static void suora_illumination_frame_class_init(SuoraIlluminationFrameClass *klass) {
	g_type_class_add_private(klass, sizeof(SuoraIlluminationFramePrivate));
}
