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

#include "skeltr_notification_frame.h"
#include "skeltr_sound_feedback_combo_box.h"
#include "roccat_notification_type_combo_box.h"
#include "roccat_volume_scale.h"
#include "i18n.h"

#define SKELTR_NOTIFICATION_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), SKELTR_NOTIFICATION_FRAME_TYPE, SkeltrNotificationFrameClass))
#define IS_SKELTR_NOTIFICATION_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), SKELTR_NOTIFICATION_FRAME_TYPE))
#define SKELTR_NOTIFICATION_FRAME_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), SKELTR_NOTIFICATION_FRAME_TYPE, SkeltrNotificationFramePrivate))

typedef struct _SkeltrNotificationFrameClass SkeltrNotificationFrameClass;
typedef struct _SkeltrNotificationFramePrivate SkeltrNotificationFramePrivate;

struct _SkeltrNotificationFrame {
	GtkFrame parent;
	SkeltrNotificationFramePrivate *priv;
};

struct _SkeltrNotificationFrameClass {
	GtkFrameClass parent_class;
};

struct _SkeltrNotificationFramePrivate {
	GtkComboBox *typing_feedback;
	GtkComboBox *timer_notification_type;
	GtkComboBox *profile_notification_type;
	GtkComboBox *live_recording_notification_type;
	GtkComboBox *sleep_notification_type;
	GtkComboBox *wakeup_notification_type;
	RoccatVolumeScale *notification_volume;
};

G_DEFINE_TYPE(SkeltrNotificationFrame, skeltr_notification_frame, GTK_TYPE_FRAME);

GtkWidget *skeltr_notification_frame_new(void) {
	return GTK_WIDGET(g_object_new(SKELTR_NOTIFICATION_FRAME_TYPE, NULL));
}

static void skeltr_notification_frame_init(SkeltrNotificationFrame *notification_frame) {
	SkeltrNotificationFramePrivate *priv = SKELTR_NOTIFICATION_FRAME_GET_PRIVATE(notification_frame);
	GtkWidget *table;

	notification_frame->priv = priv;

	table = gtk_table_new(7, 2, FALSE);
	gtk_container_add(GTK_CONTAINER(notification_frame), table);

	gtk_table_attach(GTK_TABLE(table), gtk_label_new(_("Timer")), 0, 1, 0, 1, 0, GTK_EXPAND, 0, 0);
	priv->timer_notification_type = GTK_COMBO_BOX(roccat_notification_type_combo_box_new());
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(priv->timer_notification_type), 1, 2, 0, 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 0, 0);

	gtk_table_attach(GTK_TABLE(table), gtk_label_new(_("Profile")), 0, 1, 1, 2, 0, GTK_EXPAND, 0, 0);
	priv->profile_notification_type = GTK_COMBO_BOX(roccat_notification_type_combo_box_new());
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(priv->profile_notification_type), 1, 2, 1, 2, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 0, 0);

	gtk_table_attach(GTK_TABLE(table), gtk_label_new(_("Live recording")), 0, 1, 2, 3, 0, GTK_EXPAND, 0, 0);
	priv->live_recording_notification_type = GTK_COMBO_BOX(roccat_notification_type_combo_box_new());
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(priv->live_recording_notification_type), 1, 2, 2, 3, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 0, 0);

	gtk_table_attach(GTK_TABLE(table), gtk_label_new(_("Sleep")), 0, 1, 3, 4, 0, GTK_EXPAND, 0, 0);
	priv->sleep_notification_type = GTK_COMBO_BOX(roccat_notification_type_combo_box_new());
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(priv->sleep_notification_type), 1, 2, 3, 4, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 0, 0);

	gtk_table_attach(GTK_TABLE(table), gtk_label_new(_("Wakeup")), 0, 1, 4, 5, 0, GTK_EXPAND, 0, 0);
	priv->wakeup_notification_type = GTK_COMBO_BOX(roccat_notification_type_combo_box_new());
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(priv->wakeup_notification_type), 1, 2, 4, 5, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 0, 0);

	gtk_table_attach(GTK_TABLE(table), gtk_label_new(_("Typing")), 0, 1, 5, 6, 0, GTK_EXPAND, 0, 0);
	priv->typing_feedback = GTK_COMBO_BOX(skeltr_sound_feedback_combo_box_new());
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(priv->typing_feedback), 1, 2, 5, 6, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 0, 0);

	gtk_table_attach(GTK_TABLE(table), gtk_label_new(_("Volume")), 0, 1, 6, 7, 0, GTK_EXPAND, 0, 0);
	priv->notification_volume = ROCCAT_VOLUME_SCALE(roccat_volume_scale_new());
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(priv->notification_volume), 1, 2, 6, 7, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 0, 0);

	gtk_frame_set_label(GTK_FRAME(notification_frame), _("Notifications"));
}

static void skeltr_notification_frame_class_init(SkeltrNotificationFrameClass *klass) {
	g_type_class_add_private(klass, sizeof(SkeltrNotificationFramePrivate));
}

void skeltr_notification_frame_set_from_profile_data(SkeltrNotificationFrame *notification_frame, SkeltrProfileData const *profile_data) {
	SkeltrNotificationFramePrivate *priv = notification_frame->priv;

	skeltr_sound_feedback_combo_box_set_value(priv->typing_feedback, skeltr_profile_data_get_sound_feedback_typing(profile_data));
	roccat_notification_type_combo_box_set_value(priv->timer_notification_type, skeltr_profile_data_get_timer_notification_type(profile_data));
	roccat_notification_type_combo_box_set_value(priv->profile_notification_type, skeltr_profile_data_get_profile_switch_notification_type(profile_data));
	roccat_notification_type_combo_box_set_value(priv->live_recording_notification_type, skeltr_profile_data_get_macro_record_notification_type(profile_data));
	roccat_notification_type_combo_box_set_value(priv->sleep_notification_type, skeltr_profile_data_get_sleep_notification_type(profile_data));
	roccat_notification_type_combo_box_set_value(priv->wakeup_notification_type, skeltr_profile_data_get_wakeup_notification_type(profile_data));
	roccat_volume_scale_set_value(priv->notification_volume, skeltr_profile_data_get_sound_feedback_volume(profile_data));
}

void skeltr_notification_frame_update_profile_data(SkeltrNotificationFrame *notification_frame, SkeltrProfileData *profile_data) {
	SkeltrNotificationFramePrivate *priv = notification_frame->priv;

	skeltr_profile_data_set_sound_feedback_typing(profile_data, skeltr_sound_feedback_combo_box_get_value(priv->typing_feedback));
	skeltr_profile_data_set_timer_notification_type(profile_data, roccat_notification_type_combo_box_get_value(priv->timer_notification_type));
	skeltr_profile_data_set_profile_switch_notification_type(profile_data, roccat_notification_type_combo_box_get_value(priv->profile_notification_type));
	skeltr_profile_data_set_macro_record_notification_type(profile_data, roccat_notification_type_combo_box_get_value(priv->live_recording_notification_type));
	skeltr_profile_data_set_sleep_notification_type(profile_data, roccat_notification_type_combo_box_get_value(priv->sleep_notification_type));
	skeltr_profile_data_set_wakeup_notification_type(profile_data, roccat_notification_type_combo_box_get_value(priv->wakeup_notification_type));
	skeltr_profile_data_set_sound_feedback_volume(profile_data, roccat_volume_scale_get_value(priv->notification_volume));
}
