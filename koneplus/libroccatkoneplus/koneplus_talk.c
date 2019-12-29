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
#include "talkfx.h"
#include <string.h>

static gboolean koneplus_talk_write(RoccatDevice *koneplus, KoneplusTalk *talk, GError **error) {
	talk->report_id = KONEPLUS_REPORT_ID_TALK;
	talk->size = sizeof(KoneplusTalk);

	return koneplus_device_write(koneplus, (gchar const *)talk, sizeof(KoneplusTalk), error);
}

static gboolean koneplus_talk_write_key(RoccatDevice *koneplus,
		guint8 easyshift, guint8 easyshift_lock,
		guint8 easyaim, GError **error) {
	KoneplusTalk talk;

	memset(&talk, 0, sizeof(KoneplusTalk));

	talk.easyshift = easyshift;
	talk.easyshift_lock = easyshift_lock;
	talk.easyaim = easyaim;
	talk.fx_status = KONEPLUS_TALKFX_STATE_UNUSED;

	return koneplus_talk_write(koneplus, &talk, error);
}

gboolean koneplus_talk_easyshift(RoccatDevice *koneplus, guint8 state, GError **error) {
	return koneplus_talk_write_key(koneplus,
			state,
			KONEPLUS_TALK_EASYSHIFT_UNUSED,
			KONEPLUS_TALK_EASYAIM_UNUSED,
			error);
}

gboolean koneplus_talk_easyshift_lock(RoccatDevice *koneplus, guint8 state, GError **error) {
	return koneplus_talk_write_key(koneplus,
			KONEPLUS_TALK_EASYSHIFT_UNUSED,
			state,
			KONEPLUS_TALK_EASYAIM_UNUSED,
			error);
}

gboolean koneplus_talk_easyaim(RoccatDevice *koneplus, guint8 state, GError **error) {
	return koneplus_talk_write_key(koneplus,
			KONEPLUS_TALK_EASYSHIFT_UNUSED,
			KONEPLUS_TALK_EASYSHIFT_UNUSED,
			state,
			error);
}

gboolean koneplus_talkfx_write(RoccatDevice *koneplus, KoneplusTalk *talk, GError **error) {
	talk->easyshift = KONEPLUS_TALK_EASYSHIFT_UNUSED;
	talk->easyshift_lock = KONEPLUS_TALK_EASYSHIFT_UNUSED;
	talk->easyaim = KONEPLUS_TALK_EASYAIM_UNUSED;
	return koneplus_talk_write(koneplus, talk, error);
}

KoneplusTalk *koneplus_talkfx_read(RoccatDevice *koneplus, GError **error) {
	return (KoneplusTalk *)koneplus_device_read(koneplus, KONEPLUS_REPORT_ID_TALK, sizeof(KoneplusTalk), error);
}

gboolean koneplus_talkfx(RoccatDevice *koneplus, guint32 effect, guint32 ambient_color, guint32 event_color, GError **error) {
	KoneplusTalk talk;
	guint zone;

	memset(&talk, 0, sizeof(KoneplusTalk));

	talk.fx_status = ROCCAT_TALKFX_STATE_ON;

	zone = (effect & ROCCAT_TALKFX_ZONE_BIT_MASK) >> ROCCAT_TALKFX_ZONE_BIT_SHIFT;
	talk.zone = (zone == ROCCAT_TALKFX_ZONE_AMBIENT) ? KONEPLUS_TALKFX_ZONE_AMBIENT : KONEPLUS_TALKFX_ZONE_EVENT;

	talk.effect = (effect & ROCCAT_TALKFX_EFFECT_BIT_MASK) >> ROCCAT_TALKFX_EFFECT_BIT_SHIFT;
	talk.speed = (effect & ROCCAT_TALKFX_SPEED_BIT_MASK) >> ROCCAT_TALKFX_SPEED_BIT_SHIFT;
	talk.ambient_red = (ambient_color & ROCCAT_TALKFX_COLOR_RED_MASK) >> ROCCAT_TALKFX_COLOR_RED_SHIFT;
	talk.ambient_green = (ambient_color & ROCCAT_TALKFX_COLOR_GREEN_MASK) >> ROCCAT_TALKFX_COLOR_GREEN_SHIFT;
	talk.ambient_blue = (ambient_color & ROCCAT_TALKFX_COLOR_BLUE_MASK) >> ROCCAT_TALKFX_COLOR_BLUE_SHIFT;
	talk.event_red = (event_color & ROCCAT_TALKFX_COLOR_RED_MASK) >> ROCCAT_TALKFX_COLOR_RED_SHIFT;
	talk.event_green = (event_color & ROCCAT_TALKFX_COLOR_GREEN_MASK) >> ROCCAT_TALKFX_COLOR_GREEN_SHIFT;
	talk.event_blue = (event_color & ROCCAT_TALKFX_COLOR_BLUE_MASK) >> ROCCAT_TALKFX_COLOR_BLUE_SHIFT;

	return koneplus_talkfx_write(koneplus, &talk, error);
}

gboolean koneplus_talkfx_off(RoccatDevice *koneplus, GError **error) {
	KoneplusTalk talk;

	memset(&talk, 0, sizeof(KoneplusTalk));

	talk.fx_status = ROCCAT_TALKFX_STATE_OFF;

	return koneplus_talkfx_write(koneplus, &talk, error);
}
