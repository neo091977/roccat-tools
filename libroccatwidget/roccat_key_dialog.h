#ifndef __ROCCAT_KEY_DIALOG_H__
#define __ROCCAT_KEY_DIALOG_H__

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

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define ROCCAT_KEY_DIALOG_TYPE (roccat_key_dialog_get_type())
#define ROCCAT_KEY_DIALOG(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), ROCCAT_KEY_DIALOG_TYPE, RoccatKeyDialog))
#define IS_ROCCAT_KEY_DIALOG(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), ROCCAT_KEY_DIALOG_TYPE))

typedef struct _RoccatKeyDialog RoccatKeyDialog;
typedef struct _RoccatKeyDialogPrivate RoccatKeyDialogPrivate;

struct _RoccatKeyDialog {
	GtkDialog dialog;
	RoccatKeyDialogPrivate *priv;
};

GType roccat_key_dialog_get_type(void);
GtkWidget *roccat_key_dialog_new(GtkWindow *parent);
guint8 roccat_key_dialog_get_hid(RoccatKeyDialog *key_dialog);
void roccat_key_dialog_set_hid(RoccatKeyDialog *key_dialog, guint8 hid_usage_id);

G_END_DECLS

#endif
