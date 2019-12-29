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

#include "konepuremilitary_dcu_dialog.h"
#include "konepuremilitary_move_dialog.h"
#include "koneplus_dcu_frame.h"
#include "konepuremilitary.h"
#include "roccat_warning_dialog.h"
#include "i18n-lib.h"
#include <gaminggear/gaminggear_timeout_dialog.h>

#define KONEPUREMILITARY_DCU_DIALOG_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), KONEPUREMILITARY_DCU_DIALOG_TYPE, KonepuremilitaryDcuDialogPrivate))
#define KONEPUREMILITARY_DCU_DIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), KONEPUREMILITARY_DCU_DIALOG_TYPE, KonepuremilitaryDcuDialogClass))
#define IS_KONEPUREMILITARY_DCU_DIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), KONEPUREMILITARY_DCU_DIALOG_TYPE))

typedef struct _KonepuremilitaryDcuDialogClass KonepuremilitaryDcuDialogClass;
typedef struct _KonepuremilitaryDcuDialogPrivate KonepuremilitaryDcuDialogPrivate;

struct _KonepuremilitaryDcuDialog {
	GtkDialog parent;
	KonepuremilitaryDcuDialogPrivate *priv;
};

struct _KonepuremilitaryDcuDialogClass {
	GtkDialogClass parent_class;
};

struct _KonepuremilitaryDcuDialogPrivate {
	KoneplusDcuFrame *dcu;
	RoccatDevice *device;
};

G_DEFINE_TYPE(KonepuremilitaryDcuDialog, konepuremilitary_dcu_dialog, GTK_TYPE_DIALOG);

static void dcu_changed_cb(KoneplusDcuFrame *frame, gpointer user_data) {
	KonepuremilitaryDcuDialog *dcu_dialog = KONEPUREMILITARY_DCU_DIALOG(user_data);
	KonepuremilitaryDcuDialogPrivate *priv = dcu_dialog->priv;
	GError *local_error = NULL;
	guint new_value;
	guint old_value;
	gboolean result;

	new_value = koneplus_dcu_frame_get_value(priv->dcu);

	old_value = koneplus_distance_control_unit_get(priv->device, &local_error);
	if (!roccat_handle_error_dialog(GTK_WINDOW(dcu_dialog), _("Could not access sensor data"), &local_error))
		return;

	if (new_value == old_value)
		return;

	koneplus_distance_control_unit_try(priv->device, new_value, &local_error);
	if (!roccat_handle_error_dialog(GTK_WINDOW(dcu_dialog), _("Could not access sensor data"), &local_error))
		return;

	result = konepuremilitary_move_dialog(GTK_WINDOW(dcu_dialog), _("Move!"), _("Please move the mouse continuosly"), 10);
	if (!result) {
		koneplus_distance_control_unit_cancel(priv->device, old_value, &local_error);
		g_clear_error(&local_error);
		koneplus_dcu_frame_set_value_blocked(priv->dcu, old_value);
		return;
	}

	result = gaminggear_timeout_dialog(GTK_WINDOW(dcu_dialog),
			_("Keep settings?"),
			_("Do you want to keep this settings?"), 30);

	if (result) {
		koneplus_distance_control_unit_accept(priv->device, new_value, &local_error);
		koneplus_dcu_frame_set_value_blocked(priv->dcu, new_value);
	} else {
		koneplus_distance_control_unit_cancel(priv->device, old_value, &local_error);
		koneplus_dcu_frame_set_value_blocked(priv->dcu, old_value);
	}
	roccat_handle_error_dialog(GTK_WINDOW(dcu_dialog), _("Could not access sensor data"), &local_error);
}

static void init_frames(KonepuremilitaryDcuDialog *dcu_dialog) {
	KonepuremilitaryDcuDialogPrivate *priv = dcu_dialog->priv;
	GError *local_error = NULL;
	guint actual_value;

	actual_value = koneplus_distance_control_unit_get(priv->device, &local_error);
	if (!roccat_handle_error_dialog(GTK_WINDOW(dcu_dialog), _("Could not access sensor data"), &local_error))
		return;
	koneplus_dcu_frame_set_value_blocked(priv->dcu, actual_value);
}

GtkWidget *konepuremilitary_dcu_dialog_new(GtkWindow *parent, RoccatDevice *device) {
	KonepuremilitaryDcuDialog *dcu_dialog;
	KonepuremilitaryDcuDialogPrivate *priv;

	dcu_dialog = KONEPUREMILITARY_DCU_DIALOG(g_object_new(KONEPUREMILITARY_DCU_DIALOG_TYPE,
			"transient-for", parent,
			NULL));

	priv = dcu_dialog->priv;
	priv->device = device;

	init_frames(dcu_dialog);

	return GTK_WIDGET(dcu_dialog);
}

static void konepuremilitary_dcu_dialog_init(KonepuremilitaryDcuDialog *dcu_dialog) {
	KonepuremilitaryDcuDialogPrivate *priv = KONEPUREMILITARY_DCU_DIALOG_GET_PRIVATE(dcu_dialog);
	GtkVBox *content_area;

	dcu_dialog->priv = priv;

	gtk_dialog_add_buttons(GTK_DIALOG(dcu_dialog),
			GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
			NULL);
	gtk_window_set_modal(GTK_WINDOW(dcu_dialog), TRUE);
	gtk_window_set_destroy_with_parent(GTK_WINDOW(dcu_dialog), TRUE);
	gtk_window_set_title(GTK_WINDOW(dcu_dialog), _("TCU/DCU"));

	content_area = GTK_VBOX(gtk_dialog_get_content_area(GTK_DIALOG(dcu_dialog)));

	priv->dcu = KONEPLUS_DCU_FRAME(koneplus_dcu_frame_new());

	g_signal_connect(G_OBJECT(priv->dcu), "changed", G_CALLBACK(dcu_changed_cb), dcu_dialog);

	gtk_box_pack_start(GTK_BOX(content_area), GTK_WIDGET(priv->dcu), FALSE, FALSE, 0);

	gtk_widget_show_all(GTK_WIDGET(content_area));
}

static void konepuremilitary_dcu_dialog_class_init(KonepuremilitaryDcuDialogClass *klass) {
	g_type_class_add_private(klass, sizeof(KonepuremilitaryDcuDialogPrivate));
}
