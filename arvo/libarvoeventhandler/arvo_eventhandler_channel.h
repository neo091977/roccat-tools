#ifndef __ROCCAT_ARVO_EVENTHANDLER_CHANNEL_H__
#define __ROCCAT_ARVO_EVENTHANDLER_CHANNEL_H__

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

#include "roccat_device.h"

G_BEGIN_DECLS

#define ARVO_EVENTHANDLER_CHANNEL_TYPE (arvo_eventhandler_channel_get_type())
#define ARVO_EVENTHANDLER_CHANNEL(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), ARVO_EVENTHANDLER_CHANNEL_TYPE, ArvoEventhandlerChannel))
#define IS_ARVO_EVENTHANDLER_CHANNEL(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), ARVO_EVENTHANDLER_CHANNEL_TYPE))

typedef struct _ArvoEventhandlerChannel ArvoEventhandlerChannel;

GType arvo_eventhandler_channel_get_type(void);
ArvoEventhandlerChannel *arvo_eventhandler_channel_new(void);

gboolean arvo_eventhandler_channel_start(ArvoEventhandlerChannel *eventhandler_channel, RoccatDevice *device, GError **error);
void arvo_eventhandler_channel_stop(ArvoEventhandlerChannel *eventhandler_channel);

G_END_DECLS

#endif
