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

#include "roccat_timer_dialog.h"
#include "roccat_timer_combo_box.h"
#include "i18n-lib.h"

#define ROCCAT_TIMER_DIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), ROCCAT_TIMER_DIALOG_TYPE, RoccatTimerDialogClass))
#define IS_ROCCAT_TIMER_DIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), ROCCAT_TIMER_DIALOG_TYPE))
#define ROCCAT_TIMER_DIALOG_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), ROCCAT_TIMER_DIALOG_TYPE, RoccatTimerDialogPrivate))

typedef struct _RoccatTimerDialogClass RoccatTimerDialogClass;

struct _RoccatTimerDialogClass {
	GtkDialogClass parent_class;
};

struct _RoccatTimerDialogPrivate {
	GtkWidget *ok_button;
	GtkComboBox *combo_box;
};

G_DEFINE_TYPE(RoccatTimerDialog, roccat_timer_dialog, GTK_TYPE_DIALOG);

RoccatTimer *roccat_timer_dialog(GtkWindow *parent) {
	RoccatTimerDialog *timer_dialog;
	RoccatTimers *timers;
	RoccatTimer *timer;
	GError *error = NULL;

	timers = roccat_timers_load(&error);
	if (error) {
		g_warning(_("Could not load timers: %s"), error->message);
		return NULL;
	}

	timer_dialog = ROCCAT_TIMER_DIALOG(roccat_timer_dialog_new(parent));
	roccat_timer_dialog_set_timers(timer_dialog, timers);
	roccat_timers_free(timers);

	timer = roccat_timer_dialog_run(timer_dialog);
	gtk_widget_destroy(GTK_WIDGET(timer_dialog));

	return timer;
}

RoccatTimer *roccat_timer_dialog_run(RoccatTimerDialog *timer_dialog) {
	if (gtk_dialog_run(GTK_DIALOG(timer_dialog)) == GTK_RESPONSE_ACCEPT)
		return roccat_timer_dialog_get_value(timer_dialog);
	else
		return NULL;
}

RoccatTimer *roccat_timer_dialog_get_value(RoccatTimerDialog *timer_dialog) {
	return roccat_timer_combo_box_get_value(timer_dialog->priv->combo_box);
}

void roccat_timer_dialog_set_timers(RoccatTimerDialog *timer_dialog, RoccatTimers *timers) {
	roccat_timer_combo_box_set_timers(timer_dialog->priv->combo_box, timers);
}

GtkWidget *roccat_timer_dialog_new(GtkWindow *parent) {
	RoccatTimerDialog *timer_dialog;
	RoccatTimerDialogPrivate *priv;

	timer_dialog = g_object_new(ROCCAT_TIMER_DIALOG_TYPE, NULL);
	priv = timer_dialog->priv;

	priv->ok_button = gtk_dialog_add_button(GTK_DIALOG(timer_dialog), GTK_STOCK_OK, GTK_RESPONSE_ACCEPT);
	gtk_widget_set_sensitive(priv->ok_button, FALSE);

	gtk_dialog_add_button(GTK_DIALOG(timer_dialog), GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT);

	gtk_window_set_title(GTK_WINDOW(timer_dialog), _("Select timer"));
	gtk_window_set_transient_for(GTK_WINDOW(timer_dialog), parent);
	gtk_window_set_modal(GTK_WINDOW(timer_dialog), TRUE);
	gtk_window_set_destroy_with_parent(GTK_WINDOW(timer_dialog), TRUE);

	return GTK_WIDGET(timer_dialog);
}

static void changed_cb(GtkComboBox *combo_box, gpointer user_data) {
	RoccatTimerDialog *timer_dialog = ROCCAT_TIMER_DIALOG(user_data);
	gtk_widget_set_sensitive(timer_dialog->priv->ok_button, TRUE);
}

static void roccat_timer_dialog_init(RoccatTimerDialog *timer_dialog) {
	RoccatTimerDialogPrivate *priv = ROCCAT_TIMER_DIALOG_GET_PRIVATE(timer_dialog);
	GtkVBox *content_area;

	timer_dialog->priv = priv;

	content_area = GTK_VBOX(gtk_dialog_get_content_area(GTK_DIALOG(timer_dialog)));

	priv->combo_box = GTK_COMBO_BOX(roccat_timer_combo_box_new());
	g_signal_connect(G_OBJECT(priv->combo_box), "changed", G_CALLBACK(changed_cb), timer_dialog);
	gtk_box_pack_start(GTK_BOX(content_area), GTK_WIDGET(priv->combo_box), FALSE, FALSE, 0);

	gtk_widget_show_all(GTK_WIDGET(content_area));
}

static void roccat_timer_dialog_class_init(RoccatTimerDialogClass *klass) {
	g_type_class_add_private(klass, sizeof(RoccatTimerDialogPrivate));
}
