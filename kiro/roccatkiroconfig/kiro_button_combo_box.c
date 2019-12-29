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

#include <gaminggear/key_translations.h>
#include <gaminggear/gdk_key_translations.h>
#include <gaminggear/hid_uid.h>
#include <gaminggear/gaminggear_text_dialog.h>
#include "kiro_button_combo_box.h"
#include "roccat_talk_target_dialog.h"
#include "roccat_shortcut_dialog.h"
#include "roccat_warning_dialog.h"
#include "roccat_talk.h"
#include "roccat_helper.h"
#include "gtk_roccat_helper.h"
#include "g_cclosure_roccat_marshaller.h"
#include "i18n.h"

#define KIRO_BUTTON_COMBO_BOX_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), KIRO_BUTTON_COMBO_BOX_TYPE, KiroButtonComboBoxClass))
#define IS_KIRO_BUTTON_COMBO_BOX_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), KIRO_BUTTON_COMBO_BOX_TYPE))
#define KIRO_BUTTON_COMBO_BOX_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), KIRO_BUTTON_COMBO_BOX_TYPE, KiroButtonComboBoxClass))
#define KIRO_BUTTON_COMBO_BOX_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), KIRO_BUTTON_COMBO_BOX_TYPE, KiroButtonComboBoxPrivate))

typedef struct _KiroButtonComboBoxClass KiroButtonComboBoxClass;
typedef struct _KiroButtonComboBoxPrivate KiroButtonComboBoxPrivate;

struct _KiroButtonComboBox {
	RoccatKeyComboBox parent;
	KiroButtonComboBoxPrivate *priv;
};

struct _KiroButtonComboBoxClass {
	RoccatKeyComboBoxClass parent_class;
	GtkTreeModel *model;
};

struct _KiroButtonComboBoxPrivate {
	KiroButtonComboBoxData *data;
	gulong changed_handler_id;
};

G_DEFINE_TYPE(KiroButtonComboBox, kiro_button_combo_box, ROCCAT_KEY_COMBO_BOX_TYPE);

enum {
	KEY_CHANGED,
	LAST_SIGNAL,
};

static guint signals[LAST_SIGNAL] = { 0 };

static gchar *kiro_button_combo_box_get_text_for_data(KiroButtonComboBox *button_combo_box, KiroButtonComboBoxData const *data) {
	gchar *text;
	gchar *temp;
	guint type;

	type = data->button.type;

	switch (type) {
	case KIRO_BUTTON_TYPE_SHORTCUT:
		gtk_widget_set_tooltip_text(GTK_WIDGET(button_combo_box), NULL);
		temp = roccat_shortcut_dialog_create_name(data->button.modifier, data->button.key);
		text = g_strdup_printf(_("Shortcut: %s"), temp);
		g_free(temp);
		break;
	case KIRO_BUTTON_TYPE_OPEN_APPLICATION:
		gtk_widget_set_tooltip_text(GTK_WIDGET(button_combo_box), data->opener);
		temp = g_path_get_basename(data->opener);
		text = g_strdup_printf(_("Application: %s"), temp);
		g_free(temp);
		break;
	default:
		gtk_widget_set_tooltip_text(GTK_WIDGET(button_combo_box), NULL);
		text = roccat_key_combo_box_model_get_title_for_type(gtk_combo_box_get_model(GTK_COMBO_BOX(button_combo_box)), type);
		break;
	}

	return text;
}

void kiro_button_combo_box_update(KiroButtonComboBox *button_combo_box) {
	KiroButtonComboBoxPrivate *priv = button_combo_box->priv;
	gchar *text;

	text = kiro_button_combo_box_get_text_for_data(button_combo_box, priv->data);

	g_signal_handler_block(G_OBJECT(button_combo_box), priv->changed_handler_id);
	roccat_key_combo_box_set_text(ROCCAT_KEY_COMBO_BOX(button_combo_box), text);
	g_signal_handler_unblock(G_OBJECT(button_combo_box), priv->changed_handler_id);

	g_free(text);
}

static void kiro_button_combo_box_data_set_opener(KiroButtonComboBoxData *data, gchar const *new_opener) {
	g_strlcpy(data->opener, new_opener, ROCCAT_SWARM_RMP_OPENER_LENGTH);
}

static gboolean key_open_application(KiroButtonComboBox *button_combo_box) {
	KiroButtonComboBoxPrivate *priv = button_combo_box->priv;
	GtkWidget *dialog;
	gboolean retval = FALSE;
	gchar *path;

	dialog = gtk_file_chooser_dialog_new(_("Select application"),
			gtk_roccat_widget_get_gtk_window(GTK_WIDGET(button_combo_box)),
			GTK_FILE_CHOOSER_ACTION_OPEN,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
			NULL);

	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
		roccat_button_set_to_normal(&priv->data->button, KIRO_BUTTON_TYPE_OPEN_APPLICATION);
		path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
		kiro_button_combo_box_data_set_opener(priv->data, path);
		g_free(path);
		retval = TRUE;
	}
	gtk_widget_destroy(dialog);
	return retval;
}

static gboolean key_shortcut(KiroButtonComboBox *button_combo_box) {
	KiroButtonComboBoxPrivate *priv = button_combo_box->priv;
	RoccatShortcutDialog *dialog;
	gboolean retval = FALSE;

	dialog = ROCCAT_SHORTCUT_DIALOG(roccat_shortcut_dialog_new(gtk_roccat_widget_get_gtk_window(GTK_WIDGET(button_combo_box))));
	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
		roccat_button_set_to_shortcut(&priv->data->button, KIRO_BUTTON_TYPE_SHORTCUT,
				roccat_shortcut_dialog_get_hid(dialog),
				roccat_button_build_shortcut_modifier(
						roccat_shortcut_dialog_get_ctrl(dialog),
						roccat_shortcut_dialog_get_shift(dialog),
						roccat_shortcut_dialog_get_alt(dialog),
						roccat_shortcut_dialog_get_win(dialog)));
		retval = TRUE;
	}
	gtk_widget_destroy(GTK_WIDGET(dialog));
	return retval;
}

static gboolean key_normal(KiroButtonComboBox *button_combo_box, guint type) {
	gboolean retval = TRUE;

	if (button_combo_box->priv->data->button.type == type)
		retval = FALSE;

	roccat_button_set_to_normal(&button_combo_box->priv->data->button, type);

	return retval;
}

static void changed_cb(GtkComboBox *combo, gpointer user_data) {
	KiroButtonComboBox *button_combo_box = KIRO_BUTTON_COMBO_BOX(combo);
	guint type;
	gboolean changed;

	type = roccat_key_combo_box_get_active_type(ROCCAT_KEY_COMBO_BOX(combo));
	if (type == ROCCAT_KEY_COMBO_BOX_TYPE_NONE)
		return;

	switch (type) {
	case KIRO_BUTTON_TYPE_OPEN_APPLICATION:
		changed = key_open_application(button_combo_box);
		break;
	case KIRO_BUTTON_TYPE_SHORTCUT:
		changed = key_shortcut(button_combo_box);
		break;
	default:
		changed = key_normal(button_combo_box, type);
		break;
	}

	kiro_button_combo_box_update(button_combo_box);
	if (changed)
		g_signal_emit((gpointer)button_combo_box, signals[KEY_CHANGED], 0);
}

void kiro_button_combo_box_set_data_pointer(KiroButtonComboBox *button_combo_box, KiroButtonComboBoxData *data) {
	button_combo_box->priv->data = data;
	kiro_button_combo_box_update(button_combo_box);
}

KiroButtonComboBoxData *kiro_button_combo_box_get_data_pointer(KiroButtonComboBox *button_combo_box) {
	return button_combo_box->priv->data;
}

GtkWidget *kiro_button_combo_box_new(guint mask) {
	KiroButtonComboBox *button_combo_box;
	KiroButtonComboBoxClass *button_combo_box_class;
	KiroButtonComboBoxPrivate *priv;

	button_combo_box = KIRO_BUTTON_COMBO_BOX(g_object_new(KIRO_BUTTON_COMBO_BOX_TYPE,
			"exclude-mask", mask,
			NULL));

	priv = button_combo_box->priv;

	button_combo_box_class = KIRO_BUTTON_COMBO_BOX_GET_CLASS(button_combo_box);
	roccat_key_combo_box_set_model(ROCCAT_KEY_COMBO_BOX(button_combo_box), button_combo_box_class->model);

	priv->changed_handler_id = g_signal_connect(G_OBJECT(button_combo_box), "changed", G_CALLBACK(changed_cb), NULL);

	return GTK_WIDGET(button_combo_box);
}

static void kiro_button_combo_box_init(KiroButtonComboBox *button_combo_box) {
	KiroButtonComboBoxPrivate *priv = KIRO_BUTTON_COMBO_BOX_GET_PRIVATE(button_combo_box);
	button_combo_box->priv = priv;
}

static GtkTreeModel *button_info_model_new(void) {
	GtkTreeStore *store;
	GtkTreeIter toplevel;

	store = roccat_key_combo_box_store_new();


	roccat_key_combo_box_store_append(store, NULL, NULL, _("Shortcut"), KIRO_BUTTON_TYPE_SHORTCUT, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("Basic functions"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Button 1 (Click)"), KIRO_BUTTON_TYPE_CLICK, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Button 2 (Universal scrolling)"), KIRO_BUTTON_TYPE_UNIVERSAL_SCROLLING, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Button 3 (Menu)"), KIRO_BUTTON_TYPE_MENU, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Button 4 (Scroll up)"), KIRO_BUTTON_TYPE_SCROLL_UP, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Button 5 (Scroll down)"), KIRO_BUTTON_TYPE_SCROLL_DOWN, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Button 6 (Tilt left)"), KIRO_BUTTON_TYPE_TILT_LEFT, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Button 7 (Tilt right)"), KIRO_BUTTON_TYPE_TILT_RIGHT, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Button 8 (Browser backward)"), KIRO_BUTTON_TYPE_BROWSER_BACKWARD, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Button 9 (Browser forward)"), KIRO_BUTTON_TYPE_BROWSER_FORWARD, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("*Double click"), KIRO_BUTTON_TYPE_DOUBLE_CLICK, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Home"), KIRO_BUTTON_TYPE_HOME, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("End"), KIRO_BUTTON_TYPE_END, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Page up"), KIRO_BUTTON_TYPE_PAGE_UP, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Page down"), KIRO_BUTTON_TYPE_PAGE_DOWN, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("*Left ctrl"), KIRO_BUTTON_TYPE_LEFT_CTRL, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("*Left shift"), KIRO_BUTTON_TYPE_LEFT_SHIFT, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("*Right alt"), KIRO_BUTTON_TYPE_RIGHT_ALT, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("*Windows key"), KIRO_BUTTON_TYPE_WIN, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("Advanced functions"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("CPI cycle"), KIRO_BUTTON_TYPE_CPI_CYCLE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("CPI up"), KIRO_BUTTON_TYPE_CPI_UP, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("CPI down"), KIRO_BUTTON_TYPE_CPI_DOWN, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Sensitivity cycle"), KIRO_BUTTON_TYPE_SENSITIVITY_CYCLE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Sensitivity up"), KIRO_BUTTON_TYPE_SENSITIVITY_UP, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Sensitivity down"), KIRO_BUTTON_TYPE_SENSITIVITY_DOWN, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("Multimedia"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Mute master volume"), KIRO_BUTTON_TYPE_MUTE_MASTER, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Volume up"), KIRO_BUTTON_TYPE_VOLUME_UP, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Volume down"), KIRO_BUTTON_TYPE_VOLUME_DOWN, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Previous track"), KIRO_BUTTON_TYPE_PREV_TRACK, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Next track"), KIRO_BUTTON_TYPE_NEXT_TRACK, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Play/Pause"), KIRO_BUTTON_TYPE_PLAY_PAUSE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Stop"), KIRO_BUTTON_TYPE_STOP, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("Open"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE | KIRO_BUTTON_COMBO_BOX_GROUP_EXTENDED);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Driver"), KIRO_BUTTON_TYPE_OPEN_DRIVER, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Application"), KIRO_BUTTON_TYPE_OPEN_APPLICATION, KIRO_BUTTON_COMBO_BOX_GROUP_EXTENDED);
	roccat_key_combo_box_store_append(store, NULL, NULL, _("Disabled"), KIRO_BUTTON_TYPE_DISABLED, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	return GTK_TREE_MODEL(store);
}

static void kiro_button_combo_box_class_init(KiroButtonComboBoxClass *klass) {
	g_type_class_add_private(klass, sizeof(KiroButtonComboBoxPrivate));
	klass->model = button_info_model_new();

	signals[KEY_CHANGED] = g_signal_new("key-changed",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);
}

void kiro_button_combo_box_data_copy(KiroButtonComboBoxData *destination, KiroButtonComboBoxData const *source) {
	roccat_button_copy(&destination->button, &source->button);
	g_strlcpy(destination->opener, source->opener, ROCCAT_SWARM_RMP_OPENER_LENGTH);
}

void kiro_button_combo_box_data_swap(KiroButtonComboBoxData *one, KiroButtonComboBoxData *other) {
	KiroButtonComboBoxData temp;
	kiro_button_combo_box_data_copy(&temp, one);
	kiro_button_combo_box_data_copy(one, other);
	kiro_button_combo_box_data_copy(other, &temp);
}
