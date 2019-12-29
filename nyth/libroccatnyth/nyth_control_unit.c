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

#include "nyth_control_unit.h"
#include "nyth_device.h"

NythControlUnit *nyth_control_unit_read(RoccatDevice *nyth, GError **error) {
	return (NythControlUnit *)nyth_device_read(nyth, NYTH_REPORT_ID_CONTROL_UNIT, sizeof(NythControlUnit), error);
}

static gboolean nyth_control_unit_write(RoccatDevice *nyth, NythControlUnit const *data, GError **error) {
	return nyth_device_write(nyth, (char const *)data, sizeof(NythControlUnit), error);
}

static gboolean nyth_tracking_control_unit(RoccatDevice *nyth, guint8 tcu, guint8 action, GError **error) {
	NythControlUnit control_unit;

	control_unit.report_id = NYTH_REPORT_ID_CONTROL_UNIT;
	control_unit.size = sizeof(NythControlUnit);
	control_unit.dcu = 0xff;
	control_unit.tcu = tcu;
	control_unit.unused = 0;
	control_unit.action = action;

	return nyth_control_unit_write(nyth, &control_unit, error);
}

gboolean nyth_tracking_control_unit_start(RoccatDevice *nyth, GError **error) {
	return nyth_tracking_control_unit(nyth, NYTH_TRACKING_CONTROL_UNIT_ON, NYTH_CONTROL_UNIT_ACTION_START, error);
}

gboolean nyth_tracking_control_unit_cancel(RoccatDevice *nyth, GError **error) {
	return nyth_tracking_control_unit(nyth, NYTH_TRACKING_CONTROL_UNIT_ON, NYTH_CONTROL_UNIT_ACTION_CANCEL, error);
}

gboolean nyth_tracking_control_unit_accept(RoccatDevice *nyth, GError **error) {
	return nyth_tracking_control_unit(nyth, NYTH_TRACKING_CONTROL_UNIT_ON, NYTH_CONTROL_UNIT_ACTION_ACCEPT, error);
}

gboolean nyth_tracking_control_unit_off(RoccatDevice *nyth, GError **error) {
	return nyth_tracking_control_unit(nyth, NYTH_TRACKING_CONTROL_UNIT_OFF, NYTH_CONTROL_UNIT_ACTION_OFF, error);
}

guint nyth_distance_control_unit_get(RoccatDevice *nyth, GError **error) {
	NythControlUnit *control_unit;
	guint retval;

	control_unit = nyth_control_unit_read(nyth, error);
	if (!control_unit)
		return 0;

	retval = control_unit->dcu;
	g_free(control_unit);
	return retval;
}

gboolean nyth_distance_control_unit_set(RoccatDevice *nyth, guint new_dcu, GError **error) {
	NythControlUnit control_unit;

	control_unit.report_id = NYTH_REPORT_ID_CONTROL_UNIT;
	control_unit.size = sizeof(NythControlUnit);
	control_unit.dcu = new_dcu;
	control_unit.tcu = 0xff;
	control_unit.unused = 0;
	control_unit.action = 0;

	return nyth_control_unit_write(nyth, &control_unit, error);
}
