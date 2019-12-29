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

#include "ryos_notification_frame.h"
#include "roccat_notification_type_combo_box.h"
#include "roccat_volume_scale.h"
#include "i18n.h"

#define RYOS_NOTIFICATION_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), RYOS_NOTIFICATION_FRAME_TYPE, RyosNotificationFrameClass))
#define IS_RYOS_NOTIFICATION_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), RYOS_NOTIFICATION_FRAME_TYPE))
#define RYOS_NOTIFICATION_FRAME_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), RYOS_NOTIFICATION_FRAME_TYPE, RyosNotificationFramePrivate))

typedef struct _RyosNotificationFrameClass RyosNotificationFrameClass;
typedef struct _RyosNotificationFramePrivate RyosNotificationFramePrivate;

struct _RyosNotificationFrame {
	GtkFrame parent;
	RyosNotificationFramePrivate *priv;
};

struct _RyosNotificationFrameClass {
	GtkFrameClass parent_class;
};

struct _RyosNotificationFramePrivate {
	GtkComboBox *timer_notification_type;
	GtkComboBox *profile_notification_type;
	GtkComboBox *live_recording_notification_type;
	RoccatVolumeScale *notification_volume;
};

G_DEFINE_TYPE(RyosNotificationFrame, ryos_notification_frame, GTK_TYPE_FRAME);

GtkWidget *ryos_notification_frame_new(void) {
	return GTK_WIDGET(g_object_new(RYOS_NOTIFICATION_FRAME_TYPE, NULL));
}

static void ryos_notification_frame_init(RyosNotificationFrame *notification_frame) {
	RyosNotificationFramePrivate *priv = RYOS_NOTIFICATION_FRAME_GET_PRIVATE(notification_frame);
	GtkWidget *table;

	notification_frame->priv = priv;

	table = gtk_table_new(4, 2, FALSE);
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

	gtk_table_attach(GTK_TABLE(table), gtk_label_new(_("Volume")), 0, 1, 3, 4, 0, GTK_EXPAND, 0, 0);
	priv->notification_volume = ROCCAT_VOLUME_SCALE(roccat_volume_scale_new());
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(priv->notification_volume), 1, 2, 3, 4, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 0, 0);

	gtk_frame_set_label(GTK_FRAME(notification_frame), _("Notifications"));
}

static void ryos_notification_frame_class_init(RyosNotificationFrameClass *klass) {
	g_type_class_add_private(klass, sizeof(RyosNotificationFramePrivate));
}

void ryos_notification_frame_set_timer_type(RyosNotificationFrame *notification_frame, guint8 value) {
	roccat_notification_type_combo_box_set_value(notification_frame->priv->timer_notification_type, value);
}

guint8 ryos_notification_frame_get_timer_type(RyosNotificationFrame *notification_frame) {
	return roccat_notification_type_combo_box_get_value(notification_frame->priv->timer_notification_type);
}

void ryos_notification_frame_set_profile_type(RyosNotificationFrame *notification_frame, guint8 value) {
	roccat_notification_type_combo_box_set_value(notification_frame->priv->profile_notification_type, value);
}

guint8 ryos_notification_frame_get_profile_type(RyosNotificationFrame *notification_frame) {
	return roccat_notification_type_combo_box_get_value(notification_frame->priv->profile_notification_type);
}

void ryos_notification_frame_set_live_recording_type(RyosNotificationFrame *notification_frame, guint8 value) {
	roccat_notification_type_combo_box_set_value(notification_frame->priv->live_recording_notification_type, value);
}

guint8 ryos_notification_frame_get_live_recording_type(RyosNotificationFrame *notification_frame) {
	return roccat_notification_type_combo_box_get_value(notification_frame->priv->live_recording_notification_type);
}

void ryos_notification_frame_set_volume(RyosNotificationFrame *notification_frame, gdouble volume) {
	roccat_volume_scale_set_value(notification_frame->priv->notification_volume, volume);
}

gdouble ryos_notification_frame_get_volume(RyosNotificationFrame *notification_frame) {
	return roccat_volume_scale_get_value(notification_frame->priv->notification_volume);
}
