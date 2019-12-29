#ifndef __ROCCAT_ISKUFX_DBUS_SERVER_H__
#define __ROCCAT_ISKUFX_DBUS_SERVER_H__

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

#include "iskufx.h"
#include <glib-object.h>

#define TYPE_ISKUFX_DBUS_SERVER (iskufx_dbus_server_get_type())
#define ISKUFX_DBUS_SERVER(object) (G_TYPE_CHECK_INSTANCE_CAST((object), TYPE_ISKUFX_DBUS_SERVER, IskufxDBusServer))
#define IS_ISKUFX_DBUS_SERVER(object) (G_TYPE_CHECK_INSTANCE_TYPE((object), TYPE_ISKUFX_DBUS_SERVER))

G_BEGIN_DECLS

typedef struct _IskufxDBusServer IskufxDBusServer;

struct _IskufxDBusServer {
	GObject parent;
};

IskufxDBusServer *iskufx_dbus_server_new(void);
GType iskufx_dbus_server_get_type(void);

gboolean iskufx_dbus_server_connect(IskufxDBusServer *dbus_server);

void iskufx_dbus_server_emit_profile_changed(IskufxDBusServer *object, guchar profile_number);
void iskufx_dbus_server_emit_brightness_changed(IskufxDBusServer *object, guchar profile_number, guchar brightness_number);
void iskufx_dbus_server_emit_macro_changed(IskufxDBusServer *object, guchar profile_number, guchar button_number, IskuRkpMacroKeyInfo const *key_info);

G_END_DECLS

#endif
