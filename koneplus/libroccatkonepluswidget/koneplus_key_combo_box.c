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
#include <gaminggear/key_translations.h>
#include <gaminggear/gdk_key_translations.h>
#include "koneplus_key_combo_box.h"
#include "roccat_talk_target_dialog.h"
#include "roccat_shortcut_dialog.h"
#include "roccat_timer_dialog.h"
#include "roccat_warning_dialog.h"
#include "roccat_helper.h"
#include "roccat_talk.h"
#include "gtk_roccat_helper.h"
#include "i18n-lib.h"

#define KONEPLUS_KEY_COMBO_BOX_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), KONEPLUS_KEY_COMBO_BOX_TYPE, KoneplusKeyComboBoxClass))
#define IS_KONEPLUS_KEY_COMBO_BOX_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), KONEPLUS_KEY_COMBO_BOX_TYPE))
#define KONEPLUS_KEY_COMBO_BOX_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), KONEPLUS_KEY_COMBO_BOX_TYPE, KoneplusKeyComboBoxClass))
#define KONEPLUS_KEY_COMBO_BOX_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), KONEPLUS_KEY_COMBO_BOX_TYPE, KoneplusKeyComboBoxPrivate))

typedef struct _KoneplusKeyComboBoxClass KoneplusKeyComboBoxClass;
typedef struct _KoneplusKeyComboBoxPrivate KoneplusKeyComboBoxPrivate;

struct _KoneplusKeyComboBox {
	RoccatKeyComboBox parent;
	KoneplusKeyComboBoxPrivate *priv;
};

struct _KoneplusKeyComboBoxClass {
	RoccatKeyComboBoxClass parent_class;
	GtkTreeModel *model;
};

struct _KoneplusKeyComboBoxPrivate {
	KoneplusRmpMacroKeyInfo *key_info;
	gulong changed_handler_id;
};

G_DEFINE_TYPE(KoneplusKeyComboBox, koneplus_key_combo_box, ROCCAT_KEY_COMBO_BOX_TYPE);

static void set_text(KoneplusKeyComboBox *key_combo_box) {
	KoneplusKeyComboBoxPrivate *priv = key_combo_box->priv;
	gchar *text;
	gchar *temp;

	switch (priv->key_info->type) {
	case KONEPLUS_PROFILE_BUTTON_TYPE_SHORTCUT:
		temp = roccat_shortcut_dialog_create_name(priv->key_info->keystrokes[0].action, priv->key_info->keystrokes[0].key);
		text = g_strdup_printf(_("Shortcut: %s"), temp);
		g_free(temp);
		break;
	case KONEPLUS_PROFILE_BUTTON_TYPE_MACRO:
		text = g_strdup_printf(_("Macro: %s/%s"), (gchar const *)priv->key_info->macroset_name, (gchar const *)priv->key_info->macro_name);
		break;
	case KONEPLUS_PROFILE_BUTTON_TYPE_QUICKLAUNCH:
		temp = g_path_get_basename((gchar const *)priv->key_info->filename);
		text = g_strdup_printf(_("Quicklaunch: %s"), temp);
		g_free(temp);
		break;
	case KONEPLUS_PROFILE_BUTTON_TYPE_TIMER:
		text = g_strdup_printf(_("Timer: %s"), (gchar const *)priv->key_info->timer_name);
		break;
	case KONEPLUS_PROFILE_BUTTON_TYPE_TALK_OTHER_EASYSHIFT:
		text = g_strdup_printf(_("Easyshift: %s"), roccat_talk_device_get_text_static(koneplus_rmp_macro_key_info_get_talk_device(priv->key_info)));
		break;
	case KONEPLUS_PROFILE_BUTTON_TYPE_TALK_OTHER_EASYSHIFT_LOCK:
		text = g_strdup_printf(_("Easyshift lock: %s"), roccat_talk_device_get_text_static(koneplus_rmp_macro_key_info_get_talk_device(priv->key_info)));
		break;
	default:
		text = roccat_key_combo_box_model_get_title_for_type(gtk_combo_box_get_model(GTK_COMBO_BOX(key_combo_box)), priv->key_info->type);
		break;
	}

	g_signal_handler_block(G_OBJECT(key_combo_box), priv->changed_handler_id);
	roccat_key_combo_box_set_text(ROCCAT_KEY_COMBO_BOX(key_combo_box), text);
	g_signal_handler_unblock(G_OBJECT(key_combo_box), priv->changed_handler_id);

	g_free(text);
}

static KoneplusRmpMacroKeyInfo *button_quicklaunch(GtkWindow *parent) {
	GtkWidget *dialog;
	gchar *filename;
	KoneplusRmpMacroKeyInfo *key_info = NULL;

	dialog = gtk_file_chooser_dialog_new(_("Select executable"),
			parent,
			GTK_FILE_CHOOSER_ACTION_OPEN,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
			NULL
	);

	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
		filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
		key_info = koneplus_rmp_macro_key_info_new_quicklaunch(filename);
		g_free(filename);
	}
	gtk_widget_destroy(dialog);
	return key_info;
}

static KoneplusRmpMacroKeyInfo *button_macro(GtkWindow *parent) {
	GaminggearMacro *gaminggear_macro;
	KoneplusRmpMacroKeyInfo *key_info = NULL;
	GError *local_error = NULL;

	gaminggear_macro = gaminggear_macro_dialog(parent);
	if (!gaminggear_macro)
		return NULL;

	key_info = gaminggear_macro_to_koneplus_rmp_macro_key_info(gaminggear_macro, &local_error);
	gaminggear_macro_free(gaminggear_macro);
	if (!key_info)
		roccat_handle_error_dialog(parent, _("Could not convert macro"), &local_error);

	return key_info;
}

static KoneplusRmpMacroKeyInfo *button_shortcut(GtkWindow *parent) {
	RoccatShortcutDialog *dialog;
	KoneplusRmpMacroKeyInfo *key_info = NULL;

	dialog = ROCCAT_SHORTCUT_DIALOG(roccat_shortcut_dialog_new(parent));
	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
		key_info = koneplus_rmp_macro_key_info_new_shortcut(
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

static KoneplusRmpMacroKeyInfo *button_timer(GtkWindow *parent) {
	KoneplusRmpMacroKeyInfo *key_info = NULL;
	RoccatTimer *timer;

	timer = roccat_timer_dialog(parent);

	if (timer) {
		key_info = koneplus_rmp_macro_key_info_new_timer((gchar const *)timer->name, roccat_timer_get_seconds(timer));
		roccat_timer_free(timer);
	}

	return key_info;
}

static KoneplusRmpMacroKeyInfo *button_talk_other(KoneplusKeyComboBox *key_combo_box, gint key_type) {
	KoneplusKeyComboBoxPrivate *priv = key_combo_box->priv;
	RoccatTalkTargetDialog *talk_target_dialog;
	guint target;
	KoneplusRmpMacroKeyInfo *new_key_info = NULL;

	talk_target_dialog = ROCCAT_TALK_TARGET_DIALOG(roccat_talk_target_dialog_new(
			gtk_roccat_widget_get_gtk_window(GTK_WIDGET(key_combo_box)), ROCCAT_TALK_DEVICE_MOUSE));

	roccat_talk_target_dialog_set_value(talk_target_dialog, koneplus_rmp_macro_key_info_get_talk_device(priv->key_info));

	if (gtk_dialog_run(GTK_DIALOG(talk_target_dialog)) == GTK_RESPONSE_ACCEPT) {
		target = roccat_talk_target_dialog_get_value(talk_target_dialog);
		new_key_info = koneplus_rmp_macro_key_info_new_talk(key_type, target);
	}

	gtk_widget_destroy(GTK_WIDGET(talk_target_dialog));

	return new_key_info;
}

static KoneplusRmpMacroKeyInfo *get_macro_key_info(KoneplusKeyComboBox *key_combo_box, gint type) {
	GtkWindow *parent = gtk_roccat_widget_get_gtk_window(GTK_WIDGET(key_combo_box));
	KoneplusRmpMacroKeyInfo *key_info;

	switch (type) {
	case KONEPLUS_PROFILE_BUTTON_TYPE_MACRO:
		key_info = button_macro(parent);
		break;
	case KONEPLUS_PROFILE_BUTTON_TYPE_SHORTCUT:
		key_info = button_shortcut(parent);
		break;
	case KONEPLUS_PROFILE_BUTTON_TYPE_QUICKLAUNCH:
		key_info = button_quicklaunch(parent);
		break;
	case KONEPLUS_PROFILE_BUTTON_TYPE_TIMER:
		key_info = button_timer(parent);
		break;
	case KONEPLUS_PROFILE_BUTTON_TYPE_TALK_OTHER_EASYSHIFT:
	case KONEPLUS_PROFILE_BUTTON_TYPE_TALK_OTHER_EASYSHIFT_LOCK:
		key_info = button_talk_other(key_combo_box, type);
		break;
	case KONEPLUS_PROFILE_BUTTON_TYPE_TALK_BOTH_EASYSHIFT:
		key_info = koneplus_rmp_macro_key_info_new_talk(type, ROCCAT_TALK_DEVICE_ALL);
		break;
	default:
		key_info = koneplus_rmp_macro_key_info_new_special(type);
	}

	return key_info;
}

static void button_changed_cb(GtkComboBox *combo, gpointer user_data) {
	KoneplusKeyComboBox *key_combo_box = KONEPLUS_KEY_COMBO_BOX(combo);
	KoneplusKeyComboBoxPrivate *priv = key_combo_box->priv;
	gint type_data;
	KoneplusRmpMacroKeyInfo *new_key_info;

	type_data = roccat_key_combo_box_get_active_type(ROCCAT_KEY_COMBO_BOX(combo));
	if (type_data == ROCCAT_KEY_COMBO_BOX_TYPE_NONE)
		return;

	new_key_info = get_macro_key_info(key_combo_box, type_data);
	if (new_key_info) {
		koneplus_rmp_macro_key_info_free(priv->key_info);
		priv->key_info = new_key_info;
	}
	set_text(key_combo_box);
}

GtkWidget *koneplus_key_combo_box_new(guint mask) {
	KoneplusKeyComboBox *key_combo_box;
	KoneplusKeyComboBoxClass *key_combo_box_class;
	KoneplusKeyComboBoxPrivate *priv;

	key_combo_box = KONEPLUS_KEY_COMBO_BOX(g_object_new(KONEPLUS_KEY_COMBO_BOX_TYPE,
			"exclude-mask", mask,
			NULL));

	priv = key_combo_box->priv;

	key_combo_box_class = KONEPLUS_KEY_COMBO_BOX_GET_CLASS(key_combo_box);
	roccat_key_combo_box_set_model(ROCCAT_KEY_COMBO_BOX(key_combo_box), key_combo_box_class->model);

	priv->changed_handler_id = g_signal_connect(G_OBJECT(key_combo_box), "changed", G_CALLBACK(button_changed_cb), NULL);

	return GTK_WIDGET(key_combo_box);
}

static void koneplus_key_combo_box_init(KoneplusKeyComboBox *key_combo_box) {
	KoneplusKeyComboBoxPrivate *priv = KONEPLUS_KEY_COMBO_BOX_GET_PRIVATE(key_combo_box);
	key_combo_box->priv = priv;
}

static GtkTreeModel *button_info_model_new(void) {
	GtkTreeStore *store;
	GtkTreeIter toplevel, sublevel;

	store = roccat_key_combo_box_store_new();

	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("Buttons"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Button 1 (Click)"), KONEPLUS_PROFILE_BUTTON_TYPE_CLICK, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Button 2 (Universal scrolling)"), KONEPLUS_PROFILE_BUTTON_TYPE_UNIVERSAL_SCROLLING, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Button 3 (Menu)"), KONEPLUS_PROFILE_BUTTON_TYPE_MENU, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Button 4 (Scroll up)"), KONEPLUS_PROFILE_BUTTON_TYPE_SCROLL_UP, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Button 5 (Scroll down)"), KONEPLUS_PROFILE_BUTTON_TYPE_SCROLL_DOWN, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Button 6 (Tilt left)"), KONEPLUS_PROFILE_BUTTON_TYPE_TILT_LEFT, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Button 7 (Tilt right)"), KONEPLUS_PROFILE_BUTTON_TYPE_TILT_RIGHT, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Button 8 (Browser backward)"), KONEPLUS_PROFILE_BUTTON_TYPE_BROWSER_BACKWARD, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Button 9 (Browser forward)"), KONEPLUS_PROFILE_BUTTON_TYPE_BROWSER_FORWARD, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Double click"), KONEPLUS_PROFILE_BUTTON_TYPE_DOUBLE_CLICK, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("CPI"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("CPI cycle"), KONEPLUS_PROFILE_BUTTON_TYPE_CPI_CYCLE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("CPI up"), KONEPLUS_PROFILE_BUTTON_TYPE_CPI_UP, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("CPI down"), KONEPLUS_PROFILE_BUTTON_TYPE_CPI_DOWN, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("Multimedia"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Open player"), KONEPLUS_PROFILE_BUTTON_TYPE_MULTIMEDIA_OPEN_PLAYER, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Previous track"), KONEPLUS_PROFILE_BUTTON_TYPE_MULTIMEDIA_PREV_TRACK, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Next track"), KONEPLUS_PROFILE_BUTTON_TYPE_MULTIMEDIA_NEXT_TRACK, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Play/Pause"), KONEPLUS_PROFILE_BUTTON_TYPE_MULTIMEDIA_PLAY_PAUSE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Stop"), KONEPLUS_PROFILE_BUTTON_TYPE_MULTIMEDIA_STOP, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Mute"), KONEPLUS_PROFILE_BUTTON_TYPE_MULTIMEDIA_MUTE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Volume up"), KONEPLUS_PROFILE_BUTTON_TYPE_MULTIMEDIA_VOLUME_UP, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Volume down"), KONEPLUS_PROFILE_BUTTON_TYPE_MULTIMEDIA_VOLUME_DOWN, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("Profile"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Profile cycle"), KONEPLUS_PROFILE_BUTTON_TYPE_PROFILE_CYCLE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Profile up"), KONEPLUS_PROFILE_BUTTON_TYPE_PROFILE_UP, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Profile down"), KONEPLUS_PROFILE_BUTTON_TYPE_PROFILE_DOWN, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("Sensitivity"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Sensitivity cycle"), KONEPLUS_PROFILE_BUTTON_TYPE_SENSITIVITY_CYCLE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Sensitivity up"), KONEPLUS_PROFILE_BUTTON_TYPE_SENSITIVITY_UP, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Sensitivity down"), KONEPLUS_PROFILE_BUTTON_TYPE_SENSITIVITY_DOWN, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("Easywheel"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Easywheel Sensitivity"), KONEPLUS_PROFILE_BUTTON_TYPE_EASYWHEEL_SENSITIVITY, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Easywheel Profile"), KONEPLUS_PROFILE_BUTTON_TYPE_EASYWHEEL_PROFILE, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Easywheel CPI"), KONEPLUS_PROFILE_BUTTON_TYPE_EASYWHEEL_CPI, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Easywheel Volume"), KONEPLUS_PROFILE_BUTTON_TYPE_EASYWHEEL_VOLUME, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Easywheel Alt Tab"), KONEPLUS_PROFILE_BUTTON_TYPE_EASYWHEEL_ALT_TAB, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Easywheel Aero Flip 3D"), KONEPLUS_PROFILE_BUTTON_TYPE_EASYWHEEL_AERO_FLIP_3D, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT);

	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("Easyaim"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Easyaim Setting 1"), KONEPLUS_PROFILE_BUTTON_TYPE_EASYAIM_1, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Easyaim Setting 2"), KONEPLUS_PROFILE_BUTTON_TYPE_EASYAIM_2, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Easyaim Setting 3"), KONEPLUS_PROFILE_BUTTON_TYPE_EASYAIM_3, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Easyaim Setting 4"), KONEPLUS_PROFILE_BUTTON_TYPE_EASYAIM_4, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Easyaim Setting 5"), KONEPLUS_PROFILE_BUTTON_TYPE_EASYAIM_5, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("Easyshift"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT);
	roccat_key_combo_box_store_append(store, &sublevel, &toplevel, _("Self"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Self Easyshift"), KONEPLUS_PROFILE_BUTTON_TYPE_EASYSHIFT, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT);
	roccat_key_combo_box_store_append(store, &sublevel, &toplevel, _("Other"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Other Easyshift"), KONEPLUS_PROFILE_BUTTON_TYPE_TALK_OTHER_EASYSHIFT, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Other Easyshift lock"), KONEPLUS_PROFILE_BUTTON_TYPE_TALK_OTHER_EASYSHIFT_LOCK, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT);
	roccat_key_combo_box_store_append(store, &sublevel, &toplevel, _("Both"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Both Easyshift"), KONEPLUS_PROFILE_BUTTON_TYPE_TALK_BOTH_EASYSHIFT, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT);

	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("Timer"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Timer start"), KONEPLUS_PROFILE_BUTTON_TYPE_TIMER, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Timer stop"), KONEPLUS_PROFILE_BUTTON_TYPE_TIMER_STOP, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	roccat_key_combo_box_store_append(store, NULL, NULL, _("Shortcut"), KONEPLUS_PROFILE_BUTTON_TYPE_SHORTCUT, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, NULL, _("Macro"), KONEPLUS_PROFILE_BUTTON_TYPE_MACRO, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, NULL, _("Quicklaunch"), KONEPLUS_PROFILE_BUTTON_TYPE_QUICKLAUNCH, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, NULL, _("Windows key"), KONEPLUS_PROFILE_BUTTON_TYPE_WINDOWS_KEY, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, NULL, _("Open driver"), KONEPLUS_PROFILE_BUTTON_TYPE_OPEN_DRIVER, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	roccat_key_combo_box_store_append(store, NULL, NULL, _("Unused"), KONEPLUS_PROFILE_BUTTON_TYPE_UNUSED, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, NULL, _("Disabled"), KONEPLUS_PROFILE_BUTTON_TYPE_DISABLED, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	return GTK_TREE_MODEL(store);
}

static void finalize(GObject *object) {
	KoneplusKeyComboBoxPrivate *priv = KONEPLUS_KEY_COMBO_BOX(object)->priv;
	g_free(priv->key_info);
	G_OBJECT_CLASS(koneplus_key_combo_box_parent_class)->finalize(object);
}

static void koneplus_key_combo_box_class_init(KoneplusKeyComboBoxClass *klass) {
	GObjectClass *gobject_class = (GObjectClass*)klass;

	gobject_class->finalize = finalize;
	g_type_class_add_private(klass, sizeof(KoneplusKeyComboBoxPrivate));
	klass->model = button_info_model_new();
}

void koneplus_key_combo_box_set_value_blocked(KoneplusKeyComboBox *key_combo_box, KoneplusRmpMacroKeyInfo const *key_info) {
	KoneplusKeyComboBoxPrivate *priv = key_combo_box->priv;

	koneplus_rmp_macro_key_info_free(priv->key_info);
	priv->key_info = koneplus_rmp_macro_key_info_dup(key_info);

	set_text(key_combo_box);
}

KoneplusRmpMacroKeyInfo *koneplus_key_combo_box_get_value(KoneplusKeyComboBox *key_combo_box) {
	return koneplus_rmp_macro_key_info_dup(key_combo_box->priv->key_info);
}
