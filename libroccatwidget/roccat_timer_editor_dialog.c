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

#include "roccat_timer_editor_dialog.h"
#include "roccat_timer_editor_dialog_menu_bar.h"
#include "roccat_timer_tree_view.h"
#include "roccat_timer_list_store.h"
#include "roccat_save_dialog.h"
#include "roccat_warning_dialog.h"
#include "i18n-lib.h"

#define ROCCAT_TIMER_EDITOR_DIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), ROCCAT_TIMER_EDITOR_DIALOG_TYPE, RoccatTimerEditorDialogClass))
#define IS_ROCCAT_TIMER_EDITOR_DIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), ROCCAT_TIMER_EDITOR_DIALOG_TYPE))
#define ROCCAT_TIMER_EDITOR_DIALOG_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), ROCCAT_TIMER_EDITOR_DIALOG_TYPE, RoccatTimerEditorDialogPrivate))

typedef struct _RoccatTimerEditorDialogClass RoccatTimerEditorDialogClass;

struct _RoccatTimerEditorDialogClass {
	GtkDialogClass parent_class;
};

struct _RoccatTimerEditorDialogPrivate {
	RoccatTimerTreeView *tree_view;
};

G_DEFINE_TYPE(RoccatTimerEditorDialog, roccat_timer_editor_dialog, GTK_TYPE_DIALOG);

void roccat_timer_editor_dialog(GtkWindow *parent) {
	RoccatTimerEditorDialog *dialog;
	RoccatTimerListStore *store;

	dialog = ROCCAT_TIMER_EDITOR_DIALOG(roccat_timer_editor_dialog_new(parent));

	roccat_timer_editor_dialog_load_timers(dialog);

	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
		roccat_timer_editor_dialog_save_timers(dialog);
	else {
		store = roccat_timer_tree_view_get_store(dialog->priv->tree_view);
		if (roccat_timer_list_store_get_modified(store)) {
			if (roccat_save_unsaved_dialog(parent, FALSE) == GTK_RESPONSE_ACCEPT)
				roccat_timer_editor_dialog_save_timers(dialog);
		}
	}

	gtk_widget_destroy(GTK_WIDGET(dialog));
}

void roccat_timer_editor_dialog_load_timers(RoccatTimerEditorDialog *dialog) {
	RoccatTimers *roccat_timers;
	GError *error = NULL;

	roccat_timers = roccat_timers_load(&error);
	if (error) {
		g_warning(_("Could not load timers: %s"), error->message);
		g_error_free(error);
		return;
	}

	roccat_timer_tree_view_import(dialog->priv->tree_view, roccat_timers);
	roccat_timers_free(roccat_timers);
}

gboolean roccat_timer_editor_dialog_save_timers(RoccatTimerEditorDialog *dialog) {
	RoccatTimers *roccat_timers;
	gboolean result;
	GError *error = NULL;

	roccat_timers = roccat_timer_tree_view_export(dialog->priv->tree_view);
	result = roccat_timers_save(roccat_timers, &error);
	roccat_timers_free(roccat_timers);

	if (error) {
		roccat_warning_dialog(GTK_WINDOW(dialog), _("Could not save timers"), error->message);
		g_clear_error(&error);
	} else
		roccat_timer_list_store_set_modified(roccat_timer_tree_view_get_store(dialog->priv->tree_view), FALSE);

	return result;
}

GtkWidget *roccat_timer_editor_dialog_new(GtkWindow *parent) {
	RoccatTimerEditorDialog *timer_editor_dialog;

	timer_editor_dialog = g_object_new(ROCCAT_TIMER_EDITOR_DIALOG_TYPE, NULL);

	gtk_dialog_add_buttons(GTK_DIALOG(timer_editor_dialog),
			GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
			GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT,
			NULL);
	gtk_window_set_title(GTK_WINDOW(timer_editor_dialog), _("Edit timers"));
	gtk_window_set_transient_for(GTK_WINDOW(timer_editor_dialog), parent);
	gtk_window_set_modal(GTK_WINDOW(timer_editor_dialog), TRUE);
	gtk_window_set_destroy_with_parent(GTK_WINDOW(timer_editor_dialog), TRUE);
	gtk_window_set_type_hint(GTK_WINDOW(timer_editor_dialog), GDK_WINDOW_TYPE_HINT_NORMAL);
	gtk_window_resize(GTK_WINDOW(timer_editor_dialog), 640, 480);

	return GTK_WIDGET(timer_editor_dialog);
}

static void add_cb(GtkButton *button, gpointer user_data) {
	roccat_timer_tree_view_add(ROCCAT_TIMER_TREE_VIEW(user_data));
}

static void remove_cb(GtkButton *button, gpointer user_data) {
	roccat_timer_tree_view_remove(ROCCAT_TIMER_TREE_VIEW(user_data));
}

static void roccat_timer_editor_dialog_init(RoccatTimerEditorDialog *timer_editor_dialog) {
	RoccatTimerEditorDialogPrivate *priv = ROCCAT_TIMER_EDITOR_DIALOG_GET_PRIVATE(timer_editor_dialog);
	GtkVBox *content_area;
	RoccatTimerListStore *store;
	GtkWidget *frame;
	GtkWidget *vbox;
	GtkWidget *scrolled_window;
	GtkWidget *hbox;
	GtkWidget *add_button;
	GtkWidget *remove_button;

	timer_editor_dialog->priv = priv;

	content_area = GTK_VBOX(gtk_dialog_get_content_area(GTK_DIALOG(timer_editor_dialog)));

	frame = gtk_frame_new(_("Timers"));
	vbox = gtk_vbox_new(FALSE, 0);

	scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

	hbox = gtk_hbox_new(FALSE, 0);
	add_button = gtk_button_new_with_label(_("Add"));
	remove_button = gtk_button_new_with_label(_("Remove"));

	store = roccat_timer_list_store_new();
	priv->tree_view = ROCCAT_TIMER_TREE_VIEW(roccat_timer_tree_view_new(store));
	g_object_unref(store);

	gtk_container_add(GTK_CONTAINER(scrolled_window), GTK_WIDGET(priv->tree_view));

	gtk_box_pack_start(GTK_BOX(hbox), add_button, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(hbox), remove_button, FALSE, FALSE, 0);

	gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);

	gtk_container_add(GTK_CONTAINER(frame), vbox);

	gtk_box_pack_start(GTK_BOX(content_area), roccat_timer_editor_dialog_menu_bar_new(timer_editor_dialog), FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(content_area), frame, TRUE, TRUE, 0);

	g_signal_connect(G_OBJECT(add_button), "clicked", G_CALLBACK(add_cb), priv->tree_view);
	g_signal_connect(G_OBJECT(remove_button), "clicked", G_CALLBACK(remove_cb), priv->tree_view);

	gtk_widget_show_all(GTK_WIDGET(content_area));
}

static void roccat_timer_editor_dialog_class_init(RoccatTimerEditorDialogClass *klass) {
	g_type_class_add_private(klass, sizeof(RoccatTimerEditorDialogPrivate));
}

void roccat_timer_editor_dialog_add_timers(RoccatTimerEditorDialog *dialog, RoccatTimers *roccat_timers) {
	RoccatTimerListStore *store;
	store = roccat_timer_tree_view_get_store(dialog->priv->tree_view);
	roccat_timer_list_store_add_timers(store, roccat_timers);
}

RoccatTimers *roccat_timer_editor_dialog_get_timers(RoccatTimerEditorDialog *dialog) {
	return roccat_timer_tree_view_export(dialog->priv->tree_view);
}
