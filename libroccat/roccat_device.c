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

#include <gaminggear/threads.h>
#include <gaminggear/input_events.h>
#include "roccat.h"
#include "roccat_device.h"
#include "roccat_talk.h"
#include "config.h"
#include "i18n-lib.h"
#include <fcntl.h>
#include <errno.h>

#define ROCCAT_DEVICE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), ROCCAT_DEVICE_TYPE, RoccatDeviceClass))
#define IS_ROCCAT_DEVICE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), ROCCAT_DEVICE_TYPE))
#define ROCCAT_DEVICE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), ROCCAT_DEVICE_TYPE, RoccatDevicePrivate))

typedef struct _RoccatDeviceClass RoccatDeviceClass;

struct _RoccatDeviceClass {
	GaminggearDeviceClass parent_class;
};

G_DEFINE_TYPE(RoccatDevice, roccat_device, GAMINGGEAR_DEVICE_TYPE);

static gchar const * const syspath_name = "syspath";
static gchar const * const roccat_name = "roccat";

gchar *create_hidraw_endpoint_name(guint endpoint) {
	return g_strdup_printf("hidraw%u", endpoint);
}

gchar const *roccat_device_get_syspath(RoccatDevice const *roccat_dev) {
	return gaminggear_device_get_path(GAMINGGEAR_DEVICE(roccat_dev), syspath_name);
}

void roccat_device_set_syspath(RoccatDevice *roccat_dev, gchar const *syspath) {
	gaminggear_device_set_path(GAMINGGEAR_DEVICE(roccat_dev), syspath_name, syspath);
}

gchar const *roccat_device_get_roccat_dev(RoccatDevice const *roccat_dev) {
	return gaminggear_device_get_path(GAMINGGEAR_DEVICE(roccat_dev), roccat_name);
}

void roccat_device_set_roccat_dev(RoccatDevice *roccat_dev, gchar const *device) {
	gaminggear_device_set_path(GAMINGGEAR_DEVICE(roccat_dev), roccat_name, device);
}

gchar const *roccat_device_get_hidraw_endpoint_dev(RoccatDevice const *roccat_dev, guint endpoint) {
	g_assert(endpoint < gaminggear_device_get_num_interfaces(GAMINGGEAR_DEVICE(roccat_dev)));
	gchar *name = create_hidraw_endpoint_name(endpoint);
	gchar const *path = gaminggear_device_get_path(GAMINGGEAR_DEVICE(roccat_dev), name);
	g_free(name);
	return path;
}

void roccat_device_set_hidraw_endpoint_dev(RoccatDevice *roccat_dev, gchar const *device, guint endpoint) {
	g_assert(endpoint < gaminggear_device_get_num_interfaces(GAMINGGEAR_DEVICE(roccat_dev)));
	gchar *name = create_hidraw_endpoint_name(endpoint);
	gaminggear_device_set_path(GAMINGGEAR_DEVICE(roccat_dev), name, device);
	g_free(name);
}

int roccat_device_get_hidraw_endpoint(RoccatDevice *roccat_dev, guint endpoint, GError **error) {
	g_assert(endpoint < gaminggear_device_get_num_interfaces(GAMINGGEAR_DEVICE(roccat_dev)));
	gchar *name = create_hidraw_endpoint_name(endpoint);
	int fd = gaminggear_device_open(GAMINGGEAR_DEVICE(roccat_dev), name, O_RDWR, error);
	g_free(name);
	return fd;
}

RoccatDevice *roccat_device_new(gchar const *identifier, guint vendor_id, guint product_id, guint num_interfaces) {
	return ROCCAT_DEVICE(g_object_new(ROCCAT_DEVICE_TYPE,
			"identifier", identifier,
			"vendor_id", vendor_id,
			"product_id", product_id,
			"num-interfaces", num_interfaces,
			NULL));
}

static void roccat_device_init(RoccatDevice *roccat_dev) {
}

static void roccat_device_class_init(RoccatDeviceClass *klass) {
}

void roccat_device_debug(RoccatDevice const *roccat_dev) {
	GaminggearDevice *gaminggear_dev = GAMINGGEAR_DEVICE(roccat_dev);
	guint num_interfaces = gaminggear_device_get_num_interfaces(gaminggear_dev);
	guint i;
	gchar const *path;
	gchar *string;
	
	if (!roccat_dev)
		return;

	g_debug("%s0x%04x:0x%04x (%s)", _("Roccat Device: "), gaminggear_device_get_vendor_id(gaminggear_dev),
			gaminggear_device_get_product_id(gaminggear_dev),
			roccat_device_get_name_static(roccat_dev));
	g_debug("%*s%-*s%s", 2, "", 14, _("Identifier: "), gaminggear_device_get_identifier(gaminggear_dev));
	g_debug("%*s%-*s%s", 2, "", 14, _("Syspath: "), gaminggear_device_get_path(gaminggear_dev, syspath_name));
	g_debug("%*s%-*s%s", 2, "", 14, _("Roccat: "), gaminggear_device_get_path(gaminggear_dev, roccat_name));
	g_debug("%*s%-*s%u", 2, "", 14, _("Interfaces: "), num_interfaces);

	for (i = 0; i < num_interfaces; ++i) {
		path = roccat_device_get_hidraw_endpoint_dev(roccat_dev, i);
		if (path) {
			string = g_strdup_printf(_("Hidraw[%u]: "), i);
			g_debug("%*s%-*s%s", 4, "", 12, string, path);
			g_free(string);
		}
	}
}

gchar const *roccat_device_get_name_static(RoccatDevice const *roccat_dev) {
	return roccat_talk_device_get_text_static(gaminggear_device_get_product_id(GAMINGGEAR_DEVICE(roccat_dev)));
}

