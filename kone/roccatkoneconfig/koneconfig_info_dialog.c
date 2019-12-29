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

#include "koneconfig_info_dialog.h"
#include "roccat_warning_dialog.h"
#include "kone.h"
#include "i18n.h"

#define KONECONFIG_INFO_DIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), KONECONFIG_INFO_DIALOG_TYPE, KoneconfigInfoDialogClass))
#define IS_KONECONFIG_INFO_DIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), KONECONFIG_INFO_DIALOG_TYPE))
#define KONECONFIG_INFO_DIALOG_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), KONECONFIG_INFO_DIALOG_TYPE, KoneconfigInfoDialogPrivate))

typedef struct _KoneconfigInfoDialogClass KoneconfigInfoDialogClass;

struct _KoneconfigInfoDialogClass {
	RoccatInfoDialogClass parent_class;
};

struct _KoneconfigInfoDialogPrivate {
	GtkLabel *weight_label;
	GtkLabel *firmware_label;
};

G_DEFINE_TYPE(KoneconfigInfoDialog, koneconfig_info_dialog, ROCCAT_INFO_DIALOG_TYPE);

static void set_weight(KoneconfigInfoDialog *info_dialog, RoccatDevice *device) {
	KoneconfigInfoDialogPrivate *priv = info_dialog->priv;
	GError *local_error = NULL;
	guint value;
	gchar *text;

	value = kone_weight_read(device, &local_error);
	if (local_error) {
		gtk_label_set_text(priv->weight_label, NULL);
		g_error_free(local_error);
	} else {
		text = g_strdup_printf("%d", value);
		gtk_label_set_text(priv->weight_label, text);
		g_free(text);
	}
}

static void set_firmware(KoneconfigInfoDialog *info_dialog, RoccatDevice *device) {
	KoneconfigInfoDialogPrivate *priv = info_dialog->priv;
	GError *local_error = NULL;
	guint value;
	gchar *text;

	value = kone_firmware_version_read(device, &local_error);
	if (!roccat_handle_error_dialog(GTK_WINDOW(info_dialog), _("Could not read firmware version"), &local_error))
		gtk_label_set_text(priv->firmware_label, NULL);
	else {
		text = kone_firmware_version_to_string(value);
		gtk_label_set_text(priv->firmware_label, text);
		g_free(text);
	}
}

GtkWidget *koneconfig_info_dialog_new(GtkWindow *parent, RoccatDevice *device) {
	KoneconfigInfoDialog *info_dialog;

	info_dialog = KONECONFIG_INFO_DIALOG(g_object_new(KONECONFIG_INFO_DIALOG_TYPE,
			"transient-for", parent,
			NULL));

	set_firmware(info_dialog, device);
	set_weight(info_dialog, device);

	return GTK_WIDGET(info_dialog);
}

static void koneconfig_info_dialog_init(KoneconfigInfoDialog *info_dialog) {
	KoneconfigInfoDialogPrivate *priv = KONECONFIG_INFO_DIALOG_GET_PRIVATE(info_dialog);
	info_dialog->priv = priv;

	priv->firmware_label = GTK_LABEL(gtk_label_new(NULL));
	roccat_info_dialog_add_line(ROCCAT_INFO_DIALOG(info_dialog), _("Firmware version"), GTK_WIDGET(priv->firmware_label));

	priv->weight_label = GTK_LABEL(gtk_label_new(NULL));
	roccat_info_dialog_add_line(ROCCAT_INFO_DIALOG(info_dialog), _("Fast-clip weight"), GTK_WIDGET(priv->weight_label));
}

static void koneconfig_info_dialog_class_init(KoneconfigInfoDialogClass *klass) {
	g_type_class_add_private(klass, sizeof(KoneconfigInfoDialogPrivate));
}
