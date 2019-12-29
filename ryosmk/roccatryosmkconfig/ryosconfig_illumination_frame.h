#ifndef __ROCCAT_RYOSCONFIG_ILLUMINATION_FRAME_H__
#define __ROCCAT_RYOSCONFIG_ILLUMINATION_FRAME_H__

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

#include <gtk/gtk.h>
#include "ryos_stored_lights.h"

G_BEGIN_DECLS

#define RYOSCONFIG_ILLUMINATION_FRAME_TYPE (ryosconfig_illumination_frame_get_type())
#define RYOSCONFIG_ILLUMINATION_FRAME(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), RYOSCONFIG_ILLUMINATION_FRAME_TYPE, RyosconfigIlluminationFrame))
#define IS_RYOSCONFIG_ILLUMINATION_FRAME(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), RYOSCONFIG_ILLUMINATION_FRAME_TYPE))

typedef struct _RyosconfigIlluminationFrame RyosconfigIlluminationFrame;

GType ryosconfig_illumination_frame_get_type(void);
GtkWidget *ryosconfig_illumination_frame_new(void);

void ryosconfig_illumination_frame_set_device_type(RyosconfigIlluminationFrame *illumination_frame, RoccatDevice const *device);

void ryosconfig_illumination_frame_set_mode_blocked(RyosconfigIlluminationFrame *illumination_frame, guint mode);
void ryosconfig_illumination_frame_set_pointer_blocked(RyosconfigIlluminationFrame *illumination_frame, RyosLightLayer *layer);
guint ryosconfig_illumination_frame_get_mode(RyosconfigIlluminationFrame *illumination_frame);
guint ryosconfig_illumination_frame_get_layer(RyosconfigIlluminationFrame *illumination_frame);

G_END_DECLS

#endif
