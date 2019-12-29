#ifndef __ROCCAT_RYOS_NOTIFICATION_FRAME_H__
#define __ROCCAT_RYOS_NOTIFICATION_FRAME_H__

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

G_BEGIN_DECLS

#define RYOS_NOTIFICATION_FRAME_TYPE (ryos_notification_frame_get_type())
#define RYOS_NOTIFICATION_FRAME(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), RYOS_NOTIFICATION_FRAME_TYPE, RyosNotificationFrame))
#define IS_RYOS_NOTIFICATION_FRAME(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), RYOS_NOTIFICATION_FRAME_TYPE))

typedef struct _RyosNotificationFrame RyosNotificationFrame;

GType ryos_notification_frame_get_type(void);
GtkWidget *ryos_notification_frame_new(void);

void ryos_notification_frame_set_timer_type(RyosNotificationFrame *notification_frame, guint8 value);
guint8 ryos_notification_frame_get_timer_type(RyosNotificationFrame *notification_frame);

void ryos_notification_frame_set_profile_type(RyosNotificationFrame *notification_frame, guint8 value);
guint8 ryos_notification_frame_get_profile_type(RyosNotificationFrame *notification_frame);

void ryos_notification_frame_set_live_recording_type(RyosNotificationFrame *notification_frame, guint8 value);
guint8 ryos_notification_frame_get_live_recording_type(RyosNotificationFrame *notification_frame);

void ryos_notification_frame_set_volume(RyosNotificationFrame *notification_frame, gdouble volume);
gdouble ryos_notification_frame_get_volume(RyosNotificationFrame *notification_frame);

G_END_DECLS

#endif
