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
#include "suora_dbus_services.h"
#include "suora_dbus_plain_services.h"
#include "g_dbus_roccat_helper.h"
#include "g_cclosure_roccat_marshaller.h"

DBusGProxy *suora_dbus_proxy_new(void) {
	DBusGProxy *proxy;

	proxy = dbus_roccat_proxy_new_for_name(ROCCAT_DBUS_SERVER_NAME,
			SUORA_DBUS_SERVER_PATH,
			SUORA_DBUS_SERVER_INTERFACE);

	if (!proxy)
		return NULL;

	return proxy;
}

gboolean suora_dbus_emit_profile_data_changed_outside(DBusGProxy *proxy) {
	return dbus_roccat_call(proxy, org_roccat_Suora_profile_data_changed_outside);
}

gboolean suora_dbus_emit_profile_data_changed_outside_instant(void) {
	DBusGProxy *proxy;
	gboolean retval;

	proxy = suora_dbus_proxy_new();
	if (!proxy)
		return FALSE;

	retval = suora_dbus_emit_profile_data_changed_outside(proxy);
	dbus_roccat_proxy_free(proxy);
	return retval;
}

gboolean suora_dbus_emit_configuration_changed_outside(DBusGProxy *proxy) {
	return dbus_roccat_call(proxy, org_roccat_Suora_configuration_changed_outside);
}

gboolean suora_dbus_emit_configuration_changed_outside_instant(void) {
	DBusGProxy *proxy;
	gboolean retval;

	proxy = suora_dbus_proxy_new();
	if (!proxy)
		return FALSE;

	retval = suora_dbus_emit_configuration_changed_outside(proxy);
	dbus_roccat_proxy_free(proxy);
	return retval;
}
