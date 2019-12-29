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

#include "roccat_save_dialog.h"
#include "i18n-lib.h"

gint roccat_save_dialog(GtkWindow *parent, gchar const *text, gboolean cancel) {
	GtkWidget *dialog, *label1, *label2, *image, *hbox, *vbox;
	gint retval;

	dialog = gtk_dialog_new_with_buttons(_("Warning"),
			parent, GTK_DIALOG_MODAL,
			GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
			GTK_STOCK_DISCARD, GTK_RESPONSE_REJECT,
			NULL
	);

	if (cancel)
		gtk_dialog_add_button(GTK_DIALOG(dialog), GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL);

	gtk_window_set_icon_name(GTK_WINDOW(dialog), "roccat");
	label1 = gtk_label_new(text);
	label2 = gtk_label_new(_("Do you want to save?"));
	image = gtk_image_new_from_stock(GTK_STOCK_DIALOG_WARNING, GTK_ICON_SIZE_DIALOG);
	vbox = gtk_vbox_new(FALSE, 5);
	hbox = gtk_hbox_new(FALSE, 5);
	gtk_container_set_border_width(GTK_CONTAINER(hbox), 10);
	gtk_box_pack_start(GTK_BOX(hbox), image, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(hbox), vbox, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), label1, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), label2, FALSE, FALSE, 0);

	gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog))), hbox, FALSE, FALSE, 0);
	gtk_widget_show_all(dialog);

	retval = gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
	return retval;
}

gint roccat_save_unsaved_dialog(GtkWindow *parent, gboolean cancel) {
	return roccat_save_dialog(parent, _("There is unsaved data."), cancel);
}
