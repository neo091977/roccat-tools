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

#include "tyon_control_unit.h"
#include "tyon_device.h"

TyonControlUnit *tyon_control_unit_read(RoccatDevice *tyon, GError **error) {
	return (TyonControlUnit *)tyon_device_read(tyon, TYON_REPORT_ID_CONTROL_UNIT, sizeof(TyonControlUnit), error);
}

static gboolean tyon_control_unit_write(RoccatDevice *tyon, TyonControlUnit const *data, GError **error) {
	return tyon_device_write(tyon, (char const *)data, sizeof(TyonControlUnit), error);
}

gboolean tyon_tracking_control_unit_test(RoccatDevice *tyon, guint dcu, guint median, GError **error) {
	TyonControlUnit control_unit;

	control_unit.report_id = TYON_REPORT_ID_CONTROL_UNIT;
	control_unit.size = sizeof(TyonControlUnit);
	control_unit.dcu = dcu;
	control_unit.tcu = TYON_TRACKING_CONTROL_UNIT_ON;
	control_unit.median = median;
	control_unit.action = TYON_CONTROL_UNIT_ACTION_CANCEL;

	return tyon_control_unit_write(tyon, &control_unit, error);
}

gboolean tyon_tracking_control_unit_cancel(RoccatDevice *tyon, guint dcu, GError **error) {
	return tyon_tracking_control_unit_test(tyon, dcu, 0, error);
}

gboolean tyon_tracking_control_unit_accept(RoccatDevice *tyon, guint dcu, guint median, GError **error) {
	TyonControlUnit control_unit;

	control_unit.report_id = TYON_REPORT_ID_CONTROL_UNIT;
	control_unit.size = sizeof(TyonControlUnit);
	control_unit.dcu = dcu;
	control_unit.tcu = TYON_TRACKING_CONTROL_UNIT_ON;
	control_unit.median = median;
	control_unit.action = TYON_CONTROL_UNIT_ACTION_ACCEPT;

	return tyon_control_unit_write(tyon, &control_unit, error);
}

gboolean tyon_tracking_control_unit_off(RoccatDevice *tyon, guint dcu, GError **error) {
	TyonControlUnit control_unit;

	control_unit.report_id = TYON_REPORT_ID_CONTROL_UNIT;
	control_unit.size = sizeof(TyonControlUnit);
	control_unit.dcu = dcu;
	control_unit.tcu = TYON_TRACKING_CONTROL_UNIT_OFF;
	control_unit.median = 0;
	control_unit.action = TYON_CONTROL_UNIT_ACTION_OFF;

	return tyon_control_unit_write(tyon, &control_unit, error);
}

guint tyon_distance_control_unit_get(RoccatDevice *tyon, GError **error) {
	TyonControlUnit *control_unit;
	guint retval;

	control_unit = tyon_control_unit_read(tyon, error);
	if (!control_unit)
		return 0;

	retval = control_unit->dcu;
	g_free(control_unit);
	return retval;
}

gboolean tyon_distance_control_unit_try(RoccatDevice *tyon, guint new_dcu, GError **error) {
	TyonControlUnit control_unit;

	control_unit.report_id = TYON_REPORT_ID_CONTROL_UNIT;
	control_unit.size = sizeof(TyonControlUnit);
	control_unit.dcu = new_dcu;
	control_unit.tcu = 0xff;
	control_unit.median = 0xff;
	control_unit.action = TYON_CONTROL_UNIT_ACTION_UNDEFINED;

	return tyon_control_unit_write(tyon, &control_unit, error);
}

gboolean tyon_distance_control_unit_cancel(RoccatDevice *tyon, guint old_dcu, GError **error) {
	TyonControlUnit control_unit;

	control_unit.report_id = TYON_REPORT_ID_CONTROL_UNIT;
	control_unit.size = sizeof(TyonControlUnit);
	control_unit.dcu = old_dcu;
	control_unit.tcu = 0xff;
	control_unit.median = 0xff;
	control_unit.action = TYON_CONTROL_UNIT_ACTION_CANCEL;

	return tyon_control_unit_write(tyon, &control_unit, error);
}

gboolean tyon_distance_control_unit_accept(RoccatDevice *tyon, guint new_dcu, GError **error) {
	TyonControlUnit control_unit;

	control_unit.report_id = TYON_REPORT_ID_CONTROL_UNIT;
	control_unit.size = sizeof(TyonControlUnit);
	control_unit.dcu = new_dcu;
	control_unit.tcu = 0xff;
	control_unit.median = 0xff;
	control_unit.action = TYON_CONTROL_UNIT_ACTION_ACCEPT;

	return tyon_control_unit_write(tyon, &control_unit, error);
}
