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

#include "kiro_configuration_dialog.h"
#include "kiro_config.h"
#include "kiro.h"
#include "roccat_warning_dialog.h"
#include "roccat_default_profile_combo_box.h"
#include "roccat_strings.h"
#include "g_roccat_helper.h"
#include "i18n.h"

#define KIRO_CONFIGURATION_DIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), KIRO_CONFIGURATION_DIALOG_TYPE, KiroConfigurationDialogClass))
#define IS_KIRO_CONFIGURATION_DIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), KIRO_CONFIGURATION_DIALOG_TYPE))
#define KIRO_CONFIGURATION_DIALOG_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), KIRO_CONFIGURATION_DIALOG_TYPE, KiroConfigurationDialogPrivate))

typedef struct _KiroConfigurationDialogClass KiroConfigurationDialogClass;

struct _KiroConfigurationDialogClass {
	GtkDialogClass parent_class;
};

struct _KiroConfigurationDialogPrivate {
	RoccatDefaultProfileComboBox *default_profile_number;
};

G_DEFINE_TYPE(KiroConfigurationDialog, kiro_configuration_dialog, GTK_TYPE_DIALOG);

static GtkWidget *general_frame_new(KiroConfigurationDialog *kiro_configuration_dialog) {
	KiroConfigurationDialogPrivate *priv = kiro_configuration_dialog->priv;
	GtkWidget *frame, *table, *label;

	frame =gtk_frame_new(_("General"));
	table = gtk_table_new(2, 2, FALSE);
	gtk_container_add(GTK_CONTAINER(frame), table);

	label = gtk_label_new(_("Default profile"));
	gtk_widget_set_tooltip_text(label, roccat_string_default_profile_number_tooltip());
	priv->default_profile_number = ROCCAT_DEFAULT_PROFILE_COMBO_BOX(roccat_default_profile_combo_box_new(KIRO_PROFILE_NUM));
	gtk_widget_set_tooltip_text(GTK_WIDGET(priv->default_profile_number), roccat_string_default_profile_number_tooltip());
	gtk_table_attach(GTK_TABLE(table), label, 0, 1, 0, 1, GTK_FILL, GTK_EXPAND, 0, 0);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(priv->default_profile_number), 1, 2, 0, 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 0, 0);

	return frame;
}

void kiro_configuration_dialog_get_configuration(KiroConfigurationDialog *configuration_dialog, RoccatKeyFile *config) {
	kiro_configuration_set_default_profile_number(config, roccat_default_profile_combo_box_get_value(configuration_dialog->priv->default_profile_number));
}

void kiro_configuration_dialog_set_configuration(KiroConfigurationDialog *configuration_dialog, RoccatKeyFile *config) {
	roccat_default_profile_combo_box_set_value(configuration_dialog->priv->default_profile_number,
			kiro_configuration_get_default_profile_number(config));
}

gboolean kiro_configuration_dialog_save(GtkWindow *parent, RoccatKeyFile *config) {
	GError *error = NULL;

	roccat_configuration_save(config, &error);
	return roccat_handle_error_dialog(GTK_WINDOW(parent), _("Could not save configuration"), &error);
}

gboolean kiro_configuration_dialog(GtkWindow *parent, RoccatKeyFile *config) {
	KiroConfigurationDialog *dialog;
	gboolean retval = TRUE;

	dialog = KIRO_CONFIGURATION_DIALOG(kiro_configuration_dialog_new(parent));

	kiro_configuration_dialog_set_configuration(dialog, config);

	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
		kiro_configuration_dialog_get_configuration(dialog, config);
		if (!kiro_configuration_dialog_save(parent, config))
			retval = FALSE;
	}

	gtk_widget_destroy(GTK_WIDGET(dialog));
	return retval;
}

GtkWidget *kiro_configuration_dialog_new(GtkWindow *parent) {
	KiroConfigurationDialog *kiro_configuration_dialog;

	kiro_configuration_dialog = g_object_new(KIRO_CONFIGURATION_DIALOG_TYPE, NULL);

	gtk_dialog_add_buttons(GTK_DIALOG(kiro_configuration_dialog),
			GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			NULL);
	gtk_window_set_title(GTK_WINDOW(kiro_configuration_dialog), _("Edit preferences"));
	gtk_window_set_transient_for(GTK_WINDOW(kiro_configuration_dialog), parent);
	gtk_window_set_modal(GTK_WINDOW(kiro_configuration_dialog), TRUE);
	gtk_window_set_destroy_with_parent(GTK_WINDOW(kiro_configuration_dialog), TRUE);
	gtk_window_set_type_hint(GTK_WINDOW(kiro_configuration_dialog), GDK_WINDOW_TYPE_HINT_NORMAL);

	return GTK_WIDGET(kiro_configuration_dialog);
}

static void kiro_configuration_dialog_init(KiroConfigurationDialog *kiro_configuration_dialog) {
	KiroConfigurationDialogPrivate *priv = KIRO_CONFIGURATION_DIALOG_GET_PRIVATE(kiro_configuration_dialog);
	GtkVBox *content_area;

	kiro_configuration_dialog->priv = priv;

	content_area = GTK_VBOX(gtk_dialog_get_content_area(GTK_DIALOG(kiro_configuration_dialog)));

	gtk_box_pack_start(GTK_BOX(content_area), general_frame_new(kiro_configuration_dialog), TRUE, TRUE, 0);

	gtk_widget_show_all(GTK_WIDGET(content_area));
}

static void kiro_configuration_dialog_class_init(KiroConfigurationDialogClass *klass) {
	g_type_class_add_private(klass, sizeof(KiroConfigurationDialogPrivate));
}
