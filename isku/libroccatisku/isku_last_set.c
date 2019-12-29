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
#include "roccat_helper.h"
#include <string.h>

static guint16 isku_last_set_calc_checksum(IskuLastSet const *last_set) {
	return ROCCAT_BYTESUM_PARTIALLY(last_set, IskuLastSet, report_id, checksum);
}

static void isku_last_set_set_checksum(IskuLastSet *last_set) {
	guint16 checksum = isku_last_set_calc_checksum(last_set);
	last_set->checksum = GUINT16_TO_LE(checksum);
}

static void isku_last_set_finalize(IskuLastSet *last_set) {
	last_set->report_id = ISKU_REPORT_ID_LAST_SET;
	last_set->size = sizeof(IskuLastSet);
	isku_last_set_set_checksum(last_set);
}

guint32 isku_last_set_read(RoccatDevice *isku, GError **error) {
	IskuLastSet *last_set;
	guint32 result;

	last_set = (IskuLastSet *)isku_device_read(isku, ISKU_REPORT_ID_LAST_SET, sizeof(IskuLastSet), error);

	if (*error)
		return 0;

	result = isku_last_set_get_real_time(last_set);
	g_free(last_set);

	return result;
}

gboolean isku_last_set_write(RoccatDevice *isku, GError **error) {
	IskuLastSet last_set;
	GTimeVal time;

	g_get_current_time(&time);

	memset(&last_set, 0, sizeof(IskuLastSet));
	isku_last_set_set_real_time(&last_set, time.tv_sec);
	isku_last_set_finalize(&last_set);

	return isku_device_write(isku, (gchar const *)&last_set, sizeof(IskuLastSet), error);
}
