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

#include "roccat_notification_cpi.h"
#include "roccat.h"
#include "config.h"
#include "i18n-lib.h"

struct _RoccatNotificationCpi {
	RoccatNotificator *notificator;
	GaminggearAudioNotification *audio;
	GaminggearScreenNotification *osd;
	gchar *device_name;
	RoccatNotificationType type;
};

static void roccat_notification_cpi_osd_update(GaminggearScreenNotification *notification, gchar const *prefix, guint x, guint y) {
	if (x == y)
		gaminggear_screen_notification_update(notification, "%s cpi %d", prefix, x);
	else
		gaminggear_screen_notification_update(notification, "%s cpi %d/%d", prefix, x, y);
}

static void roccat_notification_cpi_osd_update_up(GaminggearScreenNotification *notification, gchar const *prefix) {
	gaminggear_screen_notification_update(notification, "%s cpi up", prefix);
}

static void roccat_notification_cpi_osd_update_down(GaminggearScreenNotification *notification, gchar const *prefix) {
	gaminggear_screen_notification_update(notification, "%s cpi down", prefix);
}

static void roccat_notification_cpi_audio_update(GaminggearAudioNotification *notification, gdouble volume, guint number) {
	gchar *filename = g_strdup_printf(INSTALL_PREFIX "/share/roccat/sounds/cpi/cpi-%i.oga", number);
	gaminggear_audio_notification_update(notification, filename, volume);
	g_free(filename);
}

static void roccat_notification_cpi_audio_update_up(GaminggearAudioNotification *notification, gdouble volume) {
	gaminggear_audio_notification_update(notification,
			INSTALL_PREFIX "/share/roccat/sounds/cpi/cpi-up.oga",
			volume);
}

static void roccat_notification_cpi_audio_update_down(GaminggearAudioNotification *notification, gdouble volume) {
	gaminggear_audio_notification_update(notification,
			INSTALL_PREFIX "/share/roccat/sounds/cpi/cpi-down.oga",
			volume);
}

static void cancel_actual(RoccatNotificationCpi *cpi) {
	switch(cpi->type) {
	case ROCCAT_NOTIFICATION_TYPE_OFF:
		break;
	case ROCCAT_NOTIFICATION_TYPE_OSD:
		gaminggear_screen_notification_cancel(cpi->osd);
		break;
	case ROCCAT_NOTIFICATION_TYPE_AUDIO:
		gaminggear_audio_notification_cancel(cpi->audio);
		break;
	}
}

void roccat_notification_cpi_update(RoccatNotificationCpi *cpi, RoccatNotificationType type, gdouble volume, guint x, guint y) {
	cpi->type = type;
	switch(cpi->type) {
	case ROCCAT_NOTIFICATION_TYPE_OFF:
		break;
	case ROCCAT_NOTIFICATION_TYPE_OSD:
		roccat_notification_cpi_osd_update(cpi->osd, cpi->device_name, x, y);
		break;
	case ROCCAT_NOTIFICATION_TYPE_AUDIO:
		roccat_notification_cpi_audio_update(cpi->audio, volume, x);
		break;
	default:
		g_error(_("Got unknown notification type %i"), cpi->type);
	}
}

void roccat_notification_cpi_update_up(RoccatNotificationCpi *cpi, RoccatNotificationType type, gdouble volume) {
	cpi->type = type;
	switch(cpi->type) {
	case ROCCAT_NOTIFICATION_TYPE_OFF:
		break;
	case ROCCAT_NOTIFICATION_TYPE_OSD:
		roccat_notification_cpi_osd_update_up(cpi->osd, cpi->device_name);
		break;
	case ROCCAT_NOTIFICATION_TYPE_AUDIO:
		roccat_notification_cpi_audio_update_up(cpi->audio, volume);
		break;
	default:
		g_error(_("Got unknown notification type %i"), cpi->type);
	}
}

void roccat_notification_cpi_update_down(RoccatNotificationCpi *cpi, RoccatNotificationType type, gdouble volume) {
	cpi->type = type;
	switch(cpi->type) {
	case ROCCAT_NOTIFICATION_TYPE_OFF:
		break;
	case ROCCAT_NOTIFICATION_TYPE_OSD:
		roccat_notification_cpi_osd_update_down(cpi->osd, cpi->device_name);
		break;
	case ROCCAT_NOTIFICATION_TYPE_AUDIO:
		roccat_notification_cpi_audio_update_down(cpi->audio, volume);
		break;
	default:
		g_error(_("Got unknown notification type %i"), cpi->type);
	}
}

RoccatNotificationCpi *roccat_notification_cpi_new(RoccatNotificator *notificator, gchar const *device_name) {
	RoccatNotificationCpi *cpi;

	cpi = g_malloc0(sizeof(RoccatNotificationCpi));
	cpi->notificator = notificator;
	cpi->osd = gaminggear_screen_notification_new(notificator->screen_notificator);
	cpi->audio = gaminggear_audio_notification_new(notificator->audio_notificator);
	cpi->device_name = g_strdup(device_name);

	return cpi;
}

void roccat_notification_cpi_free(RoccatNotificationCpi *cpi) {
	cancel_actual(cpi);
	gaminggear_screen_notification_free(cpi->osd);
	gaminggear_audio_notification_free(cpi->audio);
	g_free(cpi->device_name);
	g_free(cpi);
}
