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

#include "kova2016_sensor.h"
#include "kova2016_device.h"

typedef struct _Kova2016Sensor Kova2016Sensor;

struct _Kova2016Sensor {
	guint8 report_id; /* KOVA2016_REPORT_ID_SENSOR */
	guint8 action;
	guint8 reg;
	guint8 value;
} __attribute__ ((packed));

typedef enum {
	KOVA2016_SENSOR_ACTION_WRITE = 1,
	KOVA2016_SENSOR_ACTION_READ = 2,
	KOVA2016_SENSOR_ACTION_FRAME_CAPTURE = 3,
} Kova2016SensorAction;

static gboolean kova2016_sensor_write(RoccatDevice *kova2016, Kova2016Sensor *sensor, GError **error) {
	return kova2016_device_write(kova2016, (gchar const *)sensor, sizeof(Kova2016Sensor), error);
}

static Kova2016Sensor *kova2016_sensor_read(RoccatDevice *kova2016, GError **error) {
	return (Kova2016Sensor *)kova2016_device_read(kova2016, KOVA2016_REPORT_ID_SENSOR, sizeof(Kova2016Sensor), error);
}

static gboolean kova2016_sensor_write_struct(RoccatDevice *kova2016, guint8 action, guint8 reg, guint8 value, GError **error) {
	Kova2016Sensor sensor;

	sensor.report_id = KOVA2016_REPORT_ID_SENSOR;
	sensor.action = action;
	sensor.reg = reg;
	sensor.value = value;

	return kova2016_sensor_write(kova2016, &sensor, error);
}

gboolean kova2016_sensor_write_register(RoccatDevice *kova2016, guint8 reg, guint8 value, GError **error) {
	return kova2016_sensor_write_struct(kova2016, KOVA2016_SENSOR_ACTION_WRITE, reg, value, error);
}

guint8 kova2016_sensor_read_register(RoccatDevice *kova2016, guint8 reg, GError **error) {
	Kova2016Sensor *sensor;
	guint8 result;

	kova2016_sensor_write_struct(kova2016, KOVA2016_SENSOR_ACTION_READ, reg, 0, error);
	if (*error)
		return 0;

	sensor = kova2016_sensor_read(kova2016, error);
	if (*error)
		return 0;
	result = sensor->value;
	g_free(sensor);

	return result;
}
