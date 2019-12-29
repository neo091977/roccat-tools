#ifndef __ROCCAT_RYOS_DBUS_SERVER_H__
#define __ROCCAT_RYOS_DBUS_SERVER_H__

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

#include "ryos_macro.h"
#include <glib-object.h>

#define TYPE_RYOS_DBUS_SERVER (ryos_dbus_server_get_type())
#define RYOS_DBUS_SERVER(object) (G_TYPE_CHECK_INSTANCE_CAST((object), TYPE_RYOS_DBUS_SERVER, RyosDBusServer))
#define IS_RYOS_DBUS_SERVER(object) (G_TYPE_CHECK_INSTANCE_TYPE((object), TYPE_RYOS_DBUS_SERVER))

G_BEGIN_DECLS

typedef struct _RyosDBusServer RyosDBusServer;

struct _RyosDBusServer {
	GObject parent;
};

RyosDBusServer *ryos_dbus_server_new(void);
GType ryos_dbus_server_get_type(void);

gboolean ryos_dbus_server_connect(RyosDBusServer *dbus_server);

void ryos_dbus_server_emit_profile_changed(RyosDBusServer *object, guchar profile_number);
void ryos_dbus_server_emit_brightness_changed(RyosDBusServer *object, guchar profile_number, guchar brightness_number);
void ryos_dbus_server_emit_macro_changed(RyosDBusServer *object, guchar profile_number, guchar button_number, RyosMacro const *macro);

G_END_DECLS

#endif
