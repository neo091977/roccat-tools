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

#include "sova_talk.h"
#include "sova_device.h"
#include "sova.h"
#include <string.h>

static gboolean sova_talk_write(RoccatDevice *sova, SovaTalk *talk, GError **error) {
	talk->report_id = SOVA_REPORT_ID_TALK;
	talk->size = sizeof(SovaTalk);

	return sova_device_write(sova, (gchar const *)talk, sizeof(SovaTalk), error);
}

gboolean sova_talk_write_easyshift(RoccatDevice *sova, guint state, GError **error) {
	SovaTalk talk;

	memset(&talk, 0, sizeof(SovaTalk));

	talk.easyshift = state;

	return sova_talk_write(sova, &talk, error);
}
