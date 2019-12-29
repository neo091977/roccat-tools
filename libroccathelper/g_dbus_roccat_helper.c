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

#include "g_dbus_roccat_helper.h"
#include <dbus/dbus-glib-bindings.h>
#include "i18n.h"

gboolean dbus_roccat_register_service(gchar const *name, gboolean *owner, GError **error) {
	DBusGConnection *connection;
	gboolean retval;
	DBusGProxy *proxy;
	guint32 request_ret;

	connection = dbus_g_bus_get(DBUS_BUS_SESSION, error);
	if (!connection)
		return FALSE;

	proxy = dbus_g_proxy_new_for_name(connection,
			DBUS_SERVICE_DBUS,
			DBUS_PATH_DBUS,
			DBUS_INTERFACE_DBUS);
	dbus_g_connection_unref(connection);

	retval = org_freedesktop_DBus_request_name(proxy, name, 0, &request_ret, error);
	g_object_unref(proxy);
	if (!retval)
		return FALSE;

	if (owner)
		*owner = (request_ret == DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER) ? TRUE : FALSE;

	return TRUE;
}

DBusGProxy *dbus_roccat_proxy_new_for_name(gchar const *name,
		gchar const *path, gchar const *iface) {
	DBusGConnection *connection;
	DBusGProxy *proxy;
	GError *error = NULL;

	connection = dbus_g_bus_get(DBUS_BUS_SESSION, &error);
	if (!connection) {
		g_warning(_("Could not get dbus: %s"), error->message);
		g_clear_error(&error);
		return NULL;
	}

	proxy = dbus_g_proxy_new_for_name(connection, name, path, iface);

	dbus_g_connection_unref(connection);

	return proxy;
}

void dbus_roccat_proxy_free(DBusGProxy *proxy) {
	if (proxy)
		g_object_unref(proxy);
}

gboolean dbus_roccat_call(DBusGProxy *proxy, gboolean (*function)(DBusGProxy *, GError **)) {
	GError *error = NULL;
	gboolean retval;

	if (!proxy)
		return FALSE;

	retval = function(proxy, &error);
	if (error) {
		/* No error if target is not running */
		if (error->code != DBUS_GERROR_SERVICE_UNKNOWN)
			g_warning(_("Could not call dbus: %s"), error->message);
		g_error_free(error);
	}
	return retval;
}

gboolean dbus_roccat_call_y(DBusGProxy *proxy, gboolean (*function)(DBusGProxy *, const guchar, GError **), const guchar a) {
	GError *error = NULL;
	gboolean retval;

	if (!proxy)
		return FALSE;

	retval = function(proxy, a, &error);
	if (error) {
		/* No error if target is not running */
		if (error->code != DBUS_GERROR_SERVICE_UNKNOWN)
			g_warning(_("Could not call dbus: %s"), error->message);
		g_error_free(error);
	}
	return retval;
}

gboolean dbus_roccat_call_uu(DBusGProxy *proxy, gboolean (*function)(DBusGProxy *, const guint32, const guint32, GError **),
		const guint32 a, const guint32 b) {
	GError *error = NULL;
	gboolean retval;

	if (!proxy)
		return FALSE;

	retval = function(proxy, a, b, &error);
	if (error) {
		/* No error if target is not running */
		if (error->code != DBUS_GERROR_SERVICE_UNKNOWN)
			g_warning(_("Could not call dbus: %s"), error->message);
		g_error_free(error);
	}
	return retval;
}
