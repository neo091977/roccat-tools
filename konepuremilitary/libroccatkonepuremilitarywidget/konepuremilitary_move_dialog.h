#ifndef __ROCCAT_KONEPUREMILITARY_MOVE_DIALOG_H__
#define __ROCCAT_KONEPUREMILITARY_MOVE_DIALOG_H__

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

#define KONEPUREMILITARY_MOVE_DIALOG_TYPE (konepuremilitary_move_dialog_get_type())
#define KONEPUREMILITARY_MOVE_DIALOG(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), KONEPUREMILITARY_MOVE_DIALOG_TYPE, KonepuremilitaryMoveDialog))
#define IS_KONEPUREMILITARY_MOVE_DIALOG(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), KONEPUREMILITARY_MOVE_DIALOG_TYPE))

typedef struct _KonepuremilitaryMoveDialog KonepuremilitaryMoveDialog;
typedef struct _KonepuremilitaryMoveDialogPrivate KonepuremilitaryMoveDialogPrivate;

struct _KonepuremilitaryMoveDialog {
	GtkDialog dialog;
	KonepuremilitaryMoveDialogPrivate *priv;
};

GType konepuremilitary_move_dialog_get_type(void);
GtkWidget *konepuremilitary_move_dialog_new(GtkWindow *parent, gchar const *title, gchar const *text);
gboolean konepuremilitary_move_dialog_run(KonepuremilitaryMoveDialog *move_dialog, guint timeout);
gboolean konepuremilitary_move_dialog(GtkWindow *parent, gchar const *title, gchar const *text, guint timeout);

G_END_DECLS

#endif
