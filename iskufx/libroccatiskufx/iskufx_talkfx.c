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

#include "iskufx.h"
#include "talkfx.h"
#include <string.h>

gboolean iskufx_talkfx_write(RoccatDevice *iskufx, IskufxTalkfx *talkfx, GError **error) {
	talkfx->report_id = ISKUFX_REPORT_ID_TALKFX;
	talkfx->size = sizeof(IskufxTalkfx);

	return isku_device_write(iskufx, (gchar const *)talkfx, sizeof(IskufxTalkfx), error);
}

IskufxTalkfx *iskufx_talkfx_read(RoccatDevice *iskufx, GError **error) {
	return (IskufxTalkfx *)isku_device_read(iskufx, ISKU_REPORT_ID_TALK, sizeof(IskufxTalkfx), error);
}

gboolean iskufx_talkfx(RoccatDevice *iskufx, guint32 effect, guint32 ambient_color, guint32 event_color, GError **error) {
	IskufxTalkfx talkfx;
	guint zone;

	memset(&talkfx, 0, sizeof(IskufxTalkfx));

	talkfx.state = ROCCAT_TALKFX_STATE_ON;

	zone = (effect & ROCCAT_TALKFX_ZONE_BIT_MASK) >> ROCCAT_TALKFX_ZONE_BIT_SHIFT;
	talkfx.zone = (zone == ROCCAT_TALKFX_ZONE_AMBIENT) ? ISKUFX_TALKFX_ZONE_AMBIENT : ISKUFX_TALKFX_ZONE_EVENT;

	talkfx.effect = (effect & ROCCAT_TALKFX_EFFECT_BIT_MASK) >> ROCCAT_TALKFX_EFFECT_BIT_SHIFT;
	talkfx.speed = (effect & ROCCAT_TALKFX_SPEED_BIT_MASK) >> ROCCAT_TALKFX_SPEED_BIT_SHIFT;
	talkfx.ambient_red = (ambient_color & ROCCAT_TALKFX_COLOR_RED_MASK) >> ROCCAT_TALKFX_COLOR_RED_SHIFT;
	talkfx.ambient_green = (ambient_color & ROCCAT_TALKFX_COLOR_GREEN_MASK) >> ROCCAT_TALKFX_COLOR_GREEN_SHIFT;
	talkfx.ambient_blue = (ambient_color & ROCCAT_TALKFX_COLOR_BLUE_MASK) >> ROCCAT_TALKFX_COLOR_BLUE_SHIFT;
	talkfx.event_red = (event_color & ROCCAT_TALKFX_COLOR_RED_MASK) >> ROCCAT_TALKFX_COLOR_RED_SHIFT;
	talkfx.event_green = (event_color & ROCCAT_TALKFX_COLOR_GREEN_MASK) >> ROCCAT_TALKFX_COLOR_GREEN_SHIFT;
	talkfx.event_blue = (event_color & ROCCAT_TALKFX_COLOR_BLUE_MASK) >> ROCCAT_TALKFX_COLOR_BLUE_SHIFT;

	return iskufx_talkfx_write(iskufx, &talkfx, error);
}

gboolean iskufx_talkfx_off(RoccatDevice *iskufx, GError **error) {
	IskufxTalkfx talkfx;

	memset(&talkfx, 0, sizeof(IskufxTalkfx));

	talkfx.state = ROCCAT_TALKFX_STATE_OFF;

	return iskufx_talkfx_write(iskufx, &talkfx, error);
}
