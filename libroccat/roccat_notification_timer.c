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

#include "roccat_notification_timer.h"
#include "g_roccat_helper.h"
#include "roccat.h"
#include "config.h"
#include "i18n-lib.h"

struct _RoccatNotificationTimer {
	RoccatNotificator *notificator;
	GaminggearAudioNotification *audio;
	GaminggearScreenNotification *osd;
	gchar *device_name;
	RoccatNotificationType type;
	guint source;
	guint seconds;
	gchar *name;
	gdouble volume;
};

static void roccat_notification_osd_timer_tick(GaminggearScreenNotification *notification, gchar const *prefix, gchar const *name, guint seconds) {
	if (seconds == 0)
		gaminggear_screen_notification_update(notification, "%s timer %s: Go!", prefix, name);
	else
		gaminggear_screen_notification_update(notification, "%s timer %s: %i", prefix, name, seconds);
}

static void roccat_notification_osd_timer_stop(GaminggearScreenNotification *notification, gchar const *prefix, gchar const *name, guint seconds) {
	gaminggear_screen_notification_update(notification, "%s timer %s deactivated", prefix, name);
}

static void roccat_notification_osd_timer_start(GaminggearScreenNotification *notification, gchar const *prefix, gchar const *name, guint seconds) {
	gaminggear_screen_notification_update(notification, "%s timer %s activated", prefix, name);
}

static void roccat_notification_audio_timer_tick(GaminggearAudioNotification *notification, gdouble volume, gchar const *name, guint seconds) {
	gchar *filename = g_strdup_printf(INSTALL_PREFIX "/share/roccat/sounds/timer/timer-%i.oga", seconds);
	gaminggear_audio_notification_update(notification, filename, volume);
	g_free(filename);
}

static void roccat_notification_audio_timer_stop(GaminggearAudioNotification *notification, gdouble volume, gchar const *name, guint seconds) {
	gaminggear_audio_notification_update(notification,
			INSTALL_PREFIX "/share/roccat/sounds/timer/timer-stopped.oga",
			volume);
}

static void roccat_notification_audio_timer_start(GaminggearAudioNotification *notification, gdouble volume, gchar const *name, guint seconds) {
	gaminggear_audio_notification_update(notification,
			INSTALL_PREFIX "/share/roccat/sounds/timer/timer-activated.oga",
			volume);
}

static void cancel_actual(RoccatNotificationTimer *timer) {
	switch(timer->type) {
	case ROCCAT_NOTIFICATION_TYPE_OFF:
		break;
	case ROCCAT_NOTIFICATION_TYPE_OSD:
		gaminggear_screen_notification_cancel(timer->osd);
		break;
	case ROCCAT_NOTIFICATION_TYPE_AUDIO:
		gaminggear_audio_notification_cancel(timer->audio);
		break;
	}
}

static gboolean timer_source_func(RoccatNotificationTimer *timer) {
	/* this is called at end of first interval */
	--timer->seconds;

	if (timer->seconds > 10 || (timer->seconds < 10 && timer->seconds > 5))
		return TRUE;

	switch(timer->type) {
	case ROCCAT_NOTIFICATION_TYPE_OFF:
		break;
	case ROCCAT_NOTIFICATION_TYPE_OSD:
		roccat_notification_osd_timer_tick(timer->osd, timer->device_name, timer->name, timer->seconds);
		break;
	case ROCCAT_NOTIFICATION_TYPE_AUDIO:
		roccat_notification_audio_timer_tick(timer->audio, timer->volume, timer->name, timer->seconds);
		break;
	default:
		g_error(_("Got unknown notification type %i"), timer->type);
	}

	return timer->seconds != 0;
}

static void timer_destroy_notification(RoccatNotificationTimer *timer) {
	timer->source = 0;
}

void roccat_notification_timer_stop(RoccatNotificationTimer *timer) {
	if (timer->source) {
		g_source_remove(timer->source);
		timer->source = 0;

		switch(timer->type) {
		case ROCCAT_NOTIFICATION_TYPE_OFF:
			break;
		case ROCCAT_NOTIFICATION_TYPE_OSD:
			roccat_notification_osd_timer_stop(timer->osd, timer->device_name, timer->name, timer->seconds);
			break;
		case ROCCAT_NOTIFICATION_TYPE_AUDIO:
			roccat_notification_audio_timer_stop(timer->audio, timer->volume, timer->name, timer->seconds);
			break;
		default:
			g_error(_("Got unknown notification type %i"), timer->type);
		}
	}
}

void roccat_notification_timer_start(RoccatNotificationTimer *timer, RoccatNotificationType type, gdouble volume, gchar const *name, guint seconds) {
	roccat_notification_timer_stop(timer);

	if (type == ROCCAT_NOTIFICATION_TYPE_OFF) /* quick exit */
		return;

	timer->seconds = seconds;
	timer->volume = volume;

	if (timer->name)
		g_free(timer->name);
	timer->name = g_strdup(name);

	/* FIXME improve timer precision */
	timer->source = g_timeout_add_full(G_PRIORITY_DEFAULT, 1 * G_ROCCAT_MSEC_PER_SEC,
			(GSourceFunc)timer_source_func, timer, (GDestroyNotify)timer_destroy_notification);
	timer->type = type;

	switch(timer->type) {
	case ROCCAT_NOTIFICATION_TYPE_OFF:
		break;
	case ROCCAT_NOTIFICATION_TYPE_OSD:
		roccat_notification_osd_timer_start(timer->osd, timer->device_name, timer->name, timer->seconds);
		break;
	case ROCCAT_NOTIFICATION_TYPE_AUDIO:
		roccat_notification_audio_timer_start(timer->audio, timer->volume, timer->name, timer->seconds);
		break;
	default:
		g_error(_("Got unknown notification type %i"), timer->type);
	}
}

RoccatNotificationTimer *roccat_notification_timer_new(RoccatNotificator *notificator, gchar const *device_name) {
	RoccatNotificationTimer *timer;

	timer = g_malloc0(sizeof(RoccatNotificationTimer));
	timer->notificator = notificator;
	timer->osd = gaminggear_screen_notification_new(notificator->screen_notificator);
	timer->audio = gaminggear_audio_notification_new(notificator->audio_notificator);
	timer->device_name = g_strdup(device_name);

	return timer;
}

void roccat_notification_timer_free(RoccatNotificationTimer *timer) {
	if (timer->source)
		g_source_remove(timer->source);

	cancel_actual(timer);

	g_free(timer->name);

	gaminggear_screen_notification_free(timer->osd);
	gaminggear_audio_notification_free(timer->audio);
	g_free(timer->device_name);
	g_free(timer);
}
