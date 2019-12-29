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

#include "roccat_device_scanner_interface.h"

enum {
	DEVICE_ADDED,
	DEVICE_REMOVED,
	LAST_SIGNAL,
};

static guint signals[LAST_SIGNAL] = { 0 };

void roccat_device_scanner_interface_start(RoccatDeviceScannerInterface *self) {
	ROCCAT_DEVICE_SCANNER_INTERFACE_GET_INTERFACE(self)->start(self);
}

void roccat_device_scanner_interface_stop(RoccatDeviceScannerInterface *self) {
	ROCCAT_DEVICE_SCANNER_INTERFACE_GET_INTERFACE(self)->stop(self);
}

void roccat_device_scanner_interface_device_added(RoccatDeviceScannerInterface *self, RoccatDevice *device) {
	g_signal_emit((gpointer)self, signals[DEVICE_ADDED], 0, device);
}

void roccat_device_scanner_interface_device_removed(RoccatDeviceScannerInterface *self, RoccatDevice *device) {
	g_signal_emit((gpointer)self, signals[DEVICE_REMOVED], 0, device);
}

G_DEFINE_INTERFACE(RoccatDeviceScannerInterface, roccat_device_scanner_interface, G_TYPE_OBJECT);

static void roccat_device_scanner_interface_default_init(RoccatDeviceScannerInterfaceInterface *iface) {
	signals[DEVICE_ADDED] = g_signal_new("device-added", ROCCAT_DEVICE_SCANNER_INTERFACE_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			G_STRUCT_OFFSET(RoccatDeviceScannerInterfaceInterface, device_added),
			NULL, NULL, g_cclosure_marshal_VOID__OBJECT, G_TYPE_NONE,
			1, G_TYPE_OBJECT);

	signals[DEVICE_REMOVED] = g_signal_new("device-removed", ROCCAT_DEVICE_SCANNER_INTERFACE_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			G_STRUCT_OFFSET(RoccatDeviceScannerInterfaceInterface, device_removed),
			NULL, NULL, g_cclosure_marshal_VOID__OBJECT, G_TYPE_NONE,
			1, G_TYPE_OBJECT);
}
