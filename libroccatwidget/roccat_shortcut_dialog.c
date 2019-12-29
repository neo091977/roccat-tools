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

#include "roccat_shortcut_dialog.h"
#include "roccat_button.h"
#include "roccat_helper.h"
#include "gtk_roccat_helper.h"
#include "i18n-lib.h"
#include <gaminggear/gaminggear_hid_combo_box.h>
#include <gaminggear/key_translations.h>
#include <gaminggear/gdk_key_translations.h>
#include <gaminggear/hid_uid.h>
#include <gdk/gdkkeysyms.h>

#define ROCCAT_SHORTCUT_DIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), ROCCAT_SHORTCUT_DIALOG_TYPE, RoccatShortcutDialogClass))
#define IS_ROCCAT_SHORTCUT_DIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), ROCCAT_SHORTCUT_DIALOG_TYPE))
#define ROCCAT_SHORTCUT_DIALOG_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), ROCCAT_SHORTCUT_DIALOG_TYPE, RoccatShortcutDialogPrivate))

typedef struct _RoccatShortcutDialogClass RoccatShortcutDialogClass;

struct _RoccatShortcutDialogClass {
	GtkDialogClass parent_class;
};

struct _RoccatShortcutDialogPrivate {
	GtkWidget *ok_button;
	GaminggearHidComboBox *combo_box;
	GtkCheckButton *ctrl;
	GtkCheckButton *shift;
	GtkCheckButton *alt;
	GtkCheckButton *win;
};

G_DEFINE_TYPE(RoccatShortcutDialog, roccat_shortcut_dialog, GTK_TYPE_DIALOG);

gboolean roccat_shortcut_dialog_get_ctrl(RoccatShortcutDialog *shortcut_dialog) {
	return gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(shortcut_dialog->priv->ctrl));
}

void roccat_shortcut_dialog_set_ctrl(RoccatShortcutDialog *shortcut_dialog, gboolean value) {
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(shortcut_dialog->priv->ctrl), value);
}

gboolean roccat_shortcut_dialog_get_shift(RoccatShortcutDialog *shortcut_dialog) {
	return gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(shortcut_dialog->priv->shift));
}

void roccat_shortcut_dialog_set_shift(RoccatShortcutDialog *shortcut_dialog, gboolean value) {
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(shortcut_dialog->priv->shift), value);
}

gboolean roccat_shortcut_dialog_get_alt(RoccatShortcutDialog *shortcut_dialog) {
	return gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(shortcut_dialog->priv->alt));
}

void roccat_shortcut_dialog_set_alt(RoccatShortcutDialog *shortcut_dialog, gboolean value) {
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(shortcut_dialog->priv->alt), value);
}

gboolean roccat_shortcut_dialog_get_win(RoccatShortcutDialog *shortcut_dialog) {
	return gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(shortcut_dialog->priv->win));
}

void roccat_shortcut_dialog_set_win(RoccatShortcutDialog *shortcut_dialog, gboolean value) {
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(shortcut_dialog->priv->win), value);
}

guint8 roccat_shortcut_dialog_get_hid(RoccatShortcutDialog *shortcut_dialog) {
	return gaminggear_hid_combo_box_get_value(shortcut_dialog->priv->combo_box);
}

void roccat_shortcut_dialog_set_hid(RoccatShortcutDialog *shortcut_dialog, guint8 hid_usage_id) {
	RoccatShortcutDialogPrivate *priv = shortcut_dialog->priv;

	gaminggear_hid_combo_box_set_value(priv->combo_box, hid_usage_id);
	gtk_widget_set_sensitive(priv->ok_button, hid_usage_id != 0);
}

GtkWidget *roccat_shortcut_dialog_new(GtkWindow *parent) {
	RoccatShortcutDialog *shortcut_dialog;
	RoccatShortcutDialogPrivate *priv;

	shortcut_dialog = g_object_new(ROCCAT_SHORTCUT_DIALOG_TYPE, NULL);
	priv = shortcut_dialog->priv;

	priv->ok_button = gtk_dialog_add_button(GTK_DIALOG(shortcut_dialog), GTK_STOCK_OK, GTK_RESPONSE_ACCEPT);
	gtk_widget_set_sensitive(priv->ok_button, FALSE);

	gtk_dialog_add_button(GTK_DIALOG(shortcut_dialog), GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT);

	gtk_window_set_title(GTK_WINDOW(shortcut_dialog), _("Assign a shortcut"));
	gtk_window_set_transient_for(GTK_WINDOW(shortcut_dialog), parent);
	gtk_window_set_modal(GTK_WINDOW(shortcut_dialog), TRUE);
	gtk_window_set_destroy_with_parent(GTK_WINDOW(shortcut_dialog), TRUE);

	return GTK_WIDGET(shortcut_dialog);
}

static gboolean key_press_event_cb(GtkWidget *dialog, GdkEventKey *event, gpointer user_data) {
	RoccatShortcutDialog *shortcut_dialog = ROCCAT_SHORTCUT_DIALOG(user_data);

	roccat_shortcut_dialog_set_hid(shortcut_dialog, gaminggear_xkeycode_to_hid(event->hardware_keycode));

	/* don't further process the event (disables return key to exit dialog) */
	return TRUE;
}

static void changed_cb(GtkComboBox *combo_box, gpointer user_data) {
	RoccatShortcutDialog *shortcut_dialog = ROCCAT_SHORTCUT_DIALOG(user_data);
	RoccatShortcutDialogPrivate *priv = shortcut_dialog->priv;
	gtk_widget_set_sensitive(priv->ok_button, roccat_shortcut_dialog_get_hid(shortcut_dialog) != 0);
}

static void roccat_shortcut_dialog_init(RoccatShortcutDialog *shortcut_dialog) {
	RoccatShortcutDialogPrivate *priv = ROCCAT_SHORTCUT_DIALOG_GET_PRIVATE(shortcut_dialog);
	GtkVBox *content_area;
	GtkLabel *caption;
	GtkTable *table;

	shortcut_dialog->priv = priv;

	content_area = GTK_VBOX(gtk_dialog_get_content_area(GTK_DIALOG(shortcut_dialog)));

	caption = GTK_LABEL(gtk_label_new(_("Please hit a key")));
	gtk_box_pack_start(GTK_BOX(content_area), GTK_WIDGET(caption), FALSE, FALSE, 0);

	table = GTK_TABLE(gtk_table_new(1, 5, FALSE));
	gtk_box_pack_start(GTK_BOX(content_area), GTK_WIDGET(table), FALSE, FALSE, 0);

	priv->ctrl = GTK_CHECK_BUTTON(gtk_check_button_new_with_label(_("Ctrl")));
	gtk_table_attach(table, GTK_WIDGET(priv->ctrl), 0, 1, 0, 1, GTK_EXPAND, GTK_EXPAND, 0, 0);

	priv->shift = GTK_CHECK_BUTTON(gtk_check_button_new_with_label(_("Shift")));
	gtk_table_attach(table, GTK_WIDGET(priv->shift), 1, 2, 0, 1, GTK_EXPAND, GTK_EXPAND, 0, 0);

	priv->alt = GTK_CHECK_BUTTON(gtk_check_button_new_with_label(_("Alt")));
	gtk_table_attach(table, GTK_WIDGET(priv->alt), 2, 3, 0, 1, GTK_EXPAND, GTK_EXPAND, 0, 0);

	priv->win = GTK_CHECK_BUTTON(gtk_check_button_new_with_label(_("Win")));
	gtk_table_attach(table, GTK_WIDGET(priv->win), 3, 4, 0, 1, GTK_EXPAND, GTK_EXPAND, 0, 0);

	priv->combo_box = GAMINGGEAR_HID_COMBO_BOX(gaminggear_hid_combo_box_new());
	gtk_table_attach(table, GTK_WIDGET(priv->combo_box), 4, 5, 0, 1, GTK_EXPAND, GTK_EXPAND, 0, 0);

	g_signal_connect(G_OBJECT(priv->combo_box), "changed", G_CALLBACK(changed_cb), shortcut_dialog);
	g_signal_connect(G_OBJECT(shortcut_dialog), "key-press-event", G_CALLBACK(key_press_event_cb), shortcut_dialog);

	gtk_widget_show_all(GTK_WIDGET(content_area));
}

static void roccat_shortcut_dialog_class_init(RoccatShortcutDialogClass *klass) {
	g_type_class_add_private(klass, sizeof(RoccatShortcutDialogPrivate));
}

GaminggearMacro *roccat_shortcut_dialog_get_macro(RoccatShortcutDialog *shortcut_dialog) {
	GaminggearMacro *gaminggear_macro;
	GaminggearMacroKeystroke keystroke;
	guint hid_usage_id;
	gboolean ctrl, shift, alt, win;
	gchar *shortcut_name;

	hid_usage_id = roccat_shortcut_dialog_get_hid(shortcut_dialog);
	if (!hid_usage_id)
		return NULL;

	ctrl = roccat_shortcut_dialog_get_ctrl(shortcut_dialog);
	shift = roccat_shortcut_dialog_get_shift(shortcut_dialog);
	alt = roccat_shortcut_dialog_get_alt(shortcut_dialog);
	win = roccat_shortcut_dialog_get_win(shortcut_dialog);

	shortcut_name = roccat_shortcut_dialog_create_name_full(ctrl, shift, alt, win, hid_usage_id);
	gaminggear_macro = gaminggear_macro_new(_("Shortcut"), shortcut_name, NULL);
	g_free(shortcut_name);

	gaminggear_macro_keystroke_set_period(&keystroke, 0);

	if (shift) {
		keystroke.key = HID_UID_KB_LEFT_SHIFT;
		keystroke.action = GAMINGGEAR_MACRO_KEYSTROKE_ACTION_PRESS;
		gaminggear_macro_add_keystroke(gaminggear_macro, &keystroke);
	}

	if (ctrl) {
		keystroke.key = HID_UID_KB_LEFT_CONTROL;
		keystroke.action = GAMINGGEAR_MACRO_KEYSTROKE_ACTION_PRESS;
		gaminggear_macro_add_keystroke(gaminggear_macro, &keystroke);
	}

	if (alt) {
		keystroke.key = HID_UID_KB_LEFT_ALT;
		keystroke.action = GAMINGGEAR_MACRO_KEYSTROKE_ACTION_PRESS;
		gaminggear_macro_add_keystroke(gaminggear_macro, &keystroke);
	}

	if (win) {
		keystroke.key = HID_UID_KB_LEFT_GUI;
		keystroke.action = GAMINGGEAR_MACRO_KEYSTROKE_ACTION_PRESS;
		gaminggear_macro_add_keystroke(gaminggear_macro, &keystroke);
	}

	keystroke.key = hid_usage_id;

	keystroke.action = GAMINGGEAR_MACRO_KEYSTROKE_ACTION_PRESS;
	gaminggear_macro_add_keystroke(gaminggear_macro, &keystroke);

	keystroke.action = GAMINGGEAR_MACRO_KEYSTROKE_ACTION_RELEASE;
	gaminggear_macro_add_keystroke(gaminggear_macro, &keystroke);

	if (win) {
		keystroke.key = HID_UID_KB_LEFT_GUI;
		keystroke.action = GAMINGGEAR_MACRO_KEYSTROKE_ACTION_RELEASE;
		gaminggear_macro_add_keystroke(gaminggear_macro, &keystroke);
	}

	if (alt) {
		keystroke.key = HID_UID_KB_LEFT_ALT;
		keystroke.action = GAMINGGEAR_MACRO_KEYSTROKE_ACTION_RELEASE;
		gaminggear_macro_add_keystroke(gaminggear_macro, &keystroke);
	}

	if (ctrl) {
		keystroke.key = HID_UID_KB_LEFT_CONTROL;
		keystroke.action = GAMINGGEAR_MACRO_KEYSTROKE_ACTION_RELEASE;
		gaminggear_macro_add_keystroke(gaminggear_macro, &keystroke);
	}

	if (shift) {
		keystroke.key = HID_UID_KB_LEFT_SHIFT;
		keystroke.action = GAMINGGEAR_MACRO_KEYSTROKE_ACTION_RELEASE;
		gaminggear_macro_add_keystroke(gaminggear_macro, &keystroke);
	}

	return gaminggear_macro;
}

gchar *roccat_shortcut_dialog_create_name_full(gboolean ctrl, gboolean shift, gboolean alt, gboolean win, guint hid_usage_id) {
	gchar *ctrl_str, *shift_str, *alt_str, *win_str;
	gchar *hid;
	gchar *result;

	ctrl_str = ctrl ? _("Ctrl+") : "";
	shift_str = shift ? _("Shift+") : "";
	alt_str = alt ? _("Alt+") : "";
	win_str = win ? _("Win+") : "";

	hid = gaminggear_hid_to_name(hid_usage_id);
	result = g_strdup_printf("%s%s%s%s%s", ctrl_str, shift_str, alt_str, win_str, hid);
	g_free(hid);

	return result;
}

gchar *roccat_shortcut_dialog_create_name(guint8 modifier, guint hid_usage_id) {
	return roccat_shortcut_dialog_create_name_full(
			roccat_get_bit8(modifier, ROCCAT_BUTTON_MODIFIER_BIT_CTRL),
			roccat_get_bit8(modifier, ROCCAT_BUTTON_MODIFIER_BIT_SHIFT),
			roccat_get_bit8(modifier, ROCCAT_BUTTON_MODIFIER_BIT_ALT),
			roccat_get_bit8(modifier, ROCCAT_BUTTON_MODIFIER_BIT_WIN),
			hid_usage_id);
}

