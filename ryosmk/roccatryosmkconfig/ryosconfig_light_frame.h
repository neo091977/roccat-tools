#ifndef __ROCCAT_RYOSCONFIG_LIGHT_FRAME_H__
#define __ROCCAT_RYOSCONFIG_LIGHT_FRAME_H__

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

#include "ryos_profile_data.h"
#include "ryos_config.h"
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define RYOSCONFIG_LIGHT_FRAME_TYPE (ryosconfig_light_frame_get_type())
#define RYOSCONFIG_LIGHT_FRAME(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), RYOSCONFIG_LIGHT_FRAME_TYPE, RyosconfigLightFrame))
#define IS_RYOSCONFIG_LIGHT_FRAME(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), RYOSCONFIG_LIGHT_FRAME_TYPE))

typedef struct _RyosconfigLightFrame RyosconfigLightFrame;

GType ryosconfig_light_frame_get_type(void);
GtkWidget *ryosconfig_light_frame_new(void);

void ryosconfig_light_frame_set_device_type(RyosconfigLightFrame *light_frame, RoccatDevice const *device);

void ryosconfig_light_frame_set_brightness(RyosconfigLightFrame *light_frame, guint new_brightness);

void ryosconfig_light_frame_set_from_profile_data(RyosconfigLightFrame *light_frame, RyosProfileData const *profile_data, RoccatKeyFile *config);
void ryosconfig_light_frame_update_profile_data(RyosconfigLightFrame *light_frame, RyosProfileData *profile_data, RoccatKeyFile *config);

G_END_DECLS

#endif
