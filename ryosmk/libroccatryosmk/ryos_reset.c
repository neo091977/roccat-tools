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

#include "ryos_reset.h"
#include "ryos_device.h"

static void ryos_reset_finalize(RyosReset *reset) {
	reset->report_id = RYOS_REPORT_ID_RESET;
	reset->size = sizeof(RyosReset);
}

static gboolean ryos_reset_write(RoccatDevice *ryos, RyosReset *reset, GError **error) {
	ryos_reset_finalize(reset);
	return ryos_device_write(ryos, (gchar const *)reset, sizeof(RyosReset), error);
}

gboolean ryos_reset(RoccatDevice *ryos, guint function, GError **error) {
	RyosReset reset;

	reset.function = function;
	return ryos_reset_write(ryos, &reset, error);
}
