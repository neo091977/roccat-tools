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

#include "skeltr_talk.h"
#include "skeltr_device.h"
#include "skeltr.h"
#include <string.h>

static gboolean skeltr_talk_write(RoccatDevice *skeltr, SkeltrTalk *talk, GError **error) {
	talk->report_id = SKELTR_REPORT_ID_TALK;
	talk->size = sizeof(SkeltrTalk);

	return skeltr_device_write(skeltr, (gchar const *)talk, sizeof(SkeltrTalk), error);
}

gboolean skeltr_talk_write_easyshift(RoccatDevice *skeltr, guint state, GError **error) {
	SkeltrTalk talk;

	memset(&talk, 0, sizeof(SkeltrTalk));

	talk.easyshift = state;

	return skeltr_talk_write(skeltr, &talk, error);
}
