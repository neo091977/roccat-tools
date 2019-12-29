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

#include "konepuremilitary_move_dialog.h"
#include <gaminggear/gaminggear_timeout_bar.h>

#define KONEPUREMILITARY_MOVE_DIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), KONEPUREMILITARY_MOVE_DIALOG_TYPE, KonepuremilitaryMoveDialogClass))
#define IS_KONEPUREMILITARY_MOVE_DIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), KONEPUREMILITARY_MOVE_DIALOG_TYPE))
#define KONEPUREMILITARY_MOVE_DIALOG_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), KONEPUREMILITARY_MOVE_DIALOG_TYPE, KonepuremilitaryMoveDialogPrivate))

typedef struct _KonepuremilitaryMoveDialogClass KonepuremilitaryMoveDialogClass;

struct _KonepuremilitaryMoveDialogClass {
	GtkDialogClass parent_class;
};

struct _KonepuremilitaryMoveDialogPrivate {
	GtkLabel *text_label;
	GaminggearTimeoutBar *timeout_bar;
};

G_DEFINE_TYPE(KonepuremilitaryMoveDialog, konepuremilitary_move_dialog, GTK_TYPE_DIALOG);

static guint const interval = 200; /* msecs */

gboolean konepuremilitary_move_dialog(GtkWindow *parent, gchar const *title, gchar const *text, guint timeout) {
	KonepuremilitaryMoveDialog *move_dialog;
	gboolean retval;

	move_dialog = KONEPUREMILITARY_MOVE_DIALOG(konepuremilitary_move_dialog_new(parent, title, text));
	retval = konepuremilitary_move_dialog_run(move_dialog, timeout);
	gtk_widget_destroy(GTK_WIDGET(move_dialog));

	return retval;
}

gboolean konepuremilitary_move_dialog_run(KonepuremilitaryMoveDialog *move_dialog, guint timeout) {
	KonepuremilitaryMoveDialogPrivate *priv = move_dialog->priv;
	gboolean retval;

	gaminggear_timeout_bar_start(priv->timeout_bar, interval, timeout);
	retval = gtk_dialog_run(GTK_DIALOG(move_dialog)) == GTK_RESPONSE_ACCEPT ? TRUE : FALSE;
	gaminggear_timeout_bar_stop(priv->timeout_bar);

	return retval;
}

GtkWidget *konepuremilitary_move_dialog_new(GtkWindow *parent, gchar const *title, gchar const *text) {
	KonepuremilitaryMoveDialog *move_dialog;

	move_dialog = g_object_new(KONEPUREMILITARY_MOVE_DIALOG_TYPE, NULL);

	gtk_dialog_add_buttons(GTK_DIALOG(move_dialog),
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			NULL);
	gtk_window_set_title(GTK_WINDOW(move_dialog), title);
	gtk_window_set_transient_for(GTK_WINDOW(move_dialog), parent);
	gtk_window_set_modal(GTK_WINDOW(move_dialog), TRUE);
	gtk_window_set_destroy_with_parent(GTK_WINDOW(move_dialog), TRUE);

	gtk_label_set_text(move_dialog->priv->text_label, text);

	return GTK_WIDGET(move_dialog);
}

static void timeout_cb(GaminggearTimeoutBar *timeout_bar, gpointer user_data) {
	gtk_dialog_response(GTK_DIALOG(user_data), GTK_RESPONSE_ACCEPT);
}

static void konepuremilitary_move_dialog_init(KonepuremilitaryMoveDialog *move_dialog) {
	KonepuremilitaryMoveDialogPrivate *priv = KONEPUREMILITARY_MOVE_DIALOG_GET_PRIVATE(move_dialog);
	GtkVBox *content_area;

	move_dialog->priv = priv;

	content_area = GTK_VBOX(gtk_dialog_get_content_area(GTK_DIALOG(move_dialog)));

	priv->text_label = GTK_LABEL(gtk_label_new(NULL));
	gtk_box_pack_start(GTK_BOX(content_area), GTK_WIDGET(priv->text_label), FALSE, FALSE, 0);

	priv->timeout_bar = GAMINGGEAR_TIMEOUT_BAR(gaminggear_timeout_bar_new());
	gtk_box_pack_start(GTK_BOX(content_area), GTK_WIDGET(priv->timeout_bar), FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(priv->timeout_bar), "timeout", G_CALLBACK(timeout_cb), move_dialog);

	gtk_widget_show_all(GTK_WIDGET(content_area));
}

static void konepuremilitary_move_dialog_class_init(KonepuremilitaryMoveDialogClass *klass) {
	g_type_class_add_private(klass, sizeof(KonepuremilitaryMoveDialogPrivate));
}
