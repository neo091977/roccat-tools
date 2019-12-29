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

#include "g_udev_roccat_helper.h"
#include "g_roccat_helper.h"
#include <stdlib.h>

/* Return same device if it fits */
static GUdevDevice *g_udev_roccat_get_parent_with_subsystem(GUdevDevice *device, gchar const *subsystem, gchar const *devtype) {
	gchar const *is_subsystem;
	gchar const *is_devtype;
	
	is_subsystem = g_udev_device_get_subsystem(device);
	is_devtype = g_udev_device_get_devtype(device);

	if (g_strcmp0(subsystem, is_subsystem) == 0 && g_strcmp0(devtype, is_devtype) == 0) {
		g_object_ref(G_OBJECT(device));
		return device;
	}
		
	return g_udev_device_get_parent_with_subsystem(device, subsystem, devtype);
}

GUdevDevice *g_udev_roccat_get_parent_hid_device(GUdevDevice *device) {
	return g_udev_roccat_get_parent_with_subsystem(device, "hid", NULL);
}

GUdevDevice *g_udev_roccat_get_parent_usbhid_device(GUdevDevice *device) {
	return g_udev_roccat_get_parent_with_subsystem(device, "usb", "usb_interface");
}

GUdevDevice *g_udev_roccat_get_parent_usb_device(GUdevDevice *device) {
	return g_udev_roccat_get_parent_with_subsystem(device, "usb", "usb_device");
}

guint g_udev_roccat_usb_get_vendor_id(GUdevDevice *usb_device) {
	gchar const *string;
	string = g_udev_device_get_sysfs_attr(usb_device, "idVendor");
	return string ? (guint)strtoul(string, NULL, 16) : 0;
}

guint g_udev_roccat_usb_get_product_id(GUdevDevice *usb_device) {
	gchar const *string;
	string = g_udev_device_get_sysfs_attr(usb_device, "idProduct");
	return string ? (guint)strtoul(string, NULL, 16) : 0;
}

guint g_udev_roccat_usb_get_num_interfaces(GUdevDevice *usb_device) {
	return g_udev_device_get_sysfs_attr_as_uint64(usb_device, "bNumInterfaces");
}

guint g_udev_roccat_usbhid_get_interfaceprotocol(GUdevDevice *usbhid_device) {
	return g_udev_device_get_sysfs_attr_as_uint64(usbhid_device, "bInterfaceProtocol");
}

guint g_udev_roccat_usbhid_get_interfacenumber(GUdevDevice *usbhid_device) {
	return g_udev_device_get_sysfs_attr_as_uint64(usbhid_device, "bInterfaceNumber");
}

void g_udev_roccat_device_list_free(GList *list) {
	g_list_free_full(list, g_object_unref);
}

gboolean g_udev_roccat_device_is_sibling(GUdevDevice *parent, GUdevDevice *sibling) {
	GUdevDevice *device;
	gboolean retval = FALSE;

	device = g_udev_device_get_parent_with_subsystem(sibling,
			g_udev_device_get_subsystem(parent),
			g_udev_device_get_devtype(parent));

	if (!device)
		return FALSE;

	if (g_strcmp0(g_udev_device_get_name(device), g_udev_device_get_name(parent)) == 0)
		retval = TRUE;

	g_object_unref(device);

	return retval;
}
