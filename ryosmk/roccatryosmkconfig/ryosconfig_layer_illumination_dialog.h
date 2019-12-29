#ifndef __ROCCAT_RYOSCONFIG_LAYER_ILLUMINATION_DIALOG_H__
#define __ROCCAT_RYOSCONFIG_LAYER_ILLUMINATION_DIALOG_H__

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

#include "ryos.h"
#include "ryos_stored_lights.h"
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define RYOSCONFIG_LAYER_ILLUMINATION_DIALOG_TYPE (ryosconfig_layer_illumination_dialog_get_type())
#define RYOSCONFIG_LAYER_ILLUMINATION_DIALOG(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), RYOSCONFIG_LAYER_ILLUMINATION_DIALOG_TYPE, RyosconfigLayerIlluminationDialog))
#define IS_RYOSCONFIG_LAYER_ILLUMINATION_DIALOG(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), RYOSCONFIG_LAYER_ILLUMINATION_DIALOG_TYPE))

typedef struct _RyosconfigLayerIlluminationDialog RyosconfigLayerIlluminationDialog;
typedef struct _RyosconfigLayerIlluminationDialogPrivate RyosconfigLayerIlluminationDialogPrivate;

struct _RyosconfigLayerIlluminationDialog {
	GtkDialog dialog;
	RyosconfigLayerIlluminationDialogPrivate *priv;
};

GType ryosconfig_layer_illumination_dialog_get_type(void);
GtkWidget *ryosconfig_layer_illumination_dialog_new(GtkWindow *parent, RoccatDevice *ryos);

void ryosconfig_layer_illumination_dialog_get_layer(RyosconfigLayerIlluminationDialog *dialog, RyosLightLayer *light_layer);
void ryosconfig_layer_illumination_dialog_set_layer(RyosconfigLayerIlluminationDialog *dialog, RyosLightLayer const *light_layer);

/* returns TRUE if changed */
gboolean ryosconfig_layer_illumination_dialog_run(GtkWindow *parent, RoccatDevice *ryos, RyosLightLayer *light_layer);

G_END_DECLS

#endif
