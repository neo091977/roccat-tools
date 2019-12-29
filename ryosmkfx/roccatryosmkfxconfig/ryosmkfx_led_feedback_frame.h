#ifndef __ROCCAT_RYOSMKFX_LED_FEEDBACK_FRAME_H__
#define __ROCCAT_RYOSMKFX_LED_FEEDBACK_FRAME_H__

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

#define RYOSMKFX_LED_FEEDBACK_FRAME_TYPE (ryosmkfx_led_feedback_frame_get_type())
#define RYOSMKFX_LED_FEEDBACK_FRAME(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), RYOSMKFX_LED_FEEDBACK_FRAME_TYPE, RyosmkfxLedFeedbackFrame))
#define IS_RYOSMKFX_LED_FEEDBACK_FRAME(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), RYOSMKFX_LED_FEEDBACK_FRAME_TYPE))

typedef struct _RyosmkfxLedFeedbackFrame RyosmkfxLedFeedbackFrame;

GType ryosmkfx_led_feedback_frame_get_type(void);
GtkWidget *ryosmkfx_led_feedback_frame_new(void);

void ryosmkfx_led_feedback_frame_set_from_profile_data(RyosmkfxLedFeedbackFrame *feedback_frame, RyosmkfxProfileData const *profile_data);
void ryosmkfx_led_feedback_frame_update_profile_data(RyosmkfxLedFeedbackFrame *feedback_frame, RyosmkfxProfileData *profile_data);

G_END_DECLS

#endif
