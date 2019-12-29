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

#include <gaminggear/gaminggear_macro_dialog.h>
#include <gaminggear/gdk_key_translations.h>
#include <gaminggear/hid_uid.h>
#include "ryos_key_combo_box.h"
#include "roccat_talk_target_dialog.h"
#include "roccat_shortcut_dialog.h"
#include "roccat_timer_dialog.h"
#include "roccat_key_dialog.h"
#include "roccat_warning_dialog.h"
#include "roccat_talk.h"
#include "roccat_helper.h"
#include "gtk_roccat_helper.h"
#include "g_cclosure_roccat_marshaller.h"
#include "i18n-lib.h"

#define RYOS_KEY_COMBO_BOX_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), RYOS_KEY_COMBO_BOX_TYPE, RyosKeyComboBoxClass))
#define IS_RYOS_KEY_COMBO_BOX_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), RYOS_KEY_COMBO_BOX_TYPE))
#define RYOS_KEY_COMBO_BOX_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), RYOS_KEY_COMBO_BOX_TYPE, RyosKeyComboBoxClass))
#define RYOS_KEY_COMBO_BOX_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), RYOS_KEY_COMBO_BOX_TYPE, RyosKeyComboBoxPrivate))

typedef struct _RyosKeyComboBoxClass RyosKeyComboBoxClass;
typedef struct _RyosKeyComboBoxPrivate RyosKeyComboBoxPrivate;

struct _RyosKeyComboBox {
	RoccatKeyComboBox parent;
	RyosKeyComboBoxPrivate *priv;
};

struct _RyosKeyComboBoxClass {
	RoccatKeyComboBoxClass parent_class;
	GtkTreeModel *model;
};

struct _RyosKeyComboBoxPrivate {
	RyosKeyComboBoxData *data;
	guint standard;
	gulong changed_handler_id;
};

G_DEFINE_TYPE(RyosKeyComboBox, ryos_key_combo_box, ROCCAT_KEY_COMBO_BOX_TYPE);

enum {
	KEY_CHANGED,
	LAST_SIGNAL,
};

static guint signals[LAST_SIGNAL] = { 0 };

static gchar *shortcut_name(RoccatButton const *key) {
	return roccat_shortcut_dialog_create_name_full(roccat_get_bit8(key->modifier, RYOS_KEY_MODIFIER_BIT_CTRL),
			roccat_get_bit8(key->modifier, RYOS_KEY_MODIFIER_BIT_SHIFT),
			roccat_get_bit8(key->modifier, RYOS_KEY_MODIFIER_BIT_ALT),
			roccat_get_bit8(key->modifier, RYOS_KEY_MODIFIER_BIT_WIN),
			key->key);
}

gchar *ryos_key_combo_box_get_text_for_data(RyosKeyComboBox *key_combo_box, RyosKeyComboBoxData const *data) {
	gchar *text;
	gchar *temp;
	guint type;

	type = data->key.type;

	switch (type) {
	case RYOS_KEY_TYPE_TALK_EASYSHIFT:
	case RYOS_KEY_TYPE_TALK_EASYSHIFT_LOCK:
	case RYOS_KEY_TYPE_TALK_EASYAIM_1:
	case RYOS_KEY_TYPE_TALK_EASYAIM_2:
	case RYOS_KEY_TYPE_TALK_EASYAIM_3:
	case RYOS_KEY_TYPE_TALK_EASYAIM_4:
	case RYOS_KEY_TYPE_TALK_EASYAIM_5:
		text = g_strdup(ryos_rkp_talk_get_name(&data->talk));
		break;
	case RYOS_KEY_TYPE_LIGHT:
		text = g_strdup(_("Light"));
		break;
	case RYOS_KEY_TYPE_RECORD:
		text = g_strdup(_("Live record"));
		break;
	case RYOS_KEY_TYPE_STANDARD_SHIFT_1:
		text = g_strdup(_("Shift+1"));
		break;
	case RYOS_KEY_TYPE_STANDARD_SHIFT_2:
		text = g_strdup(_("Shift+2"));
		break;
	case RYOS_KEY_TYPE_STANDARD_SHIFT_3:
		text = g_strdup(_("Shift+3"));
		break;
	case RYOS_KEY_TYPE_STANDARD_SHIFT_4:
		text = g_strdup(_("Shift+4"));
		break;
	case RYOS_KEY_TYPE_STANDARD_SHIFT_5:
		text = g_strdup(_("Shift+5"));
		break;
	case RYOS_KEY_TYPE_STANDARD_CTRL_1:
		text = g_strdup(_("Ctrl+1"));
		break;
	case RYOS_KEY_TYPE_STANDARD_CTRL_2:
		text = g_strdup(_("Ctrl+2"));
		break;
	case RYOS_KEY_TYPE_STANDARD_CTRL_3:
		text = g_strdup(_("Ctrl+3"));
		break;
	case RYOS_KEY_TYPE_STANDARD_CTRL_4:
		text = g_strdup(_("Ctrl+4"));
		break;
	case RYOS_KEY_TYPE_STANDARD_CTRL_5:
		text = g_strdup(_("Ctrl+5"));
		break;
	case RYOS_KEY_TYPE_SHORTCUT:
		temp = shortcut_name(&data->key);
		text = g_strdup_printf(_("Shortcut: %s"), temp);
		g_free(temp);
		break;
	case RYOS_KEY_TYPE_MACRO:
		text = g_strdup_printf(_("Macro: %s/%s"), data->macro.macroset_name, data->macro.macro_name);
		break;
	case RYOS_KEY_TYPE_LED_MACRO:
		text = g_strdup_printf(_("LED Macro: %s/%s"), data->led_macro.macroset_name, data->led_macro.macro_name);
		break;
	case RYOS_KEY_TYPE_QUICKLAUNCH:
		temp = ryos_rkp_quicklaunch_get_name(&data->quicklaunch);
		text = g_strdup_printf(_("Quicklaunch: %s"), temp);
		g_free(temp);
		break;
	case RYOS_KEY_TYPE_TIMER:
		text = g_strdup_printf(_("Timer: %s"), (gchar const *)data->timer.name);
		break;
	default:
		text = roccat_key_combo_box_model_get_title_for_type_explicit(gtk_combo_box_get_model(GTK_COMBO_BOX(key_combo_box)), type);
		if (!text)
			text = gaminggear_hid_to_name(type);
		break;
	}

	return text;
}

void ryos_key_combo_box_update(RyosKeyComboBox *key_combo_box) {
	RyosKeyComboBoxPrivate *priv = key_combo_box->priv;
	gchar *text;

	text = ryos_key_combo_box_get_text_for_data(key_combo_box, priv->data);

	g_signal_handler_block(G_OBJECT(key_combo_box), priv->changed_handler_id);
	roccat_key_combo_box_set_text(ROCCAT_KEY_COMBO_BOX(key_combo_box), text);
	g_signal_handler_unblock(G_OBJECT(key_combo_box), priv->changed_handler_id);

	g_free(text);
}

static gboolean key_quicklaunch(RyosKeyComboBox *key_combo_box) {
	RyosKeyComboBoxPrivate *priv = key_combo_box->priv;
	GtkWidget *dialog;
	gboolean retval = FALSE;
	gchar *path;

	dialog = gtk_file_chooser_dialog_new(_("Select executable"),
			gtk_roccat_widget_get_gtk_window(GTK_WIDGET(key_combo_box)),
			GTK_FILE_CHOOSER_ACTION_OPEN,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
			NULL);

	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
		roccat_button_set_to_normal(&priv->data->key, RYOS_KEY_TYPE_QUICKLAUNCH);
		path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
		ryos_rkp_quicklaunch_set(&priv->data->quicklaunch, path);
		g_free(path);
		retval = TRUE;
	}
	gtk_widget_destroy(dialog);
	return retval;
}

static gboolean set_macro(RyosKeyComboBox *key_combo_box) {
	RyosKeyComboBoxPrivate *priv = key_combo_box->priv;
	GtkWindow *parent;
	GaminggearMacro *gaminggear_macro;
	RyosMacro *ryos_macro;
	gboolean result;
	GError *local_error = NULL;

	parent = gtk_roccat_widget_get_gtk_window(GTK_WIDGET(key_combo_box));
	gaminggear_macro = gaminggear_macro_dialog(parent);
	if (!gaminggear_macro)
		return FALSE;

	ryos_macro = (RyosMacro *)g_malloc0(sizeof(RyosMacro));
	result = gaminggear_macro_to_ryos_macro(gaminggear_macro, ryos_macro, &local_error);
	gaminggear_macro_free(gaminggear_macro);
	if (!result) {
		roccat_handle_error_dialog(parent, _("Could not convert macro"), &local_error);
		g_free(ryos_macro);
		return FALSE;
	}

	ryos_macro_copy(&priv->data->macro, ryos_macro);
	g_free(ryos_macro);

	return TRUE;
}

static gboolean set_led_macro(RyosKeyComboBox *key_combo_box) {
	RyosKeyComboBoxPrivate *priv = key_combo_box->priv;
	GtkWindow *parent;
	GaminggearMacro *gaminggear_macro;
	RyosLedMacro *ryos_led_macro;
	gboolean result;
	GError *local_error = NULL;

	parent = gtk_roccat_widget_get_gtk_window(GTK_WIDGET(key_combo_box));
	gaminggear_macro = gaminggear_macro_dialog(parent);
	if (!gaminggear_macro)
		return FALSE;

	ryos_led_macro = (RyosLedMacro *)g_malloc0(sizeof(RyosLedMacro));
	result = gaminggear_macro_to_ryos_led_macro(gaminggear_macro, ryos_led_macro, &local_error);
	gaminggear_macro_free(gaminggear_macro);
	if (!result) {
		roccat_handle_error_dialog(parent, _("Could not convert macro"), &local_error);
		g_free(ryos_led_macro);
		return FALSE;
	}

	ryos_led_macro_copy(&priv->data->led_macro, ryos_led_macro);
	g_free(ryos_led_macro);

	return TRUE;
}

static gboolean key_macro(RyosKeyComboBox *key_combo_box) {
	RyosKeyComboBoxPrivate *priv = key_combo_box->priv;
	gboolean retval;

	retval = set_macro(key_combo_box);

	if (retval)
		roccat_button_set_to_normal(&priv->data->key, RYOS_KEY_TYPE_MACRO);

	return retval;
}

static gboolean key_led_macro(RyosKeyComboBox *key_combo_box) {
	RyosKeyComboBoxPrivate *priv = key_combo_box->priv;
	gboolean retval;

	retval = set_led_macro(key_combo_box);

	if (retval)
		roccat_button_set_to_normal(&priv->data->key, RYOS_KEY_TYPE_LED_MACRO);

	return retval;
}

static gboolean key_shortcut(RyosKeyComboBox *key_combo_box) {
	RyosKeyComboBoxPrivate *priv = key_combo_box->priv;
	RoccatShortcutDialog *dialog;
	gboolean retval = FALSE;

	dialog = ROCCAT_SHORTCUT_DIALOG(roccat_shortcut_dialog_new(gtk_roccat_widget_get_gtk_window(GTK_WIDGET(key_combo_box))));
	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
		roccat_button_set_to_shortcut(&priv->data->key, RYOS_KEY_TYPE_SHORTCUT,
				roccat_shortcut_dialog_get_hid(dialog),
				ryos_key_build_shortcut_modifier(
						roccat_shortcut_dialog_get_ctrl(dialog),
						roccat_shortcut_dialog_get_shift(dialog),
						roccat_shortcut_dialog_get_alt(dialog),
						roccat_shortcut_dialog_get_win(dialog)));
		retval = TRUE;
	}
	gtk_widget_destroy(GTK_WIDGET(dialog));
	return retval;
}

static gboolean key_timer(RyosKeyComboBox *key_combo_box) {
	RyosKeyComboBoxPrivate *priv = key_combo_box->priv;
	RoccatTimer *roccat_timer;
	gboolean retval = FALSE;

	roccat_timer = roccat_timer_dialog(gtk_roccat_widget_get_gtk_window(GTK_WIDGET(key_combo_box)));
	if (roccat_timer) {
		roccat_button_set_to_normal(&priv->data->key, RYOS_KEY_TYPE_TIMER);
		roccat_timer_to_ryos_rkp_timer(roccat_timer, &priv->data->timer);
		g_free(roccat_timer);
		retval = TRUE;
	}
	return retval;
}

static gboolean key_normal(RyosKeyComboBox *key_combo_box, guint type) {
	gboolean retval = TRUE;

	if (key_combo_box->priv->data->key.type == type)
		retval = FALSE;

	roccat_button_set_to_normal(&key_combo_box->priv->data->key, type);

	return retval;
}

static gboolean key_remap(RyosKeyComboBox *key_combo_box) {
	RoccatKeyDialog *dialog;
	gboolean retval = FALSE;

	dialog = ROCCAT_KEY_DIALOG(roccat_key_dialog_new(gtk_roccat_widget_get_gtk_window(GTK_WIDGET(key_combo_box))));
	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
		roccat_button_set_to_normal(&key_combo_box->priv->data->key, roccat_key_dialog_get_hid(dialog));
		retval = TRUE;
	}
	gtk_widget_destroy(GTK_WIDGET(dialog));
	return retval;
}

static gboolean key_talk_all(RyosKeyComboBox *key_combo_box) {
	RyosKeyComboBoxData *data = key_combo_box->priv->data;

	if (data->key.type == RYOS_KEY_TYPE_TALK_EASYSHIFT_ALL)
		return FALSE;

	roccat_button_set_to_normal(&data->key, RYOS_KEY_TYPE_TALK_EASYSHIFT_ALL);
	ryos_rkp_talk_set(&data->talk, ROCCAT_TALK_DEVICE_ALL, RYOS_KEY_TYPE_TALK_EASYSHIFT_ALL, _("Both Easyshift"));
	return TRUE;
}

static gboolean key_talk(RyosKeyComboBox *key_combo_box, guint type) {
	RyosKeyComboBoxData *data = key_combo_box->priv->data;
	RoccatTalkTargetDialog *talk_target_dialog;
	guint target;
	gboolean retval = FALSE;
	gchar *text;

	talk_target_dialog = ROCCAT_TALK_TARGET_DIALOG(roccat_talk_target_dialog_new(
			gtk_roccat_widget_get_gtk_window(GTK_WIDGET(key_combo_box)), ROCCAT_TALK_DEVICE_KEYBOARD));
	if (gtk_dialog_run(GTK_DIALOG(talk_target_dialog)) == GTK_RESPONSE_ACCEPT) {
		target = roccat_talk_target_dialog_get_value(talk_target_dialog);

		switch (type) {
		case RYOS_KEY_TYPE_TALK_EASYSHIFT:
			text = g_strdup_printf(_("Easyshift: %s"), roccat_talk_device_get_text_static(target));
			break;
		case RYOS_KEY_TYPE_TALK_EASYSHIFT_LOCK:
			text = g_strdup_printf(_("Easyshift lock: %s"), roccat_talk_device_get_text_static(target));
			break;
		case RYOS_KEY_TYPE_TALK_EASYAIM_1:
			text = g_strdup_printf(_("Easyaim 1: %s"), roccat_talk_device_get_text_static(target));
			break;
		case RYOS_KEY_TYPE_TALK_EASYAIM_2:
			text = g_strdup_printf(_("Easyaim 2: %s"), roccat_talk_device_get_text_static(target));
			break;
		case RYOS_KEY_TYPE_TALK_EASYAIM_3:
			text = g_strdup_printf(_("Easyaim 3: %s"), roccat_talk_device_get_text_static(target));
			break;
		case RYOS_KEY_TYPE_TALK_EASYAIM_4:
			text = g_strdup_printf(_("Easyaim 4: %s"), roccat_talk_device_get_text_static(target));
			break;
		case RYOS_KEY_TYPE_TALK_EASYAIM_5:
			text = g_strdup_printf(_("Easyaim 5: %s"), roccat_talk_device_get_text_static(target));
			break;
		default:
			text = g_strdup_printf(_("Got unknown Talk type 0x%02x"), type);
			break;
		}

		roccat_button_set_to_normal(&data->key, type);
		ryos_rkp_talk_set(&data->talk, target, type, text);
		retval = TRUE;
	}

	gtk_widget_destroy(GTK_WIDGET(talk_target_dialog));
	return retval;
}

static void changed_cb(GtkComboBox *combo, gpointer user_data) {
	RyosKeyComboBox *key_combo_box = RYOS_KEY_COMBO_BOX(combo);
	RyosKeyComboBoxPrivate *priv = key_combo_box->priv;
	guint type;
	gboolean changed;

	type = roccat_key_combo_box_get_active_type(ROCCAT_KEY_COMBO_BOX(combo));
	if (type == ROCCAT_KEY_COMBO_BOX_TYPE_NONE)
		return;

	if (type == RYOS_KEY_COMBO_BOX_TYPE_STANDARD)
		type = priv->standard;

	switch (type) {
	case RYOS_KEY_COMBO_BOX_TYPE_REMAP:
		changed = key_remap(key_combo_box);
		break;
	case RYOS_KEY_TYPE_MACRO:
		changed = key_macro(key_combo_box);
		break;
	case RYOS_KEY_TYPE_LED_MACRO:
		changed = key_led_macro(key_combo_box);
		break;
	case RYOS_KEY_TYPE_SHORTCUT:
		changed = key_shortcut(key_combo_box);
		break;
	case RYOS_KEY_TYPE_QUICKLAUNCH:
		changed = key_quicklaunch(key_combo_box);
		break;
	case RYOS_KEY_TYPE_TIMER:
		changed = key_timer(key_combo_box);
		break;
	case RYOS_KEY_TYPE_TALK_EASYSHIFT:
	case RYOS_KEY_TYPE_TALK_EASYSHIFT_LOCK:
	case RYOS_KEY_TYPE_TALK_EASYAIM_1:
	case RYOS_KEY_TYPE_TALK_EASYAIM_2:
	case RYOS_KEY_TYPE_TALK_EASYAIM_3:
	case RYOS_KEY_TYPE_TALK_EASYAIM_4:
	case RYOS_KEY_TYPE_TALK_EASYAIM_5:
		changed = key_talk(key_combo_box, type);
		break;
	case RYOS_KEY_TYPE_TALK_EASYSHIFT_ALL:
		changed = key_talk_all(key_combo_box);
		break;
	default:
		changed = key_normal(key_combo_box, type);
	}

	ryos_key_combo_box_update(key_combo_box);
	if (changed)
		g_signal_emit((gpointer)key_combo_box, signals[KEY_CHANGED], 0);
}

void ryos_key_combo_box_set_data_pointer(RyosKeyComboBox *key_combo_box, RyosKeyComboBoxData *data) {
	key_combo_box->priv->data = data;
	ryos_key_combo_box_update(key_combo_box);
}

RyosKeyComboBoxData *ryos_key_combo_box_get_data_pointer(RyosKeyComboBox *key_combo_box) {
	return key_combo_box->priv->data;
}

void ryos_key_combo_box_set_standard(RyosKeyComboBox *key_combo_box, guint standard) {
	key_combo_box->priv->standard = standard;
}

GtkWidget *ryos_key_combo_box_new(guint mask, guint standard) {
	RyosKeyComboBox *key_combo_box;
	RyosKeyComboBoxClass *key_combo_box_class;
	RyosKeyComboBoxPrivate *priv;

	key_combo_box = RYOS_KEY_COMBO_BOX(g_object_new(RYOS_KEY_COMBO_BOX_TYPE,
			"exclude-mask", mask,
			NULL));

	priv = key_combo_box->priv;

	ryos_key_combo_box_set_standard(key_combo_box, standard);

	key_combo_box_class = RYOS_KEY_COMBO_BOX_GET_CLASS(key_combo_box);
	roccat_key_combo_box_set_model(ROCCAT_KEY_COMBO_BOX(key_combo_box), key_combo_box_class->model);

	priv->changed_handler_id = g_signal_connect(G_OBJECT(key_combo_box), "changed", G_CALLBACK(changed_cb), NULL);

	return GTK_WIDGET(key_combo_box);
}

static void ryos_key_combo_box_init(RyosKeyComboBox *key_combo_box) {
	RyosKeyComboBoxPrivate *priv = RYOS_KEY_COMBO_BOX_GET_PRIVATE(key_combo_box);
	key_combo_box->priv = priv;
}

static GtkTreeModel *button_info_model_new(void) {
	GtkTreeStore *store;
	GtkTreeIter iter;
	GtkTreeIter toplevel;

	store = roccat_key_combo_box_store_new();

	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("Applications"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, RYOS_KEY_COMBO_BOX_GROUP_KEY);

	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("E-Mail"), RYOS_KEY_TYPE_APP_EMAIL, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Calculator"), RYOS_KEY_TYPE_APP_CALCULATOR, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("My computer"), RYOS_KEY_TYPE_APP_MY_COMPUTER, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Webbrowser"), RYOS_KEY_TYPE_APP_BROWSER, RYOS_KEY_COMBO_BOX_GROUP_KEY);

	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("Easyshift"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, RYOS_KEY_COMBO_BOX_GROUP_TALK);

	roccat_key_combo_box_store_append(store, &iter, &toplevel, _("Self"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, RYOS_KEY_COMBO_BOX_GROUP_TALK);
	roccat_key_combo_box_store_append(store, NULL, &iter, _("Self Easyshift"), RYOS_KEY_TYPE_EASYSHIFT, RYOS_KEY_COMBO_BOX_GROUP_TALK);

	roccat_key_combo_box_store_append(store, &iter, &toplevel, _("Other"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, RYOS_KEY_COMBO_BOX_GROUP_TALK);
	roccat_key_combo_box_store_append(store, NULL, &iter, _("Other Easyshift"), RYOS_KEY_TYPE_TALK_EASYSHIFT, RYOS_KEY_COMBO_BOX_GROUP_TALK);
	roccat_key_combo_box_store_append(store, NULL, &iter, _("Other Easyshift lock"), RYOS_KEY_TYPE_TALK_EASYSHIFT_LOCK, RYOS_KEY_COMBO_BOX_GROUP_TALK);
	roccat_key_combo_box_store_append(store, NULL, &iter, _("Other Easyaim Setting 1"), RYOS_KEY_TYPE_TALK_EASYAIM_1, RYOS_KEY_COMBO_BOX_GROUP_TALK);
	roccat_key_combo_box_store_append(store, NULL, &iter, _("Other Easyaim Setting 2"), RYOS_KEY_TYPE_TALK_EASYAIM_2, RYOS_KEY_COMBO_BOX_GROUP_TALK);
	roccat_key_combo_box_store_append(store, NULL, &iter, _("Other Easyaim Setting 3"), RYOS_KEY_TYPE_TALK_EASYAIM_3, RYOS_KEY_COMBO_BOX_GROUP_TALK);
	roccat_key_combo_box_store_append(store, NULL, &iter, _("Other Easyaim Setting 4"), RYOS_KEY_TYPE_TALK_EASYAIM_4, RYOS_KEY_COMBO_BOX_GROUP_TALK);
	roccat_key_combo_box_store_append(store, NULL, &iter, _("Other Easyaim Setting 5"), RYOS_KEY_TYPE_TALK_EASYAIM_5, RYOS_KEY_COMBO_BOX_GROUP_TALK);

	roccat_key_combo_box_store_append(store, &iter, &toplevel, _("Both"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, RYOS_KEY_COMBO_BOX_GROUP_TALK);
	roccat_key_combo_box_store_append(store, NULL, &iter, _("Both Easyshift"), RYOS_KEY_TYPE_TALK_EASYSHIFT_ALL, RYOS_KEY_COMBO_BOX_GROUP_TALK);

	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("Multimedia"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, RYOS_KEY_COMBO_BOX_GROUP_KEY);

	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Open player"), RYOS_KEY_TYPE_MULTIMEDIA_OPEN_PLAYER, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Previous track"), RYOS_KEY_TYPE_MULTIMEDIA_PREV_TRACK, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Next track"), RYOS_KEY_TYPE_MULTIMEDIA_NEXT_TRACK, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Play/Pause"), RYOS_KEY_TYPE_MULTIMEDIA_PLAY_PAUSE, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Stop"), RYOS_KEY_TYPE_MULTIMEDIA_STOP, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Mute"), RYOS_KEY_TYPE_MULTIMEDIA_MUTE, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Volume up"), RYOS_KEY_TYPE_MULTIMEDIA_VOLUME_UP, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Volume down"), RYOS_KEY_TYPE_MULTIMEDIA_VOLUME_DOWN, RYOS_KEY_COMBO_BOX_GROUP_KEY);

	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("Profile"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, RYOS_KEY_COMBO_BOX_GROUP_KEY);

	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Profile up"), RYOS_KEY_TYPE_PROFILE_UP, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Profile down"), RYOS_KEY_TYPE_PROFILE_DOWN, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Profile cycle"), RYOS_KEY_TYPE_PROFILE_CYCLE, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Profile 1"), RYOS_KEY_TYPE_PROFILE_1, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Profile 2"), RYOS_KEY_TYPE_PROFILE_2, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Profile 3"), RYOS_KEY_TYPE_PROFILE_3, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Profile 4"), RYOS_KEY_TYPE_PROFILE_4, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Profile 5"), RYOS_KEY_TYPE_PROFILE_5, RYOS_KEY_COMBO_BOX_GROUP_KEY);

	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("Timer"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, RYOS_KEY_COMBO_BOX_GROUP_KEY | RYOS_KEY_COMBO_BOX_GROUP_EXTENDED);

	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Timer start"), RYOS_KEY_TYPE_TIMER, RYOS_KEY_COMBO_BOX_GROUP_EXTENDED);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Timer stop"), RYOS_KEY_TYPE_TIMER_STOP, RYOS_KEY_COMBO_BOX_GROUP_KEY);

	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("WWW"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, RYOS_KEY_COMBO_BOX_GROUP_KEY);

	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Search"), RYOS_KEY_TYPE_WWW_SEARCH, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Home"), RYOS_KEY_TYPE_WWW_HOME, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Back"), RYOS_KEY_TYPE_WWW_BACK, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Forward"), RYOS_KEY_TYPE_WWW_FORWARD, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Stop"), RYOS_KEY_TYPE_WWW_STOP, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Refresh"), RYOS_KEY_TYPE_WWW_REFRESH, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Favorites"), RYOS_KEY_TYPE_WWW_FAVORITES, RYOS_KEY_COMBO_BOX_GROUP_KEY);

	roccat_key_combo_box_store_append(store, NULL, NULL, _("Shortcut"), RYOS_KEY_TYPE_SHORTCUT, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, NULL, _("Macro"), RYOS_KEY_TYPE_MACRO, RYOS_KEY_COMBO_BOX_GROUP_EXTENDED);
	roccat_key_combo_box_store_append(store, NULL, NULL, _("Macro play/pause"), RYOS_KEY_TYPE_MACRO_PLAY_PAUSE, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, NULL, _("LED Macro"), RYOS_KEY_TYPE_LED_MACRO, RYOS_KEY_COMBO_BOX_GROUP_EXTENDED);
	roccat_key_combo_box_store_append(store, NULL, NULL, _("Open driver"), RYOS_KEY_TYPE_OPEN_DRIVER, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, NULL, _("Quicklaunch"), RYOS_KEY_TYPE_QUICKLAUNCH, RYOS_KEY_COMBO_BOX_GROUP_EXTENDED);

	roccat_key_combo_box_store_append(store, NULL, NULL, _("Standard"), RYOS_KEY_COMBO_BOX_TYPE_STANDARD, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, NULL, _("Remap"), RYOS_KEY_COMBO_BOX_TYPE_REMAP, RYOS_KEY_COMBO_BOX_GROUP_PRIMARY);
	roccat_key_combo_box_store_append(store, NULL, NULL, _("Disabled"), RYOS_KEY_TYPE_DISABLED, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	return GTK_TREE_MODEL(store);
}

static void ryos_key_combo_box_class_init(RyosKeyComboBoxClass *klass) {
	g_type_class_add_private(klass, sizeof(RyosKeyComboBoxPrivate));
	klass->model = button_info_model_new();

	signals[KEY_CHANGED] = g_signal_new("key-changed",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);
}
