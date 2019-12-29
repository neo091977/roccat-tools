#ifndef __ROCCAT_TALKFX_H__
#define __ROCCAT_TALKFX_H__

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

/*
 * - Keyboard and mice behave differently depending on RoccatTalkfxZone
 * - IskuFX does not support ROCCAT_TALKFX_EFFECT_HEARTBEAT
 * - IskuFX does not support RoccatTalkfxSide
 * - KonePures single light is connected to ROCCAT_TALKFX_SIDE_LEFT
 * - RoccatTalkfxEffect and RoccatTalkfxSpeed are only valid
 *   for ROCCAT_TALKFX_ZONE_EVENT
 */

#include <glib.h>

G_BEGIN_DECLS

typedef enum {
	ROCCAT_TALKFX_STATE_OFF = 0x00,
	ROCCAT_TALKFX_STATE_ON = 0x01,
} RoccatTalkfxState;

typedef enum {
	ROCCAT_TALKFX_ZONE_AMBIENT = 0x00,
	ROCCAT_TALKFX_ZONE_EVENT = 0x01,
	ROCCAT_TALKFX_ZONE_BIT_MASK = 0x00ff0000,
	ROCCAT_TALKFX_ZONE_BIT_SHIFT = 16,
} RoccatTalkfxZone;

typedef enum {
	ROCCAT_TALKFX_EFFECT_OFF = 0x00,
	ROCCAT_TALKFX_EFFECT_ON = 0x01,
	ROCCAT_TALKFX_EFFECT_BLINKING = 0x02,
	ROCCAT_TALKFX_EFFECT_BREATHING = 0x03,
	ROCCAT_TALKFX_EFFECT_HEARTBEAT = 0x04, /* not supported by IskuFX */
	ROCCAT_TALKFX_EFFECT_BIT_MASK = 0x0000ff00,
	ROCCAT_TALKFX_EFFECT_BIT_SHIFT = 8,
} RoccatTalkfxEffect;

typedef enum {
	ROCCAT_TALKFX_SPEED_OFF = 0x00,
	ROCCAT_TALKFX_SPEED_SLOW = 0x01,
	ROCCAT_TALKFX_SPEED_NORMAL = 0x02,
	ROCCAT_TALKFX_SPEED_FAST = 0x03,
	ROCCAT_TALKFX_SPEED_BIT_MASK = 0x000000ff,
	ROCCAT_TALKFX_SPEED_BIT_SHIFT = 0,
} RoccatTalkfxSpeed;

typedef enum {
	ROCCAT_TALKFX_COLOR_RED_MASK = 0x00ff0000,
	ROCCAT_TALKFX_COLOR_GREEN_MASK = 0x0000ff00,
	ROCCAT_TALKFX_COLOR_BLUE_MASK = 0x000000ff,
	ROCCAT_TALKFX_COLOR_RED_SHIFT = 16,
	ROCCAT_TALKFX_COLOR_GREEN_SHIFT = 8,
	ROCCAT_TALKFX_COLOR_BLUE_SHIFT = 0,
} RoccatTalkfxColor;

G_END_DECLS

#endif
