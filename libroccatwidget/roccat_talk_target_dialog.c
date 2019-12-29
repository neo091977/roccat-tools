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

#include "roccat_talk_target_dialog.h"
#include "roccat_talk_target_combo_box.h"
#include "roccat_talk.h"
#include "i18n-lib.h"

#define ROCCAT_TALK_TARGET_DIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), ROCCAT_TALK_TARGET_DIALOG_TYPE, RoccatTalkTargetDialogClass))
#define IS_ROCCAT_TALK_TARGET_DIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), ROCCAT_TALK_TARGET_DIALOG_TYPE))
#define ROCCAT_TALK_TARGET_DIALOG_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), ROCCAT_TALK_TARGET_DIALOG_TYPE, RoccatTalkTargetDialogPrivate))

typedef struct _RoccatTalkTargetDialogClass RoccatTalkTargetDialogClass;

struct _RoccatTalkTargetDialogClass {
	GtkDialogClass parent_class;
};

struct _RoccatTalkTargetDialogPrivate {
	GtkWidget *ok_button;
	RoccatTalkTargetComboBox *combo_box;
};

enum {
	PROP_0,
	PROP_SOURCE_TALK_DEVICE_TYPE,
};

G_DEFINE_TYPE(RoccatTalkTargetDialog, roccat_talk_target_dialog, GTK_TYPE_DIALOG);

guint roccat_talk_target_dialog_get_value(RoccatTalkTargetDialog *talk_target_dialog) {
	return roccat_talk_target_combo_box_get_value(talk_target_dialog->priv->combo_box);
}

void roccat_talk_target_dialog_set_value(RoccatTalkTargetDialog *talk_target_dialog, guint value) {
	roccat_talk_target_combo_box_set_value(talk_target_dialog->priv->combo_box, value);
}

guint roccat_talk_target_dialog_get_source_talk_device_type(RoccatTalkTargetDialog *talk_target_dialog) {
	return 	roccat_talk_target_combo_box_get_source_talk_device_type(talk_target_dialog->priv->combo_box);
}

void roccat_talk_target_dialog_set_source_talk_device_type(RoccatTalkTargetDialog *talk_target_dialog, guint value) {
	roccat_talk_target_combo_box_set_source_talk_device_type(talk_target_dialog->priv->combo_box, value);
}

GtkWidget *roccat_talk_target_dialog_new(GtkWindow *parent, guint source_talk_device_type) {
	RoccatTalkTargetDialog *talk_target_dialog;
	RoccatTalkTargetDialogPrivate *priv;

	talk_target_dialog = g_object_new(ROCCAT_TALK_TARGET_DIALOG_TYPE,
			"source_talk_device_type", source_talk_device_type,
			NULL);
	priv = talk_target_dialog->priv;

	priv->ok_button = gtk_dialog_add_button(GTK_DIALOG(talk_target_dialog), GTK_STOCK_OK, GTK_RESPONSE_ACCEPT);
	gtk_widget_set_sensitive(priv->ok_button, FALSE);

	gtk_dialog_add_button(GTK_DIALOG(talk_target_dialog), GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT);

	gtk_window_set_title(GTK_WINDOW(talk_target_dialog), _("Select Talk target"));
	gtk_window_set_transient_for(GTK_WINDOW(talk_target_dialog), parent);
	gtk_window_set_modal(GTK_WINDOW(talk_target_dialog), TRUE);
	gtk_window_set_destroy_with_parent(GTK_WINDOW(talk_target_dialog), TRUE);

	return GTK_WIDGET(talk_target_dialog);
}

static void roccat_talk_target_dialog_set_property(GObject *object, guint prop_id, GValue const *value, GParamSpec *pspec) {
	RoccatTalkTargetDialog *talk_target_dialog = ROCCAT_TALK_TARGET_DIALOG(object);

	switch(prop_id) {
	case PROP_SOURCE_TALK_DEVICE_TYPE:
		roccat_talk_target_dialog_set_source_talk_device_type(talk_target_dialog, g_value_get_uint(value));
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
	}
}

static void roccat_talk_target_dialog_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec) {
	RoccatTalkTargetDialog *talk_target_dialog = ROCCAT_TALK_TARGET_DIALOG(object);

	switch(prop_id) {
	case PROP_SOURCE_TALK_DEVICE_TYPE:
		g_value_set_uint(value, roccat_talk_target_dialog_get_source_talk_device_type(talk_target_dialog));
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
	}
}

static void changed_cb(GtkComboBox *combo_box, gpointer user_data) {
	RoccatTalkTargetDialog *talk_target_dialog = ROCCAT_TALK_TARGET_DIALOG(user_data);
	RoccatTalkTargetDialogPrivate *priv = talk_target_dialog->priv;
	if (priv->ok_button)
		gtk_widget_set_sensitive(priv->ok_button, TRUE);
}

static void roccat_talk_target_dialog_init(RoccatTalkTargetDialog *talk_target_dialog) {
	RoccatTalkTargetDialogPrivate *priv = ROCCAT_TALK_TARGET_DIALOG_GET_PRIVATE(talk_target_dialog);
	GtkVBox *content_area;

	talk_target_dialog->priv = priv;

	content_area = GTK_VBOX(gtk_dialog_get_content_area(GTK_DIALOG(talk_target_dialog)));

	priv->combo_box = ROCCAT_TALK_TARGET_COMBO_BOX(roccat_talk_target_combo_box_new(ROCCAT_TALK_DEVICE_ALL));
	g_signal_connect(G_OBJECT(priv->combo_box), "changed", G_CALLBACK(changed_cb), talk_target_dialog);
	gtk_box_pack_start(GTK_BOX(content_area), GTK_WIDGET(priv->combo_box), FALSE, FALSE, 0);

	gtk_widget_show_all(GTK_WIDGET(content_area));
}

static void roccat_talk_target_dialog_class_init(RoccatTalkTargetDialogClass *klass) {
	GObjectClass *gobject_class;

	gobject_class = (GObjectClass*)klass;

	gobject_class->set_property = roccat_talk_target_dialog_set_property;
	gobject_class->get_property = roccat_talk_target_dialog_get_property;

	g_type_class_add_private(klass, sizeof(RoccatTalkTargetDialogPrivate));

	g_object_class_install_property(gobject_class, PROP_SOURCE_TALK_DEVICE_TYPE,
			g_param_spec_uint("source_talk_device_type",
					"source_talk_device_type",
					"Reads or sets source talk device type",
					0, 0xffff, 0,
					G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
}
