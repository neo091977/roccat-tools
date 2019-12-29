#ifndef __ROCCAT_TYON_CONFIGURATION_DIALOG_H__
#define __ROCCAT_TYON_CONFIGURATION_DIALOG_H__

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

#include "roccat_key_file.h"
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define TYON_CONFIGURATION_DIALOG_TYPE (tyon_configuration_dialog_get_type())
#define TYON_CONFIGURATION_DIALOG(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), TYON_CONFIGURATION_DIALOG_TYPE, TyonConfigurationDialog))
#define IS_TYON_CONFIGURATION_DIALOG(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), TYON_CONFIGURATION_DIALOG_TYPE))

typedef struct _TyonConfigurationDialog TyonConfigurationDialog;
typedef struct _TyonConfigurationDialogPrivate TyonConfigurationDialogPrivate;

struct _TyonConfigurationDialog {
	GtkDialog dialog;
	TyonConfigurationDialogPrivate *priv;
};

GType tyon_configuration_dialog_get_type(void);
gboolean tyon_configuration_dialog(GtkWindow *parent, RoccatKeyFile *config);
GtkWidget *tyon_configuration_dialog_new(GtkWindow *parent);

void tyon_configuration_dialog_get_configuration(TyonConfigurationDialog *tyon_configuration_dialog, RoccatKeyFile *config);
void tyon_configuration_dialog_set_configuration(TyonConfigurationDialog *tyon_configuration_dialog, RoccatKeyFile *config);

gboolean tyon_configuration_dialog_save(GtkWindow *parent, RoccatKeyFile *config);

G_END_DECLS

#endif
