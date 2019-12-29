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

#include "suora_key_combo_box.h"
#include "suora_rkp_keys.h"
#include "roccat_helper.h"
#include "roccat_shortcut_dialog.h"
#include "roccat_key_dialog.h"
#include "roccat_warning_dialog.h"
#include <gaminggear/gaminggear_macro_dialog.h>
#include <gaminggear/gdk_key_translations.h>
#include <gaminggear/hid_uid.h>
#include "gtk_roccat_helper.h"
#include "g_cclosure_roccat_marshaller.h"
#include "i18n.h"

#define SUORA_KEY_COMBO_BOX_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), SUORA_KEY_COMBO_BOX_TYPE, SuoraKeyComboBoxClass))
#define IS_SUORA_KEY_COMBO_BOX_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), SUORA_KEY_COMBO_BOX_TYPE))
#define SUORA_KEY_COMBO_BOX_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), SUORA_KEY_COMBO_BOX_TYPE, SuoraKeyComboBoxClass))
#define SUORA_KEY_COMBO_BOX_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), SUORA_KEY_COMBO_BOX_TYPE, SuoraKeyComboBoxPrivate))

typedef struct _SuoraKeyComboBoxClass SuoraKeyComboBoxClass;
typedef struct _SuoraKeyComboBoxPrivate SuoraKeyComboBoxPrivate;

struct _SuoraKeyComboBox {
	RoccatKeyComboBox parent;
	SuoraKeyComboBoxPrivate *priv;
};

struct _SuoraKeyComboBoxClass {
	RoccatKeyComboBoxClass parent_class;
	GtkTreeModel *model;
};

struct _SuoraKeyComboBoxPrivate {
	SuoraKeyComboBoxData *data;
	guint standard;
	gulong changed_handler_id;
};

G_DEFINE_TYPE(SuoraKeyComboBox, suora_key_combo_box, ROCCAT_KEY_COMBO_BOX_TYPE);

enum {
	KEY_CHANGED,
	LAST_SIGNAL,
};

static guint signals[LAST_SIGNAL] = { 0 };

gchar *suora_key_combo_box_get_text_for_data(SuoraKeyComboBox *key_combo_box, SuoraKeyComboBoxData const *data) {
	gchar *text;
	gchar *temp;
	guint type;

	type = data->key.type;

	switch (type) {
	case SUORA_RKP_KEY_TYPE_SHORTCUT:
		gtk_widget_set_tooltip_text(GTK_WIDGET(key_combo_box), NULL);
		temp = roccat_shortcut_dialog_create_name(data->key.modifier, data->key.key);
		text = g_strdup_printf(_("Shortcut: %s"), temp);
		g_free(temp);
		break;
	case SUORA_RKP_KEY_TYPE_MACRO:
		gtk_widget_set_tooltip_text(GTK_WIDGET(key_combo_box), NULL);
		text = g_strdup_printf(_("Macro: %s/%s"), data->macro.macroset_name, data->macro.macro_name);
		break;
	default:
		gtk_widget_set_tooltip_text(GTK_WIDGET(key_combo_box), NULL);
		text = roccat_key_combo_box_model_get_title_for_type_explicit(gtk_combo_box_get_model(GTK_COMBO_BOX(key_combo_box)), type);
		if (!text)
			text = gaminggear_hid_to_name(type);
		break;
	}

	return text;
}

void suora_key_combo_box_update(SuoraKeyComboBox *key_combo_box) {
	SuoraKeyComboBoxPrivate *priv = key_combo_box->priv;
	gchar *text;

	text = suora_key_combo_box_get_text_for_data(key_combo_box, priv->data);

	g_signal_handler_block(G_OBJECT(key_combo_box), priv->changed_handler_id);
	roccat_key_combo_box_set_text(ROCCAT_KEY_COMBO_BOX(key_combo_box), text);
	g_signal_handler_unblock(G_OBJECT(key_combo_box), priv->changed_handler_id);

	g_free(text);
}

static gboolean set_macro(SuoraKeyComboBox *key_combo_box) {
	SuoraKeyComboBoxPrivate *priv = key_combo_box->priv;
	GtkWindow *parent;
	GaminggearMacro *gaminggear_macro;
	SuoraRkpMacro *suora_rkp_macro;
	gboolean result;
	GError *local_error = NULL;

	parent = gtk_roccat_widget_get_gtk_window(GTK_WIDGET(key_combo_box));
	gaminggear_macro = gaminggear_macro_dialog(parent);
	if (!gaminggear_macro)
		return FALSE;

	suora_rkp_macro = (SuoraRkpMacro *)g_malloc0(sizeof(SuoraRkpMacro));
	result = gaminggear_macro_to_suora_rkp_macro(gaminggear_macro, suora_rkp_macro, &local_error);
	gaminggear_macro_free(gaminggear_macro);
	if (!result) {
		roccat_handle_error_dialog(parent, _("Could not convert macro"), &local_error);
		g_free(suora_rkp_macro);
		return FALSE;
	}

	suora_rkp_macro_copy(&priv->data->macro, suora_rkp_macro);
	g_free(suora_rkp_macro);

	return TRUE;
}

static gboolean key_macro(SuoraKeyComboBox *key_combo_box) {
	SuoraKeyComboBoxPrivate *priv = key_combo_box->priv;
	gboolean retval;

	retval = set_macro(key_combo_box);

	if (retval)
		roccat_button_set_to_normal(&priv->data->key, SUORA_RKP_KEY_TYPE_MACRO);

	return retval;
}

// FIXME only two modifier allowed
static gboolean key_shortcut(SuoraKeyComboBox *key_combo_box) {
	SuoraKeyComboBoxPrivate *priv = key_combo_box->priv;
	RoccatShortcutDialog *dialog;
	gboolean retval = FALSE;

	dialog = ROCCAT_SHORTCUT_DIALOG(roccat_shortcut_dialog_new(gtk_roccat_widget_get_gtk_window(GTK_WIDGET(key_combo_box))));
	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
		roccat_button_set_to_shortcut(&priv->data->key, SUORA_RKP_KEY_TYPE_SHORTCUT,
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

static gboolean key_normal(SuoraKeyComboBox *key_combo_box, guint type) {
	gboolean retval = TRUE;

	if (key_combo_box->priv->data->key.type == type)
		retval = FALSE;

	roccat_button_set_to_normal(&key_combo_box->priv->data->key, type);

	return retval;
}

static gboolean key_remap(SuoraKeyComboBox *key_combo_box) {
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

static void changed_cb(GtkComboBox *combo, gpointer user_data) {
	SuoraKeyComboBox *key_combo_box = SUORA_KEY_COMBO_BOX(combo);
	guint type;
	gboolean changed;

	type = roccat_key_combo_box_get_active_type(ROCCAT_KEY_COMBO_BOX(combo));
	if (type == ROCCAT_KEY_COMBO_BOX_TYPE_NONE)
		return;

	if (type == SUORA_KEY_COMBO_BOX_TYPE_STANDARD)
		type = key_combo_box->priv->standard;

	switch (type) {
	case SUORA_KEY_COMBO_BOX_TYPE_REMAP:
		changed = key_remap(key_combo_box);
		break;
	case SUORA_RKP_KEY_TYPE_MACRO:
		changed = key_macro(key_combo_box);
		break;
	case SUORA_RKP_KEY_TYPE_SHORTCUT:
		changed = key_shortcut(key_combo_box);
		break;
	default:
		changed = key_normal(key_combo_box, type);
		break;
	}

	suora_key_combo_box_update(key_combo_box);
	if (changed)
		g_signal_emit((gpointer)key_combo_box, signals[KEY_CHANGED], 0);
}

void suora_key_combo_box_set_data_pointer(SuoraKeyComboBox *key_combo_box, SuoraKeyComboBoxData *data) {
	key_combo_box->priv->data = data;
	suora_key_combo_box_update(key_combo_box);
}

SuoraKeyComboBoxData *suora_key_combo_box_get_data_pointer(SuoraKeyComboBox *key_combo_box) {
	return key_combo_box->priv->data;
}

void suora_key_combo_box_set_standard(SuoraKeyComboBox *key_combo_box, guint standard) {
	key_combo_box->priv->standard = standard;
}

GtkWidget *suora_key_combo_box_new(guint mask, guint standard) {
	SuoraKeyComboBox *key_combo_box;
	SuoraKeyComboBoxClass *key_combo_box_class;
	SuoraKeyComboBoxPrivate *priv;

	key_combo_box = SUORA_KEY_COMBO_BOX(g_object_new(SUORA_KEY_COMBO_BOX_TYPE,
			"exclude-mask", mask,
			NULL));

	priv = key_combo_box->priv;

	suora_key_combo_box_set_standard(key_combo_box, standard);

	key_combo_box_class = SUORA_KEY_COMBO_BOX_GET_CLASS(key_combo_box);
	roccat_key_combo_box_set_model(ROCCAT_KEY_COMBO_BOX(key_combo_box), key_combo_box_class->model);

	priv->changed_handler_id = g_signal_connect(G_OBJECT(key_combo_box), "changed", G_CALLBACK(changed_cb), NULL);

	return GTK_WIDGET(key_combo_box);
}

static void suora_key_combo_box_init(SuoraKeyComboBox *key_combo_box) {
	SuoraKeyComboBoxPrivate *priv = SUORA_KEY_COMBO_BOX_GET_PRIVATE(key_combo_box);
	key_combo_box->priv = priv;
}

static GtkTreeModel *key_info_model_new(void) {
	GtkTreeStore *store;
	GtkTreeIter toplevel;

	store = roccat_key_combo_box_store_new();

	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("Multimedia"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_SHORTCUT);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Play/Pause"), SUORA_RKP_KEY_TYPE_MULTIMEDIA_PLAY_PAUSE, ROCCAT_KEY_COMBO_BOX_GROUP_SHORTCUT);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Next track"), SUORA_RKP_KEY_TYPE_MULTIMEDIA_NEXT_TRACK, ROCCAT_KEY_COMBO_BOX_GROUP_SHORTCUT);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Previous track"), SUORA_RKP_KEY_TYPE_MULTIMEDIA_PREV_TRACK, ROCCAT_KEY_COMBO_BOX_GROUP_SHORTCUT);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Stop"), SUORA_RKP_KEY_TYPE_MULTIMEDIA_STOP, ROCCAT_KEY_COMBO_BOX_GROUP_SHORTCUT);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Volume up"), SUORA_RKP_KEY_TYPE_MULTIMEDIA_VOLUME_UP, ROCCAT_KEY_COMBO_BOX_GROUP_SHORTCUT);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Volume down"), SUORA_RKP_KEY_TYPE_MULTIMEDIA_VOLUME_DOWN, ROCCAT_KEY_COMBO_BOX_GROUP_SHORTCUT);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Mute master volume"), SUORA_RKP_KEY_TYPE_MULTIMEDIA_MUTE, ROCCAT_KEY_COMBO_BOX_GROUP_SHORTCUT);

	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("Browser"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_SHORTCUT);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Search"), SUORA_RKP_KEY_TYPE_WWW_SEARCH, ROCCAT_KEY_COMBO_BOX_GROUP_SHORTCUT);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Home"), SUORA_RKP_KEY_TYPE_WWW_HOME, ROCCAT_KEY_COMBO_BOX_GROUP_SHORTCUT);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Back"), SUORA_RKP_KEY_TYPE_WWW_BACK, ROCCAT_KEY_COMBO_BOX_GROUP_SHORTCUT);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Forward"), SUORA_RKP_KEY_TYPE_WWW_FORWARD, ROCCAT_KEY_COMBO_BOX_GROUP_SHORTCUT);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Stop"), SUORA_RKP_KEY_TYPE_WWW_STOP, ROCCAT_KEY_COMBO_BOX_GROUP_SHORTCUT);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Refresh"), SUORA_RKP_KEY_TYPE_WWW_REFRESH, ROCCAT_KEY_COMBO_BOX_GROUP_SHORTCUT);

	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("Open"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_SHORTCUT);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("E-Mail"), SUORA_RKP_KEY_TYPE_APP_EMAIL, ROCCAT_KEY_COMBO_BOX_GROUP_SHORTCUT);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Calculator"), SUORA_RKP_KEY_TYPE_APP_CALCULATOR, ROCCAT_KEY_COMBO_BOX_GROUP_SHORTCUT);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Computer"), SUORA_RKP_KEY_TYPE_APP_MY_COMPUTER, ROCCAT_KEY_COMBO_BOX_GROUP_SHORTCUT);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Player"), SUORA_RKP_KEY_TYPE_APP_MEDIA_PLAYER, ROCCAT_KEY_COMBO_BOX_GROUP_SHORTCUT);

	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("System"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_SHORTCUT);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Shutdown"), SUORA_RKP_KEY_TYPE_SYSTEM_SHUTDOWN, ROCCAT_KEY_COMBO_BOX_GROUP_SHORTCUT);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Sleep"), SUORA_RKP_KEY_TYPE_SYSTEM_SLEEP, ROCCAT_KEY_COMBO_BOX_GROUP_SHORTCUT);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Wake"), SUORA_RKP_KEY_TYPE_SYSTEM_WAKE, ROCCAT_KEY_COMBO_BOX_GROUP_SHORTCUT);

	roccat_key_combo_box_store_append(store, NULL, NULL, _("Shortcut"), SUORA_RKP_KEY_TYPE_SHORTCUT, ROCCAT_KEY_COMBO_BOX_GROUP_SHORTCUT);
	roccat_key_combo_box_store_append(store, NULL, NULL, _("Macro"), SUORA_RKP_KEY_TYPE_MACRO, ROCCAT_KEY_COMBO_BOX_GROUP_MACRO);

	roccat_key_combo_box_store_append(store, NULL, NULL, _("Standard"), SUORA_KEY_COMBO_BOX_TYPE_STANDARD, ROCCAT_KEY_COMBO_BOX_GROUP_SHORTCUT);
	roccat_key_combo_box_store_append(store, NULL, NULL, _("Remap"), SUORA_KEY_COMBO_BOX_TYPE_REMAP, ROCCAT_KEY_COMBO_BOX_GROUP_SHORTCUT);
	roccat_key_combo_box_store_append(store, NULL, NULL, _("Disabled"), SUORA_RKP_KEY_TYPE_DISABLED, ROCCAT_KEY_COMBO_BOX_GROUP_SHORTCUT);

	return GTK_TREE_MODEL(store);
}

static void suora_key_combo_box_class_init(SuoraKeyComboBoxClass *klass) {
	g_type_class_add_private(klass, sizeof(SuoraKeyComboBoxPrivate));
	klass->model = key_info_model_new();

	signals[KEY_CHANGED] = g_signal_new("key-changed",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);
}
