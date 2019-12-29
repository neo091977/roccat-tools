#ifndef __ROCCAT_TYON_SENSOR_H__
#define __ROCCAT_TYON_SENSOR_H__

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

enum {
	TYON_SENSOR_IMAGE_SIZE = 30,
};

typedef struct _TyonSensorImage TyonSensorImage;

struct _TyonSensorImage {
	guint8 report_id; /* TYON_REPORT_ID_SENSOR */
	guint8 action; /* 3 */
	guint8 unused1; /* 0 */
	guint8 data[TYON_SENSOR_IMAGE_SIZE * TYON_SENSOR_IMAGE_SIZE];
	guint8 unused2[125]; /* all bytes 0 */
} __attribute__ ((packed));

gboolean tyon_sensor_write_register(RoccatDevice *device, guint8 reg, guint8 value, GError **error);
guint8 tyon_sensor_read_register(RoccatDevice *device, guint8 reg, GError **error);

TyonSensorImage *tyon_sensor_image_read(RoccatDevice *device, GError **error);
gboolean tyon_sensor_calibrate_step(RoccatDevice *device, GError **error);
guint tyon_sensor_image_get_median(TyonSensorImage const *image);

G_END_DECLS

#endif
