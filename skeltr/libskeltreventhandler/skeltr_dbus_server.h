#ifndef __ROCCAT_SKELTR_DBUS_SERVER_H__
#define __ROCCAT_SKELTR_DBUS_SERVER_H__

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

#include "skeltr_macro.h"
#include <glib-object.h>

#define TYPE_SKELTR_DBUS_SERVER (skeltr_dbus_server_get_type())
#define SKELTR_DBUS_SERVER(object) (G_TYPE_CHECK_INSTANCE_CAST((object), TYPE_SKELTR_DBUS_SERVER, SkeltrDBusServer))
#define IS_SKELTR_DBUS_SERVER(object) (G_TYPE_CHECK_INSTANCE_TYPE((object), TYPE_SKELTR_DBUS_SERVER))

G_BEGIN_DECLS

typedef struct _SkeltrDBusServer SkeltrDBusServer;

struct _SkeltrDBusServer {
	GObject parent;
};

SkeltrDBusServer *skeltr_dbus_server_new(void);
GType skeltr_dbus_server_get_type(void);

gboolean skeltr_dbus_server_connect(SkeltrDBusServer *dbus_server);

void skeltr_dbus_server_emit_profile_changed(SkeltrDBusServer *object, guchar profile_number);
void skeltr_dbus_server_emit_brightness_changed(SkeltrDBusServer *object, guchar profile_number, guchar brightness_number);
void skeltr_dbus_server_emit_macro_changed(SkeltrDBusServer *object, guchar profile_number, guchar button_number, SkeltrMacro const *macro);

G_END_DECLS

#endif
