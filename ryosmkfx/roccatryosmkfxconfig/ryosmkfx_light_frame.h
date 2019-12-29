#ifndef __ROCCAT_RYOSMKFX_LIGHT_FRAME_H__
#define __ROCCAT_RYOSMKFX_LIGHT_FRAME_H__

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

#include "ryosmkfx_profile_data.h"
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define RYOSMKFX_LIGHT_FRAME_TYPE (ryosmkfx_light_frame_get_type())
#define RYOSMKFX_LIGHT_FRAME(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), RYOSMKFX_LIGHT_FRAME_TYPE, RyosmkfxLightFrame))
#define IS_RYOSMKFX_LIGHT_FRAME(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), RYOSMKFX_LIGHT_FRAME_TYPE))

typedef struct _RyosmkfxLightFrame RyosmkfxLightFrame;

GType ryosmkfx_light_frame_get_type(void);
GtkWidget *ryosmkfx_light_frame_new(void);

void ryosmkfx_light_frame_set_brightness(RyosmkfxLightFrame *light_frame, guint new_brightness);

void ryosmkfx_light_frame_set_from_profile_data(RyosmkfxLightFrame *light_frame, RyosmkfxProfileData const *profile_data);
void ryosmkfx_light_frame_update_profile_data(RyosmkfxLightFrame *light_frame, RyosmkfxProfileData *profile_data);

G_END_DECLS

#endif
