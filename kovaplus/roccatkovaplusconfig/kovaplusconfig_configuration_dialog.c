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

#include "kovaplusconfig_configuration_dialog.h"
#include "kovaplus.h"
#include "roccat_warning_dialog.h"
#include "roccat_notification_type_combo_box.h"
#include "roccat_default_profile_combo_box.h"
#include "roccat_strings.h"
#include "g_roccat_helper.h"
#include "roccat_volume_scale.h"
#include "i18n.h"

#define KOVAPLUSCONFIG_CONFIGURATION_DIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), KOVAPLUSCONFIG_CONFIGURATION_DIALOG_TYPE, KovaplusconfigConfigurationDialogClass))
#define IS_KOVAPLUSCONFIG_CONFIGURATION_DIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), KOVAPLUSCONFIG_CONFIGURATION_DIALOG_TYPE))
#define KOVAPLUSCONFIG_CONFIGURATION_DIALOG_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), KOVAPLUSCONFIG_CONFIGURATION_DIALOG_TYPE, KovaplusconfigConfigurationDialogPrivate))

typedef struct _KovaplusconfigConfigurationDialogClass KovaplusconfigConfigurationDialogClass;

struct _KovaplusconfigConfigurationDialogClass {
	GtkDialogClass parent_class;
};

struct _KovaplusconfigConfigurationDialogPrivate {
	GtkComboBox *sensitivity_notification_type;
	GtkComboBox *timer_notification_type;
	GtkComboBox *profile_notification_type;
	GtkComboBox *cpi_notification_type;
	RoccatDefaultProfileComboBox *default_profile_number;
	RoccatVolumeScale *notification_volume;
};

G_DEFINE_TYPE(KovaplusconfigConfigurationDialog, kovaplusconfig_configuration_dialog, GTK_TYPE_DIALOG);

static GtkWidget *notification_frame_new(KovaplusconfigConfigurationDialog *kovaplusconfig_configuration_dialog) {
	KovaplusconfigConfigurationDialogPrivate *priv = kovaplusconfig_configuration_dialog->priv;
	GtkWidget *frame, *table;

	frame =gtk_frame_new(_("Notification"));
	table = gtk_table_new(5, 2, FALSE);
	gtk_container_add(GTK_CONTAINER(frame), table);

	gtk_table_attach(GTK_TABLE(table), gtk_label_new(_("Timer")), 0, 1, 0, 1, 0, GTK_EXPAND, 0, 0);
	priv->timer_notification_type = GTK_COMBO_BOX(roccat_notification_type_combo_box_new());
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(priv->timer_notification_type), 1, 2, 0, 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 0, 0);

	gtk_table_attach(GTK_TABLE(table), gtk_label_new(_("Profile")), 0, 1, 1, 2, 0, GTK_EXPAND, 0, 0);
	priv->profile_notification_type = GTK_COMBO_BOX(roccat_notification_type_combo_box_new());
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(priv->profile_notification_type), 1, 2, 1, 2, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 0, 0);

	gtk_table_attach(GTK_TABLE(table), gtk_label_new(_("CPI")), 0, 1, 2, 3, 0, GTK_EXPAND, 0, 0);
	priv->cpi_notification_type = GTK_COMBO_BOX(roccat_notification_type_combo_box_new());
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(priv->cpi_notification_type), 1, 2, 2, 3, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 0, 0);

	gtk_table_attach(GTK_TABLE(table), gtk_label_new(_("Sensitivity")), 0, 1, 3, 4, 0, GTK_EXPAND, 0, 0);
	priv->sensitivity_notification_type = GTK_COMBO_BOX(roccat_notification_type_combo_box_new());
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(priv->sensitivity_notification_type), 1, 2, 3, 4, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 0, 0);

	gtk_table_attach(GTK_TABLE(table), gtk_label_new(_("Volume")), 0, 1, 4, 5, 0, GTK_EXPAND, 0, 0);
	priv->notification_volume = ROCCAT_VOLUME_SCALE(roccat_volume_scale_new());
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(priv->notification_volume), 1, 2, 4, 5, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 0, 0);

	return frame;
}

static GtkWidget *general_frame_new(KovaplusconfigConfigurationDialog *kovaplusconfig_configuration_dialog) {
	KovaplusconfigConfigurationDialogPrivate *priv = kovaplusconfig_configuration_dialog->priv;
	GtkWidget *frame, *vbox, *hbox;

	frame =gtk_frame_new(_("General"));
	vbox = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(frame), vbox);

	hbox = gtk_hbox_new(FALSE, 0);
	gtk_widget_set_tooltip_text(hbox, roccat_string_default_profile_number_tooltip());
	gtk_box_pack_start(GTK_BOX(hbox), gtk_label_new(_("Default profile")), FALSE, TRUE, 0);
	priv->default_profile_number = ROCCAT_DEFAULT_PROFILE_COMBO_BOX(roccat_default_profile_combo_box_new(KOVAPLUS_PROFILE_NUM));
	gtk_box_pack_start(GTK_BOX(hbox), GTK_WIDGET(priv->default_profile_number), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, TRUE, 0);

	return frame;
}

void kovaplusconfig_configuration_dialog_get_configuration(KovaplusconfigConfigurationDialog *kovaplusconfig_configuration_dialog, RoccatKeyFile *config) {
	KovaplusconfigConfigurationDialogPrivate *priv = kovaplusconfig_configuration_dialog->priv;

	kovaplus_configuration_set_profile_notification_type(config, roccat_notification_type_combo_box_get_value(priv->profile_notification_type));
	kovaplus_configuration_set_cpi_notification_type(config, roccat_notification_type_combo_box_get_value(priv->cpi_notification_type));
	kovaplus_configuration_set_timer_notification_type(config, roccat_notification_type_combo_box_get_value(priv->timer_notification_type));
	kovaplus_configuration_set_sensitivity_notification_type(config, roccat_notification_type_combo_box_get_value(priv->sensitivity_notification_type));
	kovaplus_configuration_set_default_profile_number(config, roccat_default_profile_combo_box_get_value(priv->default_profile_number));
	kovaplus_configuration_set_notification_volume(config, roccat_volume_scale_get_value(priv->notification_volume));
}

void kovaplusconfig_configuration_dialog_set_configuration(KovaplusconfigConfigurationDialog *kovaplusconfig_configuration_dialog, RoccatKeyFile *config) {
	KovaplusconfigConfigurationDialogPrivate *priv = kovaplusconfig_configuration_dialog->priv;

	roccat_notification_type_combo_box_set_value(GTK_COMBO_BOX(priv->timer_notification_type),
			kovaplus_configuration_get_timer_notification_type(config));
	roccat_notification_type_combo_box_set_value(GTK_COMBO_BOX(priv->profile_notification_type),
			kovaplus_configuration_get_profile_notification_type(config));
	roccat_notification_type_combo_box_set_value(GTK_COMBO_BOX(priv->cpi_notification_type),
			kovaplus_configuration_get_cpi_notification_type(config));
	roccat_notification_type_combo_box_set_value(GTK_COMBO_BOX(priv->sensitivity_notification_type),
			kovaplus_configuration_get_sensitivity_notification_type(config));
	roccat_default_profile_combo_box_set_value(priv->default_profile_number,
			kovaplus_configuration_get_default_profile_number(config));
	roccat_volume_scale_set_value(priv->notification_volume, kovaplus_configuration_get_notification_volume(config));
}

gboolean kovaplusconfig_configuration_dialog(GtkWindow *parent, RoccatKeyFile *config) {
	KovaplusconfigConfigurationDialog *dialog;
	gboolean retval = TRUE;

	dialog = KOVAPLUSCONFIG_CONFIGURATION_DIALOG(kovaplusconfig_configuration_dialog_new(parent));

	kovaplusconfig_configuration_dialog_set_configuration(dialog, config);

	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
		kovaplusconfig_configuration_dialog_get_configuration(dialog, config);
		if (!kovaplusconfig_configuration_save(parent, config))
			retval = FALSE;
	}

	gtk_widget_destroy(GTK_WIDGET(dialog));
	return retval;
}

GtkWidget *kovaplusconfig_configuration_dialog_new(GtkWindow *parent) {
	KovaplusconfigConfigurationDialog *kovaplusconfig_configuration_dialog;

	kovaplusconfig_configuration_dialog = g_object_new(KOVAPLUSCONFIG_CONFIGURATION_DIALOG_TYPE, NULL);

	gtk_dialog_add_buttons(GTK_DIALOG(kovaplusconfig_configuration_dialog),
			GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			NULL);
	gtk_window_set_title(GTK_WINDOW(kovaplusconfig_configuration_dialog), _("Edit preferences"));
	gtk_window_set_transient_for(GTK_WINDOW(kovaplusconfig_configuration_dialog), parent);
	gtk_window_set_modal(GTK_WINDOW(kovaplusconfig_configuration_dialog), TRUE);
	gtk_window_set_destroy_with_parent(GTK_WINDOW(kovaplusconfig_configuration_dialog), TRUE);
	gtk_window_set_type_hint(GTK_WINDOW(kovaplusconfig_configuration_dialog), GDK_WINDOW_TYPE_HINT_NORMAL);

	return GTK_WIDGET(kovaplusconfig_configuration_dialog);
}

static void kovaplusconfig_configuration_dialog_init(KovaplusconfigConfigurationDialog *kovaplusconfig_configuration_dialog) {
	KovaplusconfigConfigurationDialogPrivate *priv = KOVAPLUSCONFIG_CONFIGURATION_DIALOG_GET_PRIVATE(kovaplusconfig_configuration_dialog);
	GtkVBox *content_area;

	kovaplusconfig_configuration_dialog->priv = priv;

	content_area = GTK_VBOX(gtk_dialog_get_content_area(GTK_DIALOG(kovaplusconfig_configuration_dialog)));

	gtk_box_pack_start(GTK_BOX(content_area), notification_frame_new(kovaplusconfig_configuration_dialog), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(content_area), general_frame_new(kovaplusconfig_configuration_dialog), TRUE, TRUE, 0);

	gtk_widget_show_all(GTK_WIDGET(content_area));
}

static void kovaplusconfig_configuration_dialog_class_init(KovaplusconfigConfigurationDialogClass *klass) {
	g_type_class_add_private(klass, sizeof(KovaplusconfigConfigurationDialogPrivate));
}

gboolean kovaplusconfig_configuration_save(GtkWindow *parent, RoccatKeyFile *config) {
	GError *error = NULL;

	kovaplus_configuration_save(config, &error);
	return roccat_handle_error_dialog(GTK_WINDOW(parent), _("Could not save configuration"), &error);
}
