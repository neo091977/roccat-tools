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

#include "kiro_notification_frame.h"
#include "roccat_notification_type_combo_box.h"
#include "roccat_volume_scale.h"
#include "i18n.h"

#define KIRO_NOTIFICATION_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), KIRO_NOTIFICATION_FRAME_TYPE, KiroNotificationFrameClass))
#define IS_KIRO_NOTIFICATION_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), KIRO_NOTIFICATION_FRAME_TYPE))
#define KIRO_NOTIFICATION_FRAME_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), KIRO_NOTIFICATION_FRAME_TYPE, KiroNotificationFramePrivate))

typedef struct _KiroNotificationFrameClass KiroNotificationFrameClass;
typedef struct _KiroNotificationFramePrivate KiroNotificationFramePrivate;

struct _KiroNotificationFrame {
	GtkFrame parent;
	KiroNotificationFramePrivate *priv;
};

struct _KiroNotificationFrameClass {
	GtkFrameClass parent_class;
};

struct _KiroNotificationFramePrivate {
	GtkComboBox *sensitivity_notification_type;
	GtkComboBox *cpi_notification_type;
	RoccatVolumeScale *notification_volume;
};

G_DEFINE_TYPE(KiroNotificationFrame, kiro_notification_frame, GTK_TYPE_FRAME);

GtkWidget *kiro_notification_frame_new(void) {
	return GTK_WIDGET(g_object_new(KIRO_NOTIFICATION_FRAME_TYPE, NULL));
}

static void kiro_notification_frame_init(KiroNotificationFrame *notification_frame) {
	KiroNotificationFramePrivate *priv = KIRO_NOTIFICATION_FRAME_GET_PRIVATE(notification_frame);
	GtkWidget *table;

	notification_frame->priv = priv;

	table = gtk_table_new(5, 2, FALSE);
	gtk_container_add(GTK_CONTAINER(notification_frame), table);

	gtk_table_attach(GTK_TABLE(table), gtk_label_new(_("Sensitivity")), 0, 1, 2, 3, 0, GTK_EXPAND, 0, 0);
	priv->sensitivity_notification_type = GTK_COMBO_BOX(roccat_notification_type_combo_box_new());
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(priv->sensitivity_notification_type), 1, 2, 2, 3, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 0, 0);

	gtk_table_attach(GTK_TABLE(table), gtk_label_new(_("CPI")), 0, 1, 3, 4, 0, GTK_EXPAND, 0, 0);
	priv->cpi_notification_type = GTK_COMBO_BOX(roccat_notification_type_combo_box_new());
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(priv->cpi_notification_type), 1, 2, 3, 4, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 0, 0);

	gtk_table_attach(GTK_TABLE(table), gtk_label_new(_("Volume")), 0, 1, 4, 5, 0, GTK_EXPAND, 0, 0);
	priv->notification_volume = ROCCAT_VOLUME_SCALE(roccat_volume_scale_new());
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(priv->notification_volume), 1, 2, 4, 5, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 0, 0);

	gtk_frame_set_label(GTK_FRAME(notification_frame), _("Notifications"));
}

static void kiro_notification_frame_class_init(KiroNotificationFrameClass *klass) {
	g_type_class_add_private(klass, sizeof(KiroNotificationFramePrivate));
}

void kiro_notification_frame_set_from_profile_data(KiroNotificationFrame *notification_frame, KiroProfileData const *profile_data) {
	KiroNotificationFramePrivate *priv = notification_frame->priv;

	roccat_notification_type_combo_box_set_value(priv->sensitivity_notification_type, profile_data->eventhandler.sensitivity_notification_type);
	roccat_notification_type_combo_box_set_value(priv->cpi_notification_type, profile_data->eventhandler.cpi_notification_type);
	roccat_volume_scale_set_value(priv->notification_volume, profile_data->eventhandler.notification_volume);
}

void kiro_notification_frame_update_profile_data(KiroNotificationFrame *notification_frame, KiroProfileData *profile_data) {
	KiroProfileDataEventhandler *eventhandler = &profile_data->eventhandler;
	KiroNotificationFramePrivate *priv = notification_frame->priv;
	gint int_val;
	gdouble double_val;

	int_val = roccat_notification_type_combo_box_get_value(priv->sensitivity_notification_type);
	if (int_val != eventhandler->sensitivity_notification_type) {
		eventhandler->sensitivity_notification_type = int_val;
		eventhandler->modified = TRUE;
	}

	int_val = roccat_notification_type_combo_box_get_value(priv->cpi_notification_type);
	if (int_val != eventhandler->cpi_notification_type) {
		eventhandler->cpi_notification_type = int_val;
		eventhandler->modified = TRUE;
	}

	double_val = roccat_volume_scale_get_value(priv->notification_volume);
	if (double_val != eventhandler->notification_volume) {
		eventhandler->notification_volume = double_val;
		eventhandler->modified = TRUE;
	}
}
