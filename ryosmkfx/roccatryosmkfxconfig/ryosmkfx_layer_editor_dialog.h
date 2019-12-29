#ifndef __ROCCAT_RYOSMKFX_LAYER_EDITOR_DIALOG_H__
#define __ROCCAT_RYOSMKFX_LAYER_EDITOR_DIALOG_H__

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

#include "ryosmkfx_light_layer.h"
#include "roccat_device.h"
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define RYOSMKFX_LAYER_EDITOR_DIALOG_TYPE (ryosmkfx_layer_editor_dialog_get_type())
#define RYOSMKFX_LAYER_EDITOR_DIALOG(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), RYOSMKFX_LAYER_EDITOR_DIALOG_TYPE, RyosmkfxLayerEditorDialog))
#define IS_RYOSMKFX_LAYER_EDITOR_DIALOG(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), RYOSMKFX_LAYER_EDITOR_DIALOG_TYPE))

typedef struct _RyosmkfxLayerEditorDialog RyosmkfxLayerEditorDialog;
typedef struct _RyosmkfxLayerEditorDialogPrivate RyosmkfxLayerEditorDialogPrivate;

struct _RyosmkfxLayerEditorDialog {
	GtkDialog dialog;
	RyosmkfxLayerEditorDialogPrivate *priv;
};

GType ryosmkfx_layer_editor_dialog_get_type(void);
GtkWidget *ryosmkfx_layer_editor_dialog_new(GtkWindow *parent, RoccatDevice *device);

void ryosmkfx_layer_editor_dialog_get_layer_datas(RyosmkfxLayerEditorDialog *dialog, RyosmkfxLightLayerData *light_layer_data);
void ryosmkfx_layer_editor_dialog_set_layer_datas(RyosmkfxLayerEditorDialog *dialog, RyosmkfxLightLayerData const *light_layer_data);

/* returns TRUE if changed */
gboolean ryosmkfx_layer_editor_dialog_run(GtkWindow *parent, RoccatDevice *device, RyosmkfxLightLayerData *light_layer_data);

G_END_DECLS

#endif
