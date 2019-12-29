#ifndef __ROCCAT_SHORTCUT_DIALOG_H__
#define __ROCCAT_SHORTCUT_DIALOG_H__

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

#include <gaminggear/macro.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define ROCCAT_SHORTCUT_DIALOG_TYPE (roccat_shortcut_dialog_get_type())
#define ROCCAT_SHORTCUT_DIALOG(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), ROCCAT_SHORTCUT_DIALOG_TYPE, RoccatShortcutDialog))
#define IS_ROCCAT_SHORTCUT_DIALOG(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), ROCCAT_SHORTCUT_DIALOG_TYPE))

typedef struct _RoccatShortcutDialog RoccatShortcutDialog;
typedef struct _RoccatShortcutDialogPrivate RoccatShortcutDialogPrivate;

struct _RoccatShortcutDialog {
	GtkDialog dialog;
	RoccatShortcutDialogPrivate *priv;
};

GType roccat_shortcut_dialog_get_type(void);
gboolean roccat_shortcut_dialog_get_ctrl(RoccatShortcutDialog *shortcut_dialog);
void roccat_shortcut_dialog_set_ctrl(RoccatShortcutDialog *shortcut_dialog, gboolean value);
gboolean roccat_shortcut_dialog_get_shift(RoccatShortcutDialog *shortcut_dialog);
void roccat_shortcut_dialog_set_shift(RoccatShortcutDialog *shortcut_dialog, gboolean value);
gboolean roccat_shortcut_dialog_get_alt(RoccatShortcutDialog *shortcut_dialog);
void roccat_shortcut_dialog_set_alt(RoccatShortcutDialog *shortcut_dialog, gboolean value);
gboolean roccat_shortcut_dialog_get_win(RoccatShortcutDialog *shortcut_dialog);
void roccat_shortcut_dialog_set_win(RoccatShortcutDialog *shortcut_dialog, gboolean value);
guint8 roccat_shortcut_dialog_get_hid(RoccatShortcutDialog *shortcut_dialog);
void roccat_shortcut_dialog_set_hid(RoccatShortcutDialog *shortcut_dialog, guint8 hid_usage_id);
GaminggearMacro *roccat_shortcut_dialog_get_macro(RoccatShortcutDialog *shortcut_dialog);
gchar *roccat_shortcut_dialog_create_name_full(gboolean ctrl, gboolean shift, gboolean alt, gboolean win, guint hid_usage_id);
gchar *roccat_shortcut_dialog_create_name(guint8 modifier, guint hid_usage_id);
GtkWidget *roccat_shortcut_dialog_new(GtkWindow *parent);

G_END_DECLS

#endif
