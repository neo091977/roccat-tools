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

#include "roccat_continue_dialog.h"
#include "i18n-lib.h"

gboolean roccat_continue_dialog(GtkWindow *parent, gchar const *first_text) {
	GtkWidget *dialog;
	gint response;

	dialog = gtk_message_dialog_new(parent, GTK_DIALOG_MODAL, GTK_MESSAGE_QUESTION, GTK_BUTTONS_OK_CANCEL, "%s", first_text);
	gtk_window_set_icon_name(GTK_WINDOW(dialog), "roccat");
	gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(dialog), _("Do you want to continue?"));
	gtk_window_set_title(GTK_WINDOW(dialog), _("Continue?"));
	response = gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
	return (response == GTK_RESPONSE_OK) ? TRUE : FALSE;
}
