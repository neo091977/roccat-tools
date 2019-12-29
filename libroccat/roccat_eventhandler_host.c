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

#include "roccat_eventhandler_host.h"

RoccatNotificator *roccat_eventhandler_host_get_notificator(RoccatEventhandlerHost *self) {
	RoccatEventhandlerHostInterface *interface = ROCCAT_EVENTHANDLER_HOST_GET_INTERFACE(self);
	return interface->get_notificator ? interface->get_notificator(self) : NULL;
}

guint roccat_eventhandler_host_should_set_driver_state(RoccatEventhandlerHost *self) {
	RoccatEventhandlerHostInterface *interface = ROCCAT_EVENTHANDLER_HOST_GET_INTERFACE(self);
	return interface->should_set_driver_state(self);
}

G_DEFINE_INTERFACE(RoccatEventhandlerHost, roccat_eventhandler_host, G_TYPE_OBJECT);

static void roccat_eventhandler_host_default_init(RoccatEventhandlerHostInterface *iface) {}
