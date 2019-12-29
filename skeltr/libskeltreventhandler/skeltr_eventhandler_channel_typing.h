#ifndef __ROCCAT_SKELTR_EVENTHANDLER_CHANNEL_TYPING_H__
#define __ROCCAT_SKELTR_EVENTHANDLER_CHANNEL_TYPING_H__

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

#define SKELTR_EVENTHANDLER_CHANNEL_TYPING_TYPE (skeltr_eventhandler_channel_typing_get_type())
#define SKELTR_EVENTHANDLER_CHANNEL_TYPING(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), SKELTR_EVENTHANDLER_CHANNEL_TYPING_TYPE, SkeltrEventhandlerChannelTyping))
#define IS_SKELTR_EVENTHANDLER_CHANNEL_TYPING(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), SKELTR_EVENTHANDLER_CHANNEL_TYPING_TYPE))

typedef struct _SkeltrEventhandlerChannelTyping SkeltrEventhandlerChannelTyping;

GType skeltr_eventhandler_channel_typing_get_type(void);
SkeltrEventhandlerChannelTyping *skeltr_eventhandler_channel_typing_new(void);

gboolean skeltr_eventhandler_channel_typing_start(SkeltrEventhandlerChannelTyping *channel, RoccatDevice *device, GError **error);
void skeltr_eventhandler_channel_typing_stop(SkeltrEventhandlerChannelTyping *channel);

G_END_DECLS

#endif
