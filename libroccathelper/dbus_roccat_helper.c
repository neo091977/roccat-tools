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

#include "dbus_roccat_helper.h"
#include <string.h>

gboolean dbus_check_interface(DBusConnection *bus, gchar const * server_name, gchar const *server_path, gchar const *server_interface) {
	DBusMessage *message;
	DBusMessage *reply;
	DBusMessageIter iter;
	char const *introspection_xml;
	gboolean retval = FALSE;
	gchar *string;

	message = dbus_message_new_method_call(server_name,
			server_path,
			DBUS_INTERFACE_INTROSPECTABLE,
			"Introspect");
	reply = dbus_connection_send_with_reply_and_block(bus, message, -1, NULL);
	dbus_message_unref(message);

	if (!reply)
		return FALSE;

	dbus_message_iter_init(reply, &iter);
	dbus_message_iter_get_basic(&iter, &introspection_xml);

	string = g_strconcat("<interface name=\"", server_interface, "\">", NULL);
	if (strstr(introspection_xml, string))
		retval = TRUE;
	g_free(string);

	dbus_message_unref(reply);
	return retval;
}
