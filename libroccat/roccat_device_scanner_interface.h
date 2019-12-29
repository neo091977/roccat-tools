#ifndef __ROCCAT_DEVICE_SCANNER_INTERFACE_H__
#define __ROCCAT_DEVICE_SCANNER_INTERFACE_H__

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

#include "roccat_device.h"

G_BEGIN_DECLS

#define ROCCAT_DEVICE_SCANNER_INTERFACE_TYPE (roccat_device_scanner_interface_get_type())
#define ROCCAT_DEVICE_SCANNER_INTERFACE(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), ROCCAT_DEVICE_SCANNER_INTERFACE_TYPE, RoccatDeviceScannerInterface))
#define IS_ROCCAT_DEVICE_SCANNER_INTERFACE(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), ROCCAT_DEVICE_SCANNER_INTERFACE_TYPE))
#define ROCCAT_DEVICE_SCANNER_INTERFACE_GET_INTERFACE(inst) (G_TYPE_INSTANCE_GET_INTERFACE((inst), ROCCAT_DEVICE_SCANNER_INTERFACE_TYPE, RoccatDeviceScannerInterfaceInterface))

typedef struct _RoccatDeviceScannerInterface RoccatDeviceScannerInterface;
typedef struct _RoccatDeviceScannerInterfaceInterface RoccatDeviceScannerInterfaceInterface;

struct _RoccatDeviceScannerInterfaceInterface {
	GTypeInterface parent;

	/* signals */

	void (*device_added)(RoccatDeviceScannerInterface *self, RoccatDevice *device);
	void (*device_removed)(RoccatDeviceScannerInterface *self, RoccatDevice *device);

	/* virtual table */

	void (*start)(RoccatDeviceScannerInterface *self);
	void (*stop)(RoccatDeviceScannerInterface *self);
};

GType roccat_device_scanner_interface_get_type(void);

void roccat_device_scanner_interface_device_added(RoccatDeviceScannerInterface *self, RoccatDevice *device);
void roccat_device_scanner_interface_device_removed(RoccatDeviceScannerInterface *self, RoccatDevice *device);

void roccat_device_scanner_interface_start(RoccatDeviceScannerInterface *self);
void roccat_device_scanner_interface_stop(RoccatDeviceScannerInterface *self);

G_END_DECLS

#endif
