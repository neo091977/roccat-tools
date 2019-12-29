#ifndef __ROCCAT_RYOSTKLCONFIG_LED_FEEDBACK_FRAME_H__
#define __ROCCAT_RYOSTKLCONFIG_LED_FEEDBACK_FRAME_H__

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
#include "ryostkl_profile_data.h"

G_BEGIN_DECLS

#define RYOSTKLCONFIG_LED_FEEDBACK_FRAME_TYPE (ryostklconfig_led_feedback_frame_get_type())
#define RYOSTKLCONFIG_LED_FEEDBACK_FRAME(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), RYOSTKLCONFIG_LED_FEEDBACK_FRAME_TYPE, RyostklconfigLedFeedbackFrame))
#define IS_RYOSTKLCONFIG_LED_FEEDBACK_FRAME(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), RYOSTKLCONFIG_LED_FEEDBACK_FRAME_TYPE))

typedef struct _RyostklconfigLedFeedbackFrame RyostklconfigLedFeedbackFrame;

GType ryostklconfig_led_feedback_frame_get_type(void);
GtkWidget *ryostklconfig_led_feedback_frame_new(void);

void ryostklconfig_led_feedback_frame_set_device_type(RyostklconfigLedFeedbackFrame *feedback_frame, RoccatDevice const *device);

void ryostklconfig_led_feedback_frame_set_from_profile_data(RyostklconfigLedFeedbackFrame *feedback_frame, RyostklProfileData const *profile_data);
void ryostklconfig_led_feedback_frame_update_profile_data(RyostklconfigLedFeedbackFrame *feedback_frame, RyostklProfileData *profile_data);

G_END_DECLS

#endif
