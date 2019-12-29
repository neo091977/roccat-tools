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

#include "nyth_configuration_dialog.h"
#include "nyth_config.h"
#include "nyth.h"
#include "roccat_warning_dialog.h"
#include "roccat_default_profile_combo_box.h"
#include "roccat_strings.h"
#include "g_roccat_helper.h"
#include "i18n.h"

#define NYTH_CONFIGURATION_DIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), NYTH_CONFIGURATION_DIALOG_TYPE, NythConfigurationDialogClass))
#define IS_NYTH_CONFIGURATION_DIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), NYTH_CONFIGURATION_DIALOG_TYPE))
#define NYTH_CONFIGURATION_DIALOG_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), NYTH_CONFIGURATION_DIALOG_TYPE, NythConfigurationDialogPrivate))

typedef struct _NythConfigurationDialogClass NythConfigurationDialogClass;

struct _NythConfigurationDialogClass {
	GtkDialogClass parent_class;
};

struct _NythConfigurationDialogPrivate {
	RoccatDefaultProfileComboBox *default_profile_number;
};

G_DEFINE_TYPE(NythConfigurationDialog, nyth_configuration_dialog, GTK_TYPE_DIALOG);

static GtkWidget *general_frame_new(NythConfigurationDialog *nyth_configuration_dialog) {
	NythConfigurationDialogPrivate *priv = nyth_configuration_dialog->priv;
	GtkWidget *frame, *table, *label;

	frame =gtk_frame_new(_("General"));
	table = gtk_table_new(2, 2, FALSE);
	gtk_container_add(GTK_CONTAINER(frame), table);

	label = gtk_label_new(_("Default profile"));
	gtk_widget_set_tooltip_text(label, roccat_string_default_profile_number_tooltip());
	priv->default_profile_number = ROCCAT_DEFAULT_PROFILE_COMBO_BOX(roccat_default_profile_combo_box_new(NYTH_PROFILE_NUM));
	gtk_widget_set_tooltip_text(GTK_WIDGET(priv->default_profile_number), roccat_string_default_profile_number_tooltip());
	gtk_table_attach(GTK_TABLE(table), label, 0, 1, 0, 1, GTK_FILL, GTK_EXPAND, 0, 0);
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(priv->default_profile_number), 1, 2, 0, 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 0, 0);

	return frame;
}

void nyth_configuration_dialog_get_configuration(NythConfigurationDialog *configuration_dialog, RoccatKeyFile *config) {
	nyth_configuration_set_default_profile_number(config, roccat_default_profile_combo_box_get_value(configuration_dialog->priv->default_profile_number));
}

void nyth_configuration_dialog_set_configuration(NythConfigurationDialog *configuration_dialog, RoccatKeyFile *config) {
	roccat_default_profile_combo_box_set_value(configuration_dialog->priv->default_profile_number,
			nyth_configuration_get_default_profile_number(config));
}

gboolean nyth_configuration_dialog_save(GtkWindow *parent, RoccatKeyFile *config) {
	GError *error = NULL;

	roccat_configuration_save(config, &error);
	return roccat_handle_error_dialog(GTK_WINDOW(parent), _("Could not save configuration"), &error);
}

gboolean nyth_configuration_dialog(GtkWindow *parent, RoccatKeyFile *config) {
	NythConfigurationDialog *dialog;
	gboolean retval = TRUE;

	dialog = NYTH_CONFIGURATION_DIALOG(nyth_configuration_dialog_new(parent));

	nyth_configuration_dialog_set_configuration(dialog, config);

	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
		nyth_configuration_dialog_get_configuration(dialog, config);
		if (!nyth_configuration_dialog_save(parent, config))
			retval = FALSE;
	}

	gtk_widget_destroy(GTK_WIDGET(dialog));
	return retval;
}

GtkWidget *nyth_configuration_dialog_new(GtkWindow *parent) {
	NythConfigurationDialog *nyth_configuration_dialog;

	nyth_configuration_dialog = g_object_new(NYTH_CONFIGURATION_DIALOG_TYPE, NULL);

	gtk_dialog_add_buttons(GTK_DIALOG(nyth_configuration_dialog),
			GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			NULL);
	gtk_window_set_title(GTK_WINDOW(nyth_configuration_dialog), _("Edit preferences"));
	gtk_window_set_transient_for(GTK_WINDOW(nyth_configuration_dialog), parent);
	gtk_window_set_modal(GTK_WINDOW(nyth_configuration_dialog), TRUE);
	gtk_window_set_destroy_with_parent(GTK_WINDOW(nyth_configuration_dialog), TRUE);
	gtk_window_set_type_hint(GTK_WINDOW(nyth_configuration_dialog), GDK_WINDOW_TYPE_HINT_NORMAL);

	return GTK_WIDGET(nyth_configuration_dialog);
}

static void nyth_configuration_dialog_init(NythConfigurationDialog *nyth_configuration_dialog) {
	NythConfigurationDialogPrivate *priv = NYTH_CONFIGURATION_DIALOG_GET_PRIVATE(nyth_configuration_dialog);
	GtkVBox *content_area;

	nyth_configuration_dialog->priv = priv;

	content_area = GTK_VBOX(gtk_dialog_get_content_area(GTK_DIALOG(nyth_configuration_dialog)));

	gtk_box_pack_start(GTK_BOX(content_area), general_frame_new(nyth_configuration_dialog), TRUE, TRUE, 0);

	gtk_widget_show_all(GTK_WIDGET(content_area));
}

static void nyth_configuration_dialog_class_init(NythConfigurationDialogClass *klass) {
	g_type_class_add_private(klass, sizeof(NythConfigurationDialogPrivate));
}
