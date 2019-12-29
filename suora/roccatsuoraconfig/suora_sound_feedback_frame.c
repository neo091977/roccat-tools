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

#include "suora_sound_feedback_frame.h"
#include "roccat_volume_scale.h"
#include "gtk_roccat_helper.h"
#include <gaminggear/gaminggear_hscale.h>
#include "i18n.h"

#define SUORA_SOUND_FEEDBACK_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), SUORA_SOUND_FEEDBACK_FRAME_TYPE, SuoraSoundFeedbackFrameClass))
#define IS_SUORA_SOUND_FEEDBACK_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), SUORA_SOUND_FEEDBACK_FRAME_TYPE))
#define SUORA_SOUND_FEEDBACK_FRAME_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), SUORA_SOUND_FEEDBACK_FRAME_TYPE, SuoraSoundFeedbackFramePrivate))

typedef struct _SuoraSoundFeedbackFrameClass SuoraSoundFeedbackFrameClass;
typedef struct _SuoraSoundFeedbackFramePrivate SuoraSoundFeedbackFramePrivate;

struct _SuoraSoundFeedbackFrame {
	GtkFrame parent;
	SuoraSoundFeedbackFramePrivate *priv;
};

struct _SuoraSoundFeedbackFrameClass {
	GtkFrameClass parent_class;
};

struct _SuoraSoundFeedbackFramePrivate {
	GSList *radios_feedback;
	RoccatVolumeScale *volume;
};

G_DEFINE_TYPE(SuoraSoundFeedbackFrame, suora_sound_feedback_frame, GTK_TYPE_FRAME);

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

void suora_sound_feedback_frame_set_from_profile_data(SuoraSoundFeedbackFrame *frame, SuoraProfileData const *profile_data) {
	SuoraSoundFeedbackFramePrivate *priv = frame->priv;

	GtkRadioButton *feedback = find_radio(priv->radios_feedback, suora_profile_data_get_sound_feedback(profile_data));
	if (feedback)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(feedback), TRUE);

	roccat_volume_scale_set_value(priv->volume, suora_profile_data_get_volume(profile_data));
}

void suora_sound_feedback_frame_update_profile_data(SuoraSoundFeedbackFrame *frame, SuoraProfileData *profile_data) {
	SuoraSoundFeedbackFramePrivate *priv = frame->priv;
	GtkWidget *active;

	active = gtk_roccat_radio_button_group_get_active(priv->radios_feedback);
	suora_profile_data_set_sound_feedback(profile_data, GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(active), value_key)));
	suora_profile_data_set_volume(profile_data, roccat_volume_scale_get_value(priv->volume));
}

GtkWidget *suora_sound_feedback_frame_new(void) {
	SuoraSoundFeedbackFrame *frame;

	frame = SUORA_SOUND_FEEDBACK_FRAME(g_object_new(SUORA_SOUND_FEEDBACK_FRAME_TYPE,
			NULL));

	return GTK_WIDGET(frame);
}

static void add_radios_feedback(GtkBox *parent, SuoraSoundFeedbackFrame *frame) {
	SuoraSoundFeedbackFramePrivate *priv = frame->priv;
	GtkWidget *radio;

	radio = gtk_radio_button_new_with_label(NULL, _("SciFi"));
	g_object_set_data(G_OBJECT(radio), value_key, GUINT_TO_POINTER(SUORA_PROFILE_DATA_SOUND_FEEDBACK_SCI_FI));

	radio = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio), _("Beam"));
	g_object_set_data(G_OBJECT(radio), value_key, GUINT_TO_POINTER(SUORA_PROFILE_DATA_SOUND_FEEDBACK_BEAM));

	radio = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio), _("Typewriter"));
	g_object_set_data(G_OBJECT(radio), value_key, GUINT_TO_POINTER(SUORA_PROFILE_DATA_SOUND_FEEDBACK_TYPEWRITER));

	radio = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio), _("Click"));
	g_object_set_data(G_OBJECT(radio), value_key, GUINT_TO_POINTER(SUORA_PROFILE_DATA_SOUND_FEEDBACK_CLICK));

	radio = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio), _("None"));
	g_object_set_data(G_OBJECT(radio), value_key, GUINT_TO_POINTER(SUORA_PROFILE_DATA_SOUND_FEEDBACK_NONE));

	priv->radios_feedback = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radio));
	g_slist_foreach(priv->radios_feedback, pack_radio_button, parent);
}

static void add_volume(GtkBox *parent, SuoraSoundFeedbackFrame *frame) {
	SuoraSoundFeedbackFramePrivate *priv = frame->priv;
	GtkWidget *hbox;

	hbox = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(parent, hbox, TRUE, TRUE, 0);

	gtk_box_pack_start(GTK_BOX(hbox), gtk_label_new(_("Volume")), FALSE, FALSE, 0);

	priv->volume = ROCCAT_VOLUME_SCALE(roccat_volume_scale_new());

	gtk_box_pack_start(GTK_BOX(hbox), GTK_WIDGET(priv->volume), TRUE, TRUE, 0);
}

static void suora_sound_feedback_frame_init(SuoraSoundFeedbackFrame *frame) {
	SuoraSoundFeedbackFramePrivate *priv = SUORA_SOUND_FEEDBACK_FRAME_GET_PRIVATE(frame);
	GtkBox *box;

	frame->priv = priv;

	box = GTK_BOX(gtk_vbox_new(FALSE, 0));

	add_radios_feedback(box, frame);
	gtk_box_pack_start(box, gtk_hseparator_new(), TRUE, TRUE, 0);
	add_volume(box, frame);

	gtk_container_add(GTK_CONTAINER(frame), GTK_WIDGET(box));

	gtk_frame_set_label(GTK_FRAME(frame), _("Sound feedback"));
}

static void suora_sound_feedback_frame_class_init(SuoraSoundFeedbackFrameClass *klass) {
	g_type_class_add_private(klass, sizeof(SuoraSoundFeedbackFramePrivate));
}
