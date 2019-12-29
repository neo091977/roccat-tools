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

#include "roccat_color_selection_dialog.h"
#include "roccat_multiwidget.h"
#include "i18n-lib.h"
#include <math.h>

#define ROCCAT_COLOR_SELECTION_DIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), ROCCAT_COLOR_SELECTION_DIALOG_TYPE, RoccatColorSelectionDialogClass))
#define IS_ROCCAT_COLOR_SELECTION_DIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), ROCCAT_COLOR_SELECTION_DIALOG_TYPE))
#define ROCCAT_COLOR_SELECTION_DIALOG_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), ROCCAT_COLOR_SELECTION_DIALOG_TYPE, RoccatColorSelectionDialogPrivate))

typedef struct _RoccatColorSelectionDialogClass RoccatColorSelectionDialogClass;

struct _RoccatColorSelectionDialogClass {
	GtkDialogClass parent_class;
};

struct _RoccatColorSelectionDialogPrivate {
	RoccatMultiwidget *multiwidget;
	RoccatColorPaletteSelector *palette_selector;
	GtkColorSelection *custom_selector;
	RoccatColorSelectionDialogType view_type;
	RoccatColorSelectionDialogType result_type;
};

G_DEFINE_TYPE(RoccatColorSelectionDialog, roccat_color_selection_dialog, GTK_TYPE_DIALOG);

static void palette_color_changed_cb(RoccatColorPaletteSelector *selector, gpointer user_data) {
	RoccatColorSelectionDialog *dialog = ROCCAT_COLOR_SELECTION_DIALOG(user_data);
	RoccatColorSelectionDialogPrivate *priv = dialog->priv;

	priv->result_type = ROCCAT_COLOR_SELECTION_DIALOG_TYPE_PALETTE;
	gtk_dialog_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);
}

static void custom_color_changed_cb(GtkColorSelection *selector, gpointer user_data) {
	RoccatColorSelectionDialog *dialog = ROCCAT_COLOR_SELECTION_DIALOG(user_data);
	dialog->priv->result_type = ROCCAT_COLOR_SELECTION_DIALOG_TYPE_CUSTOM;
}

RoccatColorPaletteSelector *roccat_color_selection_dialog_get_palette_selector(RoccatColorSelectionDialog *color_selection_dialog) {
	return color_selection_dialog->priv->palette_selector;
}

GtkColorSelection *roccat_color_selection_dialog_get_custom_selector(RoccatColorSelectionDialog *color_selection_dialog) {
	return color_selection_dialog->priv->custom_selector;
}

RoccatColorSelectionDialogType roccat_color_selection_dialog_get_active_type(RoccatColorSelectionDialog *color_selection_dialog) {
	return color_selection_dialog->priv->result_type;
}

void roccat_color_selection_dialog_set_view_type(RoccatColorSelectionDialog *color_selection_dialog, RoccatColorSelectionDialogType type) {
	RoccatColorSelectionDialogPrivate *priv = color_selection_dialog->priv;

	priv->view_type = type;

	switch (type) {
	case ROCCAT_COLOR_SELECTION_DIALOG_TYPE_PALETTE:
		roccat_multiwidget_show(priv->multiwidget, GTK_WIDGET(priv->palette_selector));
		break;
	case ROCCAT_COLOR_SELECTION_DIALOG_TYPE_CUSTOM:
		roccat_multiwidget_show(priv->multiwidget, GTK_WIDGET(priv->custom_selector));
		break;
	case ROCCAT_COLOR_SELECTION_DIALOG_TYPE_BOTH:
		roccat_multiwidget_show_all(priv->multiwidget);
		break;
	default:
		break;
	}
}

GtkWidget *roccat_color_selection_dialog_new(GtkWindow *parent) {
	RoccatColorSelectionDialog *color_selection_dialog;
	RoccatColorSelectionDialogPrivate *priv;
	GtkWidget *content_area;

	color_selection_dialog = g_object_new(ROCCAT_COLOR_SELECTION_DIALOG_TYPE, NULL);
	priv = color_selection_dialog->priv;

	gtk_dialog_add_button(GTK_DIALOG(color_selection_dialog), GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL);
	gtk_dialog_add_button(GTK_DIALOG(color_selection_dialog), GTK_STOCK_OK, GTK_RESPONSE_OK);

	gtk_window_set_title(GTK_WINDOW(color_selection_dialog), _("Choose color"));
	gtk_window_set_transient_for(GTK_WINDOW(color_selection_dialog), parent);
	gtk_window_set_modal(GTK_WINDOW(color_selection_dialog), TRUE);
	gtk_window_set_destroy_with_parent(GTK_WINDOW(color_selection_dialog), TRUE);

	priv->multiwidget = ROCCAT_MULTIWIDGET(roccat_multiwidget_new());

	priv->palette_selector = ROCCAT_COLOR_PALETTE_SELECTOR(roccat_color_palette_selector_new());
	roccat_multiwidget_add(priv->multiwidget, GTK_WIDGET(priv->palette_selector));
	g_signal_connect(G_OBJECT(priv->palette_selector), "color-changed", G_CALLBACK(palette_color_changed_cb), color_selection_dialog);

	priv->custom_selector = GTK_COLOR_SELECTION(gtk_color_selection_new());
	roccat_multiwidget_add(priv->multiwidget, GTK_WIDGET(priv->custom_selector));
	g_signal_connect(G_OBJECT(priv->custom_selector), "color-changed", G_CALLBACK(custom_color_changed_cb), color_selection_dialog);

	content_area = gtk_dialog_get_content_area(GTK_DIALOG(color_selection_dialog));
	gtk_box_pack_start(GTK_BOX(content_area), GTK_WIDGET(priv->multiwidget), TRUE, TRUE, 0);
	gtk_widget_show_all(GTK_WIDGET(content_area));

	return GTK_WIDGET(color_selection_dialog);
}

static void roccat_color_selection_dialog_init(RoccatColorSelectionDialog *color_selection_dialog) {
	color_selection_dialog->priv = ROCCAT_COLOR_SELECTION_DIALOG_GET_PRIVATE(color_selection_dialog);
}

static void roccat_color_selection_dialog_class_init(RoccatColorSelectionDialogClass *klass) {
	g_type_class_add_private(klass, sizeof(RoccatColorSelectionDialogPrivate));
}
