#ifndef __ROCCAT_RYOS_EVENTHANDLER_CHANNEL_H__
#define __ROCCAT_RYOS_EVENTHANDLER_CHANNEL_H__

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

#define RYOS_EVENTHANDLER_CHANNEL_TYPE (ryos_eventhandler_channel_get_type())
#define RYOS_EVENTHANDLER_CHANNEL(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), RYOS_EVENTHANDLER_CHANNEL_TYPE, RyosEventhandlerChannel))
#define IS_RYOS_EVENTHANDLER_CHANNEL(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), RYOS_EVENTHANDLER_CHANNEL_TYPE))

typedef struct _RyosEventhandlerChannel RyosEventhandlerChannel;

GType ryos_eventhandler_channel_get_type(void);
RyosEventhandlerChannel *ryos_eventhandler_channel_new(void);

gboolean ryos_eventhandler_channel_start(RyosEventhandlerChannel *eventhandler_channel, RoccatDevice *device, GError **error);
void ryos_eventhandler_channel_stop(RyosEventhandlerChannel *eventhandler_channel);

G_END_DECLS

#endif
