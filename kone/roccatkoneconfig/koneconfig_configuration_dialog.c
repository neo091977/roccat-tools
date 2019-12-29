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

#include "koneconfig_configuration_dialog.h"
#include "kone.h"
#include "roccat_warning_dialog.h"
#include "roccat_notification_type_combo_box.h"
#include "roccat_default_profile_combo_box.h"
#include "roccat_strings.h"
#include "g_roccat_helper.h"
#include "roccat_volume_scale.h"
#include "i18n.h"

#define KONECONFIG_CONFIGURATION_DIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), KONECONFIG_CONFIGURATION_DIALOG_TYPE, KoneconfigConfigurationDialogClass))
#define IS_KONECONFIG_CONFIGURATION_DIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), KONECONFIG_CONFIGURATION_DIALOG_TYPE))
#define KONECONFIG_CONFIGURATION_DIALOG_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), KONECONFIG_CONFIGURATION_DIALOG_TYPE, KoneconfigConfigurationDialogPrivate))

typedef struct _KoneconfigConfigurationDialogClass KoneconfigConfigurationDialogClass;

struct _KoneconfigConfigurationDialogClass {
	GtkDialogClass parent_class;
};

struct _KoneconfigConfigurationDialogPrivate {
	GtkToggleButton *only_notify_on_osd;
	GtkComboBox *profile_notification_type;
	GtkComboBox *cpi_notification_type;
	RoccatDefaultProfileComboBox *default_profile_number;
	RoccatVolumeScale *notification_volume;
};

G_DEFINE_TYPE(KoneconfigConfigurationDialog, koneconfig_configuration_dialog, GTK_TYPE_DIALOG);

static GtkWidget *notification_frame_new(KoneconfigConfigurationDialog *koneconfig_configuration_dialog) {
	KoneconfigConfigurationDialogPrivate *priv = koneconfig_configuration_dialog->priv;
	GtkWidget *frame, *table;

	frame =gtk_frame_new(_("Notification"));
	table = gtk_table_new(3, 2, FALSE);
	gtk_container_add(GTK_CONTAINER(frame), table);

	gtk_table_attach(GTK_TABLE(table), gtk_label_new(_("Profile")), 0, 1, 0, 1, 0, GTK_EXPAND, 0, 0);
	priv->profile_notification_type = GTK_COMBO_BOX(roccat_notification_type_combo_box_new());
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(priv->profile_notification_type), 1, 2, 0, 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 0, 0);

	gtk_table_attach(GTK_TABLE(table), gtk_label_new(_("CPI")), 0, 1, 1, 2, 0, GTK_EXPAND, 0, 0);
	priv->cpi_notification_type = GTK_COMBO_BOX(roccat_notification_type_combo_box_new());
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(priv->cpi_notification_type), 1, 2, 1, 2, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 0, 0);

	gtk_table_attach(GTK_TABLE(table), gtk_label_new(_("Volume")), 0, 1, 2, 3, 0, GTK_EXPAND, 0, 0);
	priv->notification_volume = ROCCAT_VOLUME_SCALE(roccat_volume_scale_new());
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(priv->notification_volume), 1, 2, 2, 3, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 0, 0);

	return frame;
}

static GtkWidget *konegui_configuration_osd_frame_new(KoneconfigConfigurationDialog *koneconfig_configuration_dialog) {
	KoneconfigConfigurationDialogPrivate *priv = koneconfig_configuration_dialog->priv;
	GtkWidget *frame, *vbox;

	frame = gtk_frame_new(_("OSD"));
	vbox = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(frame), vbox);

	priv->only_notify_on_osd = GTK_TOGGLE_BUTTON(gtk_check_button_new_with_label(_("Notify only on OSD event")));
	gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(priv->only_notify_on_osd), TRUE, FALSE, 0);

	return frame;
}

static GtkWidget *general_frame_new(KoneconfigConfigurationDialog *koneconfig_configuration_dialog) {
	KoneconfigConfigurationDialogPrivate *priv = koneconfig_configuration_dialog->priv;
	GtkWidget *frame, *vbox, *hbox;

	frame =gtk_frame_new(_("General"));
	vbox = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(frame), vbox);

	hbox = gtk_hbox_new(FALSE, 0);
	gtk_widget_set_tooltip_text(hbox, roccat_string_default_profile_number_tooltip());
	gtk_box_pack_start(GTK_BOX(hbox), gtk_label_new(_("Default profile")), FALSE, TRUE, 0);
	priv->default_profile_number = ROCCAT_DEFAULT_PROFILE_COMBO_BOX(roccat_default_profile_combo_box_new(KONE_PROFILE_NUM));
	gtk_box_pack_start(GTK_BOX(hbox), GTK_WIDGET(priv->default_profile_number), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, TRUE, 0);

	return frame;
}

void koneconfig_configuration_dialog_get_configuration(KoneconfigConfigurationDialog *koneconfig_configuration_dialog, RoccatKeyFile *config) {
	KoneconfigConfigurationDialogPrivate *priv = koneconfig_configuration_dialog->priv;

	kone_configuration_set_profile_notification_type(config, roccat_notification_type_combo_box_get_value(priv->profile_notification_type));
	kone_configuration_set_cpi_notification_type(config, roccat_notification_type_combo_box_get_value(priv->cpi_notification_type));
	kone_configuration_set_only_notify_on_osd(config, gtk_toggle_button_get_active(priv->only_notify_on_osd));
	kone_configuration_set_default_profile_number(config, roccat_default_profile_combo_box_get_value(priv->default_profile_number));
	kone_configuration_set_notification_volume(config, roccat_volume_scale_get_value(priv->notification_volume));
}

void koneconfig_configuration_dialog_set_configuration(KoneconfigConfigurationDialog *koneconfig_configuration_dialog, RoccatKeyFile *config) {
	KoneconfigConfigurationDialogPrivate *priv = koneconfig_configuration_dialog->priv;

	roccat_notification_type_combo_box_set_value(GTK_COMBO_BOX(priv->profile_notification_type),
			kone_configuration_get_profile_notification_type(config));
	roccat_notification_type_combo_box_set_value(GTK_COMBO_BOX(priv->cpi_notification_type),
			kone_configuration_get_cpi_notification_type(config));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(priv->only_notify_on_osd),
			kone_configuration_get_only_notify_on_osd(config));
	roccat_default_profile_combo_box_set_value(priv->default_profile_number,
			kone_configuration_get_default_profile_number(config));
	roccat_volume_scale_set_value(priv->notification_volume, kone_configuration_get_notification_volume(config));
}

gboolean koneconfig_configuration_dialog(GtkWindow *parent, RoccatKeyFile *config) {
	KoneconfigConfigurationDialog *dialog;
	gboolean retval = TRUE;

	dialog = KONECONFIG_CONFIGURATION_DIALOG(koneconfig_configuration_dialog_new(parent));

	koneconfig_configuration_dialog_set_configuration(dialog, config);

	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
		koneconfig_configuration_dialog_get_configuration(dialog, config);
		if (!koneconfig_configuration_save(parent, config))
			retval = FALSE;
	}

	gtk_widget_destroy(GTK_WIDGET(dialog));
	return retval;
}

GtkWidget *koneconfig_configuration_dialog_new(GtkWindow *parent) {
	KoneconfigConfigurationDialog *koneconfig_configuration_dialog;

	koneconfig_configuration_dialog = g_object_new(KONECONFIG_CONFIGURATION_DIALOG_TYPE, NULL);

	gtk_dialog_add_buttons(GTK_DIALOG(koneconfig_configuration_dialog),
			GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			NULL);
	gtk_window_set_title(GTK_WINDOW(koneconfig_configuration_dialog), _("Edit preferences"));
	gtk_window_set_transient_for(GTK_WINDOW(koneconfig_configuration_dialog), parent);
	gtk_window_set_modal(GTK_WINDOW(koneconfig_configuration_dialog), TRUE);
	gtk_window_set_destroy_with_parent(GTK_WINDOW(koneconfig_configuration_dialog), TRUE);
	gtk_window_set_type_hint(GTK_WINDOW(koneconfig_configuration_dialog), GDK_WINDOW_TYPE_HINT_NORMAL);

	return GTK_WIDGET(koneconfig_configuration_dialog);
}

static void koneconfig_configuration_dialog_init(KoneconfigConfigurationDialog *koneconfig_configuration_dialog) {
	KoneconfigConfigurationDialogPrivate *priv = KONECONFIG_CONFIGURATION_DIALOG_GET_PRIVATE(koneconfig_configuration_dialog);
	GtkVBox *content_area;

	koneconfig_configuration_dialog->priv = priv;

	content_area = GTK_VBOX(gtk_dialog_get_content_area(GTK_DIALOG(koneconfig_configuration_dialog)));

	gtk_box_pack_start(GTK_BOX(content_area), notification_frame_new(koneconfig_configuration_dialog), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(content_area), konegui_configuration_osd_frame_new(koneconfig_configuration_dialog), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(content_area), general_frame_new(koneconfig_configuration_dialog), TRUE, TRUE, 0);

	gtk_widget_show_all(GTK_WIDGET(content_area));
}

static void koneconfig_configuration_dialog_class_init(KoneconfigConfigurationDialogClass *klass) {
	g_type_class_add_private(klass, sizeof(KoneconfigConfigurationDialogPrivate));
}

gboolean koneconfig_configuration_save(GtkWindow *parent, RoccatKeyFile *config) {
	GError *error = NULL;

	kone_configuration_save(config, &error);
	return roccat_handle_error_dialog(GTK_WINDOW(parent), _("Could not save configuration"), &error);
}
