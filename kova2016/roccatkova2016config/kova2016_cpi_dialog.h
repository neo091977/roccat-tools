#ifndef __KOVA2016_CPI_DIALOG_H__
#define __KOVA2016_CPI_DIALOG_H__

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

#define KOVA2016_CPI_DIALOG_TYPE (kova2016_cpi_dialog_get_type())
#define KOVA2016_CPI_DIALOG(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), KOVA2016_CPI_DIALOG_TYPE, Kova2016CpiDialog))
#define IS_KOVA2016_CPI_DIALOG(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), KOVA2016_CPI_DIALOG_TYPE))

typedef struct _Kova2016CpiDialog Kova2016CpiDialog;
typedef struct _Kova2016CpiDialogPrivate Kova2016CpiDialogPrivate;

struct _Kova2016CpiDialog {
	GtkDialog dialog;
	Kova2016CpiDialogPrivate *priv;
};

gint kova2016_cpi_dialog(GtkWindow *parent);
gint kova2016_cpi_dialog_run(Kova2016CpiDialog *cpi_dialog);
gint kova2016_cpi_dialog_get_value(Kova2016CpiDialog *cpi_dialog);
GtkWidget *kova2016_cpi_dialog_new(GtkWindow *parent);

G_END_DECLS

#endif
