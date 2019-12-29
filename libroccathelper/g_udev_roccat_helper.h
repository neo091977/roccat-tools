#ifndef __ROCCAT_G_UDEV_HELPER_H__
#define __ROCCAT_G_UDEV_HELPER_H__

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

#include <gudev/gudev.h>

G_BEGIN_DECLS

#define G_UDEV_ROCCAT_DEVICE_ACTION_ADD "add"
#define G_UDEV_ROCCAT_DEVICE_ACTION_REMOVE "remove"

GUdevDevice *g_udev_roccat_get_parent_hid_device(GUdevDevice *device);
GUdevDevice *g_udev_roccat_get_parent_usbhid_device(GUdevDevice *device);
GUdevDevice *g_udev_roccat_get_parent_usb_device(GUdevDevice *device);

guint g_udev_roccat_usb_get_vendor_id(GUdevDevice *usb_device);
guint g_udev_roccat_usb_get_product_id(GUdevDevice *usb_device);
guint g_udev_roccat_usb_get_num_interfaces(GUdevDevice *usb_device);

guint g_udev_roccat_usbhid_get_interfaceprotocol(GUdevDevice *usbhid_device);
guint g_udev_roccat_usbhid_get_interfacenumber(GUdevDevice *usbhid_device);

void g_udev_roccat_device_list_free(GList *list);

gboolean g_udev_roccat_device_is_sibling(GUdevDevice *parent, GUdevDevice *sibling);

G_END_DECLS

#endif
