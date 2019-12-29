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

#include "roccat_warning_dialog.h"
#include "roccat_firmware.h"
#include "roccat_strings.h"
#include "i18n-lib.h"

void roccat_warning_dialog(GtkWindow *parent, gchar const *first_text, gchar const *second_text) {
	GtkWidget *dialog;
	dialog = gtk_message_dialog_new(parent, GTK_DIALOG_MODAL, GTK_MESSAGE_WARNING, GTK_BUTTONS_OK, "%s", first_text);
	gtk_window_set_icon_name(GTK_WINDOW(dialog), "roccat");
	if (second_text)
		gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(dialog), "%s", second_text);
	gtk_window_set_title(GTK_WINDOW(dialog), _("Warning"));
	(void)gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
}

gboolean roccat_handle_error_dialog(GtkWindow *parent, gchar const *text, GError **error) {
	if (!*error)
		return TRUE;

	roccat_warning_dialog(parent, text, (*error)->message);
	g_clear_error(error);
	return FALSE;
}

void roccat_multiple_device_warning_dialog(GtkWindow *parent, gchar const *device_name) {
	gchar *first_text;
	gchar *second_text;

	first_text = g_strdup_printf(_("Multiple %s devices"), device_name);
	second_text = g_strdup_printf(_("This application can only handle one %s device."
			"\nPlease remove the newly plugged in device."), device_name);

	roccat_warning_dialog(parent, first_text, second_text);

	g_free(first_text);
	g_free(second_text);
}

gboolean roccat_no_device_warning_dialog(GtkWindow *parent, RoccatDevice const *device, gchar const *device_name) {
	gchar *first_text;
	gchar *second_text;

	if (device)
		return TRUE;

	first_text = g_strdup_printf(_("No %s found"), device_name);
	second_text = g_strdup_printf(_("No %s was found."
			"\n- Actions that need the device can't be issued."
			"\n- Data will only be stored on disk."), device_name);

	roccat_warning_dialog(parent, first_text, second_text);

	g_free(first_text);
	g_free(second_text);
	return FALSE;
}

gboolean roccat_too_old_firmware_version_dialog(GtkWindow *parent, guint has, guint needs) {
	gchar *string;

	if (roccat_check_firmware_version(has, needs))
		return TRUE;

	string = roccat_string_old_firmware_version(has, needs);
	roccat_warning_dialog(parent, _("You use an old firmware."), string);
	g_free(string);
	return FALSE;
}

gboolean gaminggear_macro_loop_not_supported_dialog(GtkWindow *parent, GaminggearMacro *gaminggear_macro) {
	guint loop;
	gchar *string;

	loop = gaminggear_macro->keystrokes.loop;

	if (loop == 1)
		return TRUE;

	string = g_strdup_printf(_("This macro is supposed to loop %i times."), loop);
	roccat_warning_dialog(parent, string,
			_("As this is not supported by this device, the macro will be executed only once."));
	g_free(string);
	return FALSE;
}
