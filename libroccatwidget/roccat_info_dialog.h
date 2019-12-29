#ifndef __ROCCAT_INFO_DIALOG_H__
#define __ROCCAT_INFO_DIALOG_H__

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

#define ROCCAT_INFO_DIALOG_TYPE (roccat_info_dialog_get_type())
#define ROCCAT_INFO_DIALOG(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), ROCCAT_INFO_DIALOG_TYPE, RoccatInfoDialog))
#define IS_ROCCAT_INFO_DIALOG(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), ROCCAT_INFO_DIALOG_TYPE))
#define ROCCAT_INFO_DIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), ROCCAT_INFO_DIALOG_TYPE, RoccatInfoDialogClass))
#define IS_ROCCAT_INFO_DIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), ROCCAT_INFO_DIALOG_TYPE))

typedef struct _RoccatInfoDialog RoccatInfoDialog;
typedef struct _RoccatInfoDialogClass RoccatInfoDialogClass;
typedef struct _RoccatInfoDialogPrivate RoccatInfoDialogPrivate;

struct _RoccatInfoDialog {
	GtkDialog parent;
	RoccatInfoDialogPrivate *priv;
};

struct _RoccatInfoDialogClass {
	GtkDialogClass parent_class;
};

GType roccat_info_dialog_get_type(void);
GtkWidget *roccat_info_dialog_new(GtkWindow *parent);
void roccat_info_dialog_add_line(RoccatInfoDialog *info_dialog, gchar const *labeltext, GtkWidget *widget);

G_END_DECLS

#endif
