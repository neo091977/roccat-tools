#ifndef __ROCCAT_SOVA_CONFIGURATION_DIALOG_H__
#define __ROCCAT_SOVA_CONFIGURATION_DIALOG_H__

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

#include "sova_config.h"
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define SOVA_CONFIGURATION_DIALOG_TYPE (sova_configuration_dialog_get_type())
#define SOVA_CONFIGURATION_DIALOG(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), SOVA_CONFIGURATION_DIALOG_TYPE, SovaConfigurationDialog))
#define IS_SOVA_CONFIGURATION_DIALOG(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), SOVA_CONFIGURATION_DIALOG_TYPE))

typedef struct _SovaConfigurationDialog SovaConfigurationDialog;
typedef struct _SovaConfigurationDialogPrivate SovaConfigurationDialogPrivate;

struct _SovaConfigurationDialog {
	GtkDialog dialog;
	SovaConfigurationDialogPrivate *priv;
};

GType sova_configuration_dialog_get_type(void);
gboolean sova_configuration_dialog(GtkWindow *parent, RoccatKeyFile *config);
GtkWidget *sova_configuration_dialog_new(GtkWindow *parent);

void sova_configuration_dialog_get_configuration(SovaConfigurationDialog *sova_configuration_dialog, RoccatKeyFile *config);
void sova_configuration_dialog_set_configuration(SovaConfigurationDialog *sova_configuration_dialog, RoccatKeyFile *config);

gboolean sova_configuration_dialog_save(GtkWindow *parent, RoccatKeyFile *config);

G_END_DECLS

#endif
