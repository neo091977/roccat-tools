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

#include "ryos_talk.h"
#include "ryos_device.h"
#include <string.h>

static gboolean ryos_talk_write(RoccatDevice *ryos, RyosTalk *talk, GError **error) {
	talk->report_id = RYOS_REPORT_ID_TALK;
	talk->size = sizeof(RyosTalk);

	return ryos_device_write(ryos, (gchar const *)talk, sizeof(RyosTalk), error);
}

static gboolean ryos_talk_write_key(RoccatDevice *ryos,
		guint easyshift, guint easyshift_lock, GError **error) {
	RyosTalk talk;

	memset(&talk, 0, sizeof(RyosTalk));

	talk.easyshift = easyshift;
	talk.easyshift_lock = easyshift_lock;

	return ryos_talk_write(ryos, &talk, error);
}

gboolean ryos_talk_easyshift(RoccatDevice *ryos, guint state, GError **error) {
	return ryos_talk_write_key(ryos,
			state,
			RYOS_TALK_EASYSHIFT_UNUSED,
			error);
}

gboolean ryos_talk_easyshift_lock(RoccatDevice *ryos, guint state, GError **error) {
	return ryos_talk_write_key(ryos,
			RYOS_TALK_EASYSHIFT_UNUSED,
			state,
			error);
}
