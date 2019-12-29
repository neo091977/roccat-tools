#ifndef __ROCCAT_NOTIFICATION_LIVE_RECORDING_H__
#define __ROCCAT_NOTIFICATION_LIVE_RECORDING_H__

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

#include "roccat_notificator.h"

G_BEGIN_DECLS

typedef struct _RoccatNotificationLiveRecording RoccatNotificationLiveRecording;

RoccatNotificationLiveRecording *roccat_notification_live_recording_new(RoccatNotificator *notificator, gchar const *device_name);
void roccat_notification_live_recording_free(RoccatNotificationLiveRecording *notification);
void roccat_notification_live_recording_select_macro_key(RoccatNotificationLiveRecording *notification, RoccatNotificationType type, gdouble volume);
void roccat_notification_live_recording_record_macro(RoccatNotificationLiveRecording *notification, RoccatNotificationType type, gdouble volume);
void roccat_notification_live_recording_end_success(RoccatNotificationLiveRecording *notification, RoccatNotificationType type, gdouble volume);
void roccat_notification_live_recording_end_abort(RoccatNotificationLiveRecording *notification, RoccatNotificationType type, gdouble volume);
void roccat_notification_live_recording_invalid_key(RoccatNotificationLiveRecording *notification, RoccatNotificationType type, gdouble volume);

G_END_DECLS

#endif
