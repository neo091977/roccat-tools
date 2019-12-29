#ifndef __ROCCAT_DEVICE_H__
#define __ROCCAT_DEVICE_H__

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

#include <gaminggear/gaminggear_device.h>

G_BEGIN_DECLS

#define ROCCAT_DEVICE_TYPE (roccat_device_get_type())
#define ROCCAT_DEVICE(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), ROCCAT_DEVICE_TYPE, RoccatDevice))
#define IS_ROCCAT_DEVICE(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), ROCCAT_DEVICE_TYPE))

typedef struct _RoccatDevice RoccatDevice;
typedef struct _RoccatDevicePrivate RoccatDevicePrivate;

struct _RoccatDevice {
	GaminggearDevice parent;
};

GType roccat_device_get_type(void);
RoccatDevice *roccat_device_new(gchar const *identifier, guint vendor_id, guint product_id, guint num_interfaces);

gchar const *roccat_device_get_syspath(RoccatDevice const *roccat_dev);
void roccat_device_set_syspath(RoccatDevice *roccat_dev, gchar const *syspath);

gchar const *roccat_device_get_roccat_dev(RoccatDevice const *roccat_dev);
void roccat_device_set_roccat_dev(RoccatDevice *roccat_dev, gchar const *device);

gchar const *roccat_device_get_hidraw_endpoint_dev(RoccatDevice const *roccat_dev, guint endpoint);
void roccat_device_set_hidraw_endpoint_dev(RoccatDevice *roccat_dev, gchar const *device, guint endpoint);

/*!
 * \brief opens hidraw device for reading
 *
 * hidraw stays open until roccat_dev gets finalized
 *
 * \retval If successful returns file descriptor that has to be closed with
 *         \c close(), -1 on error
 */
int roccat_device_get_hidraw_endpoint(RoccatDevice *roccat_dev, guint endpoint, GError **error);

void roccat_device_debug(RoccatDevice const *roccat_dev);

gchar const *roccat_device_get_name_static(RoccatDevice const *roccat_dev);

G_END_DECLS

#endif
