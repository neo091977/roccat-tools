#ifndef __ROCCAT_RYOSMKFX_CONFIGURATION_EDITOR_H__
#define __ROCCAT_RYOSMKFX_CONFIGURATION_EDITOR_H__

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

#define RYOSMKFX_CONFIGURATION_DIALOG_TYPE (ryosmkfx_configuration_dialog_get_type())
#define RYOSMKFX_CONFIGURATION_DIALOG(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), RYOSMKFX_CONFIGURATION_DIALOG_TYPE, RyosmkfxConfigurationDialog))
#define IS_RYOSMKFX_CONFIGURATION_DIALOG(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), RYOSMKFX_CONFIGURATION_DIALOG_TYPE))

typedef struct _RyosmkfxConfigurationDialog RyosmkfxConfigurationDialog;
typedef struct _RyosmkfxConfigurationDialogPrivate RyosmkfxConfigurationDialogPrivate;

struct _RyosmkfxConfigurationDialog {
	GtkDialog dialog;
	RyosmkfxConfigurationDialogPrivate *priv;
};

GType ryosmkfx_configuration_dialog_get_type(void);
gboolean ryosmkfx_configuration_dialog(GtkWindow *parent, RoccatKeyFile *config);
GtkWidget *ryosmkfx_configuration_dialog_new(GtkWindow *parent);

void ryosmkfx_configuration_dialog_get_configuration(RyosmkfxConfigurationDialog *configuration_dialog, RoccatKeyFile *config);
void ryosmkfx_configuration_dialog_set_configuration(RyosmkfxConfigurationDialog *configuration_dialog, RoccatKeyFile *config);

gboolean ryosmkfx_configuration_dialog_save(GtkWindow *parent, RoccatKeyFile *config);

G_END_DECLS

#endif
