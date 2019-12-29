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

#include "roccat_notification_live_recording.h"
#include "config.h"
#include "roccat.h"
#include "i18n-lib.h"

struct _RoccatNotificationLiveRecording {
	RoccatNotificator *notificator;
	GaminggearAudioNotification *audio;
	GaminggearScreenNotification *osd;
	gchar *device_name;
	RoccatNotificationType type;
};

static void roccat_notification_osd_live_recording_invoke(GaminggearScreenNotification *notification, gchar const *format, ...) {
	va_list args;

	va_start(args, format);
	gaminggear_screen_notification_update_v(notification, format, args);
	va_end(args);
}

static void roccat_notification_audio_live_recording_invoke(GaminggearAudioNotification *notification, gdouble volume, gchar const *filename) {
	gaminggear_audio_notification_update(notification, filename, volume);
}

static void roccat_notification_live_recording_invoke(RoccatNotificationLiveRecording *notification,
		RoccatNotificationType type, gdouble volume, gchar const *osd_string, gchar const *audio_filename) {
	notification->type = type;
	switch(type) {
	case ROCCAT_NOTIFICATION_TYPE_OFF:
		break;
	case ROCCAT_NOTIFICATION_TYPE_OSD:
		roccat_notification_osd_live_recording_invoke(notification->osd, "%s %s", notification->device_name, osd_string);
		break;
	case ROCCAT_NOTIFICATION_TYPE_AUDIO:
		roccat_notification_audio_live_recording_invoke(notification->audio, volume, audio_filename);
		break;
	default:
		g_error(_("Got unknown notification type %i"), notification->type);
	}
}

static void roccat_notification_live_recording_cancel(RoccatNotificationLiveRecording *notification) {
	switch(notification->type) {
	case ROCCAT_NOTIFICATION_TYPE_OFF:
		break;
	case ROCCAT_NOTIFICATION_TYPE_OSD:
		gaminggear_screen_notification_cancel(notification->osd);
		break;
	case ROCCAT_NOTIFICATION_TYPE_AUDIO:
		gaminggear_audio_notification_cancel(notification->audio);
		break;
	default:
		g_error(_("Got unknown notification type %i"), notification->type);
	}
}

RoccatNotificationLiveRecording *roccat_notification_live_recording_new(RoccatNotificator *notificator, gchar const *device_name) {
	RoccatNotificationLiveRecording *notification;
	notification = g_malloc0(sizeof(RoccatNotificationLiveRecording));
	notification->notificator = notificator;
	notification->osd = gaminggear_screen_notification_new(notificator->screen_notificator);
	notification->audio = gaminggear_audio_notification_new(notificator->audio_notificator);
	notification->device_name = g_strdup(device_name);

	return notification;
}

void roccat_notification_live_recording_free(RoccatNotificationLiveRecording *notification) {
	roccat_notification_live_recording_cancel(notification);
	gaminggear_screen_notification_free(notification->osd);
	gaminggear_audio_notification_free(notification->audio);
	g_free(notification->device_name);
	g_free(notification);
}

void roccat_notification_live_recording_select_macro_key(RoccatNotificationLiveRecording *notification, RoccatNotificationType type, gdouble volume) {
	roccat_notification_live_recording_invoke(notification, type, volume,
			"select macro key",
			INSTALL_PREFIX "/share/roccat/sounds/live_recording/select-macro-key.oga");
}

void roccat_notification_live_recording_record_macro(RoccatNotificationLiveRecording *notification, RoccatNotificationType type, gdouble volume) {
	roccat_notification_live_recording_invoke(notification, type, volume,
			"record macro and press record key again",
			INSTALL_PREFIX "/share/roccat/sounds/live_recording/record-macro.oga");
}

void roccat_notification_live_recording_end_success(RoccatNotificationLiveRecording *notification, RoccatNotificationType type, gdouble volume) {
	roccat_notification_live_recording_invoke(notification, type, volume,
			"macro successfully saved",
			INSTALL_PREFIX "/share/roccat/sounds/live_recording/end-success.oga");
}

void roccat_notification_live_recording_end_abort(RoccatNotificationLiveRecording *notification, RoccatNotificationType type, gdouble volume) {
	roccat_notification_live_recording_invoke(notification, type, volume,
			"macro recording aborted",
			INSTALL_PREFIX "/share/roccat/sounds/live_recording/end-abort.oga");
}

void roccat_notification_live_recording_invalid_key(RoccatNotificationLiveRecording *notification, RoccatNotificationType type, gdouble volume) {
	roccat_notification_live_recording_invoke(notification, type, volume,
			"invalid macro key",
			INSTALL_PREFIX "/share/roccat/sounds/live_recording/invalid-key.oga");
}
