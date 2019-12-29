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

#include "kova2016.h"
#include "kova2016_cpi_dialog.h"
#include <gaminggear/gaminggear_hscale.h>
#include "i18n.h"

#define KOVA2016_CPI_DIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), KOVA2016_CPI_DIALOG_TYPE, Kova2016CpiDialogClass))
#define IS_KOVA2016_CPI_DIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), KOVA2016_CPI_DIALOG_TYPE))
#define KOVA2016_CPI_DIALOG_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), KOVA2016_CPI_DIALOG_TYPE, Kova2016CpiDialogPrivate))

typedef struct _Kova2016CpiDialogClass Kova2016CpiDialogClass;

struct _Kova2016CpiDialogClass {
	GtkDialogClass parent_class;
};

struct _Kova2016CpiDialogPrivate {
	GaminggearHScale *scale;
};

G_DEFINE_TYPE(Kova2016CpiDialog, kova2016_cpi_dialog, GTK_TYPE_DIALOG);

gint kova2016_cpi_dialog(GtkWindow *parent) {
	Kova2016CpiDialog *cpi_dialog;
	gint value;

	cpi_dialog = KOVA2016_CPI_DIALOG(kova2016_cpi_dialog_new(parent));
	value = kova2016_cpi_dialog_run(cpi_dialog);
	gtk_widget_destroy(GTK_WIDGET(cpi_dialog));

	return value;
}

gint kova2016_cpi_dialog_run(Kova2016CpiDialog *cpi_dialog) {
	if (gtk_dialog_run(GTK_DIALOG(cpi_dialog)) == GTK_RESPONSE_ACCEPT)
		return kova2016_cpi_dialog_get_value(cpi_dialog);
	else
		return -1;
}

gint kova2016_cpi_dialog_get_value(Kova2016CpiDialog *cpi_dialog) {
	return (gint)gaminggear_hscale_get_value(cpi_dialog->priv->scale);
}

GtkWidget *kova2016_cpi_dialog_new(GtkWindow *parent) {
	Kova2016CpiDialog *cpi_dialog;

	cpi_dialog = g_object_new(KOVA2016_CPI_DIALOG_TYPE, NULL);

	(void)gtk_dialog_add_button(GTK_DIALOG(cpi_dialog), GTK_STOCK_OK, GTK_RESPONSE_ACCEPT);
	(void)gtk_dialog_add_button(GTK_DIALOG(cpi_dialog), GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT);

	gtk_window_set_title(GTK_WINDOW(cpi_dialog), _("Select cpi"));
	gtk_window_set_transient_for(GTK_WINDOW(cpi_dialog), parent);
	gtk_window_set_modal(GTK_WINDOW(cpi_dialog), TRUE);
	gtk_window_set_destroy_with_parent(GTK_WINDOW(cpi_dialog), TRUE);

	return GTK_WIDGET(cpi_dialog);
}

static void kova2016_cpi_dialog_init(Kova2016CpiDialog *cpi_dialog) {
	Kova2016CpiDialogPrivate *priv = KOVA2016_CPI_DIALOG_GET_PRIVATE(cpi_dialog);
	GtkVBox *content_area;

	cpi_dialog->priv = priv;

	content_area = GTK_VBOX(gtk_dialog_get_content_area(GTK_DIALOG(cpi_dialog)));

	priv->scale = GAMINGGEAR_HSCALE(gaminggear_hscale_new_with_range(KOVA2016_CPI_MIN, KOVA2016_CPI_MAX, KOVA2016_CPI_STEP));
	gtk_box_pack_start(GTK_BOX(content_area), GTK_WIDGET(priv->scale), FALSE, FALSE, 0);

	gtk_widget_show_all(GTK_WIDGET(content_area));
}

static void kova2016_cpi_dialog_class_init(Kova2016CpiDialogClass *klass) {
	g_type_class_add_private(klass, sizeof(Kova2016CpiDialogPrivate));
}
