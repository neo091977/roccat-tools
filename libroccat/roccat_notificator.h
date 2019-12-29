#ifndef __ROCCAT_NOTIFICATION_H__
#define __ROCCAT_NOTIFICATION_H__

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

#include <glib.h>
#include <gaminggear/audio_notificator.h>
#include <gaminggear/screen_notificator.h>

G_BEGIN_DECLS

typedef struct _RoccatNotificator RoccatNotificator;

struct _RoccatNotificator {
	GaminggearScreenNotificator *screen_notificator;
	GaminggearAudioNotificator *audio_notificator;
};

typedef enum {
	ROCCAT_NOTIFICATION_TYPE_OFF = 0,
	ROCCAT_NOTIFICATION_TYPE_AUDIO = 1,
	ROCCAT_NOTIFICATION_TYPE_OSD = 2,
} RoccatNotificationType;

RoccatNotificator *roccat_notificator_init(gchar const *app_name, gchar const *sound_driver);
void roccat_notificator_deinit(RoccatNotificator *notificator);

G_END_DECLS

#endif
