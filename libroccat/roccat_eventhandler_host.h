#ifndef __ROCCAT_EVENTHANDLER_HOST_H__
#define __ROCCAT_EVENTHANDLER_HOST_H__

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
#include <glib-object.h>

G_BEGIN_DECLS

#define ROCCAT_EVENTHANDLER_HOST_API __attribute__((visibility("default")))

#define ROCCAT_EVENTHANDLER_HOST_TYPE (roccat_eventhandler_host_get_type())
#define ROCCAT_EVENTHANDLER_HOST(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), ROCCAT_EVENTHANDLER_HOST_TYPE, RoccatEventhandlerHost))
#define IS_ROCCAT_EVENTHANDLER_HOST(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), ROCCAT_EVENTHANDLER_HOST_TYPE))
#define ROCCAT_EVENTHANDLER_HOST_GET_INTERFACE(inst) (G_TYPE_INSTANCE_GET_INTERFACE((inst), ROCCAT_EVENTHANDLER_HOST_TYPE, RoccatEventhandlerHostInterface))

typedef struct _RoccatEventhandlerHost RoccatEventhandlerHost;
typedef struct _RoccatEventhandlerHostInterface RoccatEventhandlerHostInterface;

struct _RoccatEventhandlerHostInterface {
	GTypeInterface parent;

	RoccatNotificator *(*get_notificator)(RoccatEventhandlerHost *self);
	guint (*should_set_driver_state)(RoccatEventhandlerHost *self);
};

typedef enum {
	ROCCATEVENTHANDLER_CONFIGURATION_DRIVER_STATE_ON,
	ROCCATEVENTHANDLER_CONFIGURATION_DRIVER_STATE_OFF,
	ROCCATEVENTHANDLER_CONFIGURATION_DRIVER_STATE_UNUSED,
} RoccateventhandlerConfigurationDriverState;

GType roccat_eventhandler_host_get_type(void);

RoccatNotificator *roccat_eventhandler_host_get_notificator(RoccatEventhandlerHost *self);
guint roccat_eventhandler_host_should_set_driver_state(RoccatEventhandlerHost *self);

G_END_DECLS

#endif
