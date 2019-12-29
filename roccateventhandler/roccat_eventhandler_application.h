#ifndef __ROCCAT_EVENTHANDLER_APPLICATION_H__
#define __ROCCAT_EVENTHANDLER_APPLICATION_H__

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

#include <glib-object.h>

G_BEGIN_DECLS

#define ROCCAT_EVENTHANDLER_APPLICATION_TYPE (roccat_eventhandler_application_get_type())
#define ROCCAT_EVENTHANDLER_APPLICATION(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), ROCCAT_EVENTHANDLER_APPLICATION_TYPE, RoccatEventhandlerApplication))
#define IS_ROCCAT_EVENTHANDLER_APPLICATION(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), ROCCAT_EVENTHANDLER_APPLICATION_TYPE))

typedef struct _RoccatEventhandlerApplication RoccatEventhandlerApplication;

GType roccat_eventhandler_application_get_type(void);
RoccatEventhandlerApplication *roccat_eventhandler_application_new(void);

G_END_DECLS

#endif
