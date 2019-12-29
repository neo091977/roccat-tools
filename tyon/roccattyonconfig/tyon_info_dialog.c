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

#include "tyon_info_dialog.h"
#include "roccat_warning_dialog.h"
#include "tyon_info.h"
#include "roccat_firmware.h"
#include "i18n.h"

#define TYON_INFO_DIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), TYON_INFO_DIALOG_TYPE, TyonInfoDialogClass))
#define IS_TYON_INFO_DIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), TYON_INFO_DIALOG_TYPE))
#define TYON_INFO_DIALOG_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), TYON_INFO_DIALOG_TYPE, TyonInfoDialogPrivate))

typedef struct _TyonInfoDialogClass TyonInfoDialogClass;

struct _TyonInfoDialogClass {
	RoccatInfoDialogClass parent_class;
};

struct _TyonInfoDialogPrivate {
	GtkLabel *firmware_label;
	GtkLabel *dfu_label;
};

G_DEFINE_TYPE(TyonInfoDialog, tyon_info_dialog, ROCCAT_INFO_DIALOG_TYPE);

static void set_firmware(TyonInfoDialog *info_dialog, RoccatDevice *device) {
	TyonInfoDialogPrivate *priv = info_dialog->priv;
	GError *local_error = NULL;
	TyonInfo *info;
	gchar *firmware;
	gchar *dfu;

	info = tyon_info_read(device, &local_error);
	if (!roccat_handle_error_dialog(GTK_WINDOW(info_dialog), _("Could not read informations"), &local_error)) {
		gtk_label_set_text(priv->firmware_label, NULL);
		return;
	}

	firmware = roccat_firmware_version_to_string(info->firmware_version);
	gtk_label_set_text(priv->firmware_label, firmware);
	g_free(firmware);

	dfu = roccat_firmware_version_to_string(info->dfu_version);
	gtk_label_set_text(priv->dfu_label, dfu);
	g_free(dfu);

	g_free(info);
}

GtkWidget *tyon_info_dialog_new(GtkWindow *parent, RoccatDevice *device) {
	TyonInfoDialog *info_dialog;

	info_dialog = TYON_INFO_DIALOG(g_object_new(TYON_INFO_DIALOG_TYPE,
			"transient-for", parent,
			NULL));

	set_firmware(info_dialog, device);

	return GTK_WIDGET(info_dialog);
}

static void tyon_info_dialog_init(TyonInfoDialog *info_dialog) {
	TyonInfoDialogPrivate *priv = TYON_INFO_DIALOG_GET_PRIVATE(info_dialog);
	info_dialog->priv = priv;

	priv->firmware_label = GTK_LABEL(gtk_label_new(NULL));
	priv->dfu_label = GTK_LABEL(gtk_label_new(NULL));

	roccat_info_dialog_add_line(ROCCAT_INFO_DIALOG(info_dialog), _("Firmware version"), GTK_WIDGET(priv->firmware_label));
	roccat_info_dialog_add_line(ROCCAT_INFO_DIALOG(info_dialog), _("DFU version"), GTK_WIDGET(priv->dfu_label));
}

static void tyon_info_dialog_class_init(TyonInfoDialogClass *klass) {
	g_type_class_add_private(klass, sizeof(TyonInfoDialogPrivate));
}
