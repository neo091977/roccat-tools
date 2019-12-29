#ifndef __ROCCAT_RYOSTKL_DBUS_SERVER_H__
#define __ROCCAT_RYOSTKL_DBUS_SERVER_H__

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

#define TYPE_RYOSTKL_DBUS_SERVER (ryostkl_dbus_server_get_type())
#define RYOSTKL_DBUS_SERVER(object) (G_TYPE_CHECK_INSTANCE_CAST((object), TYPE_RYOSTKL_DBUS_SERVER, RyostklDBusServer))
#define IS_RYOSTKL_DBUS_SERVER(object) (G_TYPE_CHECK_INSTANCE_TYPE((object), TYPE_RYOSTKL_DBUS_SERVER))

G_BEGIN_DECLS

typedef struct _RyostklDBusServer RyostklDBusServer;

struct _RyostklDBusServer {
	GObject parent;
};

RyostklDBusServer *ryostkl_dbus_server_new(void);
GType ryostkl_dbus_server_get_type(void);

gboolean ryostkl_dbus_server_connect(RyostklDBusServer *dbus_server);

void ryostkl_dbus_server_emit_profile_changed(RyostklDBusServer *object, guchar profile_number);
void ryostkl_dbus_server_emit_brightness_changed(RyostklDBusServer *object, guchar profile_number, guchar brightness_number);
void ryostkl_dbus_server_emit_macro_changed(RyostklDBusServer *object, guchar profile_number, guchar button_number, RyosMacro const *macro);

G_END_DECLS

#endif
