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

#include "roccat.h"
#include "konextd.h"
#include "konextd_dbus_plain_services.h"
#include "g_dbus_roccat_helper.h"

DBusGProxy *konextd_dbus_proxy_new(void) {
	DBusGProxy *proxy;

	proxy = dbus_roccat_proxy_new_for_name(ROCCAT_DBUS_SERVER_NAME,
			KONEXTD_DBUS_SERVER_PATH,
			KONEXTD_DBUS_SERVER_INTERFACE);

	if (!proxy)
		return NULL;

	dbus_g_object_register_marshaller(g_cclosure_marshal_VOID__UCHAR,
			G_TYPE_NONE, G_TYPE_UCHAR, G_TYPE_INVALID);

	dbus_g_proxy_add_signal(proxy, "ProfileChanged", G_TYPE_UCHAR, G_TYPE_INVALID);

	return proxy;
}

gboolean konextd_dbus_emit_profile_data_changed_outside(DBusGProxy *proxy, guint profile_number) {
	return dbus_roccat_call_y(proxy, org_roccat_Konextd_profile_data_changed_outside, profile_number);
}

gboolean konextd_dbus_emit_profile_data_changed_outside_instant(guint profile_number) {
	DBusGProxy *proxy;
	gboolean retval;

	proxy = konextd_dbus_proxy_new();
	if (!proxy)
		return FALSE;

	retval = konextd_dbus_emit_profile_data_changed_outside(proxy, profile_number);
	dbus_roccat_proxy_free(proxy);
	return retval;
}

gboolean konextd_dbus_emit_profile_changed_outside(DBusGProxy *proxy, guint profile_number) {
	return dbus_roccat_call_y(proxy, org_roccat_Konextd_profile_changed_outside, profile_number);
}

gboolean konextd_dbus_emit_profile_changed_outside_instant(guint profile_number) {
	DBusGProxy *proxy;
	gboolean retval;

	proxy = konextd_dbus_proxy_new();
	if (!proxy)
		return FALSE;

	retval = konextd_dbus_emit_profile_changed_outside(proxy, profile_number);
	dbus_roccat_proxy_free(proxy);
	return retval;
}

gboolean konextd_dbus_emit_configuration_changed_outside(DBusGProxy *proxy) {
	return dbus_roccat_call(proxy, org_roccat_Konextd_configuration_changed_outside);
}
