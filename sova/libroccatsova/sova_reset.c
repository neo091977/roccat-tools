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

#include "sova_reset.h"
#include "sova.h"

static void sova_reset_finalize(SovaReset *reset) {
	reset->report_id = SOVA_REPORT_ID_RESET;
	reset->size = sizeof(SovaReset);
}

static gboolean sova_reset_write(RoccatDevice *sova, SovaReset *reset, GError **error) {
	sova_reset_finalize(reset);
	return sova_device_write(sova, (gchar const *)reset, sizeof(SovaReset), error);
}

gboolean sova_reset(RoccatDevice *sova, guint function, GError **error) {
	SovaReset reset;

	reset.function = function;
	return sova_reset_write(sova, &reset, error);
}
