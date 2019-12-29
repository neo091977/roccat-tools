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
#include "pyraconfig_key_combo_box.h"
#include "roccat_shortcut_dialog.h"
#include "roccat_warning_dialog.h"
#include "roccat_helper.h"
#include "gtk_roccat_helper.h"
#include "i18n.h"

#define PYRACONFIG_KEY_COMBO_BOX_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), PYRACONFIG_KEY_COMBO_BOX_TYPE, PyraconfigKeyComboBoxClass))
#define IS_PYRACONFIG_KEY_COMBO_BOX_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), PYRACONFIG_KEY_COMBO_BOX_TYPE))
#define PYRACONFIG_KEY_COMBO_BOX_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), PYRACONFIG_KEY_COMBO_BOX_TYPE, PyraconfigKeyComboBoxClass))
#define PYRACONFIG_KEY_COMBO_BOX_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), PYRACONFIG_KEY_COMBO_BOX_TYPE, PyraconfigKeyComboBoxPrivate))

typedef struct _PyraconfigKeyComboBoxClass PyraconfigKeyComboBoxClass;
typedef struct _PyraconfigKeyComboBoxPrivate PyraconfigKeyComboBoxPrivate;

struct _PyraconfigKeyComboBox {
	RoccatKeyComboBox parent;
	PyraconfigKeyComboBoxPrivate *priv;
};

struct _PyraconfigKeyComboBoxClass {
	RoccatKeyComboBoxClass parent_class;
	GtkTreeModel *model;
};

struct _PyraconfigKeyComboBoxPrivate {
	PyraRMPMacroKeyInfo *key_info;
	gulong changed_handler_id;
};

G_DEFINE_TYPE(PyraconfigKeyComboBox, pyraconfig_key_combo_box, ROCCAT_KEY_COMBO_BOX_TYPE);

static void set_text(PyraconfigKeyComboBox *key_combo_box) {
	PyraconfigKeyComboBoxPrivate *priv = key_combo_box->priv;
	gchar *text;
	gchar *temp;

	switch (priv->key_info->type) {
	case PYRA_RMP_MACRO_KEY_INFO_TYPE_UNUSED:
	case PYRA_RMP_MACRO_KEY_INFO_TYPE_DISABLED2:
		text = g_strdup(_("Disabled"));
		break;
	case PYRA_RMP_MACRO_KEY_INFO_TYPE_SHORTCUT:
		temp = roccat_shortcut_dialog_create_name(priv->key_info->keystrokes[0].action, priv->key_info->keystrokes[0].key);
		text = g_strdup_printf(_("Shortcut: %s"), temp);
		g_free(temp);
		break;
	case PYRA_RMP_MACRO_KEY_INFO_TYPE_MACRO:
		text = g_strdup_printf(_("Macro: %s/%s"), (gchar const *)priv->key_info->macro_set_name, (gchar const *)priv->key_info->macro_name);
		break;
	case PYRA_RMP_MACRO_KEY_INFO_TYPE_QUICKLAUNCH:
		temp = g_path_get_basename((gchar const *)priv->key_info->filename);
		text = g_strdup_printf(_("Quicklaunch: %s"), temp);
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

static PyraRMPMacroKeyInfo *button_quicklaunch(GtkWindow *parent) {
	GtkWidget *dialog;
	gchar *filename;
	PyraRMPMacroKeyInfo *key_info = NULL;

	dialog = gtk_file_chooser_dialog_new(_("Select executable"), parent,
			GTK_FILE_CHOOSER_ACTION_OPEN,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
			NULL);

	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
		filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
		key_info = pyra_rmp_macro_key_info_new_quicklaunch(filename);
		g_free(filename);
	}
	gtk_widget_destroy(dialog);
	return key_info;
}

static PyraRMPMacroKeyInfo *button_macro(GtkWindow *parent) {
	GaminggearMacro *gaminggear_macro;
	PyraRMPMacroKeyInfo *key_info = NULL;
	GError *local_error = NULL;

	gaminggear_macro = gaminggear_macro_dialog(parent);
	if (!gaminggear_macro)
		return NULL;

	key_info = gaminggear_macro_to_pyra_rmp_macro_key_info(gaminggear_macro, &local_error);
	gaminggear_macro_free(gaminggear_macro);
	if (!key_info)
		roccat_handle_error_dialog(parent, _("Could not convert macro"), &local_error);

	return key_info;
}

static PyraRMPMacroKeyInfo *button_shortcut(GtkWindow *parent) {
	RoccatShortcutDialog *dialog;
	PyraRMPMacroKeyInfo *key_info = NULL;

	dialog = ROCCAT_SHORTCUT_DIALOG(roccat_shortcut_dialog_new(parent));
	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
		key_info = pyra_rmp_macro_key_info_new_shortcut(
				roccat_shortcut_dialog_get_hid(dialog),
				roccat_shortcut_dialog_get_ctrl(dialog),
				roccat_shortcut_dialog_get_shift(dialog),
				roccat_shortcut_dialog_get_alt(dialog),
				roccat_shortcut_dialog_get_win(dialog));
	}
	gtk_widget_destroy(GTK_WIDGET(dialog));
	return key_info;
}

static void button_changed_cb(GtkComboBox *combo, gpointer user_data) {
	PyraconfigKeyComboBox *key_combo_box = PYRACONFIG_KEY_COMBO_BOX(combo);
	PyraconfigKeyComboBoxPrivate *priv = key_combo_box->priv;
	GtkWindow *parent = gtk_roccat_widget_get_gtk_window(GTK_WIDGET(combo));
	guint type_data;
	PyraRMPMacroKeyInfo *new_key_info;

	type_data = roccat_key_combo_box_get_active_type(ROCCAT_KEY_COMBO_BOX(combo));
	if (type_data == ROCCAT_KEY_COMBO_BOX_TYPE_NONE)
		return;

	switch (type_data) {
	case PYRA_RMP_MACRO_KEY_INFO_TYPE_MACRO:
		new_key_info = button_macro(parent);
		break;
	case PYRA_RMP_MACRO_KEY_INFO_TYPE_SHORTCUT:
		new_key_info = button_shortcut(parent);
		break;
	case PYRA_RMP_MACRO_KEY_INFO_TYPE_QUICKLAUNCH:
		new_key_info = button_quicklaunch(parent);
		break;
	default:
		new_key_info = pyra_rmp_macro_key_info_new_special(type_data);
	}

	if (new_key_info) {
		g_free(priv->key_info);
		priv->key_info = new_key_info;
	}
	set_text(key_combo_box);
}

GtkWidget *pyraconfig_key_combo_box_new(void) {
	PyraconfigKeyComboBox *key_combo_box;
	PyraconfigKeyComboBoxClass *key_combo_box_class;
	PyraconfigKeyComboBoxPrivate *priv;

	key_combo_box = PYRACONFIG_KEY_COMBO_BOX(g_object_new(PYRACONFIG_KEY_COMBO_BOX_TYPE,
			NULL));

	priv = key_combo_box->priv;

	key_combo_box_class = PYRACONFIG_KEY_COMBO_BOX_GET_CLASS(key_combo_box);
	roccat_key_combo_box_set_model(ROCCAT_KEY_COMBO_BOX(key_combo_box), key_combo_box_class->model);

	priv->changed_handler_id = g_signal_connect(G_OBJECT(key_combo_box), "changed", G_CALLBACK(button_changed_cb), NULL);

	return GTK_WIDGET(key_combo_box);
}

static void pyraconfig_key_combo_box_init(PyraconfigKeyComboBox *key_combo_box) {
	PyraconfigKeyComboBoxPrivate *priv = PYRACONFIG_KEY_COMBO_BOX_GET_PRIVATE(key_combo_box);
	key_combo_box->priv = priv;
}

static GtkTreeModel *button_info_model_new(void) {
	GtkTreeStore *store;
	GtkTreeIter toplevel;

	store = roccat_key_combo_box_store_new();

	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("Buttons"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Button 1 (Click)"), PYRA_RMP_MACRO_KEY_INFO_TYPE_CLICK, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Button 2 (Universal scrolling)"), PYRA_RMP_MACRO_KEY_INFO_TYPE_UNIVERSAL_SCROLLING, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Button 3 (Menu)"), PYRA_RMP_MACRO_KEY_INFO_TYPE_MENU, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Button 4 (Scroll up)"), PYRA_RMP_MACRO_KEY_INFO_TYPE_SCROLL_UP, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Button 5 (Scroll down)"), PYRA_RMP_MACRO_KEY_INFO_TYPE_SCROLL_DOWN, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Button 6 (Tilt left)"), PYRA_RMP_MACRO_KEY_INFO_TYPE_TILT_LEFT, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Button 7 (Tilt right)"), PYRA_RMP_MACRO_KEY_INFO_TYPE_TILT_RIGHT, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Button 8 (Browser backward)"), PYRA_RMP_MACRO_KEY_INFO_TYPE_BROWSER_BACKWARD, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Button 9 (Browser forward)"), PYRA_RMP_MACRO_KEY_INFO_TYPE_BROWSER_FORWARD, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Double click"), PYRA_RMP_MACRO_KEY_INFO_TYPE_DOUBLE_CLICK, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("Multimedia"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Open player"), PYRA_RMP_MACRO_KEY_INFO_TYPE_MULTIMEDIA_OPEN_PLAYER, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Next track"), PYRA_RMP_MACRO_KEY_INFO_TYPE_MULTIMEDIA_NEXT_TRACK, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Previous track"), PYRA_RMP_MACRO_KEY_INFO_TYPE_MULTIMEDIA_PREV_TRACK, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Play/Pause"), PYRA_RMP_MACRO_KEY_INFO_TYPE_MULTIMEDIA_PLAY_PAUSE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Stop"), PYRA_RMP_MACRO_KEY_INFO_TYPE_MULTIMEDIA_STOP, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Mute"), PYRA_RMP_MACRO_KEY_INFO_TYPE_MULTIMEDIA_MUTE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Volume up"), PYRA_RMP_MACRO_KEY_INFO_TYPE_MULTIMEDIA_VOLUME_UP, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Volume down"), PYRA_RMP_MACRO_KEY_INFO_TYPE_MULTIMEDIA_VOLUME_DOWN, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("Easywheel"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Sensitivity"), PYRA_RMP_MACRO_KEY_INFO_TYPE_EASYWHEEL_SENSITIVITY, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Profile"), PYRA_RMP_MACRO_KEY_INFO_TYPE_EASYWHEEL_PROFILE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("CPI"), PYRA_RMP_MACRO_KEY_INFO_TYPE_EASYWHEEL_CPI, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Volume"), PYRA_RMP_MACRO_KEY_INFO_TYPE_EASYWHEEL_VOLUME, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("Profile"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Profile cycle"), PYRA_RMP_MACRO_KEY_INFO_TYPE_PROFILE_CYCLE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Profile up"), PYRA_RMP_MACRO_KEY_INFO_TYPE_PROFILE_UP, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Profile down"), PYRA_RMP_MACRO_KEY_INFO_TYPE_PROFILE_DOWN, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("CPI"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("CPI cycle"), PYRA_RMP_MACRO_KEY_INFO_TYPE_CPI_CYCLE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("CPI up"), PYRA_RMP_MACRO_KEY_INFO_TYPE_CPI_UP, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("CPI down"), PYRA_RMP_MACRO_KEY_INFO_TYPE_CPI_DOWN, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	roccat_key_combo_box_store_append(store, NULL, NULL, _("Shortcut"), PYRA_RMP_MACRO_KEY_INFO_TYPE_SHORTCUT, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, NULL, _("Macro"), PYRA_RMP_MACRO_KEY_INFO_TYPE_MACRO, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, NULL, _("Quicklaunch"), PYRA_RMP_MACRO_KEY_INFO_TYPE_QUICKLAUNCH, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, NULL, _("Easyshift"), PYRA_RMP_MACRO_KEY_INFO_TYPE_EASYSHIFT, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, NULL, _("Disabled"), PYRA_RMP_MACRO_KEY_INFO_TYPE_DISABLED, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	return GTK_TREE_MODEL(store);
}

static void finalize(GObject *object) {
	PyraconfigKeyComboBoxPrivate *priv = PYRACONFIG_KEY_COMBO_BOX(object)->priv;
	g_free(priv->key_info);
	G_OBJECT_CLASS(pyraconfig_key_combo_box_parent_class)->finalize(object);
}

static void pyraconfig_key_combo_box_class_init(PyraconfigKeyComboBoxClass *klass) {
	GObjectClass *gobject_class = (GObjectClass*)klass;

	gobject_class->finalize = finalize;
	g_type_class_add_private(klass, sizeof(PyraconfigKeyComboBoxPrivate));
	klass->model = button_info_model_new();
}

void pyraconfig_key_combo_box_set_value_blocked(PyraconfigKeyComboBox *key_combo_box, PyraRMPMacroKeyInfo const *key_info) {
	PyraconfigKeyComboBoxPrivate *priv = key_combo_box->priv;

	g_free(priv->key_info);
	priv->key_info = pyra_rmp_macro_key_info_dup(key_info);

	set_text(key_combo_box);
}

PyraRMPMacroKeyInfo *pyraconfig_key_combo_box_get_value(PyraconfigKeyComboBox *key_combo_box) {
	return pyra_rmp_macro_key_info_dup(key_combo_box->priv->key_info);
}
