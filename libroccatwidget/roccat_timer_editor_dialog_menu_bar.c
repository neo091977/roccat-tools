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

#include "roccat_config.h"
#include "roccat_timer_editor_dialog_menu_bar.h"
#include "roccat_about_dialog.h"
#include "roccat_warning_dialog.h"
#include "roccat_orig_timer.h"
#include "i18n-lib.h"

static GtkFileFilter *timer_file_filter_new(void) {
	GtkFileFilter *filter;
	filter = gtk_file_filter_new();
	gtk_file_filter_set_name(filter, _("Roccat timer files"));
	gtk_file_filter_add_pattern(filter, "Timer.ini");
	return filter;
}

static GtkFileFilter *all_file_filter_new(void) {
	GtkFileFilter *filter;
	filter = gtk_file_filter_new();
	gtk_file_filter_set_name(filter, _("All files"));
	gtk_file_filter_add_pattern(filter, "*");
	return filter;
}

static void show_import_cb(GtkMenuItem *item, gpointer user_data) {
	RoccatTimerEditorDialog *parent = ROCCAT_TIMER_EDITOR_DIALOG(user_data);
	RoccatKeyFile *config;
	GtkWidget *dialog;
	GError *error = NULL;
	gchar *filename, *path;
	RoccatTimers *roccat_timers;

	config = roccat_configuration_load();

	dialog = gtk_file_chooser_dialog_new(_("Import Roccat timers"),
			GTK_WINDOW(parent),
			GTK_FILE_CHOOSER_ACTION_OPEN,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
			NULL
	);

	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), timer_file_filter_new());
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), all_file_filter_new());

	path = roccat_configuration_get_timer_save_path(config);
	gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), path);
	g_free(path);

	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
		filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

		path = g_path_get_dirname(filename);
		roccat_configuration_set_timer_save_path(config, path);
		g_free(path);
		roccat_configuration_save(config, &error);

		roccat_timers = roccat_original_timers_import(filename, &error);
		if (error) {
			roccat_warning_dialog(GTK_WINDOW(parent), _("Could not import timers"), error->message);
			g_clear_error(&error);
		} else {
			roccat_timer_editor_dialog_add_timers(parent, roccat_timers);
			roccat_timers_free(roccat_timers);
		}
	}

	gtk_widget_destroy(dialog);
	roccat_configuration_free(config);
}

static void show_export_cb(GtkMenuItem *item, gpointer user_data) {
	RoccatTimerEditorDialog *parent = ROCCAT_TIMER_EDITOR_DIALOG(user_data);
	RoccatKeyFile *config;
	GtkWidget *dialog;
	GError *error = NULL;
	gchar *filename, *path;
	RoccatTimers *roccat_timers;

	config = roccat_configuration_load();

	dialog = gtk_file_chooser_dialog_new(_("Export Roccat timers"),
			GTK_WINDOW(parent),
			GTK_FILE_CHOOSER_ACTION_SAVE,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
			NULL
	);

	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), timer_file_filter_new());
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), all_file_filter_new());

	path = roccat_configuration_get_timer_save_path(config);
	gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), path);
	g_free(path);

	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
		filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

		path = g_path_get_dirname(filename);
		roccat_configuration_set_timer_save_path(config, path);
		g_free(path);
		roccat_configuration_save(config, &error);

		roccat_timers = roccat_timer_editor_dialog_get_timers(parent);

		roccat_original_timers_export(filename, roccat_timers, &error);
		roccat_timers_free(roccat_timers);

		if (error) {
			roccat_warning_dialog(GTK_WINDOW(parent), _("Could not export timers"), error->message);
			g_clear_error(&error);
		}
	}
	gtk_widget_destroy(dialog);
	roccat_configuration_free(config);
}

static void show_quit_cb(GtkMenuItem *item, gpointer user_data) {
	gtk_dialog_response(GTK_DIALOG(user_data), GTK_RESPONSE_REJECT);
}

static void show_about_cb(GtkMenuItem *item, gpointer user_data) {
	roccat_about_dialog(GTK_WINDOW(user_data), _("Roccat timer editor"), _("Application to edit Roccat timers"));
}

GtkWidget *roccat_timer_editor_dialog_menu_bar_new(RoccatTimerEditorDialog *dialog) {
	GtkWidget *menu_bar;
	GtkWidget *menu_item;
	GtkWidget *menu;

	menu_bar = gtk_menu_bar_new();

	menu_item = gtk_menu_item_new_with_label(_("File"));
	menu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_item), menu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), menu_item);

	menu_item = gtk_image_menu_item_new_with_label(_("Import"));
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);
	g_signal_connect(G_OBJECT(menu_item), "activate", G_CALLBACK(show_import_cb), dialog);

	menu_item = gtk_image_menu_item_new_with_label(_("Export"));
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);
	g_signal_connect(G_OBJECT(menu_item), "activate", G_CALLBACK(show_export_cb), dialog);

	menu_item = gtk_image_menu_item_new_from_stock(GTK_STOCK_QUIT, NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);
	g_signal_connect(G_OBJECT(menu_item), "activate", G_CALLBACK(show_quit_cb), dialog);

	menu_item = gtk_menu_item_new_with_label(_("Help"));
	menu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_item), menu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), menu_item);

	menu_item = gtk_image_menu_item_new_from_stock(GTK_STOCK_ABOUT, NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);
	g_signal_connect(G_OBJECT(menu_item), "activate", G_CALLBACK(show_about_cb), dialog);

	return menu_bar;
}
