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

#include "roccat_notification_profile.h"
#include "roccat.h"
#include "config.h"
#include "i18n-lib.h"

struct _RoccatNotificationProfile {
	RoccatNotificator *notificator;
	GaminggearAudioNotification *audio;
	GaminggearScreenNotification *osd;
	gchar *device_name;
	RoccatNotificationType type;
};

static void roccat_notification_profile_osd_update(GaminggearScreenNotification *notification, gchar const *prefix, guint number, gchar const *name) {
	if (name && strlen(name))
		gaminggear_screen_notification_update(notification, "%s profile %d:%s", prefix, number, name);
	else
		gaminggear_screen_notification_update(notification, "%s profile %d", prefix, number);
}

static void roccat_notification_profile_audio_update(GaminggearAudioNotification *notification, gdouble volume, guint number) {
	gchar *filename = g_strdup_printf(INSTALL_PREFIX "/share/roccat/sounds/profile/profile-%i.oga", number);
	gaminggear_audio_notification_update(notification, filename, volume);
	g_free(filename);
}

static void cancel_actual(RoccatNotificationProfile *profile) {
	switch(profile->type) {
	case ROCCAT_NOTIFICATION_TYPE_OFF:
		break;
	case ROCCAT_NOTIFICATION_TYPE_OSD:
		gaminggear_screen_notification_cancel(profile->osd);
		break;
	case ROCCAT_NOTIFICATION_TYPE_AUDIO:
		gaminggear_audio_notification_cancel(profile->audio);
		break;
	}
}

void roccat_notification_profile_update(RoccatNotificationProfile *profile, RoccatNotificationType type, gdouble volume, guint number, gchar const *name) {
	profile->type = type;
	switch(profile->type) {
	case ROCCAT_NOTIFICATION_TYPE_OFF:
		break;
	case ROCCAT_NOTIFICATION_TYPE_OSD:
		roccat_notification_profile_osd_update(profile->osd, profile->device_name, number, name);
		break;
	case ROCCAT_NOTIFICATION_TYPE_AUDIO:
		roccat_notification_profile_audio_update(profile->audio, volume, number);
		break;
	default:
		g_error(_("Got unknown notification type %i"), profile->type);
	}
}

RoccatNotificationProfile *roccat_notification_profile_new(RoccatNotificator *notificator, gchar const *device_name) {
	RoccatNotificationProfile *profile;

	profile = g_malloc0(sizeof(RoccatNotificationProfile));
	profile->notificator = notificator;
	profile->osd = gaminggear_screen_notification_new(notificator->screen_notificator);
	profile->audio = gaminggear_audio_notification_new(notificator->audio_notificator);
	profile->device_name = g_strdup(device_name);

	return profile;
}

void roccat_notification_profile_free(RoccatNotificationProfile *profile) {
	cancel_actual(profile);
	gaminggear_screen_notification_free(profile->osd);
	gaminggear_audio_notification_free(profile->audio);
	g_free(profile->device_name);
	g_free(profile);
}
