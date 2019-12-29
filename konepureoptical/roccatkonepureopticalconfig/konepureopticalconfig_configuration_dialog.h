#ifndef __ROCCAT_KONEPUREOPTICALCONFIG_CONFIGURATION_EDITOR_H__
#define __ROCCAT_KONEPUREOPTICALCONFIG_CONFIGURATION_EDITOR_H__

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

#define KONEPUREOPTICALCONFIG_CONFIGURATION_DIALOG_TYPE (konepureopticalconfig_configuration_dialog_get_type())
#define KONEPUREOPTICALCONFIG_CONFIGURATION_DIALOG(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), KONEPUREOPTICALCONFIG_CONFIGURATION_DIALOG_TYPE, KonepureopticalconfigConfigurationDialog))
#define IS_KONEPUREOPTICALCONFIG_CONFIGURATION_DIALOG(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), KONEPUREOPTICALCONFIG_CONFIGURATION_DIALOG_TYPE))

typedef struct _KonepureopticalconfigConfigurationDialog KonepureopticalconfigConfigurationDialog;
typedef struct _KonepureopticalconfigConfigurationDialogPrivate KonepureopticalconfigConfigurationDialogPrivate;

struct _KonepureopticalconfigConfigurationDialog {
	GtkDialog dialog;
	KonepureopticalconfigConfigurationDialogPrivate *priv;
};

GType konepureopticalconfig_configuration_dialog_get_type(void);
gboolean konepureopticalconfig_configuration_dialog(GtkWindow *parent, RoccatKeyFile *config);
GtkWidget *konepureopticalconfig_configuration_dialog_new(GtkWindow *parent);

void konepureopticalconfig_configuration_dialog_get_configuration(KonepureopticalconfigConfigurationDialog *konepureopticalconfig_configuration_dialog, RoccatKeyFile *config);
void konepureopticalconfig_configuration_dialog_set_configuration(KonepureopticalconfigConfigurationDialog *konepureopticalconfig_configuration_dialog, RoccatKeyFile *config);

gboolean konepureopticalconfig_configuration_save(GtkWindow *parent, RoccatKeyFile *config);

G_END_DECLS

#endif
