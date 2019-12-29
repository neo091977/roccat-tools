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

#include "kova2016_control_unit.h"
#include "kova2016_device.h"

Kova2016ControlUnit *kova2016_control_unit_read(RoccatDevice *kova2016, GError **error) {
	return (Kova2016ControlUnit *)kova2016_device_read(kova2016, KOVA2016_REPORT_ID_CONTROL_UNIT, sizeof(Kova2016ControlUnit), error);
}

static gboolean kova2016_control_unit_write(RoccatDevice *kova2016, Kova2016ControlUnit const *data, GError **error) {
	return kova2016_device_write(kova2016, (char const *)data, sizeof(Kova2016ControlUnit), error);
}

static gboolean kova2016_tracking_control_unit(RoccatDevice *kova2016, guint8 tcu, guint8 action, GError **error) {
	Kova2016ControlUnit control_unit;

	control_unit.report_id = KOVA2016_REPORT_ID_CONTROL_UNIT;
	control_unit.size = sizeof(Kova2016ControlUnit);
	control_unit.dcu = 0xff;
	control_unit.tcu = tcu;
	control_unit.unused = 0;
	control_unit.action = action;

	return kova2016_control_unit_write(kova2016, &control_unit, error);
}

gboolean kova2016_tracking_control_unit_start(RoccatDevice *kova2016, GError **error) {
	return kova2016_tracking_control_unit(kova2016, KOVA2016_TRACKING_CONTROL_UNIT_ON, KOVA2016_CONTROL_UNIT_ACTION_START, error);
}

gboolean kova2016_tracking_control_unit_cancel(RoccatDevice *kova2016, GError **error) {
	return kova2016_tracking_control_unit(kova2016, KOVA2016_TRACKING_CONTROL_UNIT_ON, KOVA2016_CONTROL_UNIT_ACTION_CANCEL, error);
}

gboolean kova2016_tracking_control_unit_accept(RoccatDevice *kova2016, GError **error) {
	return kova2016_tracking_control_unit(kova2016, KOVA2016_TRACKING_CONTROL_UNIT_ON, KOVA2016_CONTROL_UNIT_ACTION_ACCEPT, error);
}

gboolean kova2016_tracking_control_unit_off(RoccatDevice *kova2016, GError **error) {
	return kova2016_tracking_control_unit(kova2016, KOVA2016_TRACKING_CONTROL_UNIT_OFF, KOVA2016_CONTROL_UNIT_ACTION_OFF, error);
}

guint kova2016_distance_control_unit_get(RoccatDevice *kova2016, GError **error) {
	Kova2016ControlUnit *control_unit;
	guint retval;

	control_unit = kova2016_control_unit_read(kova2016, error);
	if (!control_unit)
		return 0;

	retval = control_unit->dcu;
	g_free(control_unit);
	return retval;
}

gboolean kova2016_distance_control_unit_set(RoccatDevice *kova2016, guint new_dcu, GError **error) {
	Kova2016ControlUnit control_unit;

	control_unit.report_id = KOVA2016_REPORT_ID_CONTROL_UNIT;
	control_unit.size = sizeof(Kova2016ControlUnit);
	control_unit.dcu = new_dcu;
	control_unit.tcu = 0xff;
	control_unit.unused = 0;
	control_unit.action = 0;

	return kova2016_control_unit_write(kova2016, &control_unit, error);
}
