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

#include "ryostklconfig_notification_frame.h"
#include "roccat_notification_type_combo_box.h"
#include "roccat_volume_scale.h"
#include "i18n.h"

#define RYOSTKLCONFIG_NOTIFICATION_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), RYOSTKLCONFIG_NOTIFICATION_FRAME_TYPE, RyostklconfigNotificationFrameClass))
#define IS_RYOSTKLCONFIG_NOTIFICATION_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), RYOSTKLCONFIG_NOTIFICATION_FRAME_TYPE))
#define RYOSTKLCONFIG_NOTIFICATION_FRAME_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), RYOSTKLCONFIG_NOTIFICATION_FRAME_TYPE, RyostklconfigNotificationFramePrivate))

typedef struct _RyostklconfigNotificationFrameClass RyostklconfigNotificationFrameClass;
typedef struct _RyostklconfigNotificationFramePrivate RyostklconfigNotificationFramePrivate;

struct _RyostklconfigNotificationFrame {
	GtkFrame parent;
	RyostklconfigNotificationFramePrivate *priv;
};

struct _RyostklconfigNotificationFrameClass {
	GtkFrameClass parent_class;
};

struct _RyostklconfigNotificationFramePrivate {
	GtkComboBox *timer_notification_type;
	GtkComboBox *profile_notification_type;
	GtkComboBox *live_recording_notification_type;
	RoccatVolumeScale *notification_volume;
};

G_DEFINE_TYPE(RyostklconfigNotificationFrame, ryostklconfig_notification_frame, GTK_TYPE_FRAME);

GtkWidget *ryostklconfig_notification_frame_new(void) {
	return GTK_WIDGET(g_object_new(RYOSTKLCONFIG_NOTIFICATION_FRAME_TYPE, NULL));
}

static void ryostklconfig_notification_frame_init(RyostklconfigNotificationFrame *notification_frame) {
	RyostklconfigNotificationFramePrivate *priv = RYOSTKLCONFIG_NOTIFICATION_FRAME_GET_PRIVATE(notification_frame);
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

static void ryostklconfig_notification_frame_class_init(RyostklconfigNotificationFrameClass *klass) {
	g_type_class_add_private(klass, sizeof(RyostklconfigNotificationFramePrivate));
}

void ryostklconfig_notification_frame_set_from_profile_data(RyostklconfigNotificationFrame *notification_frame, RyostklProfileData const *profile_data) {
	RyostklconfigNotificationFramePrivate *priv = notification_frame->priv;

	roccat_notification_type_combo_box_set_value(priv->timer_notification_type, profile_data->eventhandler.timer_notification_type);
	roccat_notification_type_combo_box_set_value(priv->profile_notification_type, profile_data->eventhandler.profile_notification_type);
	roccat_notification_type_combo_box_set_value(priv->live_recording_notification_type, profile_data->eventhandler.live_recording_notification_type);
	roccat_volume_scale_set_value(priv->notification_volume, profile_data->eventhandler.notification_volume);
}

void ryostklconfig_notification_frame_update_profile_data(RyostklconfigNotificationFrame *notification_frame, RyostklProfileData *profile_data) {
	RyostklconfigNotificationFramePrivate *priv = notification_frame->priv;
	gint int_val;
	gdouble double_val;

	int_val = roccat_notification_type_combo_box_get_value(priv->timer_notification_type);
	if (int_val != profile_data->eventhandler.timer_notification_type) {
		profile_data->eventhandler.timer_notification_type = int_val;
		profile_data->eventhandler.modified = TRUE;
	}

	int_val = roccat_notification_type_combo_box_get_value(priv->profile_notification_type);
	if (int_val != profile_data->eventhandler.profile_notification_type) {
		profile_data->eventhandler.profile_notification_type = int_val;
		profile_data->eventhandler.modified = TRUE;
	}

	int_val = roccat_notification_type_combo_box_get_value(priv->live_recording_notification_type);
	if (int_val != profile_data->eventhandler.live_recording_notification_type) {
		profile_data->eventhandler.live_recording_notification_type = int_val;
		profile_data->eventhandler.modified = TRUE;
	}

	double_val = roccat_volume_scale_get_value(priv->notification_volume);
	if (double_val != profile_data->eventhandler.notification_volume) {
		profile_data->eventhandler.notification_volume = double_val;
		profile_data->eventhandler.modified = TRUE;
	}
}
