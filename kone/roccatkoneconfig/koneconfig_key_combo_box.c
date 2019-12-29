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
#include "koneconfig_key_combo_box.h"
#include "koneconfig_shortcut_dialog.h"
#include "roccat_warning_dialog.h"
#include "roccat_key_dialog.h"
#include "gtk_roccat_helper.h"
#include "i18n.h"

#define KONECONFIG_KEY_COMBO_BOX_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), KONECONFIG_KEY_COMBO_BOX_TYPE, KoneconfigKeyComboBoxClass))
#define IS_KONECONFIG_KEY_COMBO_BOX_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), KONECONFIG_KEY_COMBO_BOX_TYPE))
#define KONECONFIG_KEY_COMBO_BOX_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), KONECONFIG_KEY_COMBO_BOX_TYPE, KoneconfigKeyComboBoxClass))
#define KONECONFIG_KEY_COMBO_BOX_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), KONECONFIG_KEY_COMBO_BOX_TYPE, KoneconfigKeyComboBoxPrivate))

typedef struct _KoneconfigKeyComboBoxClass KoneconfigKeyComboBoxClass;
typedef struct _KoneconfigKeyComboBoxPrivate KoneconfigKeyComboBoxPrivate;

struct _KoneconfigKeyComboBox {
	RoccatKeyComboBox parent;
	KoneconfigKeyComboBoxPrivate *priv;
};

struct _KoneconfigKeyComboBoxClass {
	RoccatKeyComboBoxClass parent_class;
	GtkTreeModel *model;
};

struct _KoneconfigKeyComboBoxPrivate {
	KoneRMPButtonInfo *key_info;
	gulong changed_handler_id;
};

G_DEFINE_TYPE(KoneconfigKeyComboBox, koneconfig_key_combo_box, ROCCAT_KEY_COMBO_BOX_TYPE);

static gchar *shortcut_create_name(KoneRMPButtonInfo const *button_info) {
	gchar *pre;
	gchar *hid;
	gchar *result;

	switch(button_info->keystrokes[0].action) {
	case KONE_KEYSTROKE_ACTION_NONE:
		pre = "";
		break;
	case KONE_KEYSTROKE_ACTION_SHIFT:
		pre = _("Shift+");
		break;
	case KONE_KEYSTROKE_ACTION_CTRL:
		pre = _("Ctrl+");
		break;
	case KONE_KEYSTROKE_ACTION_ALT:
		pre = _("Alt+");
		break;
	case KONE_KEYSTROKE_ACTION_WIN:
		pre = _("Win+");
		break;
	default:
		g_critical(_("Got unknown action %i"), button_info->keystrokes[0].action);
		pre = "";
	}

	hid = gaminggear_hid_to_name(button_info->keystrokes[0].key);
	result = g_strdup_printf("%s%s", pre, hid);
	g_free(hid);

	return result;
}

static void set_text(KoneconfigKeyComboBox *key_combo_box) {
	KoneconfigKeyComboBoxPrivate *priv = key_combo_box->priv;
	gchar *text;
	gchar *temp;

	switch (priv->key_info->type) {
	case KONE_BUTTON_INFO_TYPE_KEY:
		text = gaminggear_hid_to_name(priv->key_info->keystrokes[0].key);
		break;
	case KONE_BUTTON_INFO_TYPE_MACRO:
		text = g_strdup_printf(_("Macro: %s/%s"), (gchar *)priv->key_info->macro_set_name, (gchar *)priv->key_info->macro_name);
		break;
	case KONE_BUTTON_INFO_TYPE_SHORTCUT:
		temp = shortcut_create_name(priv->key_info);
		text = g_strdup_printf(_("Shortcut: %s"), temp);
		g_free(temp);
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

static KoneRMPButtonInfo *button_key(GtkWindow *parent) {
	RoccatKeyDialog *dialog;
	KoneRMPButtonInfo *key_info = NULL;

	dialog = ROCCAT_KEY_DIALOG(roccat_key_dialog_new(parent));
	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
		key_info = kone_rmp_button_info_new_single_key(roccat_key_dialog_get_hid(dialog));

	gtk_widget_destroy(GTK_WIDGET(dialog));
	return key_info;
}

static void macro_is_too_long_dialog(GtkWindow *parent) {
	roccat_warning_dialog(parent,
			_("Macro is too long to fit in hardware."),
			_("It will be executed by software."));
}

static void macro_has_loop_dialog(GtkWindow *parent) {
	roccat_warning_dialog(parent,
			_("Macro is looped which is not supported by hardware."),
			_("It will be executed by software."));
}

static KoneRMPButtonInfo *button_macro(GtkWindow *parent) {
	GaminggearMacro *gaminggear_macro;
	KoneRMPButtonInfo *key_info = NULL;
	GError *local_error = NULL;

	gaminggear_macro = gaminggear_macro_dialog(parent);
	if (!gaminggear_macro)
		return NULL;

	if (gaminggear_macro_keystrokes_get_count(&gaminggear_macro->keystrokes) > KONE_BUTTON_INFO_KEYSTROKES_NUM)
		macro_is_too_long_dialog(parent);
	else if (gaminggear_macro->keystrokes.loop > 1)
		macro_has_loop_dialog(parent);

	key_info = gaminggear_macro_to_kone_rmp_button_info(gaminggear_macro, &local_error);
	gaminggear_macro_free(gaminggear_macro);
	if (!key_info)
		roccat_handle_error_dialog(parent, _("Could not convert macro"), &local_error);

	return key_info;
}

static KoneRMPButtonInfo *button_shortcut(GtkWindow *parent) {
	KoneconfigShortcutDialog *dialog;
	KoneRMPButtonInfo *key_info = NULL;

	dialog = KONECONFIG_SHORTCUT_DIALOG(koneconfig_shortcut_dialog_new(parent));
	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
		key_info = kone_rmp_button_info_new_shortcut(
				gaminggear_xkeycode_to_hid(koneconfig_shortcut_dialog_get_xkeycode(dialog)),
				koneconfig_shortcut_dialog_get_modifier(dialog));
	}
	gtk_widget_destroy(GTK_WIDGET(dialog));
	return key_info;
}

static void button_changed_cb(GtkComboBox *combo, gpointer user_data) {
	KoneconfigKeyComboBox *key_combo_box = KONECONFIG_KEY_COMBO_BOX(combo);
	KoneconfigKeyComboBoxPrivate *priv = key_combo_box->priv;
	GtkWindow *parent = gtk_roccat_widget_get_gtk_window(GTK_WIDGET(combo));
	guint type_data;
	KoneRMPButtonInfo *new_key_info;

	type_data = roccat_key_combo_box_get_active_type(ROCCAT_KEY_COMBO_BOX(combo));
	if (type_data == ROCCAT_KEY_COMBO_BOX_TYPE_NONE)
		return;

	switch (type_data) {
	case KONE_BUTTON_INFO_TYPE_KEY:
		new_key_info = button_key(parent);
		break;
	case KONE_BUTTON_INFO_TYPE_MACRO:
		new_key_info = button_macro(parent);
		break;
	case KONE_BUTTON_INFO_TYPE_SHORTCUT:
		new_key_info = button_shortcut(parent);
		break;
	default:
		new_key_info = kone_rmp_button_info_new_special(type_data);
	}

	if (new_key_info) {
		g_free(priv->key_info);
		priv->key_info = new_key_info;
	}
	set_text(key_combo_box);
}

GtkWidget *koneconfig_key_combo_box_new(void) {
	KoneconfigKeyComboBox *key_combo_box;
	KoneconfigKeyComboBoxClass *key_combo_box_class;
	KoneconfigKeyComboBoxPrivate *priv;

	key_combo_box = KONECONFIG_KEY_COMBO_BOX(g_object_new(KONECONFIG_KEY_COMBO_BOX_TYPE,
			NULL));

	priv = key_combo_box->priv;

	key_combo_box_class = KONECONFIG_KEY_COMBO_BOX_GET_CLASS(key_combo_box);
	roccat_key_combo_box_set_model(ROCCAT_KEY_COMBO_BOX(key_combo_box), key_combo_box_class->model);

	priv->changed_handler_id = g_signal_connect(G_OBJECT(key_combo_box), "changed", G_CALLBACK(button_changed_cb), NULL);

	return GTK_WIDGET(key_combo_box);
}

static void koneconfig_key_combo_box_init(KoneconfigKeyComboBox *key_combo_box) {
	KoneconfigKeyComboBoxPrivate *priv = KONECONFIG_KEY_COMBO_BOX_GET_PRIVATE(key_combo_box);
	key_combo_box->priv = priv;
}

static GtkTreeModel *button_info_model_new(void) {
	GtkTreeStore *store;
	GtkTreeIter toplevel;

	store = roccat_key_combo_box_store_new();

	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("Buttons"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Button 1 (Click)"), KONE_BUTTON_INFO_TYPE_BUTTON_1, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Button 2 (Universal scrolling)"), KONE_BUTTON_INFO_TYPE_BUTTON_2, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Button 3 (Menu)"), KONE_BUTTON_INFO_TYPE_BUTTON_3, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Button 6 (Tilt left)"), KONE_BUTTON_INFO_TYPE_BUTTON_6, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Button 7 (Tilt right)"), KONE_BUTTON_INFO_TYPE_BUTTON_7, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Button 8 (Browser backward)"), KONE_BUTTON_INFO_TYPE_BUTTON_8, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Button 9 (Browser forward)"), KONE_BUTTON_INFO_TYPE_BUTTON_9, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Double click"), KONE_BUTTON_INFO_TYPE_DOUBLE_CLICK, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("Multimedia"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Open player"), KONE_BUTTON_INFO_TYPE_MULTIMEDIA_OPEN_PLAYER, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Next track"), KONE_BUTTON_INFO_TYPE_MULTIMEDIA_NEXT_TRACK, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Previous track"), KONE_BUTTON_INFO_TYPE_MULTIMEDIA_PREV_TRACK, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Play/Pause"), KONE_BUTTON_INFO_TYPE_MULTIMEDIA_PLAY_PAUSE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Stop"), KONE_BUTTON_INFO_TYPE_MULTIMEDIA_STOP, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Mute"), KONE_BUTTON_INFO_TYPE_MULTIMEDIA_MUTE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Volume up"), KONE_BUTTON_INFO_TYPE_MULTIMEDIA_VOLUME_UP, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Volume down"), KONE_BUTTON_INFO_TYPE_MULTIMEDIA_VOLUME_DOWN, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("OSD"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("OSD XY-Prescaling"), KONE_BUTTON_INFO_TYPE_OSD_XY_PRESCALING, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("OSD CPI"), KONE_BUTTON_INFO_TYPE_OSD_DPI, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("OSD Profile"), KONE_BUTTON_INFO_TYPE_OSD_PROFILE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	roccat_key_combo_box_store_append(store, NULL, NULL, _("Key"), KONE_BUTTON_INFO_TYPE_KEY, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, NULL, _("Shortcut"), KONE_BUTTON_INFO_TYPE_SHORTCUT, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, NULL, _("Macro"), KONE_BUTTON_INFO_TYPE_MACRO, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	roccat_key_combo_box_store_append(store, NULL, NULL, _("CPI up"), KONE_BUTTON_INFO_TYPE_DPI_UP, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, NULL, _("CPI down"), KONE_BUTTON_INFO_TYPE_DPI_DOWN, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, NULL, _("Profile up"), KONE_BUTTON_INFO_TYPE_PROFILE_UP, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, NULL, _("Profile down"), KONE_BUTTON_INFO_TYPE_PROFILE_DOWN, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	roccat_key_combo_box_store_append(store, NULL, NULL, _("Off"), KONE_BUTTON_INFO_TYPE_OFF, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	return GTK_TREE_MODEL(store);
}

static void finalize(GObject *object) {
	KoneconfigKeyComboBoxPrivate *priv = KONECONFIG_KEY_COMBO_BOX(object)->priv;
	g_free(priv->key_info);
	G_OBJECT_CLASS(koneconfig_key_combo_box_parent_class)->finalize(object);
}

static void koneconfig_key_combo_box_class_init(KoneconfigKeyComboBoxClass *klass) {
	GObjectClass *gobject_class = (GObjectClass*)klass;

	gobject_class->finalize = finalize;
	g_type_class_add_private(klass, sizeof(KoneconfigKeyComboBoxPrivate));
	klass->model = button_info_model_new();
}

void koneconfig_key_combo_box_set_value_blocked(KoneconfigKeyComboBox *key_combo_box, KoneRMPButtonInfo const *key_info) {
	KoneconfigKeyComboBoxPrivate *priv = key_combo_box->priv;

	g_free(priv->key_info);
	priv->key_info = kone_rmp_button_info_dup(key_info);

	set_text(key_combo_box);
}

KoneRMPButtonInfo *koneconfig_key_combo_box_get_value(KoneconfigKeyComboBox *key_combo_box) {
	return kone_rmp_button_info_dup(key_combo_box->priv->key_info);
}
