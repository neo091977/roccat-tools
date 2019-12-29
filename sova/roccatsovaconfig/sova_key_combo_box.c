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

#include "sova_key_combo_box.h"
#include "sova.h"
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

#define SOVA_KEY_COMBO_BOX_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), SOVA_KEY_COMBO_BOX_TYPE, SovaKeyComboBoxClass))
#define IS_SOVA_KEY_COMBO_BOX_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), SOVA_KEY_COMBO_BOX_TYPE))
#define SOVA_KEY_COMBO_BOX_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), SOVA_KEY_COMBO_BOX_TYPE, SovaKeyComboBoxClass))
#define SOVA_KEY_COMBO_BOX_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), SOVA_KEY_COMBO_BOX_TYPE, SovaKeyComboBoxPrivate))

typedef struct _SovaKeyComboBoxClass SovaKeyComboBoxClass;
typedef struct _SovaKeyComboBoxPrivate SovaKeyComboBoxPrivate;

struct _SovaKeyComboBox {
	RoccatKeyComboBox parent;
	SovaKeyComboBoxPrivate *priv;
};

struct _SovaKeyComboBoxClass {
	RoccatKeyComboBoxClass parent_class;
	GtkTreeModel *model;
};

struct _SovaKeyComboBoxPrivate {
	SovaKeyComboBoxData *data;
	guint standard;
	gulong changed_handler_id;
};

G_DEFINE_TYPE(SovaKeyComboBox, sova_key_combo_box, ROCCAT_KEY_COMBO_BOX_TYPE);

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

gchar *sova_key_combo_box_get_text_for_data(SovaKeyComboBox *key_combo_box, SovaKeyComboBoxData const *data) {
	gchar *text;
	gchar *temp;
	guint type;

	type = data->key.type;

	switch (type) {
	case SOVA_KEY_TYPE_LIGHT:
		text = g_strdup(_("Light"));
		break;
	case SOVA_KEY_TYPE_RECORD:
		text = g_strdup(_("Live record"));
		break;
	case SOVA_KEY_TYPE_SHORTCUT:
		gtk_widget_set_tooltip_text(GTK_WIDGET(key_combo_box), NULL);
		temp = roccat_shortcut_dialog_create_name(data->key.modifier, data->key.key);
		text = g_strdup_printf(_("Shortcut: %s"), temp);
		g_free(temp);
		break;
	case SOVA_KEY_TYPE_MACRO:
		gtk_widget_set_tooltip_text(GTK_WIDGET(key_combo_box), NULL);
		text = g_strdup_printf(_("Macro: %s/%s"), sova_macro_get_macroset_name(&data->macro), sova_macro_get_macro_name(&data->macro));
		break;
	case SOVA_KEY_TYPE_OPEN_APP:
		gtk_widget_set_tooltip_text(GTK_WIDGET(key_combo_box), data->opener);
		temp = g_path_get_basename(data->opener);
		text = g_strdup_printf(_("Application: %s"), temp);
		g_free(temp);
		break;
	case SOVA_KEY_TYPE_OPEN_DOCUMENT:
		gtk_widget_set_tooltip_text(GTK_WIDGET(key_combo_box), data->opener);
		temp = g_path_get_basename(data->opener);
		text = g_strdup_printf(_("Document: %s"), temp);
		g_free(temp);
		break;
	case SOVA_KEY_TYPE_OPEN_FOLDER:
		gtk_widget_set_tooltip_text(GTK_WIDGET(key_combo_box), data->opener);
		temp = g_path_get_basename(data->opener);
		text = g_strdup_printf(_("Folder: %s"), temp);
		g_free(temp);
		break;
	case SOVA_KEY_TYPE_OPEN_WEBSITE:
		gtk_widget_set_tooltip_text(GTK_WIDGET(key_combo_box), data->opener);
		temp = get_host(data->opener);
		text = g_strdup_printf(_("Website: %s"), temp);
		g_free(temp);
		break;
	case SOVA_KEY_TYPE_TIMER:
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

void sova_key_combo_box_update(SovaKeyComboBox *key_combo_box) {
	SovaKeyComboBoxPrivate *priv = key_combo_box->priv;
	gchar *text;

	text = sova_key_combo_box_get_text_for_data(key_combo_box, priv->data);

	g_signal_handler_block(G_OBJECT(key_combo_box), priv->changed_handler_id);
	roccat_key_combo_box_set_text(ROCCAT_KEY_COMBO_BOX(key_combo_box), text);
	g_signal_handler_unblock(G_OBJECT(key_combo_box), priv->changed_handler_id);

	g_free(text);
}

static void sova_key_combo_box_data_set_opener(SovaKeyComboBoxData *data, gchar const *new_opener) {
	g_strlcpy(data->opener, new_opener, ROCCAT_SWARM_RMP_OPENER_LENGTH);
}

static gboolean key_open_application(SovaKeyComboBox *key_combo_box) {
	SovaKeyComboBoxPrivate *priv = key_combo_box->priv;
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
		roccat_button_set_to_normal(&priv->data->key, SOVA_KEY_TYPE_OPEN_APP);
		path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
		sova_key_combo_box_data_set_opener(priv->data, path);
		g_free(path);
		retval = TRUE;
	}
	gtk_widget_destroy(dialog);
	return retval;
}

static gboolean key_open_document(SovaKeyComboBox *key_combo_box) {
	SovaKeyComboBoxPrivate *priv = key_combo_box->priv;
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
		roccat_button_set_to_normal(&priv->data->key, SOVA_KEY_TYPE_OPEN_DOCUMENT);
		path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
		sova_key_combo_box_data_set_opener(priv->data, path);
		g_free(path);
		retval = TRUE;
	}
	gtk_widget_destroy(dialog);
	return retval;
}

static gboolean key_open_folder(SovaKeyComboBox *key_combo_box) {
	SovaKeyComboBoxPrivate *priv = key_combo_box->priv;
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
		roccat_button_set_to_normal(&priv->data->key, SOVA_KEY_TYPE_OPEN_FOLDER);
		path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
		sova_key_combo_box_data_set_opener(priv->data, path);
		g_free(path);
		retval = TRUE;
	}
	gtk_widget_destroy(dialog);
	return retval;
}

static gboolean key_open_website(SovaKeyComboBox *key_combo_box) {
	SovaKeyComboBoxPrivate *priv = key_combo_box->priv;
	gchar *url;

	url = gaminggear_text_dialog(gtk_roccat_widget_get_gtk_window(GTK_WIDGET(key_combo_box)),
			_("URL dialog"), _("Please enter any complete url with scheme"), NULL);

	if (url) {
		roccat_button_set_to_normal(&priv->data->key, SOVA_KEY_TYPE_OPEN_WEBSITE);
		sova_key_combo_box_data_set_opener(priv->data, url);
		g_free(url);
		return TRUE;
	}
	return FALSE;
}

static gboolean set_macro(SovaKeyComboBox *key_combo_box) {
	SovaKeyComboBoxPrivate *priv = key_combo_box->priv;
	GtkWindow *parent;
	GaminggearMacro *gaminggear_macro;
	SovaMacro *sova_macro;
	gboolean result;
	GError *local_error = NULL;

	parent = gtk_roccat_widget_get_gtk_window(GTK_WIDGET(key_combo_box));
	gaminggear_macro = gaminggear_macro_dialog(parent);
	if (!gaminggear_macro)
		return FALSE;

	sova_macro = (SovaMacro *)g_malloc0(sizeof(SovaMacro));
	result = gaminggear_macro_to_sova_macro(gaminggear_macro, sova_macro, &local_error);
	gaminggear_macro_free(gaminggear_macro);
	if (!result) {
		roccat_handle_error_dialog(parent, _("Could not convert macro"), &local_error);
		g_free(sova_macro);
		return FALSE;
	}

	sova_macro_copy(&priv->data->macro, sova_macro);
	g_free(sova_macro);

	return TRUE;
}

static gboolean key_macro(SovaKeyComboBox *key_combo_box) {
	SovaKeyComboBoxPrivate *priv = key_combo_box->priv;
	gboolean retval;

	retval = set_macro(key_combo_box);

	if (retval)
		roccat_button_set_to_normal(&priv->data->key, SOVA_KEY_TYPE_MACRO);

	return retval;
}

static gboolean key_shortcut(SovaKeyComboBox *key_combo_box) {
	SovaKeyComboBoxPrivate *priv = key_combo_box->priv;
	RoccatShortcutDialog *dialog;
	gboolean retval = FALSE;

	dialog = ROCCAT_SHORTCUT_DIALOG(roccat_shortcut_dialog_new(gtk_roccat_widget_get_gtk_window(GTK_WIDGET(key_combo_box))));
	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
		roccat_button_set_to_shortcut(&priv->data->key, SOVA_KEY_TYPE_SHORTCUT,
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

static gboolean key_timer(SovaKeyComboBox *key_combo_box) {
	SovaKeyComboBoxPrivate *priv = key_combo_box->priv;
	RoccatTimer *roccat_timer;
	gboolean retval = FALSE;

	roccat_timer = roccat_timer_dialog(gtk_roccat_widget_get_gtk_window(GTK_WIDGET(key_combo_box)));
	if (roccat_timer) {
		roccat_button_set_to_normal(&priv->data->key, SOVA_KEY_TYPE_TIMER);
		roccat_timer_copy(&priv->data->timer, roccat_timer);
		g_free(roccat_timer);
		retval = TRUE;
	}
	return retval;
}

static gboolean key_normal(SovaKeyComboBox *key_combo_box, guint type) {
	gboolean retval = TRUE;

	if (key_combo_box->priv->data->key.type == type)
		retval = FALSE;

	roccat_button_set_to_normal(&key_combo_box->priv->data->key, type);

	return retval;
}

static gboolean key_remap(SovaKeyComboBox *key_combo_box) {
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
	SovaKeyComboBox *key_combo_box = SOVA_KEY_COMBO_BOX(combo);
	guint type;
	gboolean changed;

	type = roccat_key_combo_box_get_active_type(ROCCAT_KEY_COMBO_BOX(combo));
	if (type == ROCCAT_KEY_COMBO_BOX_TYPE_NONE)
		return;

	if (type == SOVA_KEY_COMBO_BOX_TYPE_STANDARD)
		type = key_combo_box->priv->standard;

	switch (type) {
	case SOVA_KEY_COMBO_BOX_TYPE_REMAP:
		changed = key_remap(key_combo_box);
		break;
	case SOVA_KEY_TYPE_OPEN_APP:
		changed = key_open_application(key_combo_box);
		break;
	case SOVA_KEY_TYPE_OPEN_DOCUMENT:
		changed = key_open_document(key_combo_box);
		break;
	case SOVA_KEY_TYPE_OPEN_FOLDER:
		changed = key_open_folder(key_combo_box);
		break;
	case SOVA_KEY_TYPE_OPEN_WEBSITE:
		changed = key_open_website(key_combo_box);
		break;
	case SOVA_KEY_TYPE_MACRO:
		changed = key_macro(key_combo_box);
		break;
	case SOVA_KEY_TYPE_SHORTCUT:
		changed = key_shortcut(key_combo_box);
		break;
	case SOVA_KEY_TYPE_TIMER:
		changed = key_timer(key_combo_box);
		break;
	default:
		changed = key_normal(key_combo_box, type);
		break;
	}

	sova_key_combo_box_update(key_combo_box);
	if (changed)
		g_signal_emit((gpointer)key_combo_box, signals[KEY_CHANGED], 0);
}

void sova_key_combo_box_set_data_pointer(SovaKeyComboBox *key_combo_box, SovaKeyComboBoxData *data) {
	key_combo_box->priv->data = data;
	sova_key_combo_box_update(key_combo_box);
}

SovaKeyComboBoxData *sova_key_combo_box_get_data_pointer(SovaKeyComboBox *key_combo_box) {
	return key_combo_box->priv->data;
}

void sova_key_combo_box_set_standard(SovaKeyComboBox *key_combo_box, guint standard) {
	key_combo_box->priv->standard = standard;
}

GtkWidget *sova_key_combo_box_new(guint mask, guint standard) {
	SovaKeyComboBox *key_combo_box;
	SovaKeyComboBoxClass *key_combo_box_class;
	SovaKeyComboBoxPrivate *priv;

	key_combo_box = SOVA_KEY_COMBO_BOX(g_object_new(SOVA_KEY_COMBO_BOX_TYPE,
			"exclude-mask", mask,
			NULL));

	priv = key_combo_box->priv;

	sova_key_combo_box_set_standard(key_combo_box, standard);

	key_combo_box_class = SOVA_KEY_COMBO_BOX_GET_CLASS(key_combo_box);
	roccat_key_combo_box_set_model(ROCCAT_KEY_COMBO_BOX(key_combo_box), key_combo_box_class->model);

	priv->changed_handler_id = g_signal_connect(G_OBJECT(key_combo_box), "changed", G_CALLBACK(changed_cb), NULL);

	return GTK_WIDGET(key_combo_box);
}

static void sova_key_combo_box_init(SovaKeyComboBox *key_combo_box) {
	SovaKeyComboBoxPrivate *priv = SOVA_KEY_COMBO_BOX_GET_PRIVATE(key_combo_box);
	key_combo_box->priv = priv;
}

static GtkTreeModel *key_info_model_new(void) {
	GtkTreeStore *store;
	GtkTreeIter toplevel, sublevel;

	store = roccat_key_combo_box_store_new();


	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("Easyshift"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, SOVA_KEY_COMBO_BOX_GROUP_TALK);

	roccat_key_combo_box_store_append(store, &sublevel, &toplevel, _("Self"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, SOVA_KEY_COMBO_BOX_GROUP_TALK);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Self Easyshift"), SOVA_KEY_TYPE_EASYSHIFT, SOVA_KEY_COMBO_BOX_GROUP_TALK);

	roccat_key_combo_box_store_append(store, &sublevel, &toplevel, _("Both"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, SOVA_KEY_COMBO_BOX_GROUP_TALK);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Both Easyshift"), SOVA_KEY_TYPE_TALK_EASYSHIFT_ALL, SOVA_KEY_COMBO_BOX_GROUP_TALK);

	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("Multimedia"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, SOVA_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Open player"), SOVA_KEY_TYPE_MULTIMEDIA_OPEN_PLAYER, SOVA_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Previous track"), SOVA_KEY_TYPE_MULTIMEDIA_PREVIOUS_TRACK, SOVA_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Next track"), SOVA_KEY_TYPE_MULTIMEDIA_NEXT_TRACK, SOVA_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Play/Pause"), SOVA_KEY_TYPE_MULTIMEDIA_PLAY_PAUSE, SOVA_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Stop"), SOVA_KEY_TYPE_MULTIMEDIA_STOP, SOVA_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Mute master volume"), SOVA_KEY_TYPE_MULTIMEDIA_MUTE_MASTER_VOLUME, SOVA_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Mute microphone"), SOVA_KEY_TYPE_MULTIMEDIA_MUTE_MICROPHONE, SOVA_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Volume up"), SOVA_KEY_TYPE_MULTIMEDIA_VOLUME_UP, SOVA_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Volume down"), SOVA_KEY_TYPE_MULTIMEDIA_VOLUME_DOWN, SOVA_KEY_COMBO_BOX_GROUP_KEY);

	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("Profile"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, SOVA_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Profile up"), SOVA_KEY_TYPE_PROFILE_UP, SOVA_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Profile down"), SOVA_KEY_TYPE_PROFILE_DOWN, SOVA_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Profile cycle"), SOVA_KEY_TYPE_PROFILE_CYCLE, SOVA_KEY_COMBO_BOX_GROUP_KEY);

	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("Timer"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, SOVA_KEY_COMBO_BOX_GROUP_KEY | SOVA_KEY_COMBO_BOX_GROUP_EXTENDED);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Timer start"), SOVA_KEY_TYPE_TIMER, SOVA_KEY_COMBO_BOX_GROUP_EXTENDED);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Timer stop"), SOVA_KEY_TYPE_TIMER_STOP, SOVA_KEY_COMBO_BOX_GROUP_KEY);

	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("Browser"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, SOVA_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Search"), SOVA_KEY_TYPE_BROWSER_SEARCH, SOVA_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Home"), SOVA_KEY_TYPE_BROWSER_HOME, SOVA_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Back"), SOVA_KEY_TYPE_BROWSER_BACK, SOVA_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Forward"), SOVA_KEY_TYPE_BROWSER_FORWARD, SOVA_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Stop"), SOVA_KEY_TYPE_BROWSER_STOP, SOVA_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Refresh"), SOVA_KEY_TYPE_BROWSER_REFRESH, SOVA_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("New tab"), SOVA_KEY_TYPE_BROWSER_NEW_TAB, SOVA_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("New window"), SOVA_KEY_TYPE_BROWSER_NEW_WINDOW, SOVA_KEY_COMBO_BOX_GROUP_KEY);

	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("Open"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, SOVA_KEY_COMBO_BOX_GROUP_KEY | SOVA_KEY_COMBO_BOX_GROUP_EXTENDED);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Driver"), SOVA_KEY_TYPE_OPEN_DRIVER, SOVA_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Application"), SOVA_KEY_TYPE_OPEN_APP, SOVA_KEY_COMBO_BOX_GROUP_EXTENDED);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Document"), SOVA_KEY_TYPE_OPEN_DOCUMENT, SOVA_KEY_COMBO_BOX_GROUP_EXTENDED);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Folder"), SOVA_KEY_TYPE_OPEN_FOLDER, SOVA_KEY_COMBO_BOX_GROUP_EXTENDED);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Website"), SOVA_KEY_TYPE_OPEN_WEBSITE, SOVA_KEY_COMBO_BOX_GROUP_EXTENDED);

	roccat_key_combo_box_store_append(store, &sublevel, &toplevel, _("Folder presets"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, SOVA_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Desktop"), SOVA_KEY_TYPE_FOLDER_DESKTOP, SOVA_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Favorites"), SOVA_KEY_TYPE_FOLDER_FAVORITES, SOVA_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Fonts"), SOVA_KEY_TYPE_FOLDER_FONTS, SOVA_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("My documents"), SOVA_KEY_TYPE_FOLDER_MY_DOCUMENTS, SOVA_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("My downloads"), SOVA_KEY_TYPE_FOLDER_MY_DOWNLOADS, SOVA_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("My music"), SOVA_KEY_TYPE_FOLDER_MY_MUSIC, SOVA_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("My pictures"), SOVA_KEY_TYPE_FOLDER_MY_PICTURES, SOVA_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Network"), SOVA_KEY_TYPE_FOLDER_MY_NETWORK, SOVA_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Printers"), SOVA_KEY_TYPE_FOLDER_PRINTERS, SOVA_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("System"), SOVA_KEY_TYPE_FOLDER_SYSTEM, SOVA_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Computer"), SOVA_KEY_TYPE_APP_MY_COMPUTER, SOVA_KEY_COMBO_BOX_GROUP_KEY);

	roccat_key_combo_box_store_append(store, &sublevel, &toplevel, _("Software presets"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, SOVA_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("E-Mail"), SOVA_KEY_TYPE_APP_EMAIL, SOVA_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Calculator"), SOVA_KEY_TYPE_APP_CALCULATOR, SOVA_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Webbrowser"), SOVA_KEY_TYPE_APP_BROWSER, SOVA_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Command line"), SOVA_KEY_TYPE_APP_COMMANDLINE, SOVA_KEY_COMBO_BOX_GROUP_KEY);

	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("System"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, SOVA_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Shutdown"), SOVA_KEY_TYPE_SYSTEM_SHUTDOWN, SOVA_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Sleep"), SOVA_KEY_TYPE_SYSTEM_SLEEP, SOVA_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Wake"), SOVA_KEY_TYPE_SYSTEM_WAKE, SOVA_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Hibernate"), SOVA_KEY_TYPE_SYSTEM_HIBERNATE, SOVA_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Reboot"), SOVA_KEY_TYPE_SYSTEM_REBOOT, SOVA_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Lock"), SOVA_KEY_TYPE_SYSTEM_LOCK, SOVA_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Log off"), SOVA_KEY_TYPE_SYSTEM_LOGOFF, SOVA_KEY_COMBO_BOX_GROUP_KEY);

	roccat_key_combo_box_store_append(store, &sublevel, &toplevel, _("Windows function"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Control panel"), SOVA_KEY_TYPE_SYSTEM_CONTROL_PANEL, SOVA_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("System panel"), SOVA_KEY_TYPE_SYSTEM_SYSTEM_PANEL, SOVA_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Task manager"), SOVA_KEY_TYPE_SYSTEM_TASK_MANAGER, SOVA_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Screen settings"), SOVA_KEY_TYPE_SYSTEM_SCREEN_SETTINGS, SOVA_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Screensaver"), SOVA_KEY_TYPE_SYSTEM_SCREENSAVER, SOVA_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Themes"), SOVA_KEY_TYPE_SYSTEM_THEMES, SOVA_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Date & time"), SOVA_KEY_TYPE_SYSTEM_DATE_TIME, SOVA_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Network settings"), SOVA_KEY_TYPE_SYSTEM_NETWORK_SETTINGS, SOVA_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Admin panel"), SOVA_KEY_TYPE_SYSTEM_ADMIN_PANEL, SOVA_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Firewall"), SOVA_KEY_TYPE_SYSTEM_WINDOWS_FIREWALL, SOVA_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Regedit"), SOVA_KEY_TYPE_SYSTEM_REGEDIT, SOVA_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Event monitor"), SOVA_KEY_TYPE_SYSTEM_EVENT_MONITOR, SOVA_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Performance monitor"), SOVA_KEY_TYPE_SYSTEM_PERFORMANCE_MONITOR, SOVA_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Sound settings"), SOVA_KEY_TYPE_SYSTEM_SOUND_SETTINGS, SOVA_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Internet settings"), SOVA_KEY_TYPE_SYSTEM_INTERNET_SETTINGS, SOVA_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("DirectX diagnosis"), SOVA_KEY_TYPE_SYSTEM_DIRECTX_DIAG, SOVA_KEY_COMBO_BOX_GROUP_KEY);

	roccat_key_combo_box_store_append(store, NULL, NULL, _("Shortcut"), SOVA_KEY_TYPE_SHORTCUT, SOVA_KEY_COMBO_BOX_GROUP_KEY);
	roccat_key_combo_box_store_append(store, NULL, NULL, _("Macro"), SOVA_KEY_TYPE_MACRO, SOVA_KEY_COMBO_BOX_GROUP_EXTENDED);

	roccat_key_combo_box_store_append(store, NULL, NULL, _("Standard"), SOVA_KEY_COMBO_BOX_TYPE_STANDARD, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, NULL, _("Remap"), SOVA_KEY_COMBO_BOX_TYPE_REMAP, SOVA_KEY_COMBO_BOX_GROUP_PRIMARY);
	roccat_key_combo_box_store_append(store, NULL, NULL, _("Disabled"), SOVA_KEY_TYPE_DISABLED, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	return GTK_TREE_MODEL(store);
}

static void sova_key_combo_box_class_init(SovaKeyComboBoxClass *klass) {
	g_type_class_add_private(klass, sizeof(SovaKeyComboBoxPrivate));
	klass->model = key_info_model_new();

	signals[KEY_CHANGED] = g_signal_new("key-changed",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);
}
