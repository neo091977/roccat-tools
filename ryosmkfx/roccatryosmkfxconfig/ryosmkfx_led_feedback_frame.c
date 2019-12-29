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

#include "ryosmkfx_led_feedback_frame.h"
#include "ryosmkfx_light.h"
#include "i18n.h"

#define RYOSMKFX_LED_FEEDBACK_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), RYOSMKFX_LED_FEEDBACK_FRAME_TYPE, RyosmkfxLedFeedbackFrameClass))
#define IS_RYOSMKFX_LED_FEEDBACK_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), RYOSMKFX_LED_FEEDBACK_FRAME_TYPE))
#define RYOSMKFX_LED_FEEDBACK_FRAME_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), RYOSMKFX_LED_FEEDBACK_FRAME_TYPE, RyosmkfxLedFeedbackFramePrivate))

typedef struct _RyosmkfxLedFeedbackFrameClass RyosmkfxLedFeedbackFrameClass;
typedef struct _RyosmkfxLedFeedbackFramePrivate RyosmkfxLedFeedbackFramePrivate;

struct _RyosmkfxLedFeedbackFrame {
	GtkFrame parent;
	RyosmkfxLedFeedbackFramePrivate *priv;
};

struct _RyosmkfxLedFeedbackFrameClass {
	GtkFrameClass parent_class;
};

struct _RyosmkfxLedFeedbackFramePrivate {
	GtkCheckButton *volume;
	GtkCheckButton *macro_exec;
};

G_DEFINE_TYPE(RyosmkfxLedFeedbackFrame, ryosmkfx_led_feedback_frame, GTK_TYPE_FRAME);

GtkWidget *ryosmkfx_led_feedback_frame_new(void) {
	return GTK_WIDGET(g_object_new(RYOSMKFX_LED_FEEDBACK_FRAME_TYPE, NULL));
}

static void ryosmkfx_led_feedback_frame_init(RyosmkfxLedFeedbackFrame *feedback_frame) {
	RyosmkfxLedFeedbackFramePrivate *priv = RYOSMKFX_LED_FEEDBACK_FRAME_GET_PRIVATE(feedback_frame);
	feedback_frame->priv = priv;
	GtkBox *vbox;

	vbox = GTK_BOX(gtk_vbox_new(FALSE, 0));
	priv->macro_exec = GTK_CHECK_BUTTON(gtk_check_button_new_with_label(_("Macro execution feedback")));
	priv->volume = GTK_CHECK_BUTTON(gtk_check_button_new_with_label(_("LED volume indicator")));

	gtk_box_pack_start(vbox, GTK_WIDGET(priv->macro_exec), TRUE, TRUE, 0);
	gtk_box_pack_start(vbox, GTK_WIDGET(priv->volume), TRUE, TRUE, 0);

	gtk_container_add(GTK_CONTAINER(feedback_frame), GTK_WIDGET(vbox));

	gtk_frame_set_label(GTK_FRAME(feedback_frame), _("LED feedback"));
}

static void ryosmkfx_led_feedback_frame_class_init(RyosmkfxLedFeedbackFrameClass *klass) {
	g_type_class_add_private(klass, sizeof(RyosmkfxLedFeedbackFramePrivate));
}

void ryosmkfx_led_feedback_frame_set_from_profile_data(RyosmkfxLedFeedbackFrame *feedback_frame, RyosmkfxProfileData const *profile_data) {
	RyosmkfxLedFeedbackFramePrivate *priv = feedback_frame->priv;
	gboolean macro;

	// TODO volume indicator unused

	macro = profile_data->hardware.light.led_feedback == RYOSMKFX_LIGHT_LED_FEEDBACK_MACRO_EXECUTION ? TRUE : FALSE;

	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(priv->macro_exec), macro);
}

void ryosmkfx_led_feedback_frame_update_profile_data(RyosmkfxLedFeedbackFrame *feedback_frame, RyosmkfxProfileData *profile_data) {
	RyosmkfxLedFeedbackFramePrivate *priv = feedback_frame->priv;
	guint8 feedback;

	// TODO volume indicator unused

	feedback = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(priv->macro_exec)) ?
			RYOSMKFX_LIGHT_LED_FEEDBACK_MACRO_EXECUTION : RYOSMKFX_LIGHT_LED_FEEDBACK_OFF;

	if (profile_data->hardware.light.led_feedback != feedback) {
		profile_data->hardware.light.led_feedback = feedback;
		profile_data->hardware.modified_light = TRUE;
	}
}
