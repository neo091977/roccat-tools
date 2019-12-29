#ifndef __ROCCAT_SOVA_EVENTHANDLER_CHANNEL_TYPING_H__
#define __ROCCAT_SOVA_EVENTHANDLER_CHANNEL_TYPING_H__

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

#define SOVA_EVENTHANDLER_CHANNEL_TYPING_TYPE (sova_eventhandler_channel_typing_get_type())
#define SOVA_EVENTHANDLER_CHANNEL_TYPING(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), SOVA_EVENTHANDLER_CHANNEL_TYPING_TYPE, SovaEventhandlerChannelTyping))
#define IS_SOVA_EVENTHANDLER_CHANNEL_TYPING(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), SOVA_EVENTHANDLER_CHANNEL_TYPING_TYPE))

typedef struct _SovaEventhandlerChannelTyping SovaEventhandlerChannelTyping;

GType sova_eventhandler_channel_typing_get_type(void);
SovaEventhandlerChannelTyping *sova_eventhandler_channel_typing_new(void);

gboolean sova_eventhandler_channel_typing_start(SovaEventhandlerChannelTyping *channel, RoccatDevice *device, GError **error);
void sova_eventhandler_channel_typing_stop(SovaEventhandlerChannelTyping *channel);

G_END_DECLS

#endif
