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
#include <gaminggear/hid_uid.h>
#include <gaminggear/key_translations.h>
#include <gaminggear/gdk_key_translations.h>
#include "isku_key_combo_box.h"
#include "roccat_button.h"
#include "roccat_talk_target_dialog.h"
#include "roccat_shortcut_dialog.h"
#include "roccat_timer_dialog.h"
#include "roccat_warning_dialog.h"
#include "roccat_talk.h"
#include "roccat_helper.h"
#include "gtk_roccat_helper.h"
#include "i18n-lib.h"

#define ISKU_KEY_COMBO_BOX_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), ISKU_KEY_COMBO_BOX_TYPE, IskuKeyComboBoxClass))
#define IS_ISKU_KEY_COMBO_BOX_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), ISKU_KEY_COMBO_BOX_TYPE))
#define ISKU_KEY_COMBO_BOX_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), ISKU_KEY_COMBO_BOX_TYPE, IskuKeyComboBoxClass))
#define ISKU_KEY_COMBO_BOX_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), ISKU_KEY_COMBO_BOX_TYPE, IskuKeyComboBoxPrivate))

typedef struct _IskuKeyComboBoxClass IskuKeyComboBoxClass;
typedef struct _IskuKeyComboBoxPrivate IskuKeyComboBoxPrivate;

struct _IskuKeyComboBox {
	RoccatKeyComboBox parent;
	IskuKeyComboBoxPrivate *priv;
};

struct _IskuKeyComboBoxClass {
	RoccatKeyComboBoxClass parent_class;
	GtkTreeModel *model;
};

struct _IskuKeyComboBoxPrivate {
	gint standard;
	IskuRkpMacroKeyInfo *key_info;
	gulong changed_handler_id;
};

G_DEFINE_TYPE(IskuKeyComboBox, isku_key_combo_box, ROCCAT_KEY_COMBO_BOX_TYPE);

static void set_text(IskuKeyComboBox *key_combo_box) {
	IskuKeyComboBoxPrivate *priv = key_combo_box->priv;
	gchar *text;
	gchar *temp;
	gint type;
	
	type = priv->key_info->type;
	
	if (type >= HID_UID_KB_A && type <= HID_UID_KB_F12) {
		text = gaminggear_hid_to_name(type);
	} else {
		switch (type) {
		case ISKU_KEY_TYPE_UNUSED:
			text = g_strdup(_("Disabled"));
			break;
		case ISKU_KEY_TYPE_TALK_OTHER_EASYSHIFT:
			text = g_strdup_printf(_("Easyshift: %s"), roccat_talk_device_get_text_static(isku_rkp_macro_key_info_get_talk_device(priv->key_info)));
			break;
		case ISKU_KEY_TYPE_TALK_OTHER_EASYSHIFT_LOCK:
			text = g_strdup_printf(_("Easyshift lock: %s"), roccat_talk_device_get_text_static(isku_rkp_macro_key_info_get_talk_device(priv->key_info)));
			break;
		case ISKU_KEY_TYPE_TALK_OTHER_EASYAIM_1:
			text = g_strdup_printf(_("Easyaim %u: %s"), 1, roccat_talk_device_get_text_static(isku_rkp_macro_key_info_get_talk_device(priv->key_info)));
			break;
		case ISKU_KEY_TYPE_TALK_OTHER_EASYAIM_2:
			text = g_strdup_printf(_("Easyaim %u: %s"), 2, roccat_talk_device_get_text_static(isku_rkp_macro_key_info_get_talk_device(priv->key_info)));
			break;
		case ISKU_KEY_TYPE_TALK_OTHER_EASYAIM_3:
			text = g_strdup_printf(_("Easyaim %u: %s"), 3, roccat_talk_device_get_text_static(isku_rkp_macro_key_info_get_talk_device(priv->key_info)));
			break;
		case ISKU_KEY_TYPE_TALK_OTHER_EASYAIM_4:
			text = g_strdup_printf(_("Easyaim %u: %s"), 4, roccat_talk_device_get_text_static(isku_rkp_macro_key_info_get_talk_device(priv->key_info)));
			break;
		case ISKU_KEY_TYPE_TALK_OTHER_EASYAIM_5:
			text = g_strdup_printf(_("Easyaim %u: %s"), 5, roccat_talk_device_get_text_static(isku_rkp_macro_key_info_get_talk_device(priv->key_info)));
			break;
		case ISKU_KEY_TYPE_MACRO:
			text = g_strdup_printf(_("Macro: %s/%s"), (gchar const *)priv->key_info->macroset_name, (gchar const *)priv->key_info->macro_name);
			break;
		case ISKU_KEY_TYPE_SHORTCUT:
			temp = roccat_shortcut_dialog_create_name(priv->key_info->keystrokes[0].action, priv->key_info->keystrokes[0].key);
			text = g_strdup_printf(_("Shortcut: %s"), temp);
			g_free(temp);
			break;
		case ISKU_KEY_TYPE_QUICKLAUNCH:
			temp = g_path_get_basename((gchar const *)priv->key_info->filename);
			text = g_strdup_printf(_("Quicklaunch: %s"), temp);
			g_free(temp);
			break;
		case ISKU_KEY_TYPE_TIMER:
			text = g_strdup_printf(_("Timer: %s"), (gchar const *)priv->key_info->timer_name);
			break;
		case ISKU_KEY_TYPE_STANDARD_SHIFT_1:
			text = g_strdup(_("Shift+1"));
			break;
		case ISKU_KEY_TYPE_STANDARD_SHIFT_2:
			text = g_strdup(_("Shift+2"));
			break;
		case ISKU_KEY_TYPE_STANDARD_SHIFT_3:
			text = g_strdup(_("Shift+3"));
			break;
		case ISKU_KEY_TYPE_STANDARD_SHIFT_4:
			text = g_strdup(_("Shift+4"));
			break;
		case ISKU_KEY_TYPE_STANDARD_SHIFT_5:
			text = g_strdup(_("Shift+5"));
			break;
		case ISKU_KEY_TYPE_STANDARD_CTRL_1:
			text = g_strdup(_("Ctrl+1"));
			break;
		case ISKU_KEY_TYPE_STANDARD_CTRL_2:
			text = g_strdup(_("Ctrl+2"));
			break;
		case ISKU_KEY_TYPE_STANDARD_CTRL_3:
			text = g_strdup(_("Ctrl+3"));
			break;
		case ISKU_KEY_TYPE_STANDARD_CTRL_4:
			text = g_strdup(_("Ctrl+4"));
			break;
		case ISKU_KEY_TYPE_STANDARD_CTRL_5:
			text = g_strdup(_("Ctrl+5"));
			break;
		case ISKU_KEY_TYPE_STANDARD_MULTIMEDIA_MUTE:
			text = roccat_key_combo_box_model_get_title_for_type(gtk_combo_box_get_model(GTK_COMBO_BOX(key_combo_box)), ISKU_KEY_TYPE_MULTIMEDIA_MUTE);
			break;
		case ISKU_KEY_TYPE_STANDARD_MULTIMEDIA_VOLUME_UP:
			text = roccat_key_combo_box_model_get_title_for_type(gtk_combo_box_get_model(GTK_COMBO_BOX(key_combo_box)), ISKU_KEY_TYPE_MULTIMEDIA_VOLUME_UP);
			break;
		case ISKU_KEY_TYPE_STANDARD_MULTIMEDIA_VOLUME_DOWN:
			text = roccat_key_combo_box_model_get_title_for_type(gtk_combo_box_get_model(GTK_COMBO_BOX(key_combo_box)), ISKU_KEY_TYPE_MULTIMEDIA_VOLUME_DOWN);
			break;
		case ISKU_KEY_TYPE_STANDARD_MULTIMEDIA_PLAY_PAUSE:
			text = roccat_key_combo_box_model_get_title_for_type(gtk_combo_box_get_model(GTK_COMBO_BOX(key_combo_box)), ISKU_KEY_TYPE_MULTIMEDIA_PLAY_PAUSE);
			break;
		case ISKU_KEY_TYPE_STANDARD_MULTIMEDIA_PREV_TRACK:
			text = roccat_key_combo_box_model_get_title_for_type(gtk_combo_box_get_model(GTK_COMBO_BOX(key_combo_box)), ISKU_KEY_TYPE_MULTIMEDIA_PREV_TRACK);
			break;
		case ISKU_KEY_TYPE_STANDARD_MULTIMEDIA_NEXT_TRACK:
			text = roccat_key_combo_box_model_get_title_for_type(gtk_combo_box_get_model(GTK_COMBO_BOX(key_combo_box)), ISKU_KEY_TYPE_MULTIMEDIA_NEXT_TRACK);
			break;
		case ISKU_KEY_TYPE_STANDARD_APP_IE_BROWSER:
			text = roccat_key_combo_box_model_get_title_for_type(gtk_combo_box_get_model(GTK_COMBO_BOX(key_combo_box)), ISKU_KEY_TYPE_APP_IE_BROWSER);
			break;
		case ISKU_KEY_TYPE_STANDARD_APP_MY_COMPUTER:
			text = roccat_key_combo_box_model_get_title_for_type(gtk_combo_box_get_model(GTK_COMBO_BOX(key_combo_box)), ISKU_KEY_TYPE_APP_MY_COMPUTER);
			break;
		default:
			text = roccat_key_combo_box_model_get_title_for_type(gtk_combo_box_get_model(GTK_COMBO_BOX(key_combo_box)), type);
			break;
		}
	}

	g_signal_handler_block(G_OBJECT(key_combo_box), priv->changed_handler_id);
	roccat_key_combo_box_set_text(ROCCAT_KEY_COMBO_BOX(key_combo_box), text);
	g_signal_handler_unblock(G_OBJECT(key_combo_box), priv->changed_handler_id);

	g_free(text);
}

static IskuRkpMacroKeyInfo *button_quicklaunch(GtkWindow *parent) {
	GtkWidget *dialog;
	gchar *filename;
	IskuRkpMacroKeyInfo *key_info = NULL;

	dialog = gtk_file_chooser_dialog_new(_("Select executable"),
			parent,
			GTK_FILE_CHOOSER_ACTION_OPEN,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
			NULL);

	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
		filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
		key_info = isku_rkp_macro_key_info_new_quicklaunch(filename);
		g_free(filename);
	}
	gtk_widget_destroy(dialog);
	return key_info;
}

static IskuRkpMacroKeyInfo *button_macro(GtkWindow *parent) {
	GError *local_error = NULL;
	GaminggearMacro *gaminggear_macro;
	IskuRkpMacroKeyInfo *key_info = NULL;

	gaminggear_macro = gaminggear_macro_dialog(parent);
	if (!gaminggear_macro)
		return NULL;

	key_info = gaminggear_macro_to_isku_rkp_macro_key_info(gaminggear_macro, &local_error);
	gaminggear_macro_free(gaminggear_macro);
	if (!key_info)
		roccat_handle_error_dialog(parent, _("Could not convert macro"), &local_error);

	return key_info;
}

static IskuRkpMacroKeyInfo *button_shortcut(GtkWindow *parent) {
	RoccatShortcutDialog *dialog;
	IskuRkpMacroKeyInfo *key_info = NULL;

	dialog = ROCCAT_SHORTCUT_DIALOG(roccat_shortcut_dialog_new(parent));
	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
		key_info = isku_rkp_macro_key_info_new_shortcut(
				roccat_shortcut_dialog_get_hid(dialog),
				roccat_button_build_shortcut_modifier(
						roccat_shortcut_dialog_get_ctrl(dialog),
						roccat_shortcut_dialog_get_shift(dialog),
						roccat_shortcut_dialog_get_alt(dialog),
						roccat_shortcut_dialog_get_win(dialog)));
	}
	gtk_widget_destroy(GTK_WIDGET(dialog));
	return key_info;
}

static IskuRkpMacroKeyInfo *button_timer(GtkWindow *parent) {
	IskuRkpMacroKeyInfo *key_info = NULL;
	RoccatTimer *timer;

	timer = roccat_timer_dialog(parent);
	if (timer) {
		key_info = isku_rkp_macro_key_info_new_timer((gchar const *)timer->name, roccat_timer_get_seconds(timer));
		roccat_timer_free(timer);
	}
	return key_info;
}

static IskuRkpMacroKeyInfo *button_talk_other(IskuKeyComboBox *key_combo_box, gint key_type) {
	RoccatTalkTargetDialog *talk_target_dialog;
	guint target;
	IskuRkpMacroKeyInfo *new_key_info = NULL;

	talk_target_dialog = ROCCAT_TALK_TARGET_DIALOG(roccat_talk_target_dialog_new(
			gtk_roccat_widget_get_gtk_window(GTK_WIDGET(key_combo_box)), ROCCAT_TALK_DEVICE_KEYBOARD));
	if (gtk_dialog_run(GTK_DIALOG(talk_target_dialog)) == GTK_RESPONSE_ACCEPT) {
		target = roccat_talk_target_dialog_get_value(talk_target_dialog);
		new_key_info = isku_rkp_macro_key_info_new_talk(key_type, target);
	}

	gtk_widget_destroy(GTK_WIDGET(talk_target_dialog));

	return new_key_info;
}

static IskuRkpMacroKeyInfo *get_rkp_macro_key_info(IskuKeyComboBox *key_combo_box, gint type) {
	GtkWindow *parent = gtk_roccat_widget_get_gtk_window(GTK_WIDGET(key_combo_box));
	IskuRkpMacroKeyInfo *key_info;
	
	switch (type) {
	case ISKU_KEY_TYPE_MACRO:
		key_info = button_macro(parent);
		break;
	case ISKU_KEY_TYPE_SHORTCUT:
		key_info = button_shortcut(parent);
		break;
	case ISKU_KEY_TYPE_QUICKLAUNCH:
		key_info = button_quicklaunch(parent);
		break;
	case ISKU_KEY_TYPE_TIMER:
		key_info = button_timer(parent);
		break;
	case ISKU_KEY_TYPE_TALK_BOTH_EASYSHIFT:
		key_info = isku_rkp_macro_key_info_new_talk(type, ROCCAT_TALK_DEVICE_ALL);
		break;
	case ISKU_KEY_TYPE_TALK_OTHER_EASYSHIFT:
	case ISKU_KEY_TYPE_TALK_OTHER_EASYSHIFT_LOCK:
	case ISKU_KEY_TYPE_TALK_OTHER_EASYAIM_1:
	case ISKU_KEY_TYPE_TALK_OTHER_EASYAIM_2:
	case ISKU_KEY_TYPE_TALK_OTHER_EASYAIM_3:
	case ISKU_KEY_TYPE_TALK_OTHER_EASYAIM_4:
	case ISKU_KEY_TYPE_TALK_OTHER_EASYAIM_5:
		key_info = button_talk_other(key_combo_box, type);
		break;
	default:
		key_info = isku_rkp_macro_key_info_new_special(type);
	}
	return key_info;
}

static void button_changed_cb(GtkComboBox *combo, gpointer user_data) {
	IskuKeyComboBox *key_combo_box = ISKU_KEY_COMBO_BOX(combo);
	IskuKeyComboBoxPrivate *priv = key_combo_box->priv;
	IskuRkpMacroKeyInfo *new_key_info;
	gint actual_type, new_type;

	actual_type = roccat_key_combo_box_get_active_type(ROCCAT_KEY_COMBO_BOX(combo));
	if (actual_type == ROCCAT_KEY_COMBO_BOX_TYPE_NONE)
		return;

	new_type = (actual_type == ISKU_KEY_TYPE_STANDARD) ? priv->standard : actual_type;

	new_key_info = get_rkp_macro_key_info(key_combo_box, new_type);

	if (new_key_info) {
		isku_rkp_macro_key_info_free(priv->key_info);
		priv->key_info = new_key_info;
	}
	set_text(key_combo_box);
}

GtkWidget *isku_key_combo_box_new(guint mask, gint standard) {
	IskuKeyComboBox *key_combo_box;
	IskuKeyComboBoxClass *key_combo_box_class;
	IskuKeyComboBoxPrivate *priv;

	key_combo_box = ISKU_KEY_COMBO_BOX(g_object_new(ISKU_KEY_COMBO_BOX_TYPE,
			"exclude-mask", mask,
			NULL));

	priv = key_combo_box->priv;

	priv->standard = standard;

	key_combo_box_class = ISKU_KEY_COMBO_BOX_GET_CLASS(key_combo_box);
	roccat_key_combo_box_set_model(ROCCAT_KEY_COMBO_BOX(key_combo_box), key_combo_box_class->model);

	priv->changed_handler_id = g_signal_connect(G_OBJECT(key_combo_box), "changed", G_CALLBACK(button_changed_cb), NULL);

	return GTK_WIDGET(key_combo_box);
}

static void isku_key_combo_box_init(IskuKeyComboBox *key_combo_box) {
	IskuKeyComboBoxPrivate *priv = ISKU_KEY_COMBO_BOX_GET_PRIVATE(key_combo_box);
	key_combo_box->priv = priv;
}

static void key_store_add_profile(GtkTreeStore *store) {
	GtkTreeIter toplevel;

	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("Profile"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Profile up"), ISKU_KEY_TYPE_PROFILE_UP, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Profile down"), ISKU_KEY_TYPE_PROFILE_DOWN, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Profile cycle"), ISKU_KEY_TYPE_PROFILE_CYCLE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Profile 1"), ISKU_KEY_TYPE_PROFILE_1, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Profile 2"), ISKU_KEY_TYPE_PROFILE_2, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Profile 3"), ISKU_KEY_TYPE_PROFILE_3, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Profile 4"), ISKU_KEY_TYPE_PROFILE_4, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Profile 5"), ISKU_KEY_TYPE_PROFILE_5, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
}

static void key_store_add_multimedia(GtkTreeStore *store) {
	GtkTreeIter toplevel;

	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("Multimedia"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Open player"), ISKU_KEY_TYPE_MULTIMEDIA_OPEN_PLAYER, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Previous track"), ISKU_KEY_TYPE_MULTIMEDIA_PREV_TRACK, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Next track"), ISKU_KEY_TYPE_MULTIMEDIA_NEXT_TRACK, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Play/Pause"), ISKU_KEY_TYPE_MULTIMEDIA_PLAY_PAUSE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Stop"), ISKU_KEY_TYPE_MULTIMEDIA_STOP, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Mute"), ISKU_KEY_TYPE_MULTIMEDIA_MUTE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Volume up"), ISKU_KEY_TYPE_MULTIMEDIA_VOLUME_UP, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Volume down"), ISKU_KEY_TYPE_MULTIMEDIA_VOLUME_DOWN, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
}

static void key_store_add_system(GtkTreeStore *store) {
	GtkTreeIter toplevel;

	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("System"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Shut down"), ISKU_KEY_TYPE_SYSTEM_SHUT_DOWN, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Sleep"), ISKU_KEY_TYPE_SYSTEM_SLEEP, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Wake"), ISKU_KEY_TYPE_SYSTEM_WAKE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
}

static void key_store_add_timer(GtkTreeStore *store) {
	GtkTreeIter toplevel;

	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("Timer"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Timer start"), ISKU_KEY_TYPE_TIMER, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Timer stop"), ISKU_KEY_TYPE_TIMER_STOP, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
}

static void key_store_add_www(GtkTreeStore *store) {
	GtkTreeIter toplevel;

	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("WWW"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Search"), ISKU_KEY_TYPE_WWW_SEARCH, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Home"), ISKU_KEY_TYPE_WWW_HOME, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Back"), ISKU_KEY_TYPE_WWW_BACK, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Forward"), ISKU_KEY_TYPE_WWW_FORWARD, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Stop"), ISKU_KEY_TYPE_WWW_STOP, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Refresh"), ISKU_KEY_TYPE_WWW_REFRESH, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Favorites"), ISKU_KEY_TYPE_WWW_FAVORITES, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
}

static void key_store_add_apps(GtkTreeStore *store) {
	GtkTreeIter toplevel;

	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("Applications"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Quicklaunch"), ISKU_KEY_TYPE_QUICKLAUNCH, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Open driver"), ISKU_KEY_TYPE_OPEN_DRIVER, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("E-Mail"), ISKU_KEY_TYPE_APP_EMAIL, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Calculator"), ISKU_KEY_TYPE_APP_CALCULATOR, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("My computer"), ISKU_KEY_TYPE_APP_MY_COMPUTER, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Webbrowser"), ISKU_KEY_TYPE_APP_IE_BROWSER, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
}

static void key_store_add_easyshift(GtkTreeStore *store) {
	GtkTreeIter iter, toplevel;

	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("Easyshift"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	roccat_key_combo_box_store_append(store, &iter, &toplevel, _("Self"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &iter, _("Self Easyshift"), ISKU_KEY_TYPE_EASYSHIFT, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	roccat_key_combo_box_store_append(store, &iter, &toplevel, _("Other"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &iter, _("Other Easyshift"), ISKU_KEY_TYPE_TALK_OTHER_EASYSHIFT, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &iter, _("Other Easyshift lock"), ISKU_KEY_TYPE_TALK_OTHER_EASYSHIFT_LOCK, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &iter, _("Other Easyaim Setting 1"), ISKU_KEY_TYPE_TALK_OTHER_EASYAIM_1, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &iter, _("Other Easyaim Setting 2"), ISKU_KEY_TYPE_TALK_OTHER_EASYAIM_2, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &iter, _("Other Easyaim Setting 3"), ISKU_KEY_TYPE_TALK_OTHER_EASYAIM_3, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &iter, _("Other Easyaim Setting 4"), ISKU_KEY_TYPE_TALK_OTHER_EASYAIM_4, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &iter, _("Other Easyaim Setting 5"), ISKU_KEY_TYPE_TALK_OTHER_EASYAIM_5, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	roccat_key_combo_box_store_append(store, &iter, &toplevel, _("Both"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &iter, _("Both Easyshift"), ISKU_KEY_TYPE_TALK_BOTH_EASYSHIFT, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
}

static void key_store_add_macro(GtkTreeStore *store) {
	roccat_key_combo_box_store_append(store, NULL, NULL, _("Shortcut"), ISKU_KEY_TYPE_SHORTCUT, ROCCAT_KEY_COMBO_BOX_GROUP_SHORTCUT);
	roccat_key_combo_box_store_append(store, NULL, NULL, _("Macro"), ISKU_KEY_TYPE_MACRO, ROCCAT_KEY_COMBO_BOX_GROUP_MACRO);
	roccat_key_combo_box_store_append(store, NULL, NULL, _("Macro play/pause"), ISKU_KEY_TYPE_MACRO_PLAY_PAUSE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
}

static void key_store_add_misc(GtkTreeStore *store) {
	roccat_key_combo_box_store_append(store, NULL, NULL, _("Disabled"), ISKU_KEY_TYPE_DISABLED, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, NULL, _("Standard"), ISKU_KEY_TYPE_STANDARD, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
}

static GtkTreeModel *button_info_model_new(void) {
	GtkTreeStore *store;
	
	store = roccat_key_combo_box_store_new();

	key_store_add_profile(store);
	key_store_add_multimedia(store);
	key_store_add_system(store);
	key_store_add_timer(store);
	key_store_add_www(store);
	key_store_add_apps(store);
	key_store_add_easyshift(store);
	key_store_add_macro(store);
	key_store_add_misc(store);
	
	return GTK_TREE_MODEL(store);
}

static void finalize(GObject *object) {
	IskuKeyComboBoxPrivate *priv = ISKU_KEY_COMBO_BOX(object)->priv;
	isku_rkp_macro_key_info_free(priv->key_info);
	G_OBJECT_CLASS(isku_key_combo_box_parent_class)->finalize(object);
}

static void isku_key_combo_box_class_init(IskuKeyComboBoxClass *klass) {
	GObjectClass *gobject_class = (GObjectClass*)klass;

	gobject_class->finalize = finalize;
	g_type_class_add_private(klass, sizeof(IskuKeyComboBoxPrivate));
	klass->model = button_info_model_new();
}

void isku_key_combo_box_set_value_blocked(IskuKeyComboBox *key_combo_box, IskuRkpMacroKeyInfo const *key_info) {
	IskuKeyComboBoxPrivate *priv = key_combo_box->priv;

	isku_rkp_macro_key_info_free(priv->key_info);

	if (key_info->type == ISKU_KEY_TYPE_STANDARD)
		priv->key_info = isku_rkp_macro_key_info_new_special(priv->standard);
	else
		priv->key_info = isku_rkp_macro_key_info_dup(key_info);

	set_text(key_combo_box);
}

IskuRkpMacroKeyInfo *isku_key_combo_box_get_value(IskuKeyComboBox *key_combo_box) {
	return isku_rkp_macro_key_info_dup(key_combo_box->priv->key_info);
}
