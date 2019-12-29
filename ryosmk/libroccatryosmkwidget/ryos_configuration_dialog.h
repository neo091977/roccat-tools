#ifndef __ROCCAT_RYOS_CONFIGURATION_EDITOR_H__
#define __ROCCAT_RYOS_CONFIGURATION_EDITOR_H__

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

#include "ryos_config.h"
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define RYOS_CONFIGURATION_DIALOG_TYPE (ryos_configuration_dialog_get_type())
#define RYOS_CONFIGURATION_DIALOG(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), RYOS_CONFIGURATION_DIALOG_TYPE, RyosConfigurationDialog))
#define IS_RYOS_CONFIGURATION_DIALOG(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), RYOS_CONFIGURATION_DIALOG_TYPE))

typedef struct _RyosConfigurationDialog RyosConfigurationDialog;
typedef struct _RyosConfigurationDialogPrivate RyosConfigurationDialogPrivate;

struct _RyosConfigurationDialog {
	GtkDialog dialog;
	RyosConfigurationDialogPrivate *priv;
};

GType ryos_configuration_dialog_get_type(void);
gboolean ryos_configuration_dialog(GtkWindow *parent, RoccatKeyFile *config);
GtkWidget *ryos_configuration_dialog_new(GtkWindow *parent);

void ryos_configuration_dialog_get_configuration(RyosConfigurationDialog *ryos_configuration_dialog, RoccatKeyFile *config);
void ryos_configuration_dialog_set_configuration(RyosConfigurationDialog *ryos_configuration_dialog, RoccatKeyFile *config);

gboolean ryos_configuration_dialog_save(GtkWindow *parent, RoccatKeyFile *config);

G_END_DECLS

#endif
