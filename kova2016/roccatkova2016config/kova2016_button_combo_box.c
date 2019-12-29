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
#include <gaminggear/hid_uid.h>
#include <gaminggear/gaminggear_text_dialog.h>
#include "kova2016_button_combo_box.h"
#include "kova2016_profile_button.h"
#include "kova2016_cpi_dialog.h"
#include "kova2016_profile_settings.h"
#include "roccat_talk_target_dialog.h"
#include "roccat_shortcut_dialog.h"
#include "roccat_timer_dialog.h"
#include "roccat_warning_dialog.h"
#include "roccat_talk.h"
#include "roccat_helper.h"
#include "gtk_roccat_helper.h"
#include "g_cclosure_roccat_marshaller.h"
#include "i18n.h"

#define KOVA2016_BUTTON_COMBO_BOX_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), KOVA2016_BUTTON_COMBO_BOX_TYPE, Kova2016ButtonComboBoxClass))
#define IS_KOVA2016_BUTTON_COMBO_BOX_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), KOVA2016_BUTTON_COMBO_BOX_TYPE))
#define KOVA2016_BUTTON_COMBO_BOX_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), KOVA2016_BUTTON_COMBO_BOX_TYPE, Kova2016ButtonComboBoxClass))
#define KOVA2016_BUTTON_COMBO_BOX_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), KOVA2016_BUTTON_COMBO_BOX_TYPE, Kova2016ButtonComboBoxPrivate))

typedef struct _Kova2016ButtonComboBoxClass Kova2016ButtonComboBoxClass;
typedef struct _Kova2016ButtonComboBoxPrivate Kova2016ButtonComboBoxPrivate;

struct _Kova2016ButtonComboBox {
	RoccatKeyComboBox parent;
	Kova2016ButtonComboBoxPrivate *priv;
};

struct _Kova2016ButtonComboBoxClass {
	RoccatKeyComboBoxClass parent_class;
	GtkTreeModel *model;
};

struct _Kova2016ButtonComboBoxPrivate {
	Kova2016ButtonComboBoxData *data;
	gulong changed_handler_id;
};

G_DEFINE_TYPE(Kova2016ButtonComboBox, kova2016_button_combo_box, ROCCAT_KEY_COMBO_BOX_TYPE);

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

static gchar *kova2016_button_combo_box_get_text_for_data(Kova2016ButtonComboBox *button_combo_box, Kova2016ButtonComboBoxData const *data) {
	gchar *text;
	gchar *temp;
	guint type;

	type = data->button.type;

	switch (type) {
	case KOVA2016_BUTTON_TYPE_SHORTCUT:
		gtk_widget_set_tooltip_text(GTK_WIDGET(button_combo_box), NULL);
		temp = roccat_shortcut_dialog_create_name(data->button.modifier, data->button.key);
		text = g_strdup_printf(_("Shortcut: %s"), temp);
		g_free(temp);
		break;
	case KOVA2016_BUTTON_TYPE_MACRO:
		gtk_widget_set_tooltip_text(GTK_WIDGET(button_combo_box), NULL);
		text = g_strdup_printf(_("Macro: %s/%s"), data->macro.macroset_name, data->macro.macro_name);
		break;
	case KOVA2016_BUTTON_TYPE_OPEN_APPLICATION:
		gtk_widget_set_tooltip_text(GTK_WIDGET(button_combo_box), data->opener);
		temp = g_path_get_basename(data->opener);
		text = g_strdup_printf(_("Application: %s"), temp);
		g_free(temp);
		break;
	case KOVA2016_BUTTON_TYPE_OPEN_DOCUMENT:
		gtk_widget_set_tooltip_text(GTK_WIDGET(button_combo_box), data->opener);
		temp = g_path_get_basename(data->opener);
		text = g_strdup_printf(_("Document: %s"), temp);
		g_free(temp);
		break;
	case KOVA2016_BUTTON_TYPE_OPEN_FOLDER:
		gtk_widget_set_tooltip_text(GTK_WIDGET(button_combo_box), data->opener);
		temp = g_path_get_basename(data->opener);
		text = g_strdup_printf(_("Folder: %s"), temp);
		g_free(temp);
		break;
	case KOVA2016_BUTTON_TYPE_OPEN_WEBSITE:
		gtk_widget_set_tooltip_text(GTK_WIDGET(button_combo_box), data->opener);
		temp = get_host(data->opener);
		text = g_strdup_printf(_("Website: %s"), temp);
		g_free(temp);
		break;
	case KOVA2016_BUTTON_TYPE_TIMER:
		gtk_widget_set_tooltip_text(GTK_WIDGET(button_combo_box), NULL);
		text = g_strdup_printf(_("Timer: %s"), (gchar const *)data->timer.name);
		break;
	case KOVA2016_BUTTON_TYPE_EASYAIM_CUSTOM:
		gtk_widget_set_tooltip_text(GTK_WIDGET(button_combo_box), NULL);
		text = g_strdup_printf(_("Easyaim: %i CPI"), kova2016_profile_settings_cpi_level_to_cpi(data->button.modifier));
		break;
	case KOVA2016_BUTTON_TYPE_EASYSHIFT_OTHER:
		text = g_strdup_printf(_("Easyshift: %s"), roccat_talk_device_get_text_static(data->talk_target));
		break;
	case KOVA2016_BUTTON_TYPE_EASYSHIFT_LOCK_OTHER:
		text = g_strdup_printf(_("Easyshift lock: %s"), roccat_talk_device_get_text_static(data->talk_target));
		break;
	default:
		gtk_widget_set_tooltip_text(GTK_WIDGET(button_combo_box), NULL);
		text = roccat_key_combo_box_model_get_title_for_type(gtk_combo_box_get_model(GTK_COMBO_BOX(button_combo_box)), type);
		break;
	}

	return text;
}

void kova2016_button_combo_box_update(Kova2016ButtonComboBox *button_combo_box) {
	Kova2016ButtonComboBoxPrivate *priv = button_combo_box->priv;
	gchar *text;

	text = kova2016_button_combo_box_get_text_for_data(button_combo_box, priv->data);

	g_signal_handler_block(G_OBJECT(button_combo_box), priv->changed_handler_id);
	roccat_key_combo_box_set_text(ROCCAT_KEY_COMBO_BOX(button_combo_box), text);
	g_signal_handler_unblock(G_OBJECT(button_combo_box), priv->changed_handler_id);

	g_free(text);
}

static void kova2016_button_combo_box_data_set_opener(Kova2016ButtonComboBoxData *data, gchar const *new_opener) {
	g_strlcpy(data->opener, new_opener, ROCCAT_SWARM_RMP_OPENER_LENGTH);
}

static gboolean key_open_application(Kova2016ButtonComboBox *button_combo_box) {
	Kova2016ButtonComboBoxPrivate *priv = button_combo_box->priv;
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
		roccat_button_set_to_normal(&priv->data->button, KOVA2016_BUTTON_TYPE_OPEN_APPLICATION);
		path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
		kova2016_button_combo_box_data_set_opener(priv->data, path);
		g_free(path);
		retval = TRUE;
	}
	gtk_widget_destroy(dialog);
	return retval;
}

static gboolean key_open_document(Kova2016ButtonComboBox *button_combo_box) {
	Kova2016ButtonComboBoxPrivate *priv = button_combo_box->priv;
	GtkWidget *dialog;
	gboolean retval = FALSE;
	gchar *path;

	dialog = gtk_file_chooser_dialog_new(_("Select document"),
			gtk_roccat_widget_get_gtk_window(GTK_WIDGET(button_combo_box)),
			GTK_FILE_CHOOSER_ACTION_OPEN,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
			NULL);

	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
		roccat_button_set_to_normal(&priv->data->button, KOVA2016_BUTTON_TYPE_OPEN_DOCUMENT);
		path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
		kova2016_button_combo_box_data_set_opener(priv->data, path);
		g_free(path);
		retval = TRUE;
	}
	gtk_widget_destroy(dialog);
	return retval;
}

static gboolean key_open_folder(Kova2016ButtonComboBox *button_combo_box) {
	Kova2016ButtonComboBoxPrivate *priv = button_combo_box->priv;
	GtkWidget *dialog;
	gboolean retval = FALSE;
	gchar *path;

	dialog = gtk_file_chooser_dialog_new(_("Select folder"),
			gtk_roccat_widget_get_gtk_window(GTK_WIDGET(button_combo_box)),
			GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
			NULL);

	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
		roccat_button_set_to_normal(&priv->data->button, KOVA2016_BUTTON_TYPE_OPEN_FOLDER);
		path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
		kova2016_button_combo_box_data_set_opener(priv->data, path);
		g_free(path);
		retval = TRUE;
	}
	gtk_widget_destroy(dialog);
	return retval;
}

static gboolean key_open_website(Kova2016ButtonComboBox *button_combo_box) {
	Kova2016ButtonComboBoxPrivate *priv = button_combo_box->priv;
	gchar *url;

	url = gaminggear_text_dialog(gtk_roccat_widget_get_gtk_window(GTK_WIDGET(button_combo_box)),
			_("URL dialog"), _("Please enter any complete url with scheme"), NULL);

	if (url) {
		roccat_button_set_to_normal(&priv->data->button, KOVA2016_BUTTON_TYPE_OPEN_WEBSITE);
		kova2016_button_combo_box_data_set_opener(priv->data, url);
		g_free(url);
		return TRUE;
	}
	return FALSE;
}

static gboolean set_macro(Kova2016ButtonComboBox *button_combo_box) {
	Kova2016ButtonComboBoxPrivate *priv = button_combo_box->priv;
	GtkWindow *parent;
	GaminggearMacro *gaminggear_macro;
	Kova2016Macro *kova2016_macro;
	gboolean result;
	GError *local_error = NULL;

	parent = gtk_roccat_widget_get_gtk_window(GTK_WIDGET(button_combo_box));
	gaminggear_macro = gaminggear_macro_dialog(parent);
	if (!gaminggear_macro)
		return FALSE;

	kova2016_macro = (Kova2016Macro *)g_malloc0(sizeof(Kova2016Macro));
	result = gaminggear_macro_to_kova2016_macro(gaminggear_macro, kova2016_macro, &local_error);
	gaminggear_macro_free(gaminggear_macro);
	if (!result) {
		roccat_handle_error_dialog(parent, _("Could not convert macro"), &local_error);
		g_free(kova2016_macro);
		return FALSE;
	}

	kova2016_macro_copy(&priv->data->macro, kova2016_macro);
	g_free(kova2016_macro);

	return TRUE;
}

static gboolean key_macro(Kova2016ButtonComboBox *button_combo_box) {
	Kova2016ButtonComboBoxPrivate *priv = button_combo_box->priv;
	gboolean retval;

	retval = set_macro(button_combo_box);

	if (retval)
		roccat_button_set_to_normal(&priv->data->button, KOVA2016_BUTTON_TYPE_MACRO);

	return retval;
}

static gboolean key_shortcut(Kova2016ButtonComboBox *button_combo_box) {
	Kova2016ButtonComboBoxPrivate *priv = button_combo_box->priv;
	RoccatShortcutDialog *dialog;
	gboolean retval = FALSE;

	dialog = ROCCAT_SHORTCUT_DIALOG(roccat_shortcut_dialog_new(gtk_roccat_widget_get_gtk_window(GTK_WIDGET(button_combo_box))));
	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
		roccat_button_set_to_shortcut(&priv->data->button, KOVA2016_BUTTON_TYPE_SHORTCUT,
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

static gboolean key_timer(Kova2016ButtonComboBox *button_combo_box) {
	Kova2016ButtonComboBoxPrivate *priv = button_combo_box->priv;
	RoccatTimer *roccat_timer;
	gboolean retval = FALSE;

	roccat_timer = roccat_timer_dialog(gtk_roccat_widget_get_gtk_window(GTK_WIDGET(button_combo_box)));
	if (roccat_timer) {
		roccat_button_set_to_normal(&priv->data->button, KOVA2016_BUTTON_TYPE_TIMER);
		roccat_timer_copy(&priv->data->timer, roccat_timer);
		g_free(roccat_timer);
		retval = TRUE;
	}
	return retval;
}

static gboolean key_easyaim_custom(Kova2016ButtonComboBox *button_combo_box) {
	Kova2016ButtonComboBoxPrivate *priv = button_combo_box->priv;
	gint level;
	gboolean retval = FALSE;
	
	level = kova2016_cpi_dialog(gtk_roccat_widget_get_gtk_window(GTK_WIDGET(button_combo_box)));
	if (level != -1) {
		priv->data->button.type = KOVA2016_BUTTON_TYPE_EASYAIM_CUSTOM;
		priv->data->button.modifier = kova2016_profile_settings_cpi_to_cpi_level(level);
		priv->data->button.key = 0;
		retval = TRUE;
	}
	return retval;
}

static gboolean key_normal(Kova2016ButtonComboBox *button_combo_box, guint type) {
	gboolean retval = TRUE;

	if (button_combo_box->priv->data->button.type == type)
		retval = FALSE;

	roccat_button_set_to_normal(&button_combo_box->priv->data->button, type);

	return retval;
}

static gboolean key_talk_other(Kova2016ButtonComboBox *button_combo_box, gint type) {
	Kova2016ButtonComboBoxPrivate *priv = button_combo_box->priv;
	RoccatTalkTargetDialog *talk_target_dialog;
	gboolean retval = FALSE;

	talk_target_dialog = ROCCAT_TALK_TARGET_DIALOG(roccat_talk_target_dialog_new(
			gtk_roccat_widget_get_gtk_window(GTK_WIDGET(button_combo_box)), ROCCAT_TALK_DEVICE_MOUSE));

	roccat_talk_target_dialog_set_value(talk_target_dialog, priv->data->talk_target);

	if (gtk_dialog_run(GTK_DIALOG(talk_target_dialog)) == GTK_RESPONSE_ACCEPT) {
		priv->data->talk_target = roccat_talk_target_dialog_get_value(talk_target_dialog);
		roccat_button_set_to_normal(&priv->data->button, type);
		retval = TRUE;
	}

	gtk_widget_destroy(GTK_WIDGET(talk_target_dialog));

	return retval;
}

static void changed_cb(GtkComboBox *combo, gpointer user_data) {
	Kova2016ButtonComboBox *button_combo_box = KOVA2016_BUTTON_COMBO_BOX(combo);
	guint type;
	gboolean changed;

	type = roccat_key_combo_box_get_active_type(ROCCAT_KEY_COMBO_BOX(combo));
	if (type == ROCCAT_KEY_COMBO_BOX_TYPE_NONE)
		return;

	switch (type) {
	case KOVA2016_BUTTON_TYPE_OPEN_APPLICATION:
		changed = key_open_application(button_combo_box);
		break;
	case KOVA2016_BUTTON_TYPE_OPEN_DOCUMENT:
		changed = key_open_document(button_combo_box);
		break;
	case KOVA2016_BUTTON_TYPE_OPEN_FOLDER:
		changed = key_open_folder(button_combo_box);
		break;
	case KOVA2016_BUTTON_TYPE_OPEN_WEBSITE:
		changed = key_open_website(button_combo_box);
		break;
	case KOVA2016_BUTTON_TYPE_MACRO:
		changed = key_macro(button_combo_box);
		break;
	case KOVA2016_BUTTON_TYPE_SHORTCUT:
		changed = key_shortcut(button_combo_box);
		break;
	case KOVA2016_BUTTON_TYPE_TIMER:
		changed = key_timer(button_combo_box);
		break;
	case KOVA2016_BUTTON_TYPE_EASYAIM_CUSTOM:
		changed = key_easyaim_custom(button_combo_box);
		break;
	case KOVA2016_BUTTON_TYPE_EASYSHIFT_OTHER:
	case KOVA2016_BUTTON_TYPE_EASYSHIFT_LOCK_OTHER:
		changed = key_talk_other(button_combo_box, type);
		break;
	case KOVA2016_BUTTON_TYPE_EASYSHIFT_ALL:
	default:
		changed = key_normal(button_combo_box, type);
		break;
	}

	kova2016_button_combo_box_update(button_combo_box);
	if (changed)
		g_signal_emit((gpointer)button_combo_box, signals[KEY_CHANGED], 0);
}

void kova2016_button_combo_box_set_data_pointer(Kova2016ButtonComboBox *button_combo_box, Kova2016ButtonComboBoxData *data) {
	button_combo_box->priv->data = data;
	kova2016_button_combo_box_update(button_combo_box);
}

Kova2016ButtonComboBoxData *kova2016_button_combo_box_get_data_pointer(Kova2016ButtonComboBox *button_combo_box) {
	return button_combo_box->priv->data;
}

GtkWidget *kova2016_button_combo_box_new(guint mask) {
	Kova2016ButtonComboBox *button_combo_box;
	Kova2016ButtonComboBoxClass *button_combo_box_class;
	Kova2016ButtonComboBoxPrivate *priv;

	button_combo_box = KOVA2016_BUTTON_COMBO_BOX(g_object_new(KOVA2016_BUTTON_COMBO_BOX_TYPE,
			"exclude-mask", mask,
			NULL));

	priv = button_combo_box->priv;

	button_combo_box_class = KOVA2016_BUTTON_COMBO_BOX_GET_CLASS(button_combo_box);
	roccat_key_combo_box_set_model(ROCCAT_KEY_COMBO_BOX(button_combo_box), button_combo_box_class->model);

	priv->changed_handler_id = g_signal_connect(G_OBJECT(button_combo_box), "changed", G_CALLBACK(changed_cb), NULL);

	return GTK_WIDGET(button_combo_box);
}

static void kova2016_button_combo_box_init(Kova2016ButtonComboBox *button_combo_box) {
	Kova2016ButtonComboBoxPrivate *priv = KOVA2016_BUTTON_COMBO_BOX_GET_PRIVATE(button_combo_box);
	button_combo_box->priv = priv;
}

static GtkTreeModel *button_info_model_new(void) {
	GtkTreeStore *store;
	GtkTreeIter toplevel, sublevel;

	store = roccat_key_combo_box_store_new();


	roccat_key_combo_box_store_append(store, NULL, NULL, _("Macro"), KOVA2016_BUTTON_TYPE_MACRO, KOVA2016_BUTTON_COMBO_BOX_GROUP_EXTENDED);
	roccat_key_combo_box_store_append(store, NULL, NULL, _("Shortcut"), KOVA2016_BUTTON_TYPE_SHORTCUT, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("Timer"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE | KOVA2016_BUTTON_COMBO_BOX_GROUP_EXTENDED);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Timer start"), KOVA2016_BUTTON_TYPE_TIMER, KOVA2016_BUTTON_COMBO_BOX_GROUP_EXTENDED);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Timer stop"), KOVA2016_BUTTON_TYPE_TIMER_STOP, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("Basic functions"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE | ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Button 1 (Click)"), KOVA2016_BUTTON_TYPE_CLICK, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Button 2 (Universal scrolling)"), KOVA2016_BUTTON_TYPE_UNIVERSAL_SCROLLING, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Button 3 (Menu)"), KOVA2016_BUTTON_TYPE_MENU, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Button 4 (Scroll up)"), KOVA2016_BUTTON_TYPE_SCROLL_UP, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Button 5 (Scroll down)"), KOVA2016_BUTTON_TYPE_SCROLL_DOWN, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Button 6 (Tilt left)"), KOVA2016_BUTTON_TYPE_TILT_LEFT, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Button 7 (Tilt right)"), KOVA2016_BUTTON_TYPE_TILT_RIGHT, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Button 8 (Browser backward)"), KOVA2016_BUTTON_TYPE_BROWSER_BACKWARD, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Button 9 (Browser forward)"), KOVA2016_BUTTON_TYPE_BROWSER_FORWARD, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("*Double click"), KOVA2016_BUTTON_TYPE_DOUBLE_CLICK, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Home"), KOVA2016_BUTTON_TYPE_HOME, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("End"), KOVA2016_BUTTON_TYPE_END, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Page up"), KOVA2016_BUTTON_TYPE_PAGE_UP, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Page down"), KOVA2016_BUTTON_TYPE_PAGE_DOWN, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Left ctrl"), KOVA2016_BUTTON_TYPE_LEFT_CTRL, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Left shift"), KOVA2016_BUTTON_TYPE_LEFT_SHIFT, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Left alt"), KOVA2016_BUTTON_TYPE_LEFT_ALT, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Easyshift"), KOVA2016_BUTTON_TYPE_EASYSHIFT_SELF, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT);
	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("Advanced functions"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("CPI cycle"), KOVA2016_BUTTON_TYPE_CPI_CYCLE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("CPI up"), KOVA2016_BUTTON_TYPE_CPI_UP, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("CPI down"), KOVA2016_BUTTON_TYPE_CPI_DOWN, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Profile cycle"), KOVA2016_BUTTON_TYPE_PROFILE_CYCLE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Profile up"), KOVA2016_BUTTON_TYPE_PROFILE_UP, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Profile down"), KOVA2016_BUTTON_TYPE_PROFILE_DOWN, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, &sublevel, &toplevel, _("Profile presets"), KOVA2016_BUTTON_TYPE_PROFILE_DOWN, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Profile 1"), KOVA2016_BUTTON_TYPE_PROFILE_1, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Profile 2"), KOVA2016_BUTTON_TYPE_PROFILE_2, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Profile 3"), KOVA2016_BUTTON_TYPE_PROFILE_3, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Profile 4"), KOVA2016_BUTTON_TYPE_PROFILE_4, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Profile 5"), KOVA2016_BUTTON_TYPE_PROFILE_5, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, &sublevel, &toplevel, _("Easyaim"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Easyaim 200 CPI"), KOVA2016_BUTTON_TYPE_EASYAIM_200, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Easyaim Setting 1"), KOVA2016_BUTTON_TYPE_EASYAIM_1, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Easyaim Setting 2"), KOVA2016_BUTTON_TYPE_EASYAIM_2, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Easyaim Setting 3"), KOVA2016_BUTTON_TYPE_EASYAIM_3, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Easyaim Setting 4"), KOVA2016_BUTTON_TYPE_EASYAIM_4, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Easyaim Setting 5"), KOVA2016_BUTTON_TYPE_EASYAIM_5, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Easyaim Custom"), KOVA2016_BUTTON_TYPE_EASYAIM_CUSTOM, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT);
	roccat_key_combo_box_store_append(store, &sublevel, &toplevel, _("Easywheel"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Easywheel Profile"), KOVA2016_BUTTON_TYPE_EASYWHEEL_PROFILE, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Easywheel CPI"), KOVA2016_BUTTON_TYPE_EASYWHEEL_CPI, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Easywheel Volume"), KOVA2016_BUTTON_TYPE_EASYWHEEL_VOLUME, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Easywheel Alt Tab"), KOVA2016_BUTTON_TYPE_EASYWHEEL_ALT_TAB, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Easywheel Aero Flip 3D"), KOVA2016_BUTTON_TYPE_EASYWHEEL_AERO_FLIP_3D, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT);
	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("Multimedia"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Mute master volume"), KOVA2016_BUTTON_TYPE_MUTE_MASTER, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Volume up"), KOVA2016_BUTTON_TYPE_VOLUME_UP, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Volume down"), KOVA2016_BUTTON_TYPE_VOLUME_DOWN, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Previous track"), KOVA2016_BUTTON_TYPE_PREV_TRACK, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Next track"), KOVA2016_BUTTON_TYPE_NEXT_TRACK, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Play/Pause"), KOVA2016_BUTTON_TYPE_PLAY_PAUSE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Stop"), KOVA2016_BUTTON_TYPE_STOP, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("Open"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE | KOVA2016_BUTTON_COMBO_BOX_GROUP_EXTENDED);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Driver"), KOVA2016_BUTTON_TYPE_OPEN_DRIVER, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Application"), KOVA2016_BUTTON_TYPE_OPEN_APPLICATION, KOVA2016_BUTTON_COMBO_BOX_GROUP_EXTENDED);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("*Document"), KOVA2016_BUTTON_TYPE_OPEN_DOCUMENT, KOVA2016_BUTTON_COMBO_BOX_GROUP_EXTENDED);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("*Folder"), KOVA2016_BUTTON_TYPE_OPEN_FOLDER, KOVA2016_BUTTON_COMBO_BOX_GROUP_EXTENDED);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("*Website"), KOVA2016_BUTTON_TYPE_OPEN_WEBSITE, KOVA2016_BUTTON_COMBO_BOX_GROUP_EXTENDED);
	roccat_key_combo_box_store_append(store, &sublevel, &toplevel, _("*Folder presets"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("*Computer"), KOVA2016_BUTTON_TYPE_OPEN_FOLDER_COMPUTER, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, &sublevel, &toplevel, _("*Software presets"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("*Calculator"), KOVA2016_BUTTON_TYPE_OPEN_APPLICATION_CALCULATOR, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("*E-Mail"), KOVA2016_BUTTON_TYPE_OPEN_APPLICATION_EMAIL, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("Talk"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Other Easyshift"), KOVA2016_BUTTON_TYPE_EASYSHIFT_OTHER, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Other Easyshift lock"), KOVA2016_BUTTON_TYPE_EASYSHIFT_LOCK_OTHER, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Both Easyshift"), KOVA2016_BUTTON_TYPE_EASYSHIFT_ALL, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT);
	roccat_key_combo_box_store_append(store, NULL, NULL, _("Disabled"), KOVA2016_BUTTON_TYPE_DISABLED, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	return GTK_TREE_MODEL(store);
}

static void kova2016_button_combo_box_class_init(Kova2016ButtonComboBoxClass *klass) {
	g_type_class_add_private(klass, sizeof(Kova2016ButtonComboBoxPrivate));
	klass->model = button_info_model_new();

	signals[KEY_CHANGED] = g_signal_new("key-changed",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);
}

void kova2016_button_combo_box_data_copy(Kova2016ButtonComboBoxData *destination, Kova2016ButtonComboBoxData const *source) {
	roccat_button_copy(&destination->button, &source->button);
	kova2016_macro_copy(&destination->macro, &source->macro);
	roccat_timer_copy(&destination->timer, &source->timer);
	g_strlcpy(destination->opener, source->opener, ROCCAT_SWARM_RMP_OPENER_LENGTH);
}

void kova2016_button_combo_box_data_swap(Kova2016ButtonComboBoxData *one, Kova2016ButtonComboBoxData *other) {
	Kova2016ButtonComboBoxData temp;
	kova2016_button_combo_box_data_copy(&temp, one);
	kova2016_button_combo_box_data_copy(one, other);
	kova2016_button_combo_box_data_copy(other, &temp);
}
