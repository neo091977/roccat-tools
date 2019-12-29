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

#include "ryosmkfx_key_combo_box.h"
#include "ryosmkfx.h"
#include "ryos_key_combo_box.h"
#include "roccat_talk.h"
#include "roccat_helper.h"
#include "roccat_talk_target_dialog.h"
#include "roccat_shortcut_dialog.h"
#include "roccat_timer_dialog.h"
#include "roccat_key_dialog.h"
#include "roccat_warning_dialog.h"
#include <gaminggear/gaminggear_macro_dialog.h>
#include <gaminggear/gdk_key_translations.h>
#include <gaminggear/hid_uid.h>
#include <gaminggear/gaminggear_text_dialog.h>
#include "gtk_roccat_helper.h"
#include "g_cclosure_roccat_marshaller.h"
#include "i18n.h"

#define RYOSMKFX_KEY_COMBO_BOX_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), RYOSMKFX_KEY_COMBO_BOX_TYPE, RyosmkfxKeyComboBoxClass))
#define IS_RYOSMKFX_KEY_COMBO_BOX_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), RYOSMKFX_KEY_COMBO_BOX_TYPE))
#define RYOSMKFX_KEY_COMBO_BOX_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), RYOSMKFX_KEY_COMBO_BOX_TYPE, RyosmkfxKeyComboBoxClass))
#define RYOSMKFX_KEY_COMBO_BOX_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), RYOSMKFX_KEY_COMBO_BOX_TYPE, RyosmkfxKeyComboBoxPrivate))

typedef struct _RyosmkfxKeyComboBoxClass RyosmkfxKeyComboBoxClass;
typedef struct _RyosmkfxKeyComboBoxPrivate RyosmkfxKeyComboBoxPrivate;

struct _RyosmkfxKeyComboBox {
	RoccatKeyComboBox parent;
	RyosmkfxKeyComboBoxPrivate *priv;
};

struct _RyosmkfxKeyComboBoxClass {
	RoccatKeyComboBoxClass parent_class;
	GtkTreeModel *model;
};

struct _RyosmkfxKeyComboBoxPrivate {
	RyosmkfxKeyComboBoxData *data;
	guint standard;
	gulong changed_handler_id;
};

G_DEFINE_TYPE(RyosmkfxKeyComboBox, ryosmkfx_key_combo_box, ROCCAT_KEY_COMBO_BOX_TYPE);

enum {
	KEY_CHANGED,
	LAST_SIGNAL,
};

static guint signals[LAST_SIGNAL] = { 0 };

static gchar *get_host(gchar const *url) {
	GRegex *regex;
	GMatchInfo *match_info;
	gchar *host;
	gchar *basepath;
	gchar *result;

	regex = g_regex_new("([^:]*):(//)?([^@]*@)?([^:/#?]*)(:[0-9]*)?(/([^/?#]*))*[\?#]?.*", 0, 0, NULL);
	g_regex_match(regex, url, 0, &match_info);

	host = g_match_info_fetch(match_info, 4);
	basepath = g_match_info_fetch(match_info, 7);

	g_match_info_free(match_info);
	g_regex_unref(regex);

	if (!basepath)
		result = g_strdup(host);
	else
		result = g_strdup_printf("%s/%s", host, basepath);

	g_free(host);
	g_free(basepath);

	return result;
}

gchar *ryosmkfx_key_combo_box_get_text_for_data(RyosmkfxKeyComboBox *key_combo_box, RyosmkfxKeyComboBoxData const *data) {
	gchar *text;
	gchar *temp;
	guint type;

	type = data->key.type;

	switch (type) {
	case RYOS_KEY_TYPE_TALK_EASYSHIFT:
		text = g_strdup_printf(_("Easyshift: %s"), roccat_talk_device_get_text_static(data->talk_target));
		break;
	case RYOS_KEY_TYPE_TALK_EASYSHIFT_LOCK:
		text = g_strdup_printf(_("Easyshift lock: %s"), roccat_talk_device_get_text_static(data->talk_target));
		break;
	case RYOS_KEY_TYPE_TALK_EASYAIM_1:
		text = g_strdup_printf(_("Easyaim 1: %s"), roccat_talk_device_get_text_static(data->talk_target));
		break;
	case RYOS_KEY_TYPE_TALK_EASYAIM_2:
		text = g_strdup_printf(_("Easyaim 2: %s"), roccat_talk_device_get_text_static(data->talk_target));
		break;
	case RYOS_KEY_TYPE_TALK_EASYAIM_3:
		text = g_strdup_printf(_("Easyaim 3: %s"), roccat_talk_device_get_text_static(data->talk_target));
		break;
	case RYOS_KEY_TYPE_TALK_EASYAIM_4:
		text = g_strdup_printf(_("Easyaim 4: %s"), roccat_talk_device_get_text_static(data->talk_target));
		break;
	case RYOS_KEY_TYPE_TALK_EASYAIM_5:
		text = g_strdup_printf(_("Easyaim 5: %s"), roccat_talk_device_get_text_static(data->talk_target));
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
		gtk_widget_set_tooltip_text(GTK_WIDGET(key_combo_box), NULL);
		temp = roccat_shortcut_dialog_create_name(data->key.modifier, data->key.key);
		text = g_strdup_printf(_("Shortcut: %s"), temp);
		g_free(temp);
		break;
	case RYOS_KEY_TYPE_MACRO:
		gtk_widget_set_tooltip_text(GTK_WIDGET(key_combo_box), NULL);
		text = g_strdup_printf(_("Macro: %s/%s"), data->macro.macroset_name, data->macro.macro_name);
		break;
	case RYOS_KEY_TYPE_LED_MACRO:
		text = g_strdup_printf(_("LED Macro: %s/%s"), data->led_macro.macroset_name, data->led_macro.macro_name);
		break;
	case RYOS_KEY_TYPE_QUICKLAUNCH:
		gtk_widget_set_tooltip_text(GTK_WIDGET(key_combo_box), data->opener);
		temp = g_path_get_basename(data->opener);
		text = g_strdup_printf(_("Application: %s"), temp);
		g_free(temp);
		break;
	case RYOSMKFX_KEY_TYPE_OPEN_DOCUMENT:
		gtk_widget_set_tooltip_text(GTK_WIDGET(key_combo_box), data->opener);
		temp = g_path_get_basename(data->opener);
		text = g_strdup_printf(_("Document: %s"), temp);
		g_free(temp);
		break;
	case RYOSMKFX_KEY_TYPE_OPEN_FOLDER:
		gtk_widget_set_tooltip_text(GTK_WIDGET(key_combo_box), data->opener);
		temp = g_path_get_basename(data->opener);
		text = g_strdup_printf(_("Folder: %s"), temp);
		g_free(temp);
		break;
	case RYOSMKFX_KEY_TYPE_OPEN_WEBSITE:
		gtk_widget_set_tooltip_text(GTK_WIDGET(key_combo_box), data->opener);
		temp = get_host(data->opener);
		text = g_strdup_printf(_("Website: %s"), temp);
		g_free(temp);
		break;
	case RYOS_KEY_TYPE_TIMER:
		gtk_widget_set_tooltip_text(GTK_WIDGET(key_combo_box), NULL);
		text = g_strdup_printf(_("Timer: %s"), (gchar const *)data->timer.name);
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

void ryosmkfx_key_combo_box_update(RyosmkfxKeyComboBox *key_combo_box) {
	RyosmkfxKeyComboBoxPrivate *priv = key_combo_box->priv;
	gchar *text;

	text = ryosmkfx_key_combo_box_get_text_for_data(key_combo_box, priv->data);

	g_signal_handler_block(G_OBJECT(key_combo_box), priv->changed_handler_id);
	roccat_key_combo_box_set_text(ROCCAT_KEY_COMBO_BOX(key_combo_box), text);
	g_signal_handler_unblock(G_OBJECT(key_combo_box), priv->changed_handler_id);

	g_free(text);
}

static void ryosmkfx_key_combo_box_data_set_opener(RyosmkfxKeyComboBoxData *data, gchar const *new_opener) {
	g_strlcpy(data->opener, new_opener, ROCCAT_SWARM_RMP_OPENER_LENGTH);
}

static gboolean key_open_application(RyosmkfxKeyComboBox *key_combo_box) {
	RyosmkfxKeyComboBoxPrivate *priv = key_combo_box->priv;
	GtkWidget *dialog;
	gboolean retval = FALSE;
	gchar *path;

	dialog = gtk_file_chooser_dialog_new(_("Select application"),
			gtk_roccat_widget_get_gtk_window(GTK_WIDGET(key_combo_box)),
			GTK_FILE_CHOOSER_ACTION_OPEN,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
			NULL);

	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
		roccat_button_set_to_normal(&priv->data->key, RYOS_KEY_TYPE_QUICKLAUNCH);
		path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
		ryosmkfx_key_combo_box_data_set_opener(priv->data, path);
		g_free(path);
		retval = TRUE;
	}
	gtk_widget_destroy(dialog);
	return retval;
}

static gboolean key_open_document(RyosmkfxKeyComboBox *key_combo_box) {
	RyosmkfxKeyComboBoxPrivate *priv = key_combo_box->priv;
	GtkWidget *dialog;
	gboolean retval = FALSE;
	gchar *path;

	dialog = gtk_file_chooser_dialog_new(_("Select document"),
			gtk_roccat_widget_get_gtk_window(GTK_WIDGET(key_combo_box)),
			GTK_FILE_CHOOSER_ACTION_OPEN,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
			NULL);

	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
		roccat_button_set_to_normal(&priv->data->key, RYOSMKFX_KEY_TYPE_OPEN_DOCUMENT);
		path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
		ryosmkfx_key_combo_box_data_set_opener(priv->data, path);
		g_free(path);
		retval = TRUE;
	}
	gtk_widget_destroy(dialog);
	return retval;
}

static gboolean key_open_folder(RyosmkfxKeyComboBox *key_combo_box) {
	RyosmkfxKeyComboBoxPrivate *priv = key_combo_box->priv;
	GtkWidget *dialog;
	gboolean retval = FALSE;
	gchar *path;

	dialog = gtk_file_chooser_dialog_new(_("Select folder"),
			gtk_roccat_widget_get_gtk_window(GTK_WIDGET(key_combo_box)),
			GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
			NULL);

	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
		roccat_button_set_to_normal(&priv->data->key, RYOSMKFX_KEY_TYPE_OPEN_FOLDER);
		path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
		ryosmkfx_key_combo_box_data_set_opener(priv->data, path);
		g_free(path);
		retval = TRUE;
	}
	gtk_widget_destroy(dialog);
	return retval;
}

static gboolean key_open_website(RyosmkfxKeyComboBox *key_combo_box) {
	RyosmkfxKeyComboBoxPrivate *priv = key_combo_box->priv;
	gchar *url;

	url = gaminggear_text_dialog(gtk_roccat_widget_get_gtk_window(GTK_WIDGET(key_combo_box)),
			_("URL dialog"), _("Please enter any complete url with scheme"), NULL);

	if (url) {
		roccat_button_set_to_normal(&priv->data->key, RYOSMKFX_KEY_TYPE_OPEN_WEBSITE);
		ryosmkfx_key_combo_box_data_set_opener(priv->data, url);
		g_free(url);
		return TRUE;
	}
	return FALSE;
}

static gboolean set_led_macro(RyosmkfxKeyComboBox *key_combo_box) {
	RyosmkfxKeyComboBoxPrivate *priv = key_combo_box->priv;
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

static gboolean set_macro(RyosmkfxKeyComboBox *key_combo_box) {
	RyosmkfxKeyComboBoxPrivate *priv = key_combo_box->priv;
	GtkWindow *parent;
	GaminggearMacro *gaminggear_macro;
	RyosmkfxMacro *ryosmkfx_macro;
	gboolean result;
	GError *local_error = NULL;

	parent = gtk_roccat_widget_get_gtk_window(GTK_WIDGET(key_combo_box));
	gaminggear_macro = gaminggear_macro_dialog(parent);
	if (!gaminggear_macro)
		return FALSE;

	ryosmkfx_macro = (RyosmkfxMacro *)g_malloc0(sizeof(RyosmkfxMacro));
	result = gaminggear_macro_to_ryosmkfx_macro(gaminggear_macro, ryosmkfx_macro, &local_error);
	gaminggear_macro_free(gaminggear_macro);
	if (!result) {
		roccat_handle_error_dialog(parent, _("Could not convert macro"), &local_error);
		g_free(ryosmkfx_macro);
		return FALSE;
	}

	ryosmkfx_macro_copy(&priv->data->macro, ryosmkfx_macro);
	g_free(ryosmkfx_macro);

	return TRUE;
}

static gboolean key_macro(RyosmkfxKeyComboBox *key_combo_box) {
	RyosmkfxKeyComboBoxPrivate *priv = key_combo_box->priv;
	gboolean retval;

	retval = set_macro(key_combo_box);

	if (retval)
		roccat_button_set_to_normal(&priv->data->key, RYOS_KEY_TYPE_MACRO);

	return retval;
}

static gboolean key_led_macro(RyosmkfxKeyComboBox *key_combo_box) {
	RyosmkfxKeyComboBoxPrivate *priv = key_combo_box->priv;
	gboolean retval;

	retval = set_led_macro(key_combo_box);

	if (retval)
		roccat_button_set_to_normal(&priv->data->key, RYOS_KEY_TYPE_LED_MACRO);

	return retval;
}

static gboolean key_shortcut(RyosmkfxKeyComboBox *key_combo_box) {
	RyosmkfxKeyComboBoxPrivate *priv = key_combo_box->priv;
	RoccatShortcutDialog *dialog;
	gboolean retval = FALSE;

	dialog = ROCCAT_SHORTCUT_DIALOG(roccat_shortcut_dialog_new(gtk_roccat_widget_get_gtk_window(GTK_WIDGET(key_combo_box))));
	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
		roccat_button_set_to_shortcut(&priv->data->key, RYOS_KEY_TYPE_SHORTCUT,
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

static gboolean key_timer(RyosmkfxKeyComboBox *key_combo_box) {
	RyosmkfxKeyComboBoxPrivate *priv = key_combo_box->priv;
	RoccatTimer *roccat_timer;
	gboolean retval = FALSE;

	roccat_timer = roccat_timer_dialog(gtk_roccat_widget_get_gtk_window(GTK_WIDGET(key_combo_box)));
	if (roccat_timer) {
		roccat_button_set_to_normal(&priv->data->key, RYOS_KEY_TYPE_TIMER);
		roccat_timer_copy(&priv->data->timer, roccat_timer);
		g_free(roccat_timer);
		retval = TRUE;
	}
	return retval;
}

static gboolean key_normal(RyosmkfxKeyComboBox *key_combo_box, guint type) {
	gboolean retval = TRUE;

	if (key_combo_box->priv->data->key.type == type)
		retval = FALSE;

	roccat_button_set_to_normal(&key_combo_box->priv->data->key, type);

	return retval;
}

static gboolean key_remap(RyosmkfxKeyComboBox *key_combo_box) {
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

static gboolean key_talk(RyosmkfxKeyComboBox *key_combo_box, guint type) {
	RyosmkfxKeyComboBoxData *data = key_combo_box->priv->data;
	RoccatTalkTargetDialog *talk_target_dialog;
	gboolean retval = FALSE;

	talk_target_dialog = ROCCAT_TALK_TARGET_DIALOG(roccat_talk_target_dialog_new(
			gtk_roccat_widget_get_gtk_window(GTK_WIDGET(key_combo_box)), ROCCAT_TALK_DEVICE_KEYBOARD));
	if (gtk_dialog_run(GTK_DIALOG(talk_target_dialog)) == GTK_RESPONSE_ACCEPT) {
		roccat_button_set_to_normal(&data->key, type);
		data->talk_target = roccat_talk_target_dialog_get_value(talk_target_dialog);
		retval = TRUE;
	}

	gtk_widget_destroy(GTK_WIDGET(talk_target_dialog));
	return retval;
}

static void changed_cb(GtkComboBox *combo, gpointer user_data) {
	RyosmkfxKeyComboBox *key_combo_box = RYOSMKFX_KEY_COMBO_BOX(combo);
	guint type;
	gboolean changed;

	type = roccat_key_combo_box_get_active_type(ROCCAT_KEY_COMBO_BOX(combo));
	if (type == ROCCAT_KEY_COMBO_BOX_TYPE_NONE)
		return;

	if (type == RYOS_KEY_COMBO_BOX_TYPE_STANDARD)
		type = key_combo_box->priv->standard;

	switch (type) {
	case RYOS_KEY_COMBO_BOX_TYPE_REMAP:
		changed = key_remap(key_combo_box);
		break;
	case RYOS_KEY_TYPE_QUICKLAUNCH:
		changed = key_open_application(key_combo_box);
		break;
	case RYOSMKFX_KEY_TYPE_OPEN_DOCUMENT:
		changed = key_open_document(key_combo_box);
		break;
	case RYOSMKFX_KEY_TYPE_OPEN_FOLDER:
		changed = key_open_folder(key_combo_box);
		break;
	case RYOSMKFX_KEY_TYPE_OPEN_WEBSITE:
		changed = key_open_website(key_combo_box);
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
	default:
		changed = key_normal(key_combo_box, type);
		break;
	}

	ryosmkfx_key_combo_box_update(key_combo_box);
	if (changed)
		g_signal_emit((gpointer)key_combo_box, signals[KEY_CHANGED], 0);
}

void ryosmkfx_key_combo_box_set_data_pointer(RyosmkfxKeyComboBox *key_combo_box, RyosmkfxKeyComboBoxData *data) {
	key_combo_box->priv->data = data;
	ryosmkfx_key_combo_box_update(key_combo_box);
}

RyosmkfxKeyComboBoxData *ryosmkfx_key_combo_box_get_data_pointer(RyosmkfxKeyComboBox *key_combo_box) {
	return key_combo_box->priv->data;
}

void ryosmkfx_key_combo_box_set_standard(RyosmkfxKeyComboBox *key_combo_box, guint standard) {
	key_combo_box->priv->standard = standard;
}

GtkWidget *ryosmkfx_key_combo_box_new(guint mask, guint standard) {
	RyosmkfxKeyComboBox *key_combo_box;
	RyosmkfxKeyComboBoxClass *key_combo_box_class;
	RyosmkfxKeyComboBoxPrivate *priv;

	key_combo_box = RYOSMKFX_KEY_COMBO_BOX(g_object_new(RYOSMKFX_KEY_COMBO_BOX_TYPE,
			"exclude-mask", mask,
			NULL));

	priv = key_combo_box->priv;

	ryosmkfx_key_combo_box_set_standard(key_combo_box, standard);

	key_combo_box_class = RYOSMKFX_KEY_COMBO_BOX_GET_CLASS(key_combo_box);
	roccat_key_combo_box_set_model(ROCCAT_KEY_COMBO_BOX(key_combo_box), key_combo_box_class->model);

	priv->changed_handler_id = g_signal_connect(G_OBJECT(key_combo_box), "changed", G_CALLBACK(changed_cb), NULL);

	return GTK_WIDGET(key_combo_box);
}

static void ryosmkfx_key_combo_box_init(RyosmkfxKeyComboBox *key_combo_box) {
	RyosmkfxKeyComboBoxPrivate *priv = RYOSMKFX_KEY_COMBO_BOX_GET_PRIVATE(key_combo_box);
	key_combo_box->priv = priv;
}

static GtkTreeModel *key_info_model_new(void) {
	GtkTreeStore *store;
	GtkTreeIter toplevel, sublevel;

	store = roccat_key_combo_box_store_new();


	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("Easyshift"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, RYOS_KEY_COMBO_BOX_GROUP_TALK);

	roccat_key_combo_box_store_append(store, &sublevel, &toplevel, _("Self"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, RYOS_KEY_COMBO_BOX_GROUP_TALK);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Self Easyshift"), RYOS_KEY_TYPE_EASYSHIFT, RYOS_KEY_COMBO_BOX_GROUP_TALK);

	roccat_key_combo_box_store_append(store, &sublevel, &toplevel, _("Other"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, RYOS_KEY_COMBO_BOX_GROUP_TALK);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Other Easyshift"), RYOS_KEY_TYPE_TALK_EASYSHIFT, RYOS_KEY_COMBO_BOX_GROUP_TALK);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Other Easyshift lock"), RYOS_KEY_TYPE_TALK_EASYSHIFT_LOCK, RYOS_KEY_COMBO_BOX_GROUP_TALK);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Other Easyaim Setting 1"), RYOS_KEY_TYPE_TALK_EASYAIM_1, RYOS_KEY_COMBO_BOX_GROUP_TALK);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Other Easyaim Setting 2"), RYOS_KEY_TYPE_TALK_EASYAIM_2, RYOS_KEY_COMBO_BOX_GROUP_TALK);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Other Easyaim Setting 3"), RYOS_KEY_TYPE_TALK_EASYAIM_3, RYOS_KEY_COMBO_BOX_GROUP_TALK);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Other Easyaim Setting 4"), RYOS_KEY_TYPE_TALK_EASYAIM_4, RYOS_KEY_COMBO_BOX_GROUP_TALK);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Other Easyaim Setting 5"), RYOS_KEY_TYPE_TALK_EASYAIM_5, RYOS_KEY_COMBO_BOX_GROUP_TALK);

	roccat_key_combo_box_store_append(store, &sublevel, &toplevel, _("Both"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, RYOS_KEY_COMBO_BOX_GROUP_TALK);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Both Easyshift"), RYOS_KEY_TYPE_TALK_EASYSHIFT_ALL, RYOS_KEY_COMBO_BOX_GROUP_TALK);

	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("Multimedia"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Open player"), RYOS_KEY_TYPE_MULTIMEDIA_OPEN_PLAYER, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Previous track"), RYOS_KEY_TYPE_MULTIMEDIA_PREV_TRACK, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Next track"), RYOS_KEY_TYPE_MULTIMEDIA_NEXT_TRACK, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Play/Pause"), RYOS_KEY_TYPE_MULTIMEDIA_PLAY_PAUSE, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Stop"), RYOS_KEY_TYPE_MULTIMEDIA_STOP, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Mute master volume"), RYOS_KEY_TYPE_MULTIMEDIA_MUTE, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Mute microphone"), RYOSMKFX_KEY_TYPE_MULTIMEDIA_MUTE_MICROPHONE, RYOS_KEY_COMBO_BOX_GROUP_KEY);
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

	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("Browser"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Search"), RYOS_KEY_TYPE_WWW_SEARCH, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Home"), RYOS_KEY_TYPE_WWW_HOME, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Back"), RYOS_KEY_TYPE_WWW_BACK, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Forward"), RYOS_KEY_TYPE_WWW_FORWARD, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Stop"), RYOS_KEY_TYPE_WWW_STOP, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Refresh"), RYOS_KEY_TYPE_WWW_REFRESH, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Favorites"), RYOS_KEY_TYPE_WWW_FAVORITES, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("New tab"), RYOSMKFX_KEY_TYPE_BROWSER_NEW_TAB, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("New window"), RYOSMKFX_KEY_TYPE_BROWSER_NEW_WINDOW, RYOS_KEY_COMBO_BOX_GROUP_KEY);

	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("Open"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, RYOS_KEY_COMBO_BOX_GROUP_KEY | RYOS_KEY_COMBO_BOX_GROUP_EXTENDED);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Driver"), RYOS_KEY_TYPE_OPEN_DRIVER, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Application"), RYOS_KEY_TYPE_QUICKLAUNCH, RYOS_KEY_COMBO_BOX_GROUP_EXTENDED);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Document"), RYOSMKFX_KEY_TYPE_OPEN_DOCUMENT, RYOS_KEY_COMBO_BOX_GROUP_EXTENDED);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Folder"), RYOSMKFX_KEY_TYPE_OPEN_FOLDER, RYOS_KEY_COMBO_BOX_GROUP_EXTENDED);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Website"), RYOSMKFX_KEY_TYPE_OPEN_WEBSITE, RYOS_KEY_COMBO_BOX_GROUP_EXTENDED);

	roccat_key_combo_box_store_append(store, &sublevel, &toplevel, _("Folder presets"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Desktop"), RYOSMKFX_KEY_TYPE_FOLDER_DESKTOP, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Favorites"), RYOSMKFX_KEY_TYPE_FOLDER_FAVORITES, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Fonts"), RYOSMKFX_KEY_TYPE_FOLDER_FONTS, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("My documents"), RYOSMKFX_KEY_TYPE_FOLDER_MY_DOCUMENTS, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("My downloads"), RYOSMKFX_KEY_TYPE_FOLDER_MY_DOWNLOADS, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("My music"), RYOSMKFX_KEY_TYPE_FOLDER_MY_MUSIC, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("My pictures"), RYOSMKFX_KEY_TYPE_FOLDER_MY_PICTURES, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Network"), RYOSMKFX_KEY_TYPE_FOLDER_MY_NETWORK, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Printers"), RYOSMKFX_KEY_TYPE_FOLDER_PRINTERS, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("System"), RYOSMKFX_KEY_TYPE_FOLDER_SYSTEM, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Computer"), RYOS_KEY_TYPE_APP_MY_COMPUTER, RYOS_KEY_COMBO_BOX_GROUP_KEY);

	roccat_key_combo_box_store_append(store, &sublevel, &toplevel, _("Software presets"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("E-Mail"), RYOS_KEY_TYPE_APP_EMAIL, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Calculator"), RYOS_KEY_TYPE_APP_CALCULATOR, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Webbrowser"), RYOS_KEY_TYPE_APP_BROWSER, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Command line"), RYOSMKFX_KEY_TYPE_APP_COMMANDLINE, RYOS_KEY_COMBO_BOX_GROUP_KEY);

	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("System"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Hibernate"), RYOSMKFX_KEY_TYPE_SYSTEM_HIBERNATE, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Reboot"), RYOSMKFX_KEY_TYPE_SYSTEM_REBOOT, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Lock"), RYOSMKFX_KEY_TYPE_SYSTEM_LOCK, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Log off"), RYOSMKFX_KEY_TYPE_SYSTEM_LOGOFF, RYOS_KEY_COMBO_BOX_GROUP_KEY);

	roccat_key_combo_box_store_append(store, &sublevel, &toplevel, _("Windows function"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Control panel"), RYOSMKFX_KEY_TYPE_SYSTEM_CONTROL_PANEL, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("System panel"), RYOSMKFX_KEY_TYPE_SYSTEM_SYSTEM_PANEL, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Task manager"), RYOSMKFX_KEY_TYPE_SYSTEM_TASK_MANAGER, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Screen settings"), RYOSMKFX_KEY_TYPE_SYSTEM_SCREEN_SETTINGS, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Screensaver"), RYOSMKFX_KEY_TYPE_SYSTEM_SCREEN_SAVER, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Themes"), RYOSMKFX_KEY_TYPE_SYSTEM_THEMES, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Date & time"), RYOSMKFX_KEY_TYPE_SYSTEM_DATE_TIME, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Network settings"), RYOSMKFX_KEY_TYPE_SYSTEM_NETWORK_SETTINGS, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Admin panel"), RYOSMKFX_KEY_TYPE_SYSTEM_ADMIN_PANEL, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Firewall"), RYOSMKFX_KEY_TYPE_SYSTEM_WINDOWS_FIREWALL, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Regedit"), RYOSMKFX_KEY_TYPE_SYSTEM_REGEDIT, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Event monitor"), RYOSMKFX_KEY_TYPE_SYSTEM_EVENT_MONITOR, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Performance monitor"), RYOSMKFX_KEY_TYPE_SYSTEM_PERFORMANCE_MONITOR, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Sound settings"), RYOSMKFX_KEY_TYPE_SYSTEM_SOUND_SETTINGS, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Internet settings"), RYOSMKFX_KEY_TYPE_SYSTEM_INTERNET_SETTINGS, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("DirectX diagnosis"), RYOSMKFX_KEY_TYPE_SYSTEM_DIRECTX_DIAGNOSTICS, RYOS_KEY_COMBO_BOX_GROUP_KEY);

	roccat_key_combo_box_store_append(store, NULL, NULL, _("Shortcut"), RYOS_KEY_TYPE_SHORTCUT, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, NULL, _("Macro"), RYOS_KEY_TYPE_MACRO, RYOS_KEY_COMBO_BOX_GROUP_EXTENDED);
	roccat_key_combo_box_store_append(store, NULL, NULL, _("Macro play/pause"), RYOS_KEY_TYPE_MACRO_PLAY_PAUSE, RYOS_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, NULL, _("LED Macro"), RYOS_KEY_TYPE_LED_MACRO, RYOS_KEY_COMBO_BOX_GROUP_EXTENDED);

	roccat_key_combo_box_store_append(store, NULL, NULL, _("Standard"), RYOS_KEY_COMBO_BOX_TYPE_STANDARD, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, NULL, _("Remap"), RYOS_KEY_COMBO_BOX_TYPE_REMAP, RYOS_KEY_COMBO_BOX_GROUP_PRIMARY);
	roccat_key_combo_box_store_append(store, NULL, NULL, _("Disabled"), RYOS_KEY_TYPE_DISABLED, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	return GTK_TREE_MODEL(store);
}

static void ryosmkfx_key_combo_box_class_init(RyosmkfxKeyComboBoxClass *klass) {
	g_type_class_add_private(klass, sizeof(RyosmkfxKeyComboBoxPrivate));
	klass->model = key_info_model_new();

	signals[KEY_CHANGED] = g_signal_new("key-changed",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);
}
