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

#include "roccat_talkfx.h"
#include "roccat.h"

void roccat_talkfx_restore_led_rgb(DBusConnection *bus, gchar const *server_path, gchar const *server_interface) {
	DBusMessage *message = dbus_message_new_method_call(ROCCAT_DBUS_SERVER_NAME, server_path, server_interface, "TalkfxRestoreLedRgb");
	dbus_connection_send(bus, message, NULL);
	dbus_message_unref(message);
}
