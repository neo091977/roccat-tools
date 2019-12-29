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

#include "kiro_talkfx.h"
#include "talkfx.h"
#include <string.h>

static gboolean kiro_talkfx_write(RoccatDevice *kiro, KiroTalkfx *talkfx, GError **error) {
	talkfx->report_id = KIRO_REPORT_ID_FX;
	talkfx->size = sizeof(KiroTalkfx);

	return kiro_device_write(kiro, (gchar const *)talkfx, sizeof(KiroTalkfx), error);
}

gboolean kiro_talkfx(RoccatDevice *kiro, guint32 effect, guint32 ambient_color, guint32 event_color, GError **error) {
	KiroTalkfx talk;
	guint zone;

	memset(&talk, 0, sizeof(KiroTalkfx));

	talk.fx_status = ROCCAT_TALKFX_STATE_ON;

	zone = (effect & ROCCAT_TALKFX_ZONE_BIT_MASK) >> ROCCAT_TALKFX_ZONE_BIT_SHIFT;
	talk.zone = (zone == ROCCAT_TALKFX_ZONE_AMBIENT) ? KIRO_TALKFX_ZONE_AMBIENT : KIRO_TALKFX_ZONE_EVENT;

	talk.effect = (effect & ROCCAT_TALKFX_EFFECT_BIT_MASK) >> ROCCAT_TALKFX_EFFECT_BIT_SHIFT;
	talk.speed = (effect & ROCCAT_TALKFX_SPEED_BIT_MASK) >> ROCCAT_TALKFX_SPEED_BIT_SHIFT;
	talk.ambient_red = (ambient_color & ROCCAT_TALKFX_COLOR_RED_MASK) >> ROCCAT_TALKFX_COLOR_RED_SHIFT;
	talk.ambient_green = (ambient_color & ROCCAT_TALKFX_COLOR_GREEN_MASK) >> ROCCAT_TALKFX_COLOR_GREEN_SHIFT;
	talk.ambient_blue = (ambient_color & ROCCAT_TALKFX_COLOR_BLUE_MASK) >> ROCCAT_TALKFX_COLOR_BLUE_SHIFT;
	talk.event_red = (event_color & ROCCAT_TALKFX_COLOR_RED_MASK) >> ROCCAT_TALKFX_COLOR_RED_SHIFT;
	talk.event_green = (event_color & ROCCAT_TALKFX_COLOR_GREEN_MASK) >> ROCCAT_TALKFX_COLOR_GREEN_SHIFT;
	talk.event_blue = (event_color & ROCCAT_TALKFX_COLOR_BLUE_MASK) >> ROCCAT_TALKFX_COLOR_BLUE_SHIFT;

	return kiro_talkfx_write(kiro, &talk, error);
}

gboolean kiro_talkfx_off(RoccatDevice *kiro, GError **error) {
	KiroTalkfx talk;

	memset(&talk, 0, sizeof(KiroTalkfx));

	talk.fx_status = ROCCAT_TALKFX_STATE_OFF;

	return kiro_talkfx_write(kiro, &talk, error);
}
