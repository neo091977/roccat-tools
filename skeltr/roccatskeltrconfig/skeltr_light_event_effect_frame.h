#ifndef __ROCCAT_SKELTR_LIGHT_EVENT_EFFECT_FRAME_H__
#define __ROCCAT_SKELTR_LIGHT_EVENT_EFFECT_FRAME_H__

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

#include "skeltr_profile_data.h"
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define SKELTR_LIGHT_EVENT_EFFECT_FRAME_TYPE (skeltr_light_event_effect_frame_get_type())
#define SKELTR_LIGHT_EVENT_EFFECT_FRAME(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), SKELTR_LIGHT_EVENT_EFFECT_FRAME_TYPE, SkeltrLightEventEffectFrame))
#define IS_SKELTR_LIGHT_EVENT_EFFECT_FRAME(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), SKELTR_LIGHT_EVENT_EFFECT_FRAME_TYPE))

typedef struct _SkeltrLightEventEffectFrame SkeltrLightEventEffectFrame;

GType skeltr_light_event_effect_frame_get_type(void);
GtkWidget *skeltr_light_event_effect_frame_new(void);

void skeltr_light_event_effect_frame_set_from_profile_data(SkeltrLightEventEffectFrame *frame, SkeltrProfileData const *profile_data);
void skeltr_light_event_effect_frame_update_profile_data(SkeltrLightEventEffectFrame *frame, SkeltrProfileData *profile_data);

G_END_DECLS

#endif
