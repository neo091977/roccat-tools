#ifndef __ROCCAT_G_DBUS_HELPER_H__
#define __ROCCAT_G_DBUS_HELPER_H__

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

#include <glib.h>
#include <dbus/dbus-glib.h>

G_BEGIN_DECLS

gboolean dbus_roccat_register_service(gchar const *name, gboolean *owner, GError **error);

/*
 * Needs g_type_init() to be called before.
 */
DBusGProxy *dbus_roccat_proxy_new_for_name(gchar const *name, gchar const *path, gchar const *iface);

void dbus_roccat_proxy_free(DBusGProxy *proxy);

gboolean dbus_roccat_call(DBusGProxy *proxy,
		gboolean (*function)(DBusGProxy *, GError **));
gboolean dbus_roccat_call_y(DBusGProxy *proxy,
		gboolean (*function)(DBusGProxy *, const guchar, GError **),
		const guchar a);
gboolean dbus_roccat_call_uu(DBusGProxy *proxy,
		gboolean (*function)(DBusGProxy *, const guint32, const guint32, GError **),
		const guint32 a, const guint32 b);

G_END_DECLS

#endif
