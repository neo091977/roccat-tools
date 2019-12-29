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

#include "kova2016_talk.h"
#include "talkfx.h"
#include <string.h>

static gboolean kova2016_talk_write(RoccatDevice *device, Kova2016Talk *talk, GError **error) {
	talk->report_id = KOVA2016_REPORT_ID_TALK;
	talk->size = sizeof(Kova2016Talk);

	return kova2016_device_write(device, (gchar const *)talk, sizeof(Kova2016Talk), error);
}

static gboolean kova2016_talk_write_key(RoccatDevice *device,
		guint8 easyshift, guint8 easyshift_lock,
		guint8 easyaim, GError **error) {
	Kova2016Talk talk;

	memset(&talk, 0, sizeof(Kova2016Talk));

	talk.easyshift = easyshift;
	talk.easyshift_lock = easyshift_lock;
	talk.easyaim = easyaim;
	talk.fx_status = KOVA2016_TALKFX_STATE_UNUSED;

	return kova2016_talk_write(device, &talk, error);
}

gboolean kova2016_talk_easyshift(RoccatDevice *device, guint8 state, GError **error) {
	return kova2016_talk_write_key(device,
			state,
			KOVA2016_TALK_EASYSHIFT_UNUSED,
			KOVA2016_TALK_EASYAIM_UNUSED,
			error);
}

gboolean kova2016_talk_easyshift_lock(RoccatDevice *device, guint8 state, GError **error) {
	return kova2016_talk_write_key(device,
			KOVA2016_TALK_EASYSHIFT_UNUSED,
			state,
			KOVA2016_TALK_EASYAIM_UNUSED,
			error);
}

gboolean kova2016_talk_easyaim(RoccatDevice *device, guint8 state, GError **error) {
	return kova2016_talk_write_key(device,
			KOVA2016_TALK_EASYSHIFT_UNUSED,
			KOVA2016_TALK_EASYSHIFT_UNUSED,
			state,
			error);
}

static gboolean kova2016_talkfx_write(RoccatDevice *kova2016, Kova2016Talk *talk, GError **error) {
	talk->easyshift = KOVA2016_TALK_EASYSHIFT_UNUSED;
	talk->easyshift_lock = KOVA2016_TALK_EASYSHIFT_UNUSED;
	talk->easyaim = KOVA2016_TALK_EASYAIM_UNUSED;
	return kova2016_talk_write(kova2016, talk, error);
}

gboolean kova2016_talkfx(RoccatDevice *kova2016, guint32 effect, guint32 ambient_color, guint32 event_color, GError **error) {
	Kova2016Talk talk;
	guint zone;

	memset(&talk, 0, sizeof(Kova2016Talk));

	talk.fx_status = ROCCAT_TALKFX_STATE_ON;

	zone = (effect & ROCCAT_TALKFX_ZONE_BIT_MASK) >> ROCCAT_TALKFX_ZONE_BIT_SHIFT;
	talk.zone = (zone == ROCCAT_TALKFX_ZONE_AMBIENT) ? KOVA2016_TALKFX_ZONE_AMBIENT : KOVA2016_TALKFX_ZONE_EVENT;

	talk.effect = (effect & ROCCAT_TALKFX_EFFECT_BIT_MASK) >> ROCCAT_TALKFX_EFFECT_BIT_SHIFT;
	talk.speed = (effect & ROCCAT_TALKFX_SPEED_BIT_MASK) >> ROCCAT_TALKFX_SPEED_BIT_SHIFT;
	talk.ambient_red = (ambient_color & ROCCAT_TALKFX_COLOR_RED_MASK) >> ROCCAT_TALKFX_COLOR_RED_SHIFT;
	talk.ambient_green = (ambient_color & ROCCAT_TALKFX_COLOR_GREEN_MASK) >> ROCCAT_TALKFX_COLOR_GREEN_SHIFT;
	talk.ambient_blue = (ambient_color & ROCCAT_TALKFX_COLOR_BLUE_MASK) >> ROCCAT_TALKFX_COLOR_BLUE_SHIFT;
	talk.event_red = (event_color & ROCCAT_TALKFX_COLOR_RED_MASK) >> ROCCAT_TALKFX_COLOR_RED_SHIFT;
	talk.event_green = (event_color & ROCCAT_TALKFX_COLOR_GREEN_MASK) >> ROCCAT_TALKFX_COLOR_GREEN_SHIFT;
	talk.event_blue = (event_color & ROCCAT_TALKFX_COLOR_BLUE_MASK) >> ROCCAT_TALKFX_COLOR_BLUE_SHIFT;

	return kova2016_talkfx_write(kova2016, &talk, error);
}

gboolean kova2016_talkfx_off(RoccatDevice *kova2016, GError **error) {
	Kova2016Talk talk;

	memset(&talk, 0, sizeof(Kova2016Talk));

	talk.fx_status = ROCCAT_TALKFX_STATE_OFF;

	return kova2016_talkfx_write(kova2016, &talk, error);
}
