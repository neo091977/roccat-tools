#ifndef __ROCCAT_TALK_TARGET_DIALOG_H__
#define __ROCCAT_TALK_TARGET_DIALOG_H__

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

#define ROCCAT_TALK_TARGET_DIALOG_TYPE (roccat_talk_target_dialog_get_type())
#define ROCCAT_TALK_TARGET_DIALOG(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), ROCCAT_TALK_TARGET_DIALOG_TYPE, RoccatTalkTargetDialog))
#define IS_ROCCAT_TALK_TARGET_DIALOG(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), ROCCAT_TALK_TARGET_DIALOG_TYPE))

typedef struct _RoccatTalkTargetDialog RoccatTalkTargetDialog;
typedef struct _RoccatTalkTargetDialogPrivate RoccatTalkTargetDialogPrivate;

struct _RoccatTalkTargetDialog {
	GtkDialog dialog;
	RoccatTalkTargetDialogPrivate *priv;
};

GType roccat_talk_target_dialog_get_type(void);
guint roccat_talk_target_dialog_get_value(RoccatTalkTargetDialog *talk_target_dialog);
void roccat_talk_target_dialog_set_value(RoccatTalkTargetDialog *talk_target_dialog, guint value);
guint roccat_talk_target_dialog_get_source_talk_device_type(RoccatTalkTargetDialog *talk_target_dialog);
void roccat_talk_target_dialog_set_source_talk_device_type(RoccatTalkTargetDialog *talk_target_dialog, guint value);
GtkWidget *roccat_talk_target_dialog_new(GtkWindow *parent, guint source_talk_device);

G_END_DECLS

#endif
