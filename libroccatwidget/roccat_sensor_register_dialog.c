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

#include "roccat_sensor_register_dialog.h"
#include "roccat_timer_combo_box.h"
#include "i18n-lib.h"
#include <stdlib.h>

#define ROCCAT_SENSOR_REGISTER_DIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), ROCCAT_SENSOR_REGISTER_DIALOG_TYPE, RoccatSensorRegisterDialogClass))
#define IS_ROCCAT_SENSOR_REGISTER_DIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), ROCCAT_SENSOR_REGISTER_DIALOG_TYPE))
#define ROCCAT_SENSOR_REGISTER_DIALOG_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), ROCCAT_SENSOR_REGISTER_DIALOG_TYPE, RoccatSensorRegisterDialogPrivate))

typedef struct _RoccatSensorRegisterDialogClass RoccatSensorRegisterDialogClass;

struct _RoccatSensorRegisterDialogClass {
	GtkDialogClass parent_class;
	void (*read)(RoccatSensorRegisterDialog *dialog);
	void (*write)(RoccatSensorRegisterDialog *dialog);
};

struct _RoccatSensorRegisterDialogPrivate {
	GtkEntry *reg;
	GtkEntry *value;
};

G_DEFINE_TYPE(RoccatSensorRegisterDialog, roccat_sensor_register_dialog, GTK_TYPE_DIALOG);

enum {
	READ,
	WRITE,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

void roccat_sensor_register_dialog_set_value(RoccatSensorRegisterDialog *sensor_register_dialog, guint value) {
	gchar *string;
	string = g_strdup_printf("0x%02x", value);
	gtk_entry_set_text(sensor_register_dialog->priv->value, string);
	g_free(string);
}

guint roccat_sensor_register_dialog_get_value(RoccatSensorRegisterDialog *sensor_register_dialog) {
	guint value;
	gchar const *string;
	string = gtk_entry_get_text(sensor_register_dialog->priv->value);
	value = strtol(string, NULL, 0);
	return value;
}

guint roccat_sensor_register_dialog_get_register(RoccatSensorRegisterDialog *sensor_register_dialog) {
	guint value;
	gchar const *string;
	string = gtk_entry_get_text(sensor_register_dialog->priv->reg);
	value = strtol(string, NULL, 0);
	return value;
}

GtkWidget *roccat_sensor_register_dialog_new(GtkWindow *parent) {
	RoccatSensorRegisterDialog *sensor_register_dialog;

	sensor_register_dialog = g_object_new(ROCCAT_SENSOR_REGISTER_DIALOG_TYPE, NULL);

	gtk_dialog_add_buttons(GTK_DIALOG(sensor_register_dialog),
			GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
			NULL);
	gtk_window_set_title(GTK_WINDOW(sensor_register_dialog), _("Sensor register"));
	gtk_window_set_transient_for(GTK_WINDOW(sensor_register_dialog), parent);
	gtk_window_set_modal(GTK_WINDOW(sensor_register_dialog), TRUE);
	gtk_window_set_destroy_with_parent(GTK_WINDOW(sensor_register_dialog), TRUE);

	return GTK_WIDGET(sensor_register_dialog);
}

static void read_cb(GtkButton *button, gpointer user_data) {
	RoccatSensorRegisterDialog *sensor_register_dialog = ROCCAT_SENSOR_REGISTER_DIALOG(user_data);
	g_signal_emit((gpointer)sensor_register_dialog, signals[READ], 0);
}

static void write_cb(GtkButton *button, gpointer user_data) {
	RoccatSensorRegisterDialog *sensor_register_dialog = ROCCAT_SENSOR_REGISTER_DIALOG(user_data);
	g_signal_emit((gpointer)sensor_register_dialog, signals[WRITE], 0);
}

static void roccat_sensor_register_dialog_init(RoccatSensorRegisterDialog *sensor_register_dialog) {
	RoccatSensorRegisterDialogPrivate *priv = ROCCAT_SENSOR_REGISTER_DIALOG_GET_PRIVATE(sensor_register_dialog);
	GtkVBox *content_area;
	GtkTable *table;
	GtkWidget *read_button;
	GtkWidget *write_button;

	sensor_register_dialog->priv = priv;

	content_area = GTK_VBOX(gtk_dialog_get_content_area(GTK_DIALOG(sensor_register_dialog)));
	table = GTK_TABLE(gtk_table_new(2, 3, FALSE));
	read_button = gtk_button_new_with_label(_("Read"));
	write_button = gtk_button_new_with_label(_("Write"));
	priv->reg = GTK_ENTRY(gtk_entry_new());
	priv->value = GTK_ENTRY(gtk_entry_new());

	g_signal_connect(G_OBJECT(read_button), "clicked", G_CALLBACK(read_cb), sensor_register_dialog);
	g_signal_connect(G_OBJECT(write_button), "clicked", G_CALLBACK(write_cb), sensor_register_dialog);

	gtk_table_attach(table, gtk_label_new(_("Register")), 0, 1, 0, 1, GTK_EXPAND, GTK_EXPAND, 0, 0);
	gtk_table_attach(table, gtk_label_new(_("Value")), 0, 1, 1, 2, GTK_EXPAND, GTK_EXPAND, 0, 0);
	gtk_table_attach(table, GTK_WIDGET(priv->reg), 1, 2, 0, 1, GTK_EXPAND, GTK_EXPAND, 0, 0);
	gtk_table_attach(table, GTK_WIDGET(priv->value), 1, 2, 1, 2, GTK_EXPAND, GTK_EXPAND, 0, 0);
	gtk_table_attach(table, read_button, 2, 3, 0, 1, GTK_EXPAND, GTK_EXPAND, 0, 0);
	gtk_table_attach(table, write_button, 2, 3, 1, 2, GTK_EXPAND, GTK_EXPAND, 0, 0);

	gtk_box_pack_start(GTK_BOX(content_area), GTK_WIDGET(table), FALSE, FALSE, 0);

	gtk_widget_show_all(GTK_WIDGET(content_area));
}

static void roccat_sensor_register_dialog_class_init(RoccatSensorRegisterDialogClass *klass) {
	g_type_class_add_private(klass, sizeof(RoccatSensorRegisterDialogPrivate));

	signals[READ] = g_signal_new("read",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			G_STRUCT_OFFSET(RoccatSensorRegisterDialogClass, read),
			NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);

	signals[WRITE] = g_signal_new("write",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			G_STRUCT_OFFSET(RoccatSensorRegisterDialogClass, write),
			NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);
}
