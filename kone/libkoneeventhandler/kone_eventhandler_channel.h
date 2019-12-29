#ifndef __ROCCAT_KONE_EVENTHANDLER_CHANNEL_H__
#define __ROCCAT_KONE_EVENTHANDLER_CHANNEL_H__

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

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define KONE_EVENTHANDLER_CHANNEL_TYPE (kone_eventhandler_channel_get_type())
#define KONE_EVENTHANDLER_CHANNEL(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), KONE_EVENTHANDLER_CHANNEL_TYPE, KoneEventhandlerChannel))
#define IS_KONE_EVENTHANDLER_CHANNEL(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), KONE_EVENTHANDLER_CHANNEL_TYPE))

typedef struct _KoneEventhandlerChannel KoneEventhandlerChannel;

GType kone_eventhandler_channel_get_type(void);
KoneEventhandlerChannel *kone_eventhandler_channel_new(void);

gboolean kone_eventhandler_channel_start(KoneEventhandlerChannel *eventhandler_channel, gchar const *path, GError **error);
void kone_eventhandler_channel_stop(KoneEventhandlerChannel *eventhandler_channel);

G_END_DECLS

#endif
