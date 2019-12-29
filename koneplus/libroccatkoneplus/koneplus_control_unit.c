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

KoneplusControlUnit *koneplus_control_unit_read(RoccatDevice *koneplus, GError **error) {
	return (KoneplusControlUnit *)koneplus_device_read(koneplus, KONEPLUS_REPORT_ID_CONTROL_UNIT, sizeof(KoneplusControlUnit), error);
}

static gboolean koneplus_control_unit_write(RoccatDevice *koneplus, KoneplusControlUnit const *data, GError **error) {
	return koneplus_device_write(koneplus, (char const *)data, sizeof(KoneplusControlUnit), error);
}

gboolean koneplus_tracking_control_unit_test(RoccatDevice *koneplus, guint dcu, guint median, GError **error) {
	KoneplusControlUnit control_unit;

	control_unit.report_id = KONEPLUS_REPORT_ID_CONTROL_UNIT;
	control_unit.size = sizeof(KoneplusControlUnit);
	control_unit.dcu = dcu;
	control_unit.tcu = KONEPLUS_TRACKING_CONTROL_UNIT_ON;
	control_unit.median = median;
	control_unit.action = KONEPLUS_CONTROL_UNIT_ACTION_CANCEL;

	return koneplus_control_unit_write(koneplus, &control_unit, error);
}

gboolean koneplus_tracking_control_unit_cancel(RoccatDevice *koneplus, guint dcu, GError **error) {
	return koneplus_tracking_control_unit_test(koneplus, dcu, 0, error);
}

gboolean koneplus_tracking_control_unit_accept(RoccatDevice *koneplus, guint dcu, guint median, GError **error) {
	KoneplusControlUnit control_unit;

	control_unit.report_id = KONEPLUS_REPORT_ID_CONTROL_UNIT;
	control_unit.size = sizeof(KoneplusControlUnit);
	control_unit.dcu = dcu;
	control_unit.tcu = KONEPLUS_TRACKING_CONTROL_UNIT_ON;
	control_unit.median = median;
	control_unit.action = KONEPLUS_CONTROL_UNIT_ACTION_ACCEPT;

	return koneplus_control_unit_write(koneplus, &control_unit, error);
}

gboolean koneplus_tracking_control_unit_off(RoccatDevice *koneplus, guint dcu, GError **error) {
	KoneplusControlUnit control_unit;

	control_unit.report_id = KONEPLUS_REPORT_ID_CONTROL_UNIT;
	control_unit.size = sizeof(KoneplusControlUnit);
	control_unit.dcu = dcu;
	control_unit.tcu = KONEPLUS_TRACKING_CONTROL_UNIT_OFF;
	control_unit.median = 0;
	control_unit.action = KONEPLUS_CONTROL_UNIT_ACTION_OFF;

	return koneplus_control_unit_write(koneplus, &control_unit, error);
}

guint koneplus_distance_control_unit_get(RoccatDevice *koneplus, GError **error) {
	KoneplusControlUnit *control_unit;
	guint retval;

	control_unit = koneplus_control_unit_read(koneplus, error);
	if (!control_unit)
		return 0;

	retval = control_unit->dcu;
	g_free(control_unit);
	return retval;
}

gboolean koneplus_distance_control_unit_try(RoccatDevice *koneplus, guint new_dcu, GError **error) {
	KoneplusControlUnit control_unit;

	control_unit.report_id = KONEPLUS_REPORT_ID_CONTROL_UNIT;
	control_unit.size = sizeof(KoneplusControlUnit);
	control_unit.dcu = new_dcu;
	control_unit.tcu = 0xff;
	control_unit.median = 0xff;
	control_unit.action = KONEPLUS_CONTROL_UNIT_ACTION_UNDEFINED;

	return koneplus_control_unit_write(koneplus, &control_unit, error);
}

gboolean koneplus_distance_control_unit_cancel(RoccatDevice *koneplus, guint old_dcu, GError **error) {
	KoneplusControlUnit control_unit;

	control_unit.report_id = KONEPLUS_REPORT_ID_CONTROL_UNIT;
	control_unit.size = sizeof(KoneplusControlUnit);
	control_unit.dcu = old_dcu;
	control_unit.tcu = 0xff;
	control_unit.median = 0xff;
	control_unit.action = KONEPLUS_CONTROL_UNIT_ACTION_CANCEL;

	return koneplus_control_unit_write(koneplus, &control_unit, error);
}

gboolean koneplus_distance_control_unit_accept(RoccatDevice *koneplus, guint new_dcu, GError **error) {
	KoneplusControlUnit control_unit;

	control_unit.report_id = KONEPLUS_REPORT_ID_CONTROL_UNIT;
	control_unit.size = sizeof(KoneplusControlUnit);
	control_unit.dcu = new_dcu;
	control_unit.tcu = 0xff;
	control_unit.median = 0xff;
	control_unit.action = KONEPLUS_CONTROL_UNIT_ACTION_ACCEPT;

	return koneplus_control_unit_write(koneplus, &control_unit, error);
}
