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

#include "roccat_gfx.h"
#include "roccat.h"
#include "talkfx.h"

GfxResult roccat_gfx_update(DBusConnection *bus, gchar const * const server_path, gchar const * const server_interface) {
	DBusMessage *message = dbus_message_new_method_call(ROCCAT_DBUS_SERVER_NAME, server_path, server_interface, "GfxUpdate");
	dbus_connection_send(bus, message, NULL);
	dbus_message_unref(message);
	return GFX_SUCCESS;
}

GfxResult roccat_gfx_set_light_color(DBusConnection *bus, gchar const * const server_path, gchar const * const server_interface,
		unsigned int const light_index, uint32_t const color) {
	DBusMessage *message;
	dbus_uint32_t dbus_light_index;

	dbus_light_index = light_index;

	message = dbus_message_new_method_call(ROCCAT_DBUS_SERVER_NAME, server_path, server_interface, "GfxSetLedRgb");

	dbus_message_append_args(message,
			DBUS_TYPE_BYTE, &dbus_light_index,
			DBUS_TYPE_UINT32, &color,
			DBUS_TYPE_INVALID);

	dbus_connection_send(bus, message, NULL);

	dbus_message_unref(message);
	return GFX_SUCCESS;
}

GfxResult roccat_gfx_get_light_color(DBusConnection *bus, gchar const * const server_path, gchar const * const server_interface,
		unsigned int const light_index, uint32_t * const color) {
	DBusMessage *message;
	DBusMessage *reply;
	dbus_uint32_t dbus_light_index;

	dbus_light_index = light_index;

	message = dbus_message_new_method_call(ROCCAT_DBUS_SERVER_NAME, server_path, server_interface, "GfxGetLedRgb");

	dbus_message_append_args(message,
			DBUS_TYPE_BYTE, &dbus_light_index,
			DBUS_TYPE_INVALID);

	reply = dbus_connection_send_with_reply_and_block(bus, message, -1, NULL);
	dbus_message_unref(message);
	if (!reply)
		return GFX_FAILURE;

	dbus_message_get_args(reply, NULL,
			DBUS_TYPE_UINT32, color,
			DBUS_TYPE_INVALID);

	dbus_message_unref(reply);
	return GFX_SUCCESS;
}
