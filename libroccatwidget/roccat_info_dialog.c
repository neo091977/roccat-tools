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

#include "roccat_info_dialog.h"
#include "i18n-lib.h"

#define ROCCAT_INFO_DIALOG_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), ROCCAT_INFO_DIALOG_TYPE, RoccatInfoDialogPrivate))

struct _RoccatInfoDialogPrivate {
	GtkTable *table;
	guint rows;
};

G_DEFINE_TYPE(RoccatInfoDialog, roccat_info_dialog, GTK_TYPE_DIALOG);

GtkWidget *roccat_info_dialog_new(GtkWindow *parent) {
	return GTK_WIDGET(g_object_new(ROCCAT_INFO_DIALOG_TYPE,
			"transient-for", parent,
			NULL));
}

void roccat_info_dialog_add_line(RoccatInfoDialog *info_dialog, gchar const *labeltext, GtkWidget *widget) {
	RoccatInfoDialogPrivate *priv = info_dialog->priv;
	GtkWidget *label;

	label = gtk_label_new(labeltext);

	gtk_table_resize(priv->table, priv->rows + 1, 2);

	gtk_table_attach(priv->table, label, 0, 1, priv->rows, priv->rows + 1, GTK_SHRINK, GTK_EXPAND, 5, 5);
	gtk_table_attach(priv->table, widget, 1, 2, priv->rows, priv->rows + 1, GTK_EXPAND, GTK_EXPAND, 5, 5);

	gtk_widget_show(label);
	gtk_widget_show(widget);

	++priv->rows;
}

static void roccat_info_dialog_init(RoccatInfoDialog *info_dialog) {
	RoccatInfoDialogPrivate *priv = ROCCAT_INFO_DIALOG_GET_PRIVATE(info_dialog);
	GtkVBox *content_area;

	info_dialog->priv = priv;

	gtk_dialog_add_buttons(GTK_DIALOG(info_dialog),
			GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
			NULL);
	gtk_window_set_modal(GTK_WINDOW(info_dialog), TRUE);
	gtk_window_set_destroy_with_parent(GTK_WINDOW(info_dialog), TRUE);
	gtk_window_set_title(GTK_WINDOW(info_dialog), _("Device info"));

	content_area = GTK_VBOX(gtk_dialog_get_content_area(GTK_DIALOG(info_dialog)));

	priv->table = GTK_TABLE(gtk_table_new(1, 2, FALSE));
	priv->rows = 0;

	gtk_box_pack_start(GTK_BOX(content_area), GTK_WIDGET(priv->table), TRUE, TRUE, 0);

	gtk_widget_show_all(GTK_WIDGET(content_area));
}

static void roccat_info_dialog_class_init(RoccatInfoDialogClass *klass) {
	g_type_class_add_private(klass, sizeof(RoccatInfoDialogPrivate));
}
