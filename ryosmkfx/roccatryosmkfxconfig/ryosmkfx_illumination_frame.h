#ifndef __ROCCAT_RYOSMKFX_ILLUMINATION_FRAME_H__
#define __ROCCAT_RYOSMKFX_ILLUMINATION_FRAME_H__

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
#include "ryosmkfx_light.h"

G_BEGIN_DECLS

#define RYOSMKFX_ILLUMINATION_FRAME_TYPE (ryosmkfx_illumination_frame_get_type())
#define RYOSMKFX_ILLUMINATION_FRAME(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), RYOSMKFX_ILLUMINATION_FRAME_TYPE, RyosmkfxIlluminationFrame))
#define IS_RYOSMKFX_ILLUMINATION_FRAME(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), RYOSMKFX_ILLUMINATION_FRAME_TYPE))

typedef struct _RyosmkfxIlluminationFrame RyosmkfxIlluminationFrame;

GType ryosmkfx_illumination_frame_get_type(void);
GtkWidget *ryosmkfx_illumination_frame_new(void);

void ryosmkfx_illumination_frame_set_plain_color(RyosmkfxIlluminationFrame *illumination_frame, GdkColor const *color);
void ryosmkfx_illumination_frame_get_plain_color(RyosmkfxIlluminationFrame *illumination_frame, GdkColor *color);

void ryosmkfx_illumination_frame_set_mode(RyosmkfxIlluminationFrame *illumination_frame, RyosmkfxLightMode mode);
RyosmkfxLightMode ryosmkfx_illumination_frame_get_mode(RyosmkfxIlluminationFrame *illumination_frame);

G_END_DECLS

#endif
