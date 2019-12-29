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

#include "roccat_device_scanner.h"
#include "roccat.h"
#include "roccat_device.h"
#include "g_udev_roccat_helper.h"
#include "g_roccat_helper.h"
#include "i18n-lib.h"
#include <string.h>
#include <unistd.h>

#define ROCCAT_DEVICE_SCANNER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), ROCCAT_DEVICE_SCANNER_TYPE, RoccatDeviceScannerClass))
#define IS_ROCCAT_DEVICE_SCANNER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), ROCCAT_DEVICE_SCANNER_TYPE))
#define ROCCAT_DEVICE_SCANNER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), ROCCAT_DEVICE_SCANNER_TYPE, RoccatDeviceScannerPrivate))

typedef struct _RoccatDeviceScannerClass RoccatDeviceScannerClass;

struct _RoccatDeviceScannerClass {
	GObjectClass parent_class;
};

struct _RoccatDeviceScannerPrivate {
	GUdevClient *client;
	gulong handler;
	guint const *product_ids;
};

static gchar const * const hidraw_subsystem = "hidraw";
static gchar const * const usb_subsystem = "usb";

static gint get_num_interface(GUdevDevice *device) {
	GUdevDevice *usbhid_device;
	gint num_interface;

	usbhid_device = g_udev_roccat_get_parent_usbhid_device(device);
	if (!usbhid_device)
		return -1;

	num_interface = g_udev_roccat_usbhid_get_interfacenumber(usbhid_device);
	g_object_unref(usbhid_device);

	return num_interface;
}

static gboolean product_id_fits(GUdevDevice *usb_device, guint const *product_ids) {
	guint product_id;

	product_id = g_udev_roccat_usb_get_product_id(usb_device);

	while (*product_ids) {
		if (product_id == *product_ids)
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

static gchar const *get_identifier(GUdevDevice *usb_device) {
	return g_udev_device_get_name(usb_device);
}

static void fill_hidraw(GUdevClient *client, GUdevDevice *usb_device, RoccatDevice *roccat_device) {
	GList *list;
	GList *iterator;
	GUdevDevice *hidraw_device;
	gint num_interface;

	/* Finding the hidraws and inputs is racy. Give them some time to appear. */
	g_usleep(100 * G_ROCCAT_USEC_PER_MSEC);

	list = g_udev_client_query_by_subsystem(client, hidraw_subsystem);

	for (iterator = g_list_first(list); iterator; iterator = g_list_next(iterator)) {
		hidraw_device = (GUdevDevice *)iterator->data;

		if (!g_udev_roccat_device_is_sibling(usb_device, hidraw_device))
			continue;

		num_interface = get_num_interface(hidraw_device);
		if (num_interface == -1) {
			g_warning(_("Could not get interface number"));
			continue;
		}

		roccat_device_set_hidraw_endpoint_dev(roccat_device,
				g_udev_device_get_device_file(hidraw_device),
				num_interface);
	}

	g_udev_roccat_device_list_free(list);
}

static RoccatDevice *roccat_device_from_usb_device(GUdevClient *client, GUdevDevice *usb_device) {
	RoccatDevice *roccat_device = roccat_device_new(get_identifier(usb_device),
			g_udev_roccat_usb_get_vendor_id(usb_device),
			g_udev_roccat_usb_get_product_id(usb_device),
			g_udev_roccat_usb_get_num_interfaces(usb_device));

	fill_hidraw(client, usb_device, roccat_device);

	return roccat_device;
}

static void handle_add(RoccatDeviceScanner *device_scanner, GUdevClient *client, GUdevDevice *usb_device, gboolean just_plugged_in) {
	RoccatDevice *roccat_device;

	if (!usb_device_fits(usb_device, device_scanner->priv->product_ids))
		return;

	roccat_device = roccat_device_from_usb_device(client, usb_device);

	if (just_plugged_in)
		sleep(1);

	roccat_device_scanner_interface_device_added(ROCCAT_DEVICE_SCANNER_INTERFACE(device_scanner), roccat_device);
	g_object_unref(G_OBJECT(roccat_device));
}

static void handle_remove(RoccatDeviceScanner *device_scanner, GUdevDevice *usb_device) {
	RoccatDevice *roccat_device;
	/*
	 * Sending more events than needed.
	 * Not testing on vendor/product id because these values aren't
	 * available anymore after firmware update.
	 */

	roccat_device = roccat_device_new(get_identifier(usb_device), 0, 0, 0);

	roccat_device_scanner_interface_device_removed(ROCCAT_DEVICE_SCANNER_INTERFACE(device_scanner), roccat_device);
	g_object_unref(G_OBJECT(roccat_device));
}

static void uevent_cb(GUdevClient *client, gchar const *action, GUdevDevice *usb_device, gpointer user_data) {
	RoccatDeviceScanner *device_scanner = ROCCAT_DEVICE_SCANNER(user_data);

	if (g_strcmp0(action, G_UDEV_ROCCAT_DEVICE_ACTION_ADD) == 0)
		handle_add(device_scanner, client, usb_device, TRUE);
	else if (g_strcmp0(action, G_UDEV_ROCCAT_DEVICE_ACTION_REMOVE) == 0)
		handle_remove(device_scanner, usb_device);
}

static void iterate_existing(RoccatDeviceScanner *device_scanner) {
	GUdevClient *client;
	GList *list;
	GList *iterator;

	client = g_udev_client_new(NULL);
	list = g_udev_client_query_by_subsystem(client, usb_subsystem);

	for (iterator = g_list_first(list); iterator; iterator = g_list_next(iterator))
		handle_add(device_scanner, client, (GUdevDevice *)iterator->data, FALSE);

	g_udev_roccat_device_list_free(list);
	g_object_unref(client);
}

static void roccat_device_scanner_start(RoccatDeviceScannerInterface *self) {
	RoccatDeviceScanner *device_scanner = ROCCAT_DEVICE_SCANNER(self);
	RoccatDeviceScannerPrivate *priv = device_scanner->priv;
	priv->handler = g_signal_connect(G_OBJECT(priv->client), "uevent", G_CALLBACK(uevent_cb), device_scanner);
	iterate_existing(device_scanner);
}

static void roccat_device_scanner_stop(RoccatDeviceScannerInterface *self) {
	RoccatDeviceScanner *device_scanner = ROCCAT_DEVICE_SCANNER(self);
	RoccatDeviceScannerPrivate *priv = device_scanner->priv;
	g_signal_handler_disconnect(G_OBJECT(priv->client), priv->handler);
}

static void roccat_device_scanner_interface_init(RoccatDeviceScannerInterfaceInterface *iface) {
	iface->start = roccat_device_scanner_start;
	iface->stop = roccat_device_scanner_stop;
}

G_DEFINE_TYPE_WITH_CODE(RoccatDeviceScanner, roccat_device_scanner, G_TYPE_OBJECT,
		G_IMPLEMENT_INTERFACE(ROCCAT_DEVICE_SCANNER_INTERFACE_TYPE, roccat_device_scanner_interface_init));

RoccatDeviceScanner *roccat_device_scanner_new(guint const *product_ids) {
	RoccatDeviceScanner *device_scanner;
	RoccatDeviceScannerPrivate *priv;
	gchar const * const subsystems[] = { "usb/usb_device", NULL };

	device_scanner = ROCCAT_DEVICE_SCANNER(g_object_new(ROCCAT_DEVICE_SCANNER_TYPE, NULL));
	priv = device_scanner->priv;

	priv->client = g_udev_client_new(subsystems);
	priv->product_ids = product_ids;

	return device_scanner;
}

static void roccat_device_scanner_init(RoccatDeviceScanner *device_scanner) {
	RoccatDeviceScannerPrivate *priv = ROCCAT_DEVICE_SCANNER_GET_PRIVATE(device_scanner);
	device_scanner->priv = priv;
}

static void roccat_device_scanner_dispose(GObject *object) {
	RoccatDeviceScannerPrivate *priv = ROCCAT_DEVICE_SCANNER(object)->priv;
	g_clear_object(&priv->client);
	G_OBJECT_CLASS(roccat_device_scanner_parent_class)->dispose(object);
}

static void roccat_device_scanner_class_init(RoccatDeviceScannerClass *klass) {
	GObjectClass *gobject_class;

	gobject_class = (GObjectClass*)klass;
	gobject_class->dispose = roccat_device_scanner_dispose;

	g_type_class_add_private(klass, sizeof(RoccatDeviceScannerPrivate));
}

static RoccatDevice *device_list_get_first_by_product_id(GUdevClient *client, GList *list, guint const *product_ids) {
	GList *iterator;
	GUdevDevice *usb_device;

	for (iterator = g_list_first(list); iterator; iterator = g_list_next(iterator)) {
		usb_device = (GUdevDevice *)iterator->data;

		if (usb_device_fits(usb_device, product_ids))
			return roccat_device_from_usb_device(client, usb_device);
	}

	return NULL;
}

RoccatDevice *roccat_device_first(guint const *product_ids) {
	GUdevClient *client;
	GList *list;
	RoccatDevice *roccat_device;

	client = g_udev_client_new(NULL);
	list = g_udev_client_query_by_subsystem(client, usb_subsystem);

	roccat_device = device_list_get_first_by_product_id(client, list, product_ids);
	if (roccat_device)
		roccat_device_debug(roccat_device);

	g_udev_roccat_device_list_free(list);
	g_object_unref(client);

	return roccat_device;
}

gboolean roccat_device_reenumerate(RoccatDevice *roccat_device) {
	GUdevClient *client;
	GUdevDevice *usb_device;
	GList *list;
	GList *iterator;
	gboolean retval = FALSE;

	client = g_udev_client_new(NULL);
	list = g_udev_client_query_by_subsystem(client, usb_subsystem);

	for (iterator = g_list_first(list); iterator; iterator = g_list_next(iterator)) {
		usb_device = (GUdevDevice *)iterator->data;
		if (!strcmp(gaminggear_device_get_identifier(GAMINGGEAR_DEVICE(roccat_device)), get_identifier(usb_device))) {
			fill_hidraw(client, usb_device, roccat_device);
			roccat_device_debug(roccat_device);
			retval = TRUE;
			break;
		}
	}

	g_udev_roccat_device_list_free(list);
	g_object_unref(client);

	return retval;
}
