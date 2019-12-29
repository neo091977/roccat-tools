#ifndef __ROCCAT_ARVOCONFIG_CONFIGURATION_DIALOG_H__
#define __ROCCAT_ARVOCONFIG_CONFIGURATION_DIALOG_H__

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

#include "roccat_config.h"
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define ARVOCONFIG_CONFIGURATION_DIALOG_TYPE (arvoconfig_configuration_dialog_get_type())
#define ARVOCONFIG_CONFIGURATION_DIALOG(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), ARVOCONFIG_CONFIGURATION_DIALOG_TYPE, ArvoconfigConfigurationDialog))
#define IS_ARVOCONFIG_CONFIGURATION_DIALOG(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), ARVOCONFIG_CONFIGURATION_DIALOG_TYPE))

typedef struct _ArvoconfigConfigurationDialog ArvoconfigConfigurationDialog;
typedef struct _ArvoconfigConfigurationDialogPrivate ArvoconfigConfigurationDialogPrivate;

struct _ArvoconfigConfigurationDialog {
	GtkDialog dialog;
	ArvoconfigConfigurationDialogPrivate *priv;
};

GType arvoconfig_configuration_dialog_get_type(void);
gboolean arvoconfig_configuration_dialog(GtkWindow *parent, RoccatKeyFile *config);
GtkWidget *arvoconfig_configuration_dialog_new(GtkWindow *parent);

void arvoconfig_configuration_dialog_get_configuration(ArvoconfigConfigurationDialog *arvoconfig_configuration_dialog, RoccatKeyFile *config);
void arvoconfig_configuration_dialog_set_configuration(ArvoconfigConfigurationDialog *arvoconfig_configuration_dialog, RoccatKeyFile *config);

gboolean arvoconfig_configuration_save(GtkWindow *parent, RoccatKeyFile *config);

G_END_DECLS

#endif
