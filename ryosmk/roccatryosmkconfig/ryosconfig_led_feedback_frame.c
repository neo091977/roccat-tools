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

#include "ryosconfig_led_feedback_frame.h"
#include "roccat_multiwidget.h"
#include "ryos_rkp.h"
#include "i18n.h"

#define RYOSCONFIG_LED_FEEDBACK_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), RYOSCONFIG_LED_FEEDBACK_FRAME_TYPE, RyosconfigLedFeedbackFrameClass))
#define IS_RYOSCONFIG_LED_FEEDBACK_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), RYOSCONFIG_LED_FEEDBACK_FRAME_TYPE))
#define RYOSCONFIG_LED_FEEDBACK_FRAME_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), RYOSCONFIG_LED_FEEDBACK_FRAME_TYPE, RyosconfigLedFeedbackFramePrivate))

typedef struct _RyosconfigLedFeedbackFrameClass RyosconfigLedFeedbackFrameClass;
typedef struct _RyosconfigLedFeedbackFramePrivate RyosconfigLedFeedbackFramePrivate;

struct _RyosconfigLedFeedbackFrame {
	GtkFrame parent;
	RyosconfigLedFeedbackFramePrivate *priv;
};

struct _RyosconfigLedFeedbackFrameClass {
	GtkFrameClass parent_class;
};

struct _RyosconfigLedFeedbackFramePrivate {
	RoccatMultiwidget *multiwidget;
	GtkBox *box;
	GtkLabel *nothing;
	GtkCheckButton *volume;
	GtkCheckButton *macro_exec;
};

G_DEFINE_TYPE(RyosconfigLedFeedbackFrame, ryosconfig_led_feedback_frame, GTK_TYPE_FRAME);

GtkWidget *ryosconfig_led_feedback_frame_new(void) {
	return GTK_WIDGET(g_object_new(RYOSCONFIG_LED_FEEDBACK_FRAME_TYPE, NULL));
}

static void ryosconfig_led_feedback_frame_init(RyosconfigLedFeedbackFrame *feedback_frame) {
	RyosconfigLedFeedbackFramePrivate *priv = RYOSCONFIG_LED_FEEDBACK_FRAME_GET_PRIVATE(feedback_frame);
	feedback_frame->priv = priv;

	priv->multiwidget = ROCCAT_MULTIWIDGET(roccat_multiwidget_new());
	priv->box = GTK_BOX(gtk_vbox_new(FALSE, 0));
	priv->nothing = GTK_LABEL(gtk_label_new(_("This model does not support LED feedback settings")));
	priv->macro_exec = GTK_CHECK_BUTTON(gtk_check_button_new_with_label(_("Macro execution feedback")));
	priv->volume = GTK_CHECK_BUTTON(gtk_check_button_new_with_label(_("LED volume indicator")));

	gtk_label_set_line_wrap(GTK_LABEL(priv->nothing), TRUE);

	roccat_multiwidget_add(priv->multiwidget, GTK_WIDGET(priv->box));
	roccat_multiwidget_add(priv->multiwidget, GTK_WIDGET(priv->nothing));

	gtk_box_pack_start(priv->box, GTK_WIDGET(priv->macro_exec), TRUE, TRUE, 0);
	gtk_box_pack_start(priv->box, GTK_WIDGET(priv->volume), TRUE, TRUE, 0);

	gtk_container_add(GTK_CONTAINER(feedback_frame), GTK_WIDGET(priv->multiwidget));

	gtk_frame_set_label(GTK_FRAME(feedback_frame), _("LED feedback"));
}

static void ryosconfig_led_feedback_frame_class_init(RyosconfigLedFeedbackFrameClass *klass) {
	g_type_class_add_private(klass, sizeof(RyosconfigLedFeedbackFramePrivate));
}

void ryosconfig_led_feedback_frame_set_from_profile_data(RyosconfigLedFeedbackFrame *feedback_frame, RyosProfileData const *profile_data) {
	RyosconfigLedFeedbackFramePrivate *priv = feedback_frame->priv;
	gboolean volume;
	gboolean macro;

	volume = profile_data->eventhandler.led_volume_indicator == RYOS_RKP_DATA_LED_VOLUME_INDICATOR_ON ? TRUE : FALSE;
	macro = profile_data->hardware.light.macro_exec_feedback == RYOS_LIGHT_MACRO_EXEC_FEEDBACK_ON ? TRUE : FALSE;

	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(priv->volume), volume);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(priv->macro_exec), macro);
}

void ryosconfig_led_feedback_frame_update_profile_data(RyosconfigLedFeedbackFrame *feedback_frame, RyosProfileData *profile_data) {
	RyosconfigLedFeedbackFramePrivate *priv = feedback_frame->priv;
	guint8 volume;
	guint8 macro_exec;

	volume = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(priv->volume)) ?
		RYOS_RKP_DATA_LED_VOLUME_INDICATOR_ON : RYOS_RKP_DATA_LED_VOLUME_INDICATOR_OFF;

	macro_exec = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(priv->macro_exec)) ?
		RYOS_LIGHT_MACRO_EXEC_FEEDBACK_ON : RYOS_LIGHT_MACRO_EXEC_FEEDBACK_OFF;

	if (profile_data->eventhandler.led_volume_indicator != volume) {
		profile_data->eventhandler.led_volume_indicator = volume;
		profile_data->eventhandler.modified = TRUE;
	}

	if (profile_data->hardware.light.macro_exec_feedback != macro_exec) {
		profile_data->hardware.light.macro_exec_feedback = macro_exec;
		profile_data->hardware.modified_light = TRUE;
	}
}

void ryosconfig_led_feedback_frame_set_device_type(RyosconfigLedFeedbackFrame *feedback_frame, RoccatDevice const *device) {
	RyosconfigLedFeedbackFramePrivate *priv = feedback_frame->priv;
	switch (gaminggear_device_get_product_id(GAMINGGEAR_DEVICE(device))) {
	case USB_DEVICE_ID_ROCCAT_RYOS_MK:
	case USB_DEVICE_ID_ROCCAT_RYOS_MK_GLOW:
		roccat_multiwidget_show(priv->multiwidget, GTK_WIDGET(priv->nothing));
		break;
	case USB_DEVICE_ID_ROCCAT_RYOS_MK_PRO:
		roccat_multiwidget_show(priv->multiwidget, GTK_WIDGET(priv->box));
		break;
	default:
		break;
	}
}
