#ifndef __ROCCAT_RYOSTKLCONFIG_ILLUMINATION_FRAME_H__
#define __ROCCAT_RYOSTKLCONFIG_ILLUMINATION_FRAME_H__

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

#define RYOSTKLCONFIG_ILLUMINATION_FRAME_TYPE (ryostklconfig_illumination_frame_get_type())
#define RYOSTKLCONFIG_ILLUMINATION_FRAME(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), RYOSTKLCONFIG_ILLUMINATION_FRAME_TYPE, RyostklconfigIlluminationFrame))
#define IS_RYOSTKLCONFIG_ILLUMINATION_FRAME(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), RYOSTKLCONFIG_ILLUMINATION_FRAME_TYPE))

typedef struct _RyostklconfigIlluminationFrame RyostklconfigIlluminationFrame;

GType ryostklconfig_illumination_frame_get_type(void);
GtkWidget *ryostklconfig_illumination_frame_new(void);

void ryostklconfig_illumination_frame_set_device_type(RyostklconfigIlluminationFrame *illumination_frame, RoccatDevice const *device);

void ryostklconfig_illumination_frame_set_mode_blocked(RyostklconfigIlluminationFrame *illumination_frame, guint mode);
void ryostklconfig_illumination_frame_set_pointer_blocked(RyostklconfigIlluminationFrame *illumination_frame, RyostklLightLayer *layer);
guint ryostklconfig_illumination_frame_get_mode(RyostklconfigIlluminationFrame *illumination_frame);
guint ryostklconfig_illumination_frame_get_layer(RyostklconfigIlluminationFrame *illumination_frame);

G_END_DECLS

#endif
