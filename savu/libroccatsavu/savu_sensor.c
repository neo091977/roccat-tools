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

#include "savu.h"

static SavuSensor *savu_sensor_read(RoccatDevice *savu, GError **error) {
	return (SavuSensor *)savu_device_read(savu, SAVU_REPORT_ID_SENSOR, sizeof(SavuSensor), error);
}

static gboolean savu_sensor_write(RoccatDevice *savu, SavuSensor *sensor, GError **error) {
	return savu_device_write(savu, (gchar const *)sensor, sizeof(SavuSensor), error);
}

static gboolean savu_sensor_write_struct(RoccatDevice *savu, guint8 action, guint8 reg, guint8 value, GError **error) {
	SavuSensor sensor;

	sensor.report_id = SAVU_REPORT_ID_SENSOR;
	sensor.action = action;
	sensor.reg = reg;
	sensor.value = value;

	return savu_sensor_write(savu, &sensor, error);
}

gboolean savu_sensor_write_value(RoccatDevice *savu, guint8 reg, guint8 value, GError **error) {
	return savu_sensor_write_struct(savu, SAVU_SENSOR_ACTION_WRITE, reg, value, error);
}

guint8 savu_sensor_read_value(RoccatDevice *savu, guint8 reg, GError **error) {
	SavuSensor *sensor;
	guint8 result;

	savu_sensor_write_struct(savu, SAVU_SENSOR_ACTION_READ, reg, 0, error);
	if (*error)
		return 0;

	sensor = savu_sensor_read(savu, error);
	if (*error)
		return 0;
	result = sensor->value;
	g_free(sensor);

	return result;
}
