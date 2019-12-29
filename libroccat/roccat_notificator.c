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
#include "config.h"
#include <unistd.h>

RoccatNotificator *roccat_notificator_init(gchar const *app_name, gchar const *sound_driver) {
	RoccatNotificator *notificator;

	notificator = g_malloc0(sizeof(RoccatNotificator));

	notificator->screen_notificator = gaminggear_screen_notificator_init(app_name, "roccat");
	notificator->audio_notificator = gaminggear_audio_notificator_init(app_name, VERSION_STRING, sound_driver);

	return notificator;
}

void roccat_notificator_deinit(RoccatNotificator *notificator) {
	gaminggear_screen_notificator_deinit(notificator->screen_notificator);
	gaminggear_audio_notificator_deinit(notificator->audio_notificator);
	g_free(notificator);
}
