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

#include "koneplus.h"
#include <unistd.h>

static gboolean koneplus_sensor_write(RoccatDevice *koneplus, KoneplusSensor *sensor, GError **error) {
	return koneplus_device_write(koneplus, (gchar const *)sensor, sizeof(KoneplusSensor), error);
}

static KoneplusSensor *koneplus_sensor_read(RoccatDevice *koneplus, GError **error) {
	return (KoneplusSensor *)koneplus_device_read(koneplus, KONEPLUS_REPORT_ID_SENSOR, sizeof(KoneplusSensor), error);
}

KoneplusSensorImage *koneplus_sensor_image_read(RoccatDevice *koneplus, GError **error) {
	return (KoneplusSensorImage *)koneplus_device_read(koneplus, KONEPLUS_REPORT_ID_SENSOR, sizeof(KoneplusSensorImage), error);
}

static gboolean koneplus_sensor_write_struct(RoccatDevice *koneplus, guint8 action, guint8 reg, guint8 value, GError **error) {
	KoneplusSensor sensor;

	sensor.report_id = KONEPLUS_REPORT_ID_SENSOR;
	sensor.action = action;
	sensor.reg = reg;
	sensor.value = value;

	return koneplus_sensor_write(koneplus, &sensor, error);
}

gboolean koneplus_sensor_calibrate_step(RoccatDevice *koneplus, GError **error) {
	return koneplus_sensor_write_struct(koneplus, KONEPLUS_SENSOR_ACTION_FRAME_CAPTURE, 1, 0, error);
}

guint koneplus_sensor_image_get_median(KoneplusSensorImage const *image) {
	guint i;
	gulong sum = 0;
	for (i = 0; i < KONEPLUS_TCU_IMAGE_SIZE * KONEPLUS_TCU_IMAGE_SIZE; ++i)
		sum += image->data[i];

	return sum / (KONEPLUS_TCU_IMAGE_SIZE * KONEPLUS_TCU_IMAGE_SIZE);
}

gboolean koneplus_sensor_write_register(RoccatDevice *koneplus, guint8 reg, guint8 value, GError **error) {
	return koneplus_sensor_write_struct(koneplus, KONEPLUS_SENSOR_ACTION_WRITE, reg, value, error);
}

guint8 koneplus_sensor_read_register(RoccatDevice *koneplus, guint8 reg, GError **error) {
	KoneplusSensor *sensor;
	guint8 result;

	koneplus_sensor_write_struct(koneplus, KONEPLUS_SENSOR_ACTION_READ, reg, 0, error);
	if (*error)
		return 0;

	sensor = koneplus_sensor_read(koneplus, error);
	if (*error)
		return 0;
	result = sensor->value;
	g_free(sensor);

	return result;
}
