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

#include "nyth_button_combo_box.h"

#include <gaminggear/gaminggear_macro_dialog.h>
#include <gaminggear/key_translations.h>
#include <gaminggear/gdk_key_translations.h>
#include <gaminggear/hid_uid.h>
#include <gaminggear/gaminggear_text_dialog.h>
#include "nyth_profile_button.h"
#include "roccat_talk_target_dialog.h"
#include "roccat_shortcut_dialog.h"
#include "roccat_timer_dialog.h"
#include "roccat_warning_dialog.h"
#include "roccat_talk.h"
#include "roccat_helper.h"
#include "gtk_roccat_helper.h"
#include "g_cclosure_roccat_marshaller.h"
#include "i18n.h"

#define NYTH_BUTTON_COMBO_BOX_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), NYTH_BUTTON_COMBO_BOX_TYPE, NythButtonComboBoxClass))
#define IS_NYTH_BUTTON_COMBO_BOX_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), NYTH_BUTTON_COMBO_BOX_TYPE))
#define NYTH_BUTTON_COMBO_BOX_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), NYTH_BUTTON_COMBO_BOX_TYPE, NythButtonComboBoxClass))
#define NYTH_BUTTON_COMBO_BOX_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), NYTH_BUTTON_COMBO_BOX_TYPE, NythButtonComboBoxPrivate))

typedef struct _NythButtonComboBoxClass NythButtonComboBoxClass;
typedef struct _NythButtonComboBoxPrivate NythButtonComboBoxPrivate;

struct _NythButtonComboBox {
	RoccatKeyComboBox parent;
	NythButtonComboBoxPrivate *priv;
};

struct _NythButtonComboBoxClass {
	RoccatKeyComboBoxClass parent_class;
	GtkTreeModel *model;
};

struct _NythButtonComboBoxPrivate {
	NythButtonComboBoxData *data;
	gulong changed_handler_id;
};

G_DEFINE_TYPE(NythButtonComboBox, nyth_button_combo_box, ROCCAT_KEY_COMBO_BOX_TYPE);

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

static gchar *nyth_button_combo_box_get_text_for_data(NythButtonComboBox *button_combo_box, NythButtonComboBoxData const *data) {
	gchar *text;
	gchar *temp;
	guint type;

	type = data->button.type;

	switch (type) {
	case NYTH_BUTTON_TYPE_SHORTCUT:
		gtk_widget_set_tooltip_text(GTK_WIDGET(button_combo_box), NULL);
		temp = roccat_shortcut_dialog_create_name(data->button.modifier, data->button.key);
		text = g_strdup_printf(_("Shortcut: %s"), temp);
		g_free(temp);
		break;
	case NYTH_BUTTON_TYPE_MACRO:
		gtk_widget_set_tooltip_text(GTK_WIDGET(button_combo_box), NULL);
		text = g_strdup_printf(_("Macro: %s/%s"), data->macro.macroset_name, data->macro.macro_name);
		break;
	case NYTH_BUTTON_TYPE_OPEN_APPLICATION:
		gtk_widget_set_tooltip_text(GTK_WIDGET(button_combo_box), data->opener);
		temp = g_path_get_basename(data->opener);
		text = g_strdup_printf(_("Application: %s"), temp);
		g_free(temp);
		break;
	case NYTH_BUTTON_TYPE_OPEN_DOCUMENT:
		gtk_widget_set_tooltip_text(GTK_WIDGET(button_combo_box), data->opener);
		temp = g_path_get_basename(data->opener);
		text = g_strdup_printf(_("Document: %s"), temp);
		g_free(temp);
		break;
	case NYTH_BUTTON_TYPE_OPEN_FOLDER:
		gtk_widget_set_tooltip_text(GTK_WIDGET(button_combo_box), data->opener);
		temp = g_path_get_basename(data->opener);
		text = g_strdup_printf(_("Folder: %s"), temp);
		g_free(temp);
		break;
	case NYTH_BUTTON_TYPE_OPEN_WEBSITE:
		gtk_widget_set_tooltip_text(GTK_WIDGET(button_combo_box), data->opener);
		temp = get_host(data->opener);
		text = g_strdup_printf(_("Website: %s"), temp);
		g_free(temp);
		break;
	case NYTH_BUTTON_TYPE_TIMER:
		gtk_widget_set_tooltip_text(GTK_WIDGET(button_combo_box), NULL);
		text = g_strdup_printf(_("Timer: %s"), (gchar const *)data->timer.name);
		break;
	case NYTH_BUTTON_TYPE_EASYSHIFT_OTHER:
		text = g_strdup_printf(_("Easyshift: %s"), roccat_talk_device_get_text_static(data->talk_target));
		break;
	case NYTH_BUTTON_TYPE_EASYSHIFT_LOCK_OTHER:
		text = g_strdup_printf(_("Easyshift lock: %s"), roccat_talk_device_get_text_static(data->talk_target));
		break;
	default:
		gtk_widget_set_tooltip_text(GTK_WIDGET(button_combo_box), NULL);
		text = roccat_key_combo_box_model_get_title_for_type(gtk_combo_box_get_model(GTK_COMBO_BOX(button_combo_box)), type);
		break;
	}

	return text;
}

void nyth_button_combo_box_update(NythButtonComboBox *button_combo_box) {
	NythButtonComboBoxPrivate *priv = button_combo_box->priv;
	gchar *text;

	text = nyth_button_combo_box_get_text_for_data(button_combo_box, priv->data);

	g_signal_handler_block(G_OBJECT(button_combo_box), priv->changed_handler_id);
	roccat_key_combo_box_set_text(ROCCAT_KEY_COMBO_BOX(button_combo_box), text);
	g_signal_handler_unblock(G_OBJECT(button_combo_box), priv->changed_handler_id);

	g_free(text);
}

static void nyth_button_combo_box_data_set_opener(NythButtonComboBoxData *data, gchar const *new_opener) {
	g_strlcpy(data->opener, new_opener, ROCCAT_SWARM_RMP_OPENER_LENGTH);
}

static gboolean key_open_application(NythButtonComboBox *button_combo_box) {
	NythButtonComboBoxPrivate *priv = button_combo_box->priv;
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
		roccat_button_set_to_normal(&priv->data->button, NYTH_BUTTON_TYPE_OPEN_APPLICATION);
		path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
		nyth_button_combo_box_data_set_opener(priv->data, path);
		g_free(path);
		retval = TRUE;
	}
	gtk_widget_destroy(dialog);
	return retval;
}

static gboolean key_open_document(NythButtonComboBox *button_combo_box) {
	NythButtonComboBoxPrivate *priv = button_combo_box->priv;
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
		roccat_button_set_to_normal(&priv->data->button, NYTH_BUTTON_TYPE_OPEN_DOCUMENT);
		path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
		nyth_button_combo_box_data_set_opener(priv->data, path);
		g_free(path);
		retval = TRUE;
	}
	gtk_widget_destroy(dialog);
	return retval;
}

static gboolean key_open_folder(NythButtonComboBox *button_combo_box) {
	NythButtonComboBoxPrivate *priv = button_combo_box->priv;
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
		roccat_button_set_to_normal(&priv->data->button, NYTH_BUTTON_TYPE_OPEN_FOLDER);
		path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
		nyth_button_combo_box_data_set_opener(priv->data, path);
		g_free(path);
		retval = TRUE;
	}
	gtk_widget_destroy(dialog);
	return retval;
}

static gboolean key_open_website(NythButtonComboBox *button_combo_box) {
	NythButtonComboBoxPrivate *priv = button_combo_box->priv;
	gchar *url;

	url = gaminggear_text_dialog(gtk_roccat_widget_get_gtk_window(GTK_WIDGET(button_combo_box)),
			_("URL dialog"), _("Please enter any complete url with scheme"), NULL);

	if (url) {
		roccat_button_set_to_normal(&priv->data->button, NYTH_BUTTON_TYPE_OPEN_WEBSITE);
		nyth_button_combo_box_data_set_opener(priv->data, url);
		g_free(url);
		return TRUE;
	}
	return FALSE;
}

static gboolean set_macro(NythButtonComboBox *button_combo_box) {
	NythButtonComboBoxPrivate *priv = button_combo_box->priv;
	GtkWindow *parent;
	GaminggearMacro *gaminggear_macro;
	NythMacro *nyth_macro;
	gboolean result;
	GError *local_error = NULL;

	parent = gtk_roccat_widget_get_gtk_window(GTK_WIDGET(button_combo_box));
	gaminggear_macro = gaminggear_macro_dialog(parent);
	if (!gaminggear_macro)
		return FALSE;

	nyth_macro = (NythMacro *)g_malloc0(sizeof(NythMacro));
	result = gaminggear_macro_to_nyth_macro(gaminggear_macro, nyth_macro, &local_error);
	gaminggear_macro_free(gaminggear_macro);
	if (!result) {
		roccat_handle_error_dialog(parent, _("Could not convert macro"), &local_error);
		g_free(nyth_macro);
		return FALSE;
	}

	nyth_macro_copy(&priv->data->macro, nyth_macro);
	g_free(nyth_macro);

	return TRUE;
}

static gboolean key_macro(NythButtonComboBox *button_combo_box) {
	NythButtonComboBoxPrivate *priv = button_combo_box->priv;
	gboolean retval;

	retval = set_macro(button_combo_box);

	if (retval)
		roccat_button_set_to_normal(&priv->data->button, NYTH_BUTTON_TYPE_MACRO);

	return retval;
}

static gboolean key_shortcut(NythButtonComboBox *button_combo_box) {
	NythButtonComboBoxPrivate *priv = button_combo_box->priv;
	RoccatShortcutDialog *dialog;
	gboolean retval = FALSE;

	dialog = ROCCAT_SHORTCUT_DIALOG(roccat_shortcut_dialog_new(gtk_roccat_widget_get_gtk_window(GTK_WIDGET(button_combo_box))));
	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
		roccat_button_set_to_shortcut(&priv->data->button, NYTH_BUTTON_TYPE_SHORTCUT,
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

static gboolean key_timer(NythButtonComboBox *button_combo_box) {
	NythButtonComboBoxPrivate *priv = button_combo_box->priv;
	RoccatTimer *roccat_timer;
	gboolean retval = FALSE;

	roccat_timer = roccat_timer_dialog(gtk_roccat_widget_get_gtk_window(GTK_WIDGET(button_combo_box)));
	if (roccat_timer) {
		roccat_button_set_to_normal(&priv->data->button, NYTH_BUTTON_TYPE_TIMER);
		roccat_timer_copy(&priv->data->timer, roccat_timer);
		g_free(roccat_timer);
		retval = TRUE;
	}
	return retval;
}

static gboolean key_normal(NythButtonComboBox *button_combo_box, guint type) {
	gboolean retval = TRUE;

	if (button_combo_box->priv->data->button.type == type)
		retval = FALSE;

	roccat_button_set_to_normal(&button_combo_box->priv->data->button, type);

	return retval;
}

static gboolean key_talk_other(NythButtonComboBox *button_combo_box, gint type) {
	NythButtonComboBoxPrivate *priv = button_combo_box->priv;
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
	NythButtonComboBox *button_combo_box = NYTH_BUTTON_COMBO_BOX(combo);
	guint type;
	gboolean changed;

	type = roccat_key_combo_box_get_active_type(ROCCAT_KEY_COMBO_BOX(combo));
	if (type == ROCCAT_KEY_COMBO_BOX_TYPE_NONE)
		return;

	switch (type) {
	case NYTH_BUTTON_TYPE_OPEN_APPLICATION:
		changed = key_open_application(button_combo_box);
		break;
	case NYTH_BUTTON_TYPE_OPEN_DOCUMENT:
		changed = key_open_document(button_combo_box);
		break;
	case NYTH_BUTTON_TYPE_OPEN_FOLDER:
		changed = key_open_folder(button_combo_box);
		break;
	case NYTH_BUTTON_TYPE_OPEN_WEBSITE:
		changed = key_open_website(button_combo_box);
		break;
	case NYTH_BUTTON_TYPE_MACRO:
		changed = key_macro(button_combo_box);
		break;
	case NYTH_BUTTON_TYPE_SHORTCUT:
		changed = key_shortcut(button_combo_box);
		break;
	case NYTH_BUTTON_TYPE_TIMER:
		changed = key_timer(button_combo_box);
		break;
	case NYTH_BUTTON_TYPE_EASYSHIFT_OTHER:
	case NYTH_BUTTON_TYPE_EASYSHIFT_LOCK_OTHER:
		changed = key_talk_other(button_combo_box, type);
		break;
	case NYTH_BUTTON_TYPE_EASYSHIFT_ALL:
	default:
		changed = key_normal(button_combo_box, type);
		break;
	}

	nyth_button_combo_box_update(button_combo_box);
	if (changed)
		g_signal_emit((gpointer)button_combo_box, signals[KEY_CHANGED], 0);
}

void nyth_button_combo_box_set_data_pointer(NythButtonComboBox *button_combo_box, NythButtonComboBoxData *data) {
	button_combo_box->priv->data = data;
	nyth_button_combo_box_update(button_combo_box);
}

NythButtonComboBoxData *nyth_button_combo_box_get_data_pointer(NythButtonComboBox *button_combo_box) {
	return button_combo_box->priv->data;
}

GtkWidget *nyth_button_combo_box_new(guint mask) {
	NythButtonComboBox *button_combo_box;
	NythButtonComboBoxClass *button_combo_box_class;
	NythButtonComboBoxPrivate *priv;

	button_combo_box = NYTH_BUTTON_COMBO_BOX(g_object_new(NYTH_BUTTON_COMBO_BOX_TYPE,
			"exclude-mask", mask,
			NULL));

	priv = button_combo_box->priv;

	button_combo_box_class = NYTH_BUTTON_COMBO_BOX_GET_CLASS(button_combo_box);
	roccat_key_combo_box_set_model(ROCCAT_KEY_COMBO_BOX(button_combo_box), button_combo_box_class->model);

	priv->changed_handler_id = g_signal_connect(G_OBJECT(button_combo_box), "changed", G_CALLBACK(changed_cb), NULL);

	return GTK_WIDGET(button_combo_box);
}

static void nyth_button_combo_box_init(NythButtonComboBox *button_combo_box) {
	NythButtonComboBoxPrivate *priv = NYTH_BUTTON_COMBO_BOX_GET_PRIVATE(button_combo_box);
	button_combo_box->priv = priv;
}

static GtkTreeModel *button_info_model_new(void) {
	GtkTreeStore *store;
	GtkTreeIter toplevel, sublevel;

	store = roccat_key_combo_box_store_new();


	roccat_key_combo_box_store_append(store, NULL, NULL, _("Macro"), NYTH_BUTTON_TYPE_MACRO, NYTH_BUTTON_COMBO_BOX_GROUP_EXTENDED);
	roccat_key_combo_box_store_append(store, NULL, NULL, _("Shortcut"), NYTH_BUTTON_TYPE_SHORTCUT, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("Timer"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE | NYTH_BUTTON_COMBO_BOX_GROUP_EXTENDED);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Timer start"), NYTH_BUTTON_TYPE_TIMER, NYTH_BUTTON_COMBO_BOX_GROUP_EXTENDED);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Timer stop"), NYTH_BUTTON_TYPE_TIMER_STOP, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("Basic functions"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE | ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Button 1 (Click)"), NYTH_BUTTON_TYPE_CLICK, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Button 2 (Universal scrolling)"), NYTH_BUTTON_TYPE_UNIVERSAL_SCROLLING, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Button 3 (Menu)"), NYTH_BUTTON_TYPE_MENU, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Button 4 (Scroll up)"), NYTH_BUTTON_TYPE_SCROLL_UP, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Button 5 (Scroll down)"), NYTH_BUTTON_TYPE_SCROLL_DOWN, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Button 6 (Tilt left)"), NYTH_BUTTON_TYPE_TILT_LEFT, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Button 7 (Tilt right)"), NYTH_BUTTON_TYPE_TILT_RIGHT, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Button 8 (Browser backward)"), NYTH_BUTTON_TYPE_BROWSER_BACKWARD, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Button 9 (Browser forward)"), NYTH_BUTTON_TYPE_BROWSER_FORWARD, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Double click"), NYTH_BUTTON_TYPE_DOUBLE_CLICK, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Home"), NYTH_BUTTON_TYPE_HOME, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("End"), NYTH_BUTTON_TYPE_END, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Easyshift"), NYTH_BUTTON_TYPE_EASYSHIFT_SELF, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT);
	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("Advanced functions"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("CPI cycle"), NYTH_BUTTON_TYPE_CPI_CYCLE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("CPI up"), NYTH_BUTTON_TYPE_CPI_UP, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("CPI down"), NYTH_BUTTON_TYPE_CPI_DOWN, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Profile cycle"), NYTH_BUTTON_TYPE_PROFILE_CYCLE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Profile up"), NYTH_BUTTON_TYPE_PROFILE_UP, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Profile down"), NYTH_BUTTON_TYPE_PROFILE_DOWN, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, &sublevel, &toplevel, _("Profile presets"), NYTH_BUTTON_TYPE_PROFILE_DOWN, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Profile 1"), NYTH_BUTTON_TYPE_PROFILE_1, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Profile 2"), NYTH_BUTTON_TYPE_PROFILE_2, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Profile 3"), NYTH_BUTTON_TYPE_PROFILE_3, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Profile 4"), NYTH_BUTTON_TYPE_PROFILE_4, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Profile 5"), NYTH_BUTTON_TYPE_PROFILE_5, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Sensitivity cycle"), NYTH_BUTTON_TYPE_SENSITIVITY_CYCLE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Sensitivity up"), NYTH_BUTTON_TYPE_SENSITIVITY_UP, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Sensitivity down"), NYTH_BUTTON_TYPE_SENSITIVITY_DOWN, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, &sublevel, &toplevel, _("Sensitivity presets"), NYTH_BUTTON_TYPE_SENSITIVITY_DOWN, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Sensitivity -5"), NYTH_BUTTON_TYPE_SENSITIVITY__5, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Sensitivity -4"), NYTH_BUTTON_TYPE_SENSITIVITY__4, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Sensitivity -3"), NYTH_BUTTON_TYPE_SENSITIVITY__3, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Sensitivity -2"), NYTH_BUTTON_TYPE_SENSITIVITY__2, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Sensitivity -1"), NYTH_BUTTON_TYPE_SENSITIVITY__1, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Sensitivity  0"), NYTH_BUTTON_TYPE_SENSITIVITY_0, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Sensitivity  1"), NYTH_BUTTON_TYPE_SENSITIVITY_1, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Sensitivity  2"), NYTH_BUTTON_TYPE_SENSITIVITY_2, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Sensitivity  3"), NYTH_BUTTON_TYPE_SENSITIVITY_3, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Sensitivity  4"), NYTH_BUTTON_TYPE_SENSITIVITY_4, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Sensitivity  5"), NYTH_BUTTON_TYPE_SENSITIVITY_5, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, &sublevel, &toplevel, _("Easyaim"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Easyaim Setting 1"), NYTH_BUTTON_TYPE_EASYAIM_1, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Easyaim Setting 2"), NYTH_BUTTON_TYPE_EASYAIM_2, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Easyaim Setting 3"), NYTH_BUTTON_TYPE_EASYAIM_3, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Easyaim Setting 4"), NYTH_BUTTON_TYPE_EASYAIM_4, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Easyaim Setting 5"), NYTH_BUTTON_TYPE_EASYAIM_5, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT);
	roccat_key_combo_box_store_append(store, &sublevel, &toplevel, _("Easywheel"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Easywheel Sensitivity"), NYTH_BUTTON_TYPE_EASYWHEEL_SENSITIVITY, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Easywheel Profile"), NYTH_BUTTON_TYPE_EASYWHEEL_PROFILE, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Easywheel CPI"), NYTH_BUTTON_TYPE_EASYWHEEL_CPI, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Easywheel Volume"), NYTH_BUTTON_TYPE_EASYWHEEL_VOLUME, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Easywheel Alt Tab"), NYTH_BUTTON_TYPE_EASYWHEEL_ALT_TAB, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Easywheel Aero Flip 3D"), NYTH_BUTTON_TYPE_EASYWHEEL_AERO_FLIP_3D, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT);
	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("Internet"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Browser"), NYTH_BUTTON_TYPE_BROWSER, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Favorites"), NYTH_BUTTON_TYPE_BROWSER_FAVORITES, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Search"), NYTH_BUTTON_TYPE_BROWSER_SEARCH, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Home"), NYTH_BUTTON_TYPE_BROWSER_HOME, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Stop"), NYTH_BUTTON_TYPE_BROWSER_STOP, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Refresh"), NYTH_BUTTON_TYPE_BROWSER_REFRESH, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("New tab"), NYTH_BUTTON_TYPE_BROWSER_NEW_TAB, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("New window"), NYTH_BUTTON_TYPE_BROWSER_NEW_WINDOW, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("Multimedia"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Mute master volume"), NYTH_BUTTON_TYPE_MUTE_MASTER, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Mute microphone"), NYTH_BUTTON_TYPE_MUTE_MICROPHONE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Volume up"), NYTH_BUTTON_TYPE_VOLUME_UP, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Volume down"), NYTH_BUTTON_TYPE_VOLUME_DOWN, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Previous track"), NYTH_BUTTON_TYPE_PREV_TRACK, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Next track"), NYTH_BUTTON_TYPE_NEXT_TRACK, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Play/Pause"), NYTH_BUTTON_TYPE_PLAY_PAUSE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Stop"), NYTH_BUTTON_TYPE_STOP, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("Open"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE | NYTH_BUTTON_COMBO_BOX_GROUP_EXTENDED);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Driver"), NYTH_BUTTON_TYPE_OPEN_DRIVER, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Application"), NYTH_BUTTON_TYPE_OPEN_APPLICATION, NYTH_BUTTON_COMBO_BOX_GROUP_EXTENDED);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Document"), NYTH_BUTTON_TYPE_OPEN_DOCUMENT, NYTH_BUTTON_COMBO_BOX_GROUP_EXTENDED);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Folder"), NYTH_BUTTON_TYPE_OPEN_FOLDER, NYTH_BUTTON_COMBO_BOX_GROUP_EXTENDED);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Website"), NYTH_BUTTON_TYPE_OPEN_WEBSITE, NYTH_BUTTON_COMBO_BOX_GROUP_EXTENDED);
	roccat_key_combo_box_store_append(store, &sublevel, &toplevel, _("Folder presets"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Computer"), NYTH_BUTTON_TYPE_OPEN_FOLDER_COMPUTER, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Desktop"), NYTH_BUTTON_TYPE_OPEN_FOLDER_DESKTOP, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Favorites"), NYTH_BUTTON_TYPE_OPEN_FOLDER_FAVORITES, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Fonts"), NYTH_BUTTON_TYPE_OPEN_FOLDER_FONTS, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("My documents"), NYTH_BUTTON_TYPE_OPEN_FOLDER_MY_DOCUMENTS, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("My downloads"), NYTH_BUTTON_TYPE_OPEN_FOLDER_MY_DOWNLOADS, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("My music"), NYTH_BUTTON_TYPE_OPEN_FOLDER_MY_MUSIC, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("My pictures"), NYTH_BUTTON_TYPE_OPEN_FOLDER_MY_PICTURES, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Network"), NYTH_BUTTON_TYPE_OPEN_FOLDER_NETWORK, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Printers"), NYTH_BUTTON_TYPE_OPEN_FOLDER_PRINTERS, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("System"), NYTH_BUTTON_TYPE_OPEN_FOLDER_SYSTEM, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, &sublevel, &toplevel, _("Software presets"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Calculator"), NYTH_BUTTON_TYPE_OPEN_APPLICATION_CALCULATOR, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("E-Mail"), NYTH_BUTTON_TYPE_OPEN_APPLICATION_EMAIL, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("Talk"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Other Easyshift"), NYTH_BUTTON_TYPE_EASYSHIFT_OTHER, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Other Easyshift lock"), NYTH_BUTTON_TYPE_EASYSHIFT_LOCK_OTHER, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Both Easyshift"), NYTH_BUTTON_TYPE_EASYSHIFT_ALL, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT);
	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("System"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Hibernate"), NYTH_BUTTON_TYPE_SYSTEM_HIBERNATE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Reboot"), NYTH_BUTTON_TYPE_SYSTEM_REBOOT, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Lock"), NYTH_BUTTON_TYPE_SYSTEM_LOCK, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Log off"), NYTH_BUTTON_TYPE_SYSTEM_LOGOFF, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Shut down"), NYTH_BUTTON_TYPE_SYSTEM_SHUTDOWN, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Sleep"), NYTH_BUTTON_TYPE_SYSTEM_SLEEP, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Wake"), NYTH_BUTTON_TYPE_SYSTEM_WAKE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, &sublevel, &toplevel, _("Windows function"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Control panel"), NYTH_BUTTON_TYPE_WINDOWS_FUNCTION_CONTROL_PANEL, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("System panel"), NYTH_BUTTON_TYPE_WINDOWS_FUNCTION_SYSTEM_PANEL, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Task manager"), NYTH_BUTTON_TYPE_WINDOWS_FUNCTION_TASK_MANAGER, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Screen settings"), NYTH_BUTTON_TYPE_WINDOWS_FUNCTION_SCREEN_SETTINGS, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Screensaver"), NYTH_BUTTON_TYPE_WINDOWS_FUNCTION_SCREENSAVER, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Themes"), NYTH_BUTTON_TYPE_WINDOWS_FUNCTION_THEMES, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Date & time"), NYTH_BUTTON_TYPE_WINDOWS_FUNCTION_DATE_TIME, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Network settings"), NYTH_BUTTON_TYPE_WINDOWS_FUNCTION_NETWORK_SETTINGS, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Admin panel"), NYTH_BUTTON_TYPE_WINDOWS_FUNCTION_ADMIN_PANEL, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Firewall"), NYTH_BUTTON_TYPE_WINDOWS_FUNCTION_FIREWALL, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Regedit"), NYTH_BUTTON_TYPE_WINDOWS_FUNCTION_REGEDIT, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Event monitor"), NYTH_BUTTON_TYPE_WINDOWS_FUNCTION_EVENT_MONITOR, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Performance monitor"), NYTH_BUTTON_TYPE_WINDOWS_FUNCTION_PERFORMANCE_MONITOR, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Sound settings"), NYTH_BUTTON_TYPE_WINDOWS_FUNCTION_SOUND_SETTINGS, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Internet settings"), NYTH_BUTTON_TYPE_WINDOWS_FUNCTION_INTERNET_SETTINGS, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("DirectX diagnosis"), NYTH_BUTTON_TYPE_WINDOWS_FUNCTION_DIREKTX_DIAG, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &sublevel, _("Command line"), NYTH_BUTTON_TYPE_WINDOWS_FUNCTION_COMMAND_LINE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, NULL, _("Disabled"), NYTH_BUTTON_TYPE_DISABLED, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	return GTK_TREE_MODEL(store);
}

static void nyth_button_combo_box_class_init(NythButtonComboBoxClass *klass) {
	g_type_class_add_private(klass, sizeof(NythButtonComboBoxPrivate));
	klass->model = button_info_model_new();

	signals[KEY_CHANGED] = g_signal_new("key-changed",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);
}
