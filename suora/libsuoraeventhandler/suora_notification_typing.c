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

#include "suora_notification_typing.h"
#include "suora_profile_data.h"
#include "config.h"
#include "i18n-lib.h"

struct _SuoraNotificationTyping {
	RoccatNotificator *notificator;
	GaminggearAudioNotification *audio;
};

static void suora_notification_typing_audio_update(GaminggearAudioNotification *notification, guint sound_feedback, gdouble volume) {
	gchar const *filename;

	switch (sound_feedback) {
	case SUORA_PROFILE_DATA_SOUND_FEEDBACK_CLICK:
		filename = "click";
		break;
	case SUORA_PROFILE_DATA_SOUND_FEEDBACK_TYPEWRITER:
		filename = "typewriter";
		break;
	case SUORA_PROFILE_DATA_SOUND_FEEDBACK_BEAM:
		filename = "laser-beam";
		break;
	case SUORA_PROFILE_DATA_SOUND_FEEDBACK_SCI_FI:
		filename = "sci-fi";
		break;
	default:
		filename = NULL;
		break;
	}

	if (!filename)
		return;

	gchar *path = g_strdup_printf(INSTALL_PREFIX "/share/roccat/sounds/typing/%s.oga", filename);
	gaminggear_audio_notification_update(notification, path, volume);
	g_free(path);
}

static void cancel_actual(SuoraNotificationTyping *profile) {
	gaminggear_audio_notification_cancel(profile->audio);
}

void suora_notification_typing_update(SuoraNotificationTyping *typing, guint sound_feedback, gdouble volume) {
	suora_notification_typing_audio_update(typing->audio, sound_feedback, volume);
}

SuoraNotificationTyping *suora_notification_typing_new(RoccatNotificator *notificator) {
	SuoraNotificationTyping *profile;

	profile = g_malloc0(sizeof(SuoraNotificationTyping));
	profile->notificator = notificator;
	profile->audio = gaminggear_audio_notification_new(notificator->audio_notificator);

	return profile;
}

void suora_notification_typing_free(SuoraNotificationTyping *typing) {
	cancel_actual(typing);
	gaminggear_audio_notification_free(typing->audio);
	g_free(typing);
}
