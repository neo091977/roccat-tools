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
#include "savuconfig_key_combo_box.h"
#include "roccat_shortcut_dialog.h"
#include "roccat_timer_dialog.h"
#include "roccat_warning_dialog.h"
#include "roccat_helper.h"
#include "gtk_roccat_helper.h"
#include "i18n.h"

#define SAVUCONFIG_KEY_COMBO_BOX_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), SAVUCONFIG_KEY_COMBO_BOX_TYPE, SavuconfigKeyComboBoxClass))
#define IS_SAVUCONFIG_KEY_COMBO_BOX_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), SAVUCONFIG_KEY_COMBO_BOX_TYPE))
#define SAVUCONFIG_KEY_COMBO_BOX_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), SAVUCONFIG_KEY_COMBO_BOX_TYPE, SavuconfigKeyComboBoxClass))
#define SAVUCONFIG_KEY_COMBO_BOX_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), SAVUCONFIG_KEY_COMBO_BOX_TYPE, SavuconfigKeyComboBoxPrivate))

typedef struct _SavuconfigKeyComboBoxClass SavuconfigKeyComboBoxClass;
typedef struct _SavuconfigKeyComboBoxPrivate SavuconfigKeyComboBoxPrivate;

struct _SavuconfigKeyComboBox {
	RoccatKeyComboBox parent;
	SavuconfigKeyComboBoxPrivate *priv;
};

struct _SavuconfigKeyComboBoxClass {
	RoccatKeyComboBoxClass parent_class;
	GtkTreeModel *model;
};

struct _SavuconfigKeyComboBoxPrivate {
	RoccatButton button;
	SavuRmpMacroKeyInfo *macro;
	RoccatTimer *timer;
	gchar *launchpath;
	gulong changed_handler_id;
};

G_DEFINE_TYPE(SavuconfigKeyComboBox, savuconfig_key_combo_box, ROCCAT_KEY_COMBO_BOX_TYPE);

static void set_text(SavuconfigKeyComboBox *key_combo_box) {
	SavuconfigKeyComboBoxPrivate *priv = key_combo_box->priv;
	gchar *text;
	gchar *temp;

	switch (priv->button.type) {
	case SAVU_BUTTON_TYPE_SHORTCUT:
		temp = roccat_shortcut_dialog_create_name(priv->button.modifier, priv->button.key);
		text = g_strdup_printf(_("Shortcut: %s"), temp);
		g_free(temp);
		break;
	case SAVU_BUTTON_TYPE_MACRO:
		text = g_strdup_printf(_("Macro: %s"), priv->macro->name);
		break;
	case SAVU_BUTTON_TYPE_QUICKLAUNCH:
		temp = g_path_get_basename(priv->launchpath);
		text = g_strdup_printf(_("Quicklaunch: %s"), temp);
		g_free(temp);
		break;
	case SAVU_BUTTON_TYPE_TIMER:
		text = g_strdup_printf(_("Timer: %s"), (gchar const *)priv->timer->name);
		break;
	default:
		text = roccat_key_combo_box_model_get_title_for_type(gtk_combo_box_get_model(GTK_COMBO_BOX(key_combo_box)), priv->button.type);
		break;
	}

	g_signal_handler_block(G_OBJECT(key_combo_box), priv->changed_handler_id);
	roccat_key_combo_box_set_text(ROCCAT_KEY_COMBO_BOX(key_combo_box), text);
	g_signal_handler_unblock(G_OBJECT(key_combo_box), priv->changed_handler_id);

	g_free(text);
}

static void button_quicklaunch(SavuconfigKeyComboBox *key_combo_box) {
	SavuconfigKeyComboBoxPrivate *priv = key_combo_box->priv;
	GtkWidget *dialog;

	dialog = gtk_file_chooser_dialog_new(_("Select executable"),
			gtk_roccat_widget_get_gtk_window(GTK_WIDGET(key_combo_box)),
			GTK_FILE_CHOOSER_ACTION_OPEN,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
			NULL);

	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
		roccat_button_set_to_normal(&priv->button, SAVU_BUTTON_TYPE_QUICKLAUNCH);
		g_free(priv->launchpath);
		priv->launchpath = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
	}
	gtk_widget_destroy(dialog);
}

static void button_macro(SavuconfigKeyComboBox *key_combo_box) {
	SavuconfigKeyComboBoxPrivate *priv = key_combo_box->priv;
	GaminggearMacro *gaminggear_macro;
	GtkWindow *parent;
	SavuRmpMacroKeyInfo *savu_macro;
	GError *local_error = NULL;

	parent = gtk_roccat_widget_get_gtk_window(GTK_WIDGET(key_combo_box));

	gaminggear_macro = gaminggear_macro_dialog(parent);
	if (!gaminggear_macro)
		return;

	savu_macro = savu_gaminggear_macro_to_rmp_macro_key_info(gaminggear_macro, &local_error);
	gaminggear_macro_free(gaminggear_macro);
	if (!savu_macro) {
		roccat_handle_error_dialog(parent, _("Could not convert macro"), &local_error);
		return;
	}

	roccat_button_set_to_normal(&priv->button, SAVU_BUTTON_TYPE_MACRO);
	savu_rmp_macro_key_info_free(priv->macro);
	priv->macro = savu_macro;
}

static void button_shortcut(SavuconfigKeyComboBox *key_combo_box) {
	SavuconfigKeyComboBoxPrivate *priv = key_combo_box->priv;
	RoccatShortcutDialog *dialog;

	dialog = ROCCAT_SHORTCUT_DIALOG(roccat_shortcut_dialog_new(gtk_roccat_widget_get_gtk_window(GTK_WIDGET(key_combo_box))));
	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
		roccat_button_set_to_shortcut(&priv->button, SAVU_BUTTON_TYPE_SHORTCUT,
				roccat_shortcut_dialog_get_hid(dialog),
				roccat_button_build_shortcut_modifier(
						roccat_shortcut_dialog_get_ctrl(dialog),
						roccat_shortcut_dialog_get_shift(dialog),
						roccat_shortcut_dialog_get_alt(dialog),
						roccat_shortcut_dialog_get_win(dialog)));
	}
	gtk_widget_destroy(GTK_WIDGET(dialog));
}

static void button_timer(SavuconfigKeyComboBox *key_combo_box) {
	SavuconfigKeyComboBoxPrivate *priv = key_combo_box->priv;
	RoccatTimer *roccat_timer;

	roccat_timer = roccat_timer_dialog(gtk_roccat_widget_get_gtk_window(GTK_WIDGET(key_combo_box)));
	if (roccat_timer) {
		roccat_button_set_to_normal(&priv->button, SAVU_BUTTON_TYPE_TIMER);
		roccat_timer_free(priv->timer);
		priv->timer = roccat_timer;
	}
}

static void button_normal(SavuconfigKeyComboBox *key_combo_box, gint type) {
	roccat_button_set_to_normal(&key_combo_box->priv->button, type);
}

static void button_changed_cb(GtkComboBox *combo, gpointer user_data) {
	SavuconfigKeyComboBox *key_combo_box = SAVUCONFIG_KEY_COMBO_BOX(combo);
	gint type;

	type = roccat_key_combo_box_get_active_type(ROCCAT_KEY_COMBO_BOX(combo));
	if (type == ROCCAT_KEY_COMBO_BOX_TYPE_NONE)
		return;

	switch (type) {
	case SAVU_BUTTON_TYPE_MACRO:
		button_macro(key_combo_box);
		break;
	case SAVU_BUTTON_TYPE_SHORTCUT:
		button_shortcut(key_combo_box);
		break;
	case SAVU_BUTTON_TYPE_QUICKLAUNCH:
		button_quicklaunch(key_combo_box);
		break;
	case SAVU_BUTTON_TYPE_TIMER:
		button_timer(key_combo_box);
		break;
	default:
		button_normal(key_combo_box, type);
	}

	set_text(key_combo_box);
}

GtkWidget *savuconfig_key_combo_box_new(guint mask) {
	SavuconfigKeyComboBox *key_combo_box;
	SavuconfigKeyComboBoxClass *key_combo_box_class;
	SavuconfigKeyComboBoxPrivate *priv;

	key_combo_box = SAVUCONFIG_KEY_COMBO_BOX(g_object_new(SAVUCONFIG_KEY_COMBO_BOX_TYPE,
			"exclude-mask", mask,
			NULL));

	priv = key_combo_box->priv;

	key_combo_box_class = SAVUCONFIG_KEY_COMBO_BOX_GET_CLASS(key_combo_box);
	roccat_key_combo_box_set_model(ROCCAT_KEY_COMBO_BOX(key_combo_box), key_combo_box_class->model);

	priv->changed_handler_id = g_signal_connect(G_OBJECT(key_combo_box), "changed", G_CALLBACK(button_changed_cb), NULL);

	return GTK_WIDGET(key_combo_box);
}

static void savuconfig_key_combo_box_init(SavuconfigKeyComboBox *key_combo_box) {
	SavuconfigKeyComboBoxPrivate *priv = SAVUCONFIG_KEY_COMBO_BOX_GET_PRIVATE(key_combo_box);
	key_combo_box->priv = priv;
}

static GtkTreeModel *button_info_model_new(void) {
	GtkTreeStore *store;
	GtkTreeIter toplevel;

	store = roccat_key_combo_box_store_new();

	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("Buttons"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Button 1 (Click)"), SAVU_BUTTON_TYPE_CLICK, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Button 2 (Universal scrolling)"), SAVU_BUTTON_TYPE_UNIVERSAL_SCROLLING, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Button 3 (Menu)"), SAVU_BUTTON_TYPE_MENU, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Button 4 (Scroll up)"), SAVU_BUTTON_TYPE_SCROLL_UP, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Button 5 (Scroll down)"), SAVU_BUTTON_TYPE_SCROLL_DOWN, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Button 6 (Tilt left)"), SAVU_BUTTON_TYPE_TILT_LEFT, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Button 7 (Tilt right)"), SAVU_BUTTON_TYPE_TILT_RIGHT, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Button 8 (Browser backward)"), SAVU_BUTTON_TYPE_BROWSER_BACKWARD, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Button 9 (Browser forward)"), SAVU_BUTTON_TYPE_BROWSER_FORWARD, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Double click"), SAVU_BUTTON_TYPE_DOUBLE_CLICK, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("CPI"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("CPI cycle"), SAVU_BUTTON_TYPE_CPI_CYCLE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("CPI up"), SAVU_BUTTON_TYPE_CPI_UP, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("CPI down"), SAVU_BUTTON_TYPE_CPI_DOWN, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("Easyaim"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT);

	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("400 CPI"), SAVU_BUTTON_TYPE_EASYAIM_400, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("800 CPI"), SAVU_BUTTON_TYPE_EASYAIM_800, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("1600 CPI"), SAVU_BUTTON_TYPE_EASYAIM_1600, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("4000 CPI"), SAVU_BUTTON_TYPE_EASYAIM_4000, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT);

	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("Easywheel"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT);

	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Sensitivity"), SAVU_BUTTON_TYPE_EASYWHEEL_SENSITIVITY, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Profile"), SAVU_BUTTON_TYPE_EASYWHEEL_PROFILE, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("CPI"), SAVU_BUTTON_TYPE_EASYWHEEL_CPI, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Volume"), SAVU_BUTTON_TYPE_EASYWHEEL_VOLUME, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT);
//	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Alt Tab"), SAVU_BUTTON_TYPE_EASYWHEEL_ALT_TAB, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT);
//	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Aero Flip 3D"), SAVU_BUTTON_TYPE_EASYWHEEL_AERO_FLIP_3D, ROCCAT_KEY_COMBO_BOX_GROUP_EASYSHIFT);

	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("Multimedia"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Open player"), SAVU_BUTTON_TYPE_MULTIMEDIA_OPEN_PLAYER, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Previous track"), SAVU_BUTTON_TYPE_MULTIMEDIA_PREV_TRACK, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Next track"), SAVU_BUTTON_TYPE_MULTIMEDIA_NEXT_TRACK, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Play/Pause"), SAVU_BUTTON_TYPE_MULTIMEDIA_PLAY_PAUSE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Stop"), SAVU_BUTTON_TYPE_MULTIMEDIA_STOP, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Mute"), SAVU_BUTTON_TYPE_MULTIMEDIA_MUTE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Volume up"), SAVU_BUTTON_TYPE_MULTIMEDIA_VOLUME_UP, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Volume down"), SAVU_BUTTON_TYPE_MULTIMEDIA_VOLUME_DOWN, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("Profile"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Profile cycle"), SAVU_BUTTON_TYPE_PROFILE_CYCLE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Profile up"), SAVU_BUTTON_TYPE_PROFILE_UP, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Profile down"), SAVU_BUTTON_TYPE_PROFILE_DOWN, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("Sensitivity"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Sensitivity cycle"), SAVU_BUTTON_TYPE_SENSITIVITY_CYCLE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Sensitivity up"), SAVU_BUTTON_TYPE_SENSITIVITY_UP, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Sensitivity down"), SAVU_BUTTON_TYPE_SENSITIVITY_DOWN, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	roccat_key_combo_box_store_append(store, &toplevel, NULL, _("Timer"), ROCCAT_KEY_COMBO_BOX_TYPE_NONE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Timer start"), SAVU_BUTTON_TYPE_TIMER, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, &toplevel, _("Timer stop"), SAVU_BUTTON_TYPE_TIMER_STOP, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	roccat_key_combo_box_store_append(store, NULL, NULL, _("Shortcut"), SAVU_BUTTON_TYPE_SHORTCUT, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, NULL, _("Macro"), SAVU_BUTTON_TYPE_MACRO, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, NULL, _("Quicklaunch"), SAVU_BUTTON_TYPE_QUICKLAUNCH, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, NULL, _("Windows key"), SAVU_BUTTON_TYPE_WINDOWS_KEY, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, NULL, _("Open driver"), SAVU_BUTTON_TYPE_OPEN_DRIVER, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, NULL, _("Easyshift"), SAVU_BUTTON_TYPE_EASYSHIFT, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	roccat_key_combo_box_store_append(store, NULL, NULL, _("Disabled"), SAVU_BUTTON_TYPE_DISABLED, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	return GTK_TREE_MODEL(store);
}

static void finalize(GObject *object) {
	SavuconfigKeyComboBoxPrivate *priv = SAVUCONFIG_KEY_COMBO_BOX(object)->priv;
	savu_rmp_macro_key_info_free(priv->macro);
	roccat_timer_free(priv->timer);
	g_free(priv->launchpath);
	G_OBJECT_CLASS(savuconfig_key_combo_box_parent_class)->finalize(object);
}

static void savuconfig_key_combo_box_class_init(SavuconfigKeyComboBoxClass *klass) {
	GObjectClass *gobject_class = (GObjectClass*)klass;

	gobject_class->finalize = finalize;
	g_type_class_add_private(klass, sizeof(SavuconfigKeyComboBoxPrivate));
	klass->model = button_info_model_new();
}

void savuconfig_key_combo_box_set_from_rmp(SavuconfigKeyComboBox *key_combo_box, SavuRmp *rmp, guint index) {
	SavuconfigKeyComboBoxPrivate *priv = key_combo_box->priv;
	RoccatButton *button;
	SavuRmpTimer *timer;

	button = savu_rmp_get_button(rmp, index);
	priv->button = *button;
	g_free(button);

	switch (priv->button.type) {
	case SAVU_BUTTON_TYPE_MACRO:
		savu_rmp_macro_key_info_free(priv->macro);
		priv->macro = savu_rmp_get_macro_key_info(rmp, index);
		break;
	case SAVU_BUTTON_TYPE_QUICKLAUNCH:
		g_free(priv->launchpath);
		priv->launchpath = savu_rmp_get_launchpath(rmp, index);
		break;
	case SAVU_BUTTON_TYPE_TIMER:
		timer = savu_rmp_get_timer(rmp, index);
		roccat_timer_free(priv->timer);
		priv->timer = savu_rmp_timer_to_roccat_timer(timer);
		g_free(timer);
		break;
	default:
		break;
	}

	set_text(key_combo_box);
}

void savuconfig_key_combo_box_update_rmp(SavuconfigKeyComboBox *key_combo_box, SavuRmp *rmp, guint index) {
	SavuconfigKeyComboBoxPrivate *priv = key_combo_box->priv;
	SavuRmpTimer *timer;

	savu_rmp_set_button(rmp, index, &priv->button);

	switch (priv->button.type) {
	case SAVU_BUTTON_TYPE_MACRO:
		savu_rmp_set_macro_key_info(rmp, index, priv->macro);
		break;
	case SAVU_BUTTON_TYPE_QUICKLAUNCH:
		savu_rmp_set_launchpath(rmp, index, priv->launchpath);
		break;
	case SAVU_BUTTON_TYPE_TIMER:
		timer = savu_roccat_timer_to_rmp_timer(priv->timer);
		savu_rmp_set_timer(rmp, index, timer);
		g_free(timer);
		break;
	}
}
