#ifndef __ROCCAT_KONEPUREMILITARYCONFIG_CONFIGURATION_EDITOR_H__
#define __ROCCAT_KONEPUREMILITARYCONFIG_CONFIGURATION_EDITOR_H__

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

#define KONEPUREMILITARYCONFIG_CONFIGURATION_DIALOG_TYPE (konepuremilitaryconfig_configuration_dialog_get_type())
#define KONEPUREMILITARYCONFIG_CONFIGURATION_DIALOG(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), KONEPUREMILITARYCONFIG_CONFIGURATION_DIALOG_TYPE, KonepuremilitaryconfigConfigurationDialog))
#define IS_KONEPUREMILITARYCONFIG_CONFIGURATION_DIALOG(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), KONEPUREMILITARYCONFIG_CONFIGURATION_DIALOG_TYPE))

typedef struct _KonepuremilitaryconfigConfigurationDialog KonepuremilitaryconfigConfigurationDialog;
typedef struct _KonepuremilitaryconfigConfigurationDialogPrivate KonepuremilitaryconfigConfigurationDialogPrivate;

struct _KonepuremilitaryconfigConfigurationDialog {
	GtkDialog dialog;
	KonepuremilitaryconfigConfigurationDialogPrivate *priv;
};

GType konepuremilitaryconfig_configuration_dialog_get_type(void);
gboolean konepuremilitaryconfig_configuration_dialog(GtkWindow *parent, RoccatKeyFile *config);
GtkWidget *konepuremilitaryconfig_configuration_dialog_new(GtkWindow *parent);

void konepuremilitaryconfig_configuration_dialog_get_configuration(KonepuremilitaryconfigConfigurationDialog *konepuremilitaryconfig_configuration_dialog, RoccatKeyFile *config);
void konepuremilitaryconfig_configuration_dialog_set_configuration(KonepuremilitaryconfigConfigurationDialog *konepuremilitaryconfig_configuration_dialog, RoccatKeyFile *config);

gboolean konepuremilitaryconfig_configuration_save(GtkWindow *parent, RoccatKeyFile *config);

G_END_DECLS

#endif
