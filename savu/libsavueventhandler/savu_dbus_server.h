#ifndef __ROCCAT_SAVU_DBUS_SERVER_H__
#define __ROCCAT_SAVU_DBUS_SERVER_H__

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

#define TYPE_SAVU_DBUS_SERVER (savu_dbus_server_get_type())
#define SAVU_DBUS_SERVER(object) (G_TYPE_CHECK_INSTANCE_CAST((object), TYPE_SAVU_DBUS_SERVER, SavuDBusServer))
#define IS_SAVU_DBUS_SERVER(object) (G_TYPE_CHECK_INSTANCE_TYPE((object), TYPE_SAVU_DBUS_SERVER))

G_BEGIN_DECLS

typedef struct _SavuDBusServer SavuDBusServer;

struct _SavuDBusServer {
	GObject parent;
};

SavuDBusServer *savu_dbus_server_new(void);
GType savu_dbus_server_get_type(void);

gboolean savu_dbus_server_connect(SavuDBusServer *dbus_server);

void savu_dbus_server_emit_profile_changed(SavuDBusServer *object, guchar number);

G_END_DECLS

#endif
