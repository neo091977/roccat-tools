#ifndef __ROCCAT_RYOSTKLCONFIG_LAYER_ILLUMINATION_DIALOG_H__
#define __ROCCAT_RYOSTKLCONFIG_LAYER_ILLUMINATION_DIALOG_H__

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

#include "ryostkl_light_layer.h"
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define RYOSTKLCONFIG_LAYER_ILLUMINATION_DIALOG_TYPE (ryostklconfig_layer_illumination_dialog_get_type())
#define RYOSTKLCONFIG_LAYER_ILLUMINATION_DIALOG(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), RYOSTKLCONFIG_LAYER_ILLUMINATION_DIALOG_TYPE, RyostklconfigLayerIlluminationDialog))
#define IS_RYOSTKLCONFIG_LAYER_ILLUMINATION_DIALOG(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), RYOSTKLCONFIG_LAYER_ILLUMINATION_DIALOG_TYPE))

typedef struct _RyostklconfigLayerIlluminationDialog RyostklconfigLayerIlluminationDialog;
typedef struct _RyostklconfigLayerIlluminationDialogPrivate RyostklconfigLayerIlluminationDialogPrivate;

struct _RyostklconfigLayerIlluminationDialog {
	GtkDialog dialog;
	RyostklconfigLayerIlluminationDialogPrivate *priv;
};

GType ryostklconfig_layer_illumination_dialog_get_type(void);
GtkWidget *ryostklconfig_layer_illumination_dialog_new(GtkWindow *parent, RoccatDevice *ryos);

void ryostklconfig_layer_illumination_dialog_get_layer(RyostklconfigLayerIlluminationDialog *dialog, RyostklLightLayer *light_layer);
void ryostklconfig_layer_illumination_dialog_set_layer(RyostklconfigLayerIlluminationDialog *dialog, RyostklLightLayer const *light_layer);

/* returns TRUE if changed */
gboolean ryostklconfig_layer_illumination_dialog_run(GtkWindow *parent, RoccatDevice *ryos, RyostklLightLayer *light_layer);

G_END_DECLS

#endif
