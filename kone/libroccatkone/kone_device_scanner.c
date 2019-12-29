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

#include "kone_device_scanner.h"
#include "kone.h"
#include "roccat.h"
#include "roccat_device.h"
#include "g_roccat_helper.h"
#include "g_udev_roccat_helper.h"
#include "i18n-lib.h"
#include <string.h>

#define KONE_DEVICE_SCANNER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), KONE_DEVICE_SCANNER_TYPE, KoneDeviceScannerClass))
#define IS_KONE_DEVICE_SCANNER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), KONE_DEVICE_SCANNER_TYPE))
#define KONE_DEVICE_SCANNER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), KONE_DEVICE_SCANNER_TYPE, KoneDeviceScannerPrivate))

typedef struct _KoneDeviceScannerClass KoneDeviceScannerClass;

struct _KoneDeviceScannerClass {
	GObjectClass parent_class;
};

struct _KoneDeviceScannerPrivate {
	GUdevClient *add_client;
	gulong add_handler;
	GUdevClient *remove_client;
	gulong remove_handler;
	guint const *product_ids;
	gchar const *class_name;
};

static gchar const * const roccat = "roccat";
static gchar const * const driver_name = "kone";
static guint const device_ids[2] = { USB_DEVICE_ID_ROCCAT_KONE, 0 };

static gboolean product_id_fits(GUdevDevice *usb_device, guint const *product_ids) {
	guint device_id;

	device_id = g_udev_roccat_usb_get_product_id(usb_device);

	while (*product_ids) {
		if (device_id == *product_ids)
			return TRUE;
		++product_ids;
	}
	return FALSE;
}

static gboolean vendor_id_fits(GUdevDevice *usb_device) {
	return (g_udev_roccat_usb_get_vendor_id(usb_device) == USB_VENDOR_ID_ROCCAT) ? TRUE : FALSE;
}

static gboolean usb_device_fits(GUdevDevice *usb_device, guint const *product_ids) {
	if (!vendor_id_fits(usb_device))
		return FALSE;

	if (!product_id_fits(usb_device, product_ids))
		return FALSE;

	return TRUE;
}

static gboolean usbhid_device_fits(GUdevDevice *usbhid_device) {
	return (g_udev_roccat_usbhid_get_interfacenumber(usbhid_device) == KONE_INTERFACE_MOUSE) ? TRUE : FALSE;
}

static gboolean have_special_driver(GUdevDevice *hid_device) {
	gchar const *driver;
	driver = g_udev_device_get_driver(hid_device);
	return (!g_strcmp0(driver, "hid-generic") || !g_strcmp0(driver, "generic-usb")) ? FALSE : TRUE;
}

static void fill_syspath(RoccatDevice *roccat_device, GUdevDevice *usbhid_device, GUdevDevice *some_device) {
	if (!g_strcmp0(g_udev_device_get_subsystem(some_device), "hid"))
		roccat_device_set_syspath(roccat_device, g_udev_device_get_sysfs_path(usbhid_device));
	else if (!g_strcmp0(g_udev_device_get_subsystem(some_device), roccat))
		roccat_device_set_syspath(roccat_device, g_udev_device_get_sysfs_path(usbhid_device));
	else
		roccat_device_set_syspath(roccat_device, g_udev_device_get_sysfs_path(some_device));
}

static RoccatDevice *roccat_device_from_some_device(GUdevClient *client, GUdevDevice *some_device, guint const *product_ids) {
	RoccatDevice *roccat_device = NULL;
	GUdevDevice *usbhid_device;
	GUdevDevice *usb_device;
	GUdevDevice *hid_device;

	hid_device = g_udev_roccat_get_parent_hid_device(some_device);
	if (!hid_device)
		goto exit0;

	/*
	 * Case: special driver without chardev and class
	 * Accept hid only with special driver
	 */
	if (!have_special_driver(hid_device) && !g_strcmp0(g_udev_device_get_subsystem(some_device), "hid"))
		goto exit1;

	usbhid_device = g_udev_roccat_get_parent_usbhid_device(some_device);
	if (!usbhid_device)
		goto exit1;

	if (!usbhid_device_fits(usbhid_device))
		goto exit2;

	usb_device = g_udev_roccat_get_parent_usb_device(some_device);
	if (!usb_device)
		goto exit2;

	if (!usb_device_fits(usb_device, product_ids))
		goto exit3;

	roccat_device = roccat_device_new(g_udev_device_get_name(usb_device),
			g_udev_roccat_usb_get_vendor_id(usb_device),
			g_udev_roccat_usb_get_product_id(usb_device),
			g_udev_roccat_usb_get_num_interfaces(usb_device));

	roccat_device_set_roccat_dev(roccat_device, g_udev_device_get_device_file(some_device));
	fill_syspath(roccat_device, usbhid_device, some_device);

exit3:
	g_object_unref(usb_device);
exit2:
	g_object_unref(usbhid_device);
exit1:
	g_object_unref(hid_device);
exit0:
	return roccat_device;
}

static void uevent_add_cb(GUdevClient *client, gchar *action, GUdevDevice *some_device, gpointer user_data) {
	KoneDeviceScanner *device_scanner = KONE_DEVICE_SCANNER(user_data);
	KoneDeviceScannerPrivate *priv = device_scanner->priv;
	RoccatDevice *roccat_device;

	if (g_strcmp0(action, G_UDEV_ROCCAT_DEVICE_ACTION_ADD) != 0)
		return;

	roccat_device = roccat_device_from_some_device(client, some_device, priv->product_ids);
	if (!roccat_device)
		return;

	roccat_device_scanner_interface_device_added(ROCCAT_DEVICE_SCANNER_INTERFACE(user_data), roccat_device);
	g_object_unref(G_OBJECT(roccat_device));
}

static void uevent_remove_cb(GUdevClient *client, gchar *action, GUdevDevice *usb_device, gpointer user_data) {
	RoccatDevice *roccat_device;

	if (g_strcmp0(action, G_UDEV_ROCCAT_DEVICE_ACTION_REMOVE) != 0)
		return;

	/*
	 * Sending more events than needed.
	 * Not testing on vendor/product id because these values aren't available
	 * anymore after firmware update.
	 */

	roccat_device = roccat_device_new(g_udev_device_get_name(usb_device), 0, 0, 0);

	roccat_device_scanner_interface_device_removed(ROCCAT_DEVICE_SCANNER_INTERFACE(user_data), roccat_device);
	g_object_unref(G_OBJECT(roccat_device));
}

static void iterate_existing(KoneDeviceScanner *device_scanner) {
	KoneDeviceScannerPrivate *priv = device_scanner->priv;
	GUdevClient *client;
	GList *list;
	GList *iterator;

	client = g_udev_client_new(NULL);
	list = g_udev_client_query_by_subsystem(client, priv->class_name);

	for (iterator = g_list_first(list); iterator; iterator = g_list_next(iterator))
		uevent_add_cb(client, G_UDEV_ROCCAT_DEVICE_ACTION_ADD, (GUdevDevice *)iterator->data, device_scanner);

	g_udev_roccat_device_list_free(list);
	g_object_unref(client);
}

void kone_device_scanner_start(RoccatDeviceScannerInterface *self) {
	KoneDeviceScanner *device_scanner = KONE_DEVICE_SCANNER(self);
	KoneDeviceScannerPrivate *priv = device_scanner->priv;
	priv->add_handler = g_signal_connect(G_OBJECT(priv->add_client), "uevent", G_CALLBACK(uevent_add_cb), device_scanner);
	priv->remove_handler = g_signal_connect(G_OBJECT(priv->remove_client), "uevent", G_CALLBACK(uevent_remove_cb), device_scanner);
	iterate_existing(device_scanner);
}

void kone_device_scanner_stop(RoccatDeviceScannerInterface *self) {
	KoneDeviceScanner *device_scanner = KONE_DEVICE_SCANNER(self);
	KoneDeviceScannerPrivate *priv = device_scanner->priv;
	g_signal_handler_disconnect(G_OBJECT(priv->add_client), priv->add_handler);
	g_signal_handler_disconnect(G_OBJECT(priv->remove_client), priv->remove_handler);
}

static void roccat_device_scanner_interface_init(RoccatDeviceScannerInterfaceInterface *iface) {
	iface->start = kone_device_scanner_start;
	iface->stop = kone_device_scanner_stop;
}

G_DEFINE_TYPE_WITH_CODE(KoneDeviceScanner, kone_device_scanner, G_TYPE_OBJECT,
		G_IMPLEMENT_INTERFACE(ROCCAT_DEVICE_SCANNER_INTERFACE_TYPE, roccat_device_scanner_interface_init));

KoneDeviceScanner *kone_device_scanner_new(void) {
	KoneDeviceScanner *device_scanner;
	KoneDeviceScannerPrivate *priv;
	gchar const * const add_subsystems[] = { roccat, driver_name, NULL };
	gchar const * const remove_subsystems[] = { "usb/usb_device", NULL };

	device_scanner = KONE_DEVICE_SCANNER(g_object_new(KONE_DEVICE_SCANNER_TYPE, NULL));
	priv = device_scanner->priv;

	priv->add_client = g_udev_client_new(add_subsystems);

	priv->remove_client = g_udev_client_new(remove_subsystems);
	priv->product_ids = device_ids;
	priv->class_name = driver_name;

	return device_scanner;
}

static void kone_device_scanner_init(KoneDeviceScanner *device_scanner) {
	KoneDeviceScannerPrivate *priv = KONE_DEVICE_SCANNER_GET_PRIVATE(device_scanner);
	device_scanner->priv = priv;
}

static void kone_device_scanner_dispose(GObject *object) {
	KoneDeviceScannerPrivate *priv = KONE_DEVICE_SCANNER(object)->priv;
	g_clear_object(&priv->add_client);
	g_clear_object(&priv->remove_client);
	G_OBJECT_CLASS(kone_device_scanner_parent_class)->dispose(object);
}

static void kone_device_scanner_class_init(KoneDeviceScannerClass *klass) {
	GObjectClass *gobject_class;

	gobject_class = (GObjectClass*)klass;

	gobject_class->dispose = kone_device_scanner_dispose;

	g_type_class_add_private(klass, sizeof(KoneDeviceScannerPrivate));
}

static RoccatDevice *device_list_get_first_wanted(GUdevClient *client, GList *list, guint const *device_ids) {
	RoccatDevice *roccat_device;
	GList *iterator;
	GUdevDevice *some_device;

	for (iterator = g_list_first(list); iterator; iterator = g_list_next(iterator)) {
		some_device = (GUdevDevice *)iterator->data;
		roccat_device = roccat_device_from_some_device(client, some_device, device_ids);
		if (roccat_device)
			return roccat_device;
	}

	return NULL;
}

RoccatDevice *kone_device_first(void) {
	GUdevClient *client;
	GList *list;
	RoccatDevice *roccat_device;

	client = g_udev_client_new(NULL);
	list = g_udev_client_query_by_subsystem(client, driver_name);

	roccat_device = device_list_get_first_wanted(client, list, device_ids);

	g_udev_roccat_device_list_free(list);
	g_object_unref(client);

	return roccat_device;
}
