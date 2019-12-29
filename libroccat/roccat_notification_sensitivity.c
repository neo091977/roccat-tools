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

#include "roccat_notification_sensitivity.h"
#include "roccat.h"
#include "config.h"
#include "i18n-lib.h"

struct _RoccatNotificationSensitivity {
	RoccatNotificator *notificator;
	GaminggearAudioNotification *audio;
	GaminggearScreenNotification *osd;
	gchar *device_name;
	RoccatNotificationType type;
};

static void roccat_notification_sensitivity_osd_update(GaminggearScreenNotification *notification, gchar const *prefix, gint x, gint y) {
	if (x == y)
		gaminggear_screen_notification_update(notification, "%s sensitivity %d", prefix, x);
	else
		gaminggear_screen_notification_update(notification, "%s sensitivity %d/%d", prefix, x, y);
}

static void roccat_notification_sensitivity_osd_update_up(GaminggearScreenNotification *notification, gchar const *prefix) {
	gaminggear_screen_notification_update(notification, "%s sensitivity up", prefix);
}

static void roccat_notification_sensitivity_osd_update_down(GaminggearScreenNotification *notification, gchar const *prefix) {
	gaminggear_screen_notification_update(notification, "%s sensitivity down", prefix);
}

static void roccat_notification_sensitivity_audio_update_up(GaminggearAudioNotification *notification, gdouble volume) {
	gaminggear_audio_notification_update(notification,
			INSTALL_PREFIX "/share/roccat/sounds/sensitivity/sensitivity-up.oga",
			volume);
}

static void roccat_notification_sensitivity_audio_update_down(GaminggearAudioNotification *notification, gdouble volume) {
	gaminggear_audio_notification_update(notification,
			INSTALL_PREFIX "/share/roccat/sounds/sensitivity/sensitivity-down.oga",
			volume);
}

static void cancel_actual(RoccatNotificationSensitivity *sensitivity) {
	switch(sensitivity->type) {
	case ROCCAT_NOTIFICATION_TYPE_OFF:
		break;
	case ROCCAT_NOTIFICATION_TYPE_OSD:
		gaminggear_screen_notification_cancel(sensitivity->osd);
		break;
	case ROCCAT_NOTIFICATION_TYPE_AUDIO:
		gaminggear_audio_notification_cancel(sensitivity->audio);
		break;
	}
}

void roccat_notification_sensitivity_update(RoccatNotificationSensitivity *sensitivity, RoccatNotificationType type, gint x, gint y) {
	sensitivity->type = type;
	switch(sensitivity->type) {
	case ROCCAT_NOTIFICATION_TYPE_OFF:
		break;
	case ROCCAT_NOTIFICATION_TYPE_OSD:
		roccat_notification_sensitivity_osd_update(sensitivity->osd, sensitivity->device_name, x, y);
		break;
	default:
		g_error(_("Got unknown notification type %i"), sensitivity->type);
	}
}

void roccat_notification_sensitivity_update_up(RoccatNotificationSensitivity *sensitivity, RoccatNotificationType type, gdouble volume) {
	sensitivity->type = type;
	switch(sensitivity->type) {
	case ROCCAT_NOTIFICATION_TYPE_OFF:
		break;
	case ROCCAT_NOTIFICATION_TYPE_OSD:
		roccat_notification_sensitivity_osd_update_up(sensitivity->osd, sensitivity->device_name);
		break;
	case ROCCAT_NOTIFICATION_TYPE_AUDIO:
		roccat_notification_sensitivity_audio_update_up(sensitivity->audio, volume);
		break;
	default:
		g_error(_("Got unknown notification type %i"), sensitivity->type);
	}
}

void roccat_notification_sensitivity_update_down(RoccatNotificationSensitivity *sensitivity, RoccatNotificationType type, gdouble volume) {
	sensitivity->type = type;
	switch(sensitivity->type) {
	case ROCCAT_NOTIFICATION_TYPE_OFF:
		break;
	case ROCCAT_NOTIFICATION_TYPE_OSD:
		roccat_notification_sensitivity_osd_update_down(sensitivity->osd, sensitivity->device_name);
		break;
	case ROCCAT_NOTIFICATION_TYPE_AUDIO:
		roccat_notification_sensitivity_audio_update_down(sensitivity->audio, volume);
		break;
	default:
		g_error(_("Got unknown notification type %i"), sensitivity->type);
	}
}

RoccatNotificationSensitivity *roccat_notification_sensitivity_new(RoccatNotificator *notificator, gchar const *device_name) {
	RoccatNotificationSensitivity *sensitivity;

	sensitivity = g_malloc0(sizeof(RoccatNotificationSensitivity));
	sensitivity->notificator = notificator;
	sensitivity->osd = gaminggear_screen_notification_new(notificator->screen_notificator);
	sensitivity->audio = gaminggear_audio_notification_new(notificator->audio_notificator);
	sensitivity->device_name = g_strdup(device_name);

	return sensitivity;
}

void roccat_notification_sensitivity_free(RoccatNotificationSensitivity *sensitivity) {
	cancel_actual(sensitivity);
	gaminggear_screen_notification_free(sensitivity->osd);
	gaminggear_audio_notification_free(sensitivity->audio);
	g_free(sensitivity->device_name);
	g_free(sensitivity);
}
