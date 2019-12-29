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
#include "arvoconfig_key_combo_box.h"
#include "roccat_warning_dialog.h"
#include "roccat_timer_dialog.h"
#include "roccat_shortcut_dialog.h"
#include "gtk_roccat_helper.h"
#include "i18n.h"

#define ARVOCONFIG_KEY_COMBO_BOX_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), ARVOCONFIG_KEY_COMBO_BOX_TYPE, ArvoconfigKeyComboBoxClass))
#define IS_ARVOCONFIG_KEY_COMBO_BOX_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), ARVOCONFIG_KEY_COMBO_BOX_TYPE))
#define ARVOCONFIG_KEY_COMBO_BOX_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), ARVOCONFIG_KEY_COMBO_BOX_TYPE, ArvoconfigKeyComboBoxClass))
#define ARVOCONFIG_KEY_COMBO_BOX_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), ARVOCONFIG_KEY_COMBO_BOX_TYPE, ArvoconfigKeyComboBoxPrivate))

typedef struct _ArvoconfigKeyComboBoxClass ArvoconfigKeyComboBoxClass;
typedef struct _ArvoconfigKeyComboBoxPrivate ArvoconfigKeyComboBoxPrivate;

struct _ArvoconfigKeyComboBox {
	RoccatKeyComboBox parent;
	ArvoconfigKeyComboBoxPrivate *priv;
};

struct _ArvoconfigKeyComboBoxClass {
	RoccatKeyComboBoxClass parent_class;
	GtkTreeModel *model;
};

struct _ArvoconfigKeyComboBoxPrivate {
	ArvoRkpButtonInfo *key_info;
	gulong changed_handler_id;
};

G_DEFINE_TYPE(ArvoconfigKeyComboBox, arvoconfig_key_combo_box, ROCCAT_KEY_COMBO_BOX_TYPE);

static gchar *shortcut_name(ArvoRkpButtonInfo const *button_info) {
	return roccat_shortcut_dialog_create_name_full(button_info->shortcut_ctrl == ARVO_RKP_BUTTON_INFO_SHORTCUT_STATE_ON,
			button_info->shortcut_shift == ARVO_RKP_BUTTON_INFO_SHORTCUT_STATE_ON,
			button_info->shortcut_alt == ARVO_RKP_BUTTON_INFO_SHORTCUT_STATE_ON,
			button_info->shortcut_win == ARVO_RKP_BUTTON_INFO_SHORTCUT_STATE_ON,
			button_info->shortcut_key);
}

static void set_text(ArvoconfigKeyComboBox *key_combo_box) {
	ArvoconfigKeyComboBoxPrivate *priv = key_combo_box->priv;
	gchar *text;
	gchar *temp;

	switch (priv->key_info->type) {
	case ARVO_RKP_BUTTON_INFO_TYPE_SHORTCUT:
		temp = shortcut_name(priv->key_info);
		text = g_strdup_printf(_("Shortcut: %s"), temp);
		g_free(temp);
		break;
	case ARVO_RKP_BUTTON_INFO_TYPE_MACRO:
		text = g_strdup_printf(_("Macro: %s/%s"), (gchar const *)priv->key_info->macroset_name, (gchar const *)priv->key_info->macro_name);
		break;
	case ARVO_RKP_BUTTON_INFO_TYPE_QUICKLAUNCH_NS:
		temp = g_path_get_basename((gchar const *)priv->key_info->macroset_name);
		text = g_strdup_printf(_("Quicklaunch: %s"), temp);
		g_free(temp);
		break;
	case ARVO_RKP_BUTTON_INFO_TYPE_TIMER_START:
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

static ArvoRkpButtonInfo *button_quicklaunch_ns(GtkWindow *parent) {
	GtkWidget *dialog;
	gchar *filename;
	ArvoRkpButtonInfo *key_info = NULL;

	dialog = gtk_file_chooser_dialog_new(_("Select executable"),
			parent,
			GTK_FILE_CHOOSER_ACTION_OPEN,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
			NULL
	);

	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
		filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
		key_info = arvo_rkp_button_info_new_quicklaunch_ns(filename);
		g_free(filename);
	}
	gtk_widget_destroy(dialog);
	return key_info;
}

static ArvoRkpButtonInfo *button_macro(GtkWindow *parent) {
	ArvoRkpButtonInfo *key_info = NULL;
	GaminggearMacro *gaminggear_macro;
	GError *local_error = NULL;

	gaminggear_macro = gaminggear_macro_dialog(parent);
	if (!gaminggear_macro)
		return NULL;

	key_info = gaminggear_macro_to_arvo_rkp_button_info(gaminggear_macro, &local_error);
	gaminggear_macro_free(gaminggear_macro);
	if (!key_info)
		roccat_handle_error_dialog(parent, _("Could not convert macro"), &local_error);

	return key_info;
}

static ArvoRkpButtonInfo *button_shortcut(GtkWindow *parent) {
	RoccatShortcutDialog *dialog;
	ArvoRkpButtonInfo *key_info = NULL;

	dialog = ROCCAT_SHORTCUT_DIALOG(roccat_shortcut_dialog_new(parent));
	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
		key_info = arvo_rkp_button_info_new_shortcut(
				roccat_shortcut_dialog_get_hid(dialog),
				roccat_shortcut_dialog_get_shift(dialog),
				roccat_shortcut_dialog_get_ctrl(dialog),
				roccat_shortcut_dialog_get_win(dialog),
				roccat_shortcut_dialog_get_alt(dialog)
		);
	}
	gtk_widget_destroy(GTK_WIDGET(dialog));
	return key_info;
}

static ArvoRkpButtonInfo *button_timer(GtkWindow *parent) {
	ArvoRkpButtonInfo *key_info = NULL;
	RoccatTimer *timer;

	timer = roccat_timer_dialog(parent);

	if (timer) {
		key_info = arvo_rkp_button_info_new_timer((gchar const *)timer->name, roccat_timer_get_seconds(timer));
		roccat_timer_free(timer);
	}

	return key_info;
}

static void button_changed_cb(GtkComboBox *combo, gpointer user_data) {
	ArvoconfigKeyComboBox *key_combo_box = ARVOCONFIG_KEY_COMBO_BOX(combo);
	ArvoconfigKeyComboBoxPrivate *priv = key_combo_box->priv;
	GtkWindow *parent = gtk_roccat_widget_get_gtk_window(GTK_WIDGET(combo));
	gint type_data;
	ArvoRkpButtonInfo *new_key_info;

	type_data = roccat_key_combo_box_get_active_type(ROCCAT_KEY_COMBO_BOX(combo));
	if (type_data == ROCCAT_KEY_COMBO_BOX_TYPE_NONE)
		return;

	switch (type_data) {
	case ARVO_RKP_BUTTON_INFO_TYPE_MACRO:
		new_key_info = button_macro(parent);
		break;
	case ARVO_RKP_BUTTON_INFO_TYPE_SHORTCUT:
		new_key_info = button_shortcut(parent);
		break;
	case ARVO_RKP_BUTTON_INFO_TYPE_QUICKLAUNCH_NS:
		new_key_info = button_quicklaunch_ns(parent);
		break;
	case ARVO_RKP_BUTTON_INFO_TYPE_TIMER_START:
		new_key_info = button_timer(parent);
		break;
	default:
		new_key_info = arvo_rkp_button_info_new_special(type_data);
	}

	if (new_key_info) {
		g_free(priv->key_info);
		priv->key_info = new_key_info;
	}
	set_text(key_combo_box);
}

GtkWidget *arvoconfig_key_combo_box_new(void) {
	ArvoconfigKeyComboBox *key_combo_box;
	ArvoconfigKeyComboBoxClass *key_combo_box_class;
	ArvoconfigKeyComboBoxPrivate *priv;

	key_combo_box = ARVOCONFIG_KEY_COMBO_BOX(g_object_new(ARVOCONFIG_KEY_COMBO_BOX_TYPE,
			NULL));

	priv = key_combo_box->priv;

	key_combo_box_class = ARVOCONFIG_KEY_COMBO_BOX_GET_CLASS(key_combo_box);
	roccat_key_combo_box_set_model(ROCCAT_KEY_COMBO_BOX(key_combo_box), key_combo_box_class->model);

	priv->changed_handler_id = g_signal_connect(G_OBJECT(key_combo_box), "changed", G_CALLBACK(button_changed_cb), NULL);

	return GTK_WIDGET(key_combo_box);
}

static void arvoconfig_key_combo_box_init(ArvoconfigKeyComboBox *key_combo_box) {
	ArvoconfigKeyComboBoxPrivate *priv = ARVOCONFIG_KEY_COMBO_BOX_GET_PRIVATE(key_combo_box);
	key_combo_box->priv = priv;
}

static GtkTreeModel *button_info_model_new(void) {
	GtkTreeStore *store;
	GtkTreeIter toplevel, sublevel;

	store = roccat_key_combo_box_store_new();

	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("Multimedia"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Open player"), ARVO_RKP_BUTTON_INFO_TYPE_MULTIMEDIA_OPEN_PLAYER, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Previous track"), ARVO_RKP_BUTTON_INFO_TYPE_MULTIMEDIA_PREV_TRACK, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Next track"), ARVO_RKP_BUTTON_INFO_TYPE_MULTIMEDIA_NEXT_TRACK, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Play/Pause"), ARVO_RKP_BUTTON_INFO_TYPE_MULTIMEDIA_PLAY_PAUSE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Stop"), ARVO_RKP_BUTTON_INFO_TYPE_MULTIMEDIA_STOP, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Mute"), ARVO_RKP_BUTTON_INFO_TYPE_MULTIMEDIA_MUTE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Volume up"), ARVO_RKP_BUTTON_INFO_TYPE_MULTIMEDIA_VOLUME_UP, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Volume down"), ARVO_RKP_BUTTON_INFO_TYPE_MULTIMEDIA_VOLUME_DOWN, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("WWW"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("WWW search"), ARVO_RKP_BUTTON_INFO_TYPE_WWW_SEARCH, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("WWW home"), ARVO_RKP_BUTTON_INFO_TYPE_WWW_HOME, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("WWW back"), ARVO_RKP_BUTTON_INFO_TYPE_WWW_BACK, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("WWW forward"), ARVO_RKP_BUTTON_INFO_TYPE_WWW_FORWARD, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("WWW stop"), ARVO_RKP_BUTTON_INFO_TYPE_WWW_STOP, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("WWW refresh"), ARVO_RKP_BUTTON_INFO_TYPE_WWW_REFRESH, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("WWW favorites"), ARVO_RKP_BUTTON_INFO_TYPE_WWW_FAVORITES, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("*Profile"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("*Profile cycle"), ARVO_RKP_BUTTON_INFO_TYPE_PROFILE_CYCLE_NS, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("*Profile up"), ARVO_RKP_BUTTON_INFO_TYPE_PROFILE_UP_NS, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("*Profile down"), ARVO_RKP_BUTTON_INFO_TYPE_PROFILE_DOWN_NS, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("*Easyshift"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, &sublevel, &toplevel, _("*Other"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("*Other Easyshift"), ARVO_RKP_BUTTON_INFO_TYPE_TALK_OTHER_EASYSHIFT_NS, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("*Other Easyshift lock"), ARVO_RKP_BUTTON_INFO_TYPE_TALK_OTHER_EASYSHIFT_LOCK_NS, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("*Other Easyaim Setting 1"), ARVO_RKP_BUTTON_INFO_TYPE_TALK_OTHER_EASYAIM_1_NS, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("*Other Easyaim Setting 2"), ARVO_RKP_BUTTON_INFO_TYPE_TALK_OTHER_EASYAIM_2_NS, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("*Other Easyaim Setting 3"), ARVO_RKP_BUTTON_INFO_TYPE_TALK_OTHER_EASYAIM_3_NS, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("*Other Easyaim Setting 4"), ARVO_RKP_BUTTON_INFO_TYPE_TALK_OTHER_EASYAIM_4_NS, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("*Other Easyaim Setting 5"), ARVO_RKP_BUTTON_INFO_TYPE_TALK_OTHER_EASYAIM_5_NS, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	roccat_key_combo_box_store_append(store, NULL, NULL, _("Shortcut"), ARVO_RKP_BUTTON_INFO_TYPE_SHORTCUT, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, NULL, _("Macro"), ARVO_RKP_BUTTON_INFO_TYPE_MACRO, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, NULL, _("Disabled"), ARVO_RKP_BUTTON_INFO_TYPE_OFF, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, NULL, _("Timer"), ARVO_RKP_BUTTON_INFO_TYPE_TIMER_START, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, NULL, _("Timer stop"), ARVO_RKP_BUTTON_INFO_TYPE_TIMER_STOP, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, NULL, _("*Open driver"), ARVO_RKP_BUTTON_INFO_TYPE_OPEN_DRIVER_NS, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, NULL, _("*Quicklaunch"), ARVO_RKP_BUTTON_INFO_TYPE_QUICKLAUNCH_NS, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	return GTK_TREE_MODEL(store);
}

static void finalize(GObject *object) {
	ArvoconfigKeyComboBoxPrivate *priv = ARVOCONFIG_KEY_COMBO_BOX(object)->priv;
	g_free(priv->key_info);
	G_OBJECT_CLASS(arvoconfig_key_combo_box_parent_class)->finalize(object);
}

static void arvoconfig_key_combo_box_class_init(ArvoconfigKeyComboBoxClass *klass) {
	GObjectClass *gobject_class = (GObjectClass*)klass;

	gobject_class->finalize = finalize;
	g_type_class_add_private(klass, sizeof(ArvoconfigKeyComboBoxPrivate));
	klass->model = button_info_model_new();
}

void arvoconfig_key_combo_box_set_value_blocked(ArvoconfigKeyComboBox *key_combo_box, ArvoRkpButtonInfo const *key_info) {
	ArvoconfigKeyComboBoxPrivate *priv = key_combo_box->priv;

	g_free(priv->key_info);
	priv->key_info = arvo_rkp_button_info_dup(key_info);

	set_text(key_combo_box);
}

ArvoRkpButtonInfo *arvoconfig_key_combo_box_get_value(ArvoconfigKeyComboBox *key_combo_box) {
	return arvo_rkp_button_info_dup(key_combo_box->priv->key_info);
}
