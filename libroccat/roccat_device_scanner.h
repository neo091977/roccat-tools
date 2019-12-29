#ifndef __ROCCAT_DEVICE_SCANNER_H__
#define __ROCCAT_DEVICE_SCANNER_H__

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
#include <glib-object.h>

G_BEGIN_DECLS

#define ROCCAT_DEVICE_SCANNER_TYPE (roccat_device_scanner_get_type())
#define ROCCAT_DEVICE_SCANNER(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), ROCCAT_DEVICE_SCANNER_TYPE, RoccatDeviceScanner))
#define IS_ROCCAT_DEVICE_SCANNER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), ROCCAT_DEVICE_SCANNER_TYPE))

typedef struct _RoccatDeviceScanner RoccatDeviceScanner;
typedef struct _RoccatDeviceScannerPrivate RoccatDeviceScannerPrivate;

struct _RoccatDeviceScanner {
	GObject parent;
	RoccatDeviceScannerPrivate *priv;
};

GType roccat_device_scanner_get_type(void);

/* product_ids have to be persistent. */

RoccatDeviceScanner *roccat_device_scanner_new(guint const *product_ids);

RoccatDevice *roccat_device_first(guint const *product_ids);
gboolean roccat_device_reenumerate(RoccatDevice *old_device);

G_END_DECLS

#endif
