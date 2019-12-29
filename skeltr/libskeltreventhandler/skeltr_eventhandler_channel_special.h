#ifndef __ROCCAT_SKELTR_EVENTHANDLER_CHANNEL_SPECIAL_H__
#define __ROCCAT_SKELTR_EVENTHANDLER_CHANNEL_SPECIAL_H__

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

#define SKELTR_EVENTHANDLER_CHANNEL_SPECIAL_TYPE (skeltr_eventhandler_channel_special_get_type())
#define SKELTR_EVENTHANDLER_CHANNEL_SPECIAL(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), SKELTR_EVENTHANDLER_CHANNEL_SPECIAL_TYPE, SkeltrEventhandlerChannelSpecial))
#define IS_SKELTR_EVENTHANDLER_CHANNEL_SPECIAL(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), SKELTR_EVENTHANDLER_CHANNEL_SPECIAL_TYPE))

typedef struct _SkeltrEventhandlerChannelSpecial SkeltrEventhandlerChannelSpecial;

GType skeltr_eventhandler_channel_special_get_type(void);
SkeltrEventhandlerChannelSpecial *skeltr_eventhandler_channel_special_new(void);

gboolean skeltr_eventhandler_channel_special_start(SkeltrEventhandlerChannelSpecial *channel, RoccatDevice *device, GError **error);
void skeltr_eventhandler_channel_special_stop(SkeltrEventhandlerChannelSpecial *channel);

G_END_DECLS

#endif
