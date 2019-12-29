#ifndef __ROCCAT_ARVO_DBUS_SERVER_H__
#define __ROCCAT_ARVO_DBUS_SERVER_H__

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

#include "arvo.h"
#include <glib-object.h>

#define TYPE_ARVO_DBUS_SERVER (arvo_dbus_server_get_type())
#define ARVO_DBUS_SERVER(object) (G_TYPE_CHECK_INSTANCE_CAST((object), TYPE_ARVO_DBUS_SERVER, ArvoDBusServer))
#define IS_ARVO_DBUS_SERVER(object) (G_TYPE_CHECK_INSTANCE_TYPE((object), TYPE_ARVO_DBUS_SERVER))

G_BEGIN_DECLS

typedef struct _ArvoDBusServer ArvoDBusServer;

struct _ArvoDBusServer {
	GObject parent;
};

ArvoDBusServer *arvo_dbus_server_new(void);
GType arvo_dbus_server_get_type(void);

gboolean arvo_dbus_server_connect(ArvoDBusServer *dbus_server);

void arvo_dbus_server_emit_profile_changed(ArvoDBusServer *object, guchar profile_number);

G_END_DECLS

#endif
