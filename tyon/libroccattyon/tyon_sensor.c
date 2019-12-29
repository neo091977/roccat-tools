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

#include "tyon_sensor.h"
#include "tyon_device.h"

typedef struct _TyonSensor TyonSensor;

struct _TyonSensor {
	guint8 report_id; /* TYON_REPORT_ID_SENSOR */
	guint8 action;
	guint8 reg;
	guint8 value;
} __attribute__ ((packed));

typedef enum {
	TYON_SENSOR_ACTION_WRITE = 1,
	TYON_SENSOR_ACTION_READ = 2,
	TYON_SENSOR_ACTION_FRAME_CAPTURE = 3,
} TyonSensorAction;

static TyonSensor *tyon_sensor_read(RoccatDevice *device, GError **error) {
	return (TyonSensor *)tyon_device_read(device, TYON_REPORT_ID_SENSOR, sizeof(TyonSensor), error);
}

static gboolean tyon_sensor_write(RoccatDevice *device, TyonSensor *sensor, GError **error) {
	return tyon_device_write(device, (gchar const *)sensor, sizeof(TyonSensor), error);
}

TyonSensorImage *tyon_sensor_image_read(RoccatDevice *device, GError **error) {
	return (TyonSensorImage *)tyon_device_read(device, TYON_REPORT_ID_SENSOR, sizeof(TyonSensorImage), error);
}

static gboolean tyon_sensor_write_struct(RoccatDevice *device, guint8 action, guint8 reg, guint8 value, GError **error) {
	TyonSensor sensor;

	sensor.report_id = TYON_REPORT_ID_SENSOR;
	sensor.action = action;
	sensor.reg = reg;
	sensor.value = value;

	return tyon_sensor_write(device, &sensor, error);
}

gboolean tyon_sensor_write_register(RoccatDevice *device, guint8 reg, guint8 value, GError **error) {
	return tyon_sensor_write_struct(device, TYON_SENSOR_ACTION_WRITE, reg, value, error);
}

guint8 tyon_sensor_read_register(RoccatDevice *device, guint8 reg, GError **error) {
	TyonSensor *sensor;
	guint8 result;

	tyon_sensor_write_struct(device, TYON_SENSOR_ACTION_READ, reg, 0, error);
	if (*error)
		return 0;

	sensor = tyon_sensor_read(device, error);
	if (*error)
		return 0;
	result = sensor->value;
	g_free(sensor);

	return result;
}

gboolean tyon_sensor_calibrate_step(RoccatDevice *device, GError **error) {
	return tyon_sensor_write_struct(device, TYON_SENSOR_ACTION_FRAME_CAPTURE, 1, 0, error);
}

guint tyon_sensor_image_get_median(TyonSensorImage const *image) {
	guint i;
	gulong sum = 0;
	for (i = 0; i < TYON_SENSOR_IMAGE_SIZE * TYON_SENSOR_IMAGE_SIZE; ++i)
		sum += image->data[i];

	return sum / (TYON_SENSOR_IMAGE_SIZE * TYON_SENSOR_IMAGE_SIZE);
}

