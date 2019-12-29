#ifndef __ROCCAT_KONECONFIG_CONFIGURATION_EDITOR_H__
#define __ROCCAT_KONECONFIG_CONFIGURATION_EDITOR_H__

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

#define KONECONFIG_CONFIGURATION_DIALOG_TYPE (koneconfig_configuration_dialog_get_type())
#define KONECONFIG_CONFIGURATION_DIALOG(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), KONECONFIG_CONFIGURATION_DIALOG_TYPE, KoneconfigConfigurationDialog))
#define IS_KONECONFIG_CONFIGURATION_DIALOG(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), KONECONFIG_CONFIGURATION_DIALOG_TYPE))

typedef struct _KoneconfigConfigurationDialog KoneconfigConfigurationDialog;
typedef struct _KoneconfigConfigurationDialogPrivate KoneconfigConfigurationDialogPrivate;

struct _KoneconfigConfigurationDialog {
	GtkDialog dialog;
	KoneconfigConfigurationDialogPrivate *priv;
};

GType koneconfig_configuration_dialog_get_type(void);
gboolean koneconfig_configuration_dialog(GtkWindow *parent, RoccatKeyFile *config);
GtkWidget *koneconfig_configuration_dialog_new(GtkWindow *parent);

void koneconfig_configuration_dialog_get_configuration(KoneconfigConfigurationDialog *koneconfig_configuration_dialog, RoccatKeyFile *config);
void koneconfig_configuration_dialog_set_configuration(KoneconfigConfigurationDialog *koneconfig_configuration_dialog, RoccatKeyFile *config);

gboolean koneconfig_configuration_save(GtkWindow *parent, RoccatKeyFile *config);

G_END_DECLS

#endif
