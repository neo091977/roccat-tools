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

#include "sova_notification_typing.h"
#include "sova_rkp_sound_feedback.h"
#include "config.h"
#include "i18n-lib.h"

struct _SovaNotificationTyping {
	RoccatNotificator *notificator;
	GaminggearAudioNotification *audio;
};

static void sova_notification_typing_audio_update(GaminggearAudioNotification *notification, guint sound_feedback, gdouble volume) {
	gchar const *filename;

	switch (sound_feedback) {
	case SOVA_RKP_SOUND_FEEDBACK_TYPING_SOUND_CLICK:
		filename = "click";
		break;
	case SOVA_RKP_SOUND_FEEDBACK_TYPING_SOUND_TYPEWRITER:
		filename = "typewriter";
		break;
	case SOVA_RKP_SOUND_FEEDBACK_TYPING_SOUND_BEAM:
		filename = "laser-beam";
		break;
	case SOVA_RKP_SOUND_FEEDBACK_TYPING_SOUND_SCIFI:
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

static void cancel_actual(SovaNotificationTyping *typing) {
	gaminggear_audio_notification_cancel(typing->audio);
}

void sova_notification_typing_update(SovaNotificationTyping *typing, guint sound_feedback, gdouble volume) {
	sova_notification_typing_audio_update(typing->audio, sound_feedback, volume);
}

SovaNotificationTyping *sova_notification_typing_new(RoccatNotificator *notificator) {
	SovaNotificationTyping *typing;

	typing = g_malloc0(sizeof(SovaNotificationTyping));
	typing->notificator = notificator;
	typing->audio = gaminggear_audio_notification_new(notificator->audio_notificator);

	return typing;
}

void sova_notification_typing_free(SovaNotificationTyping *typing) {
	cancel_actual(typing);
	gaminggear_audio_notification_free(typing->audio);
	g_free(typing);
}
