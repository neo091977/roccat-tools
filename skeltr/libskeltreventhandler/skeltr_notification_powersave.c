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

#include "skeltr_notification_powersave.h"
#include "skeltr_rkp_sound_feedback.h"
#include "config.h"
#include "i18n-lib.h"

struct _SkeltrNotificationPowersave {
	GaminggearAudioNotification *audio;
	GaminggearScreenNotification *osd;
	RoccatNotificationType type;
	gchar *device_name;
};

static void cancel_actual(SkeltrNotificationPowersave *powersave) {
	switch(powersave->type) {
	case ROCCAT_NOTIFICATION_TYPE_OFF:
		break;
	case ROCCAT_NOTIFICATION_TYPE_OSD:
		gaminggear_screen_notification_cancel(powersave->osd);
		break;
	case ROCCAT_NOTIFICATION_TYPE_AUDIO:
		gaminggear_audio_notification_cancel(powersave->audio);
		break;
	default:
		g_error(_("Got unknown notification type %i"), powersave->type);
	}
}

void skeltr_notification_powersave_sleep(SkeltrNotificationPowersave *powersave, RoccatNotificationType type, gdouble volume) {
	powersave->type = type;
	switch(powersave->type) {
	case ROCCAT_NOTIFICATION_TYPE_OFF:
		break;
	case ROCCAT_NOTIFICATION_TYPE_OSD:
		gaminggear_screen_notification_update(powersave->osd, _("%s is sleeping"), powersave->device_name);
		break;
	case ROCCAT_NOTIFICATION_TYPE_AUDIO:
		gaminggear_audio_notification_update(powersave->audio, INSTALL_PREFIX "/share/roccat/sounds/sleep.oga", volume);
		break;
	default:
		g_error(_("Got unknown notification type %i"), powersave->type);
	}
}

void skeltr_notification_powersave_wakeup(SkeltrNotificationPowersave *powersave, RoccatNotificationType type, gdouble volume) {
	powersave->type = type;
	switch(powersave->type) {
	case ROCCAT_NOTIFICATION_TYPE_OFF:
		break;
	case ROCCAT_NOTIFICATION_TYPE_OSD:
		gaminggear_screen_notification_update(powersave->osd, _("%s woke up"), powersave->device_name);
		break;
	case ROCCAT_NOTIFICATION_TYPE_AUDIO:
		gaminggear_audio_notification_update(powersave->audio, INSTALL_PREFIX "/share/roccat/sounds/wakeup.oga", volume);
		break;
	default:
		g_error(_("Got unknown notification type %i"), powersave->type);
	}
}

SkeltrNotificationPowersave *skeltr_notification_powersave_new(RoccatNotificator *notificator, gchar const *device_name) {
	SkeltrNotificationPowersave *powersave;

	powersave = g_malloc0(sizeof(SkeltrNotificationPowersave));
	powersave->osd = gaminggear_screen_notification_new(notificator->screen_notificator);
	powersave->audio = gaminggear_audio_notification_new(notificator->audio_notificator);
	powersave->device_name = g_strdup(device_name);

	return powersave;
}

void skeltr_notification_powersave_free(SkeltrNotificationPowersave *powersave) {
	cancel_actual(powersave);
	gaminggear_screen_notification_free(powersave->osd);
	gaminggear_audio_notification_free(powersave->audio);
	g_free(powersave->device_name);
	g_free(powersave);
}
