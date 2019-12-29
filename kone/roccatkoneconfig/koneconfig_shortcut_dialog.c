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

#include <gaminggear/gdk_key_translations.h>
#include "koneconfig_shortcut_dialog.h"
#include "kone.h"
#include "i18n.h"

static gchar const * const radio_modifier_key = "modifier";

#define KONECONFIG_SHORTCUT_DIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), KONECONFIG_SHORTCUT_DIALOG_TYPE, KoneconfigShortcutDialogClass))
#define IS_KONECONFIG_SHORTCUT_DIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), KONECONFIG_SHORTCUT_DIALOG_TYPE))
#define KONECONFIG_SHORTCUT_DIALOG_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), KONECONFIG_SHORTCUT_DIALOG_TYPE, KoneconfigShortcutDialogPrivate))

typedef struct _KoneconfigShortcutDialogClass KoneconfigShortcutDialogClass;
typedef struct _KoneconfigShortcutDialogPrivate KoneconfigShortcutDialogPrivate;

struct _KoneconfigShortcutDialog {
	GtkDialog parent;
	KoneconfigShortcutDialogPrivate *priv;
};

struct _KoneconfigShortcutDialogClass {
	GtkDialogClass parent_class;
};

struct _KoneconfigShortcutDialogPrivate {
	GtkLabel *label;
	guint modifier;
	guint xkeycode;
};

G_DEFINE_TYPE(KoneconfigShortcutDialog, koneconfig_shortcut_dialog, GTK_TYPE_DIALOG);

static gboolean dialog_key_press_event_cb(GtkWidget *dialog, GdkEventKey *event, gpointer user_data) {
	KoneconfigShortcutDialog *koneconfig_shortcut_dialog = KONECONFIG_SHORTCUT_DIALOG(dialog);
	KoneconfigShortcutDialogPrivate *priv = koneconfig_shortcut_dialog->priv;
	gchar *labeltext;
	labeltext = gaminggear_xkeycode_to_keyname(event->hardware_keycode);
	gtk_label_set_text(priv->label, labeltext);
	g_free(labeltext);
	priv->xkeycode = event->hardware_keycode;
	return TRUE;
}

static void radio_clicked_cb(GtkRadioButton *radio, gpointer user_data) {
	KoneconfigShortcutDialog *koneconfig_shortcut_dialog = KONECONFIG_SHORTCUT_DIALOG(user_data);
	koneconfig_shortcut_dialog->priv->modifier = GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(radio), radio_modifier_key));
}

GtkWidget *koneconfig_shortcut_dialog_new(GtkWindow *parent) {
	KoneconfigShortcutDialog *koneconfig_shortcut_dialog;

	koneconfig_shortcut_dialog = KONECONFIG_SHORTCUT_DIALOG(g_object_new(KONECONFIG_SHORTCUT_DIALOG_TYPE,
			NULL));

	gtk_dialog_add_buttons(GTK_DIALOG(koneconfig_shortcut_dialog),
			GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			NULL);
	gtk_window_set_title(GTK_WINDOW(koneconfig_shortcut_dialog), _("Assign a shortcut"));
	gtk_window_set_transient_for(GTK_WINDOW(koneconfig_shortcut_dialog), parent);
	gtk_window_set_modal(GTK_WINDOW(koneconfig_shortcut_dialog), TRUE);
	gtk_window_set_destroy_with_parent(GTK_WINDOW(koneconfig_shortcut_dialog), TRUE);
	gtk_window_set_type_hint(GTK_WINDOW(koneconfig_shortcut_dialog), GDK_WINDOW_TYPE_HINT_NORMAL);

	return GTK_WIDGET(koneconfig_shortcut_dialog);
}

static void koneconfig_shortcut_dialog_init(KoneconfigShortcutDialog *koneconfig_shortcut_dialog) {
	KoneconfigShortcutDialogPrivate *priv = KONECONFIG_SHORTCUT_DIALOG_GET_PRIVATE(koneconfig_shortcut_dialog);
	koneconfig_shortcut_dialog->priv = priv;
	GtkWidget *content_area;
	GtkWidget *caption;
	GtkWidget *table;
	GtkRadioButton *radio;
	guint i;

	struct Radios {
		gchar const * const label;
		guint modifier;
	} radios[5] = {
		{N_("None"), KONE_KEYSTROKE_ACTION_NONE},
		{N_("Shift"), KONE_KEYSTROKE_ACTION_SHIFT},
		{N_("Ctrl"), KONE_KEYSTROKE_ACTION_CTRL},
		{N_("Alt"), KONE_KEYSTROKE_ACTION_ALT},
		{N_("Win"), KONE_KEYSTROKE_ACTION_WIN}
	};

	content_area = gtk_dialog_get_content_area(GTK_DIALOG(koneconfig_shortcut_dialog));

	caption = gtk_label_new(_("Please hit a key"));
	gtk_box_pack_start(GTK_BOX(content_area), caption, FALSE, FALSE, 0);

	table = gtk_table_new(1, 6, TRUE);
	gtk_box_pack_start(GTK_BOX(content_area), table, FALSE, FALSE, 0);

	radio = NULL;
	for (i = 0; i < 5; ++i) {
		radio = GTK_RADIO_BUTTON(gtk_radio_button_new_with_label_from_widget(radio, _N(radios[i].label)));
		g_object_set_data(G_OBJECT(radio), radio_modifier_key, GUINT_TO_POINTER(radios[i].modifier));
		g_signal_connect(G_OBJECT(radio), "clicked", G_CALLBACK(radio_clicked_cb), koneconfig_shortcut_dialog);
		gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(radio), i, i + 1, 0, 1, GTK_EXPAND, GTK_EXPAND, 0, 0);
	}

	priv->modifier = KONE_KEYSTROKE_ACTION_NONE;

	priv->label = GTK_LABEL(gtk_label_new(NULL));
	gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(priv->label), 5, 6, 0, 1, GTK_EXPAND, GTK_EXPAND, 0, 0);

	g_signal_connect(G_OBJECT(koneconfig_shortcut_dialog), "key-press-event", G_CALLBACK(dialog_key_press_event_cb), NULL);
	gtk_widget_show_all(GTK_WIDGET(koneconfig_shortcut_dialog));
}

static void koneconfig_shortcut_dialog_class_init(KoneconfigShortcutDialogClass *klass) {
	g_type_class_add_private(klass, sizeof(KoneconfigShortcutDialogPrivate));
}


guint koneconfig_shortcut_dialog_get_modifier(KoneconfigShortcutDialog *dialog) {
	return dialog->priv->modifier;
}

guint koneconfig_shortcut_dialog_get_xkeycode(KoneconfigShortcutDialog *dialog) {
	return dialog->priv->xkeycode;
}
