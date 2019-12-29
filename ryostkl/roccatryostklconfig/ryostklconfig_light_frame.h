#ifndef __ROCCAT_RYOSTKLCONFIG_LIGHT_FRAME_H__
#define __ROCCAT_RYOSTKLCONFIG_LIGHT_FRAME_H__

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

#include "ryostkl_profile_data.h"
#include "ryos_config.h"
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define RYOSTKLCONFIG_LIGHT_FRAME_TYPE (ryostklconfig_light_frame_get_type())
#define RYOSTKLCONFIG_LIGHT_FRAME(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), RYOSTKLCONFIG_LIGHT_FRAME_TYPE, RyostklconfigLightFrame))
#define IS_RYOSTKLCONFIG_LIGHT_FRAME(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), RYOSTKLCONFIG_LIGHT_FRAME_TYPE))

typedef struct _RyostklconfigLightFrame RyostklconfigLightFrame;

GType ryostklconfig_light_frame_get_type(void);
GtkWidget *ryostklconfig_light_frame_new(void);

void ryostklconfig_light_frame_set_device_type(RyostklconfigLightFrame *light_frame, RoccatDevice const *device);

void ryostklconfig_light_frame_set_brightness(RyostklconfigLightFrame *light_frame, guint new_brightness);

void ryostklconfig_light_frame_set_from_profile_data(RyostklconfigLightFrame *light_frame, RyostklProfileData const *profile_data, RoccatKeyFile *config);
void ryostklconfig_light_frame_update_profile_data(RyostklconfigLightFrame *light_frame, RyostklProfileData *profile_data, RoccatKeyFile *config);

G_END_DECLS

#endif
