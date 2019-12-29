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
#include <string.h>

gboolean isku_talk_write(RoccatDevice *isku, IskuTalk *talk, GError **error) {
	talk->report_id = ISKU_REPORT_ID_TALK;
	talk->size = sizeof(IskuTalk);

	return isku_device_write(isku, (gchar const *)talk, sizeof(IskuTalk), error);
}

static gboolean isku_talk_write_key(RoccatDevice *isku,
		guint easyshift, guint easyshift_lock, GError **error) {
	IskuTalk talk;

	memset(&talk, 0, sizeof(IskuTalk));

	talk.easyshift = easyshift;
	talk.easyshift_lock = easyshift_lock;

	return isku_talk_write(isku, &talk, error);
}

gboolean isku_talk_easyshift(RoccatDevice *isku, guint state, GError **error) {
	return isku_talk_write_key(isku,
			state,
			ISKU_TALK_EASYSHIFT_UNUSED,
			error);
}

gboolean isku_talk_easyshift_lock(RoccatDevice *isku, guint state, GError **error) {
	return isku_talk_write_key(isku,
			ISKU_TALK_EASYSHIFT_UNUSED,
			state,
			error);
}
