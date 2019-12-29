#ifndef __ROCCAT_KONE_DEVICE_SCANNER_H__
#define __ROCCAT_KONE_DEVICE_SCANNER_H__

/*
 * This file is part of kone-tools.
 *
 * kone-tools is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * kone-tools is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with kone-tools. If not, see <http://www.gnu.org/licenses/>.
 */

#include "roccat_device_scanner_interface.h"
#include <glib-object.h>

G_BEGIN_DECLS

#define KONE_DEVICE_SCANNER_TYPE (kone_device_scanner_get_type())
#define KONE_DEVICE_SCANNER(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), KONE_DEVICE_SCANNER_TYPE, KoneDeviceScanner))
#define IS_KONE_DEVICE_SCANNER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), KONE_DEVICE_SCANNER_TYPE))

typedef struct _KoneDeviceScanner KoneDeviceScanner;
typedef struct _KoneDeviceScannerPrivate KoneDeviceScannerPrivate;

struct _KoneDeviceScanner {
	GObject parent;
	KoneDeviceScannerPrivate *priv;
};

GType kone_device_scanner_get_type(void);
KoneDeviceScanner *kone_device_scanner_new(void);

RoccatDevice *kone_device_first(void);

G_END_DECLS

#endif
