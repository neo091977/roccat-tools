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
#include "kovaplusconfig_key_combo_box.h"
#include "kovaplus.h"
#include "roccat_button.h"
#include "roccat_shortcut_dialog.h"
#include "roccat_timer_dialog.h"
#include "roccat_warning_dialog.h"
#include "roccat_helper.h"
#include "gtk_roccat_helper.h"
#include "i18n.h"

#define KOVAPLUSCONFIG_KEY_COMBO_BOX_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), KOVAPLUSCONFIG_KEY_COMBO_BOX_TYPE, KovaplusconfigKeyComboBoxClass))
#define IS_KOVAPLUSCONFIG_KEY_COMBO_BOX_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), KOVAPLUSCONFIG_KEY_COMBO_BOX_TYPE))
#define KOVAPLUSCONFIG_KEY_COMBO_BOX_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), KOVAPLUSCONFIG_KEY_COMBO_BOX_TYPE, KovaplusconfigKeyComboBoxClass))
#define KOVAPLUSCONFIG_KEY_COMBO_BOX_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), KOVAPLUSCONFIG_KEY_COMBO_BOX_TYPE, KovaplusconfigKeyComboBoxPrivate))

typedef struct _KovaplusconfigKeyComboBoxClass KovaplusconfigKeyComboBoxClass;
typedef struct _KovaplusconfigKeyComboBoxPrivate KovaplusconfigKeyComboBoxPrivate;

struct _KovaplusconfigKeyComboBox {
	RoccatKeyComboBox parent;
	KovaplusconfigKeyComboBoxPrivate *priv;
};

struct _KovaplusconfigKeyComboBoxClass {
	RoccatKeyComboBoxClass parent_class;
	GtkTreeModel *model;
};

struct _KovaplusconfigKeyComboBoxPrivate {
	KovaplusRmpMacroKeyInfo *key_info;
	gulong changed_handler_id;
};

G_DEFINE_TYPE(KovaplusconfigKeyComboBox, kovaplusconfig_key_combo_box, ROCCAT_KEY_COMBO_BOX_TYPE);

static void set_text(KovaplusconfigKeyComboBox *key_combo_box) {
	KovaplusconfigKeyComboBoxPrivate *priv = key_combo_box->priv;
	gchar *text;
	gchar *temp;

	switch (priv->key_info->type) {
	case KOVAPLUS_PROFILE_BUTTON_TYPE_UNUSED:
		text = g_strdup(_("Disabled"));
		break;
	case KOVAPLUS_PROFILE_BUTTON_TYPE_SHORTCUT:
		temp = roccat_shortcut_dialog_create_name(priv->key_info->keystrokes[0].action, priv->key_info->keystrokes[0].key);
		text = g_strdup_printf(_("Shortcut: %s"), temp);
		g_free(temp);
		break;
	case KOVAPLUS_PROFILE_BUTTON_TYPE_MACRO:
		text = g_strdup_printf(_("Macro: %s/%s"), (gchar const *)priv->key_info->macroset_name, (gchar const *)priv->key_info->macro_name);
		break;
	case KOVAPLUS_PROFILE_BUTTON_TYPE_QUICKLAUNCH:
		temp = g_path_get_basename((gchar const *)priv->key_info->filename);
		text = g_strdup_printf(_("Quicklaunch: %s"), temp);
		g_free(temp);
		break;
	case KOVAPLUS_PROFILE_BUTTON_TYPE_TIMER:
		text = g_strdup_printf(_("Timer: %s"), (gchar const *)priv->key_info->timer_name);
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

static KovaplusRmpMacroKeyInfo *button_quicklaunch(GtkWindow *parent) {
	GtkWidget *dialog;
	gchar *filename;
	KovaplusRmpMacroKeyInfo *key_info = NULL;

	dialog = gtk_file_chooser_dialog_new(_("Select executable"),
			parent,
			GTK_FILE_CHOOSER_ACTION_OPEN,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
			NULL
	);

	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
		filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
		key_info = kovaplus_rmp_macro_key_info_new_quicklaunch(filename);
		g_free(filename);
	}
	gtk_widget_destroy(dialog);
	return key_info;
}

static KovaplusRmpMacroKeyInfo *button_macro(GtkWindow *parent) {
	GaminggearMacro *gaminggear_macro;
	KovaplusRmpMacroKeyInfo *key_info = NULL;
	GError *local_error = NULL;

	gaminggear_macro = gaminggear_macro_dialog(parent);
	if (!gaminggear_macro)
		return NULL;

	key_info = gaminggear_macro_to_kovaplus_rmp_macro_key_info(gaminggear_macro, &local_error);
	gaminggear_macro_free(gaminggear_macro);
	if (!key_info)
		roccat_handle_error_dialog(parent, _("Could not convert macro"), &local_error);

	return key_info;
}

static KovaplusRmpMacroKeyInfo *button_shortcut(GtkWindow *parent) {
	RoccatShortcutDialog *dialog;
	KovaplusRmpMacroKeyInfo *key_info = NULL;

	dialog = ROCCAT_SHORTCUT_DIALOG(roccat_shortcut_dialog_new(parent));
	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
		key_info = kovaplus_rmp_macro_key_info_new_shortcut(
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

static KovaplusRmpMacroKeyInfo *button_timer(GtkWindow *parent) {
	KovaplusRmpMacroKeyInfo *key_info = NULL;
	RoccatTimer *timer;

	timer = roccat_timer_dialog(parent);

	if (timer) {
		key_info = kovaplus_rmp_macro_key_info_new_timer((gchar const *)timer->name, roccat_timer_get_seconds(timer));
		roccat_timer_free(timer);
	}

	return key_info;
}

static void button_changed_cb(GtkComboBox *combo, gpointer user_data) {
	KovaplusconfigKeyComboBox *key_combo_box = KOVAPLUSCONFIG_KEY_COMBO_BOX(combo);
	KovaplusconfigKeyComboBoxPrivate *priv = key_combo_box->priv;
	GtkWindow *parent = gtk_roccat_widget_get_gtk_window(GTK_WIDGET(combo));
	gint type_data;
	KovaplusRmpMacroKeyInfo *new_key_info;

	type_data = roccat_key_combo_box_get_active_type(ROCCAT_KEY_COMBO_BOX(combo));
	if (type_data == ROCCAT_KEY_COMBO_BOX_TYPE_NONE)
		return;

	switch (type_data) {
	case KOVAPLUS_PROFILE_BUTTON_TYPE_MACRO:
		new_key_info = button_macro(parent);
		break;
	case KOVAPLUS_PROFILE_BUTTON_TYPE_SHORTCUT:
		new_key_info = button_shortcut(parent);
		break;
	case KOVAPLUS_PROFILE_BUTTON_TYPE_QUICKLAUNCH:
		new_key_info = button_quicklaunch(parent);
		break;
	case KOVAPLUS_PROFILE_BUTTON_TYPE_TIMER:
		new_key_info = button_timer(parent);
		break;
	default:
		new_key_info = kovaplus_rmp_macro_key_info_new_special(type_data);
	}

	if (new_key_info) {
		g_free(priv->key_info);
		priv->key_info = new_key_info;
	}
	set_text(key_combo_box);
}

GtkWidget *kovaplusconfig_key_combo_box_new(guint mask) {
	KovaplusconfigKeyComboBox *key_combo_box;
	KovaplusconfigKeyComboBoxClass *key_combo_box_class;
	KovaplusconfigKeyComboBoxPrivate *priv;

	key_combo_box = KOVAPLUSCONFIG_KEY_COMBO_BOX(g_object_new(KOVAPLUSCONFIG_KEY_COMBO_BOX_TYPE,
			"exclude-mask", mask,
			NULL));

	priv = key_combo_box->priv;

	key_combo_box_class = KOVAPLUSCONFIG_KEY_COMBO_BOX_GET_CLASS(key_combo_box);
	roccat_key_combo_box_set_model(ROCCAT_KEY_COMBO_BOX(key_combo_box), key_combo_box_class->model);

	priv->changed_handler_id = g_signal_connect(G_OBJECT(key_combo_box), "changed", G_CALLBACK(button_changed_cb), NULL);

	return GTK_WIDGET(key_combo_box);
}

static void kovaplusconfig_key_combo_box_init(KovaplusconfigKeyComboBox *key_combo_box) {
	KovaplusconfigKeyComboBoxPrivate *priv = KOVAPLUSCONFIG_KEY_COMBO_BOX_GET_PRIVATE(key_combo_box);
	key_combo_box->priv = priv;
}

static GtkTreeModel *button_info_model_new(void) {
	GtkTreeStore *store;
	GtkTreeIter toplevel;

	store = roccat_key_combo_box_store_new();

	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("Buttons"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Button 1 (Click)"), KOVAPLUS_PROFILE_BUTTON_TYPE_CLICK, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Button 2 (Universal scrolling)"), KOVAPLUS_PROFILE_BUTTON_TYPE_UNIVERSAL_SCROLLING, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Button 3 (Menu)"), KOVAPLUS_PROFILE_BUTTON_TYPE_MENU, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Button 4 (Scroll up)"), KOVAPLUS_PROFILE_BUTTON_TYPE_SCROLL_UP, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Button 5 (Scroll down)"), KOVAPLUS_PROFILE_BUTTON_TYPE_SCROLL_DOWN, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Button 6 (Tilt left)"), KOVAPLUS_PROFILE_BUTTON_TYPE_TILT_LEFT, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Button 7 (Tilt right)"), KOVAPLUS_PROFILE_BUTTON_TYPE_TILT_RIGHT, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Button 8 (Browser backward)"), KOVAPLUS_PROFILE_BUTTON_TYPE_BROWSER_BACKWARD, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Button 9 (Browser forward)"), KOVAPLUS_PROFILE_BUTTON_TYPE_BROWSER_FORWARD, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Double click"), KOVAPLUS_PROFILE_BUTTON_TYPE_DOUBLE_CLICK, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("Multimedia"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Open player"), KOVAPLUS_PROFILE_BUTTON_TYPE_MULTIMEDIA_OPEN_PLAYER, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Previous track"), KOVAPLUS_PROFILE_BUTTON_TYPE_MULTIMEDIA_PREV_TRACK, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Next track"), KOVAPLUS_PROFILE_BUTTON_TYPE_MULTIMEDIA_NEXT_TRACK, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Play/Pause"), KOVAPLUS_PROFILE_BUTTON_TYPE_MULTIMEDIA_PLAY_PAUSE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Stop"), KOVAPLUS_PROFILE_BUTTON_TYPE_MULTIMEDIA_STOP, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Mute"), KOVAPLUS_PROFILE_BUTTON_TYPE_MULTIMEDIA_MUTE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Volume up"), KOVAPLUS_PROFILE_BUTTON_TYPE_MULTIMEDIA_VOLUME_UP, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Volume down"), KOVAPLUS_PROFILE_BUTTON_TYPE_MULTIMEDIA_VOLUME_DOWN, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("Easywheel"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT);

	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Sensitivity"), KOVAPLUS_PROFILE_BUTTON_TYPE_EASYWHEEL_SENSITIVITY, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Profile"), KOVAPLUS_PROFILE_BUTTON_TYPE_EASYWHEEL_PROFILE, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("CPI"), KOVAPLUS_PROFILE_BUTTON_TYPE_EASYWHEEL_CPI, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Volume"), KOVAPLUS_PROFILE_BUTTON_TYPE_EASYWHEEL_VOLUME, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Alt Tab"), KOVAPLUS_PROFILE_BUTTON_TYPE_EASYWHEEL_ALT_TAB, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Aero Flip 3D"), KOVAPLUS_PROFILE_BUTTON_TYPE_EASYWHEEL_AERO_FLIP_3D, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT);

	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("Profile"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Profile cycle"), KOVAPLUS_PROFILE_BUTTON_TYPE_PROFILE_CYCLE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Profile up"), KOVAPLUS_PROFILE_BUTTON_TYPE_PROFILE_UP, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Profile down"), KOVAPLUS_PROFILE_BUTTON_TYPE_PROFILE_DOWN, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("CPI"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("CPI cycle"), KOVAPLUS_PROFILE_BUTTON_TYPE_CPI_CYCLE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("CPI up"), KOVAPLUS_PROFILE_BUTTON_TYPE_CPI_UP, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("CPI down"), KOVAPLUS_PROFILE_BUTTON_TYPE_CPI_DOWN, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("Sensitivity"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Sensitivity cycle"), KOVAPLUS_PROFILE_BUTTON_TYPE_SENSITIVITY_CYCLE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Sensitivity up"), KOVAPLUS_PROFILE_BUTTON_TYPE_SENSITIVITY_UP, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Sensitivity down"), KOVAPLUS_PROFILE_BUTTON_TYPE_SENSITIVITY_DOWN, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	roccat_key_combo_box_store_append(store, NULL, NULL, _("Shortcut"), KOVAPLUS_PROFILE_BUTTON_TYPE_SHORTCUT, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, NULL, _("Macro"), KOVAPLUS_PROFILE_BUTTON_TYPE_MACRO, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, NULL, _("Quicklaunch"), KOVAPLUS_PROFILE_BUTTON_TYPE_QUICKLAUNCH, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, NULL, _("Windows key"), KOVAPLUS_PROFILE_BUTTON_TYPE_WINDOWS_KEY, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, NULL, _("Easyshift"), KOVAPLUS_PROFILE_BUTTON_TYPE_EASYSHIFT, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT);

	roccat_key_combo_box_store_append(store, NULL, NULL, _("Disabled"), KOVAPLUS_PROFILE_BUTTON_TYPE_DISABLED, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	roccat_key_combo_box_store_append(store, NULL, NULL, _("Timer"), KOVAPLUS_PROFILE_BUTTON_TYPE_TIMER, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, NULL, _("Timer stop"), KOVAPLUS_PROFILE_BUTTON_TYPE_TIMER_STOP, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	return GTK_TREE_MODEL(store);
}

static void finalize(GObject *object) {
	KovaplusconfigKeyComboBoxPrivate *priv = KOVAPLUSCONFIG_KEY_COMBO_BOX(object)->priv;
	g_free(priv->key_info);
	G_OBJECT_CLASS(kovaplusconfig_key_combo_box_parent_class)->finalize(object);
}

static void kovaplusconfig_key_combo_box_class_init(KovaplusconfigKeyComboBoxClass *klass) {
	GObjectClass *gobject_class = (GObjectClass*)klass;

	gobject_class->finalize = finalize;
	g_type_class_add_private(klass, sizeof(KovaplusconfigKeyComboBoxPrivate));
	klass->model = button_info_model_new();
}

void kovaplusconfig_key_combo_box_set_value_blocked(KovaplusconfigKeyComboBox *key_combo_box, KovaplusRmpMacroKeyInfo const *key_info) {
	KovaplusconfigKeyComboBoxPrivate *priv = key_combo_box->priv;

	g_free(priv->key_info);
	priv->key_info = kovaplus_rmp_macro_key_info_dup(key_info);

	set_text(key_combo_box);
}

KovaplusRmpMacroKeyInfo *kovaplusconfig_key_combo_box_get_value(KovaplusconfigKeyComboBox *key_combo_box) {
	return kovaplus_rmp_macro_key_info_dup(key_combo_box->priv->key_info);
}
