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

#include "isku.h"

static void isku_reset_finalize(IskuReset *reset) {
	reset->report_id = ISKU_REPORT_ID_RESET;
	reset->size = sizeof(IskuReset);
}

static gboolean isku_reset_write(RoccatDevice *isku, IskuReset *reset, GError **error) {
	isku_reset_finalize(reset);
	return isku_device_write(isku, (gchar const *)reset, sizeof(IskuReset), error);
}

gboolean isku_reset(RoccatDevice *isku, guint function, GError **error) {
	IskuReset reset;
	gboolean retval;

	reset.function = function;
	retval = isku_reset_write(isku, &reset, error);
	if (retval == FALSE)
		return FALSE;

	return isku_last_set_write(isku, error);
}
