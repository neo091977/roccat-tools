#ifndef __ROCCAT_TYON_TALK_H__
#define __ROCCAT_TYON_TALK_H__

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

#include "tyon_device.h"

G_BEGIN_DECLS

typedef struct _TyonTalk TyonTalk;

struct _TyonTalk {
	guint8 report_id; /* TYON_REPORT_ID_TALK */
	guint8 size; /* always 16 */
	guint8 easyshift;
	guint8 easyshift_lock;
	guint8 easyaim;
	guint8 fx_status;
	guint8 zone;
	guint8 unused;
	guint8 effect;
	guint8 speed;
	guint8 ambient_red;
	guint8 ambient_green;
	guint8 ambient_blue;
	guint8 event_red;
	guint8 event_green;
	guint8 event_blue;
} __attribute__ ((packed));

/* also valid as TyonTalkEasyshiftLock */
typedef enum {
	TYON_TALK_EASYSHIFT_OFF = 0,
	TYON_TALK_EASYSHIFT_ON = 1,
	TYON_TALK_EASYSHIFT_UNUSED = 0xff,
} TyonTalkEasyshift;

typedef enum {
	TYON_TALK_EASYAIM_OFF = 0,
	TYON_TALK_EASYAIM_1 = 1,
	TYON_TALK_EASYAIM_2 = 2,
	TYON_TALK_EASYAIM_3 = 3,
	TYON_TALK_EASYAIM_4 = 4,
	TYON_TALK_EASYAIM_5 = 5,
	TYON_TALK_EASYAIM_UNUSED = 0xff,
} TyonTalkEasyaim;

typedef enum {
	TYON_TALKFX_ZONE_EVENT = 4,
	TYON_TALKFX_ZONE_AMBIENT = 3,
} TyonTalkfxZone;

typedef enum {
	TYON_TALKFX_STATE_UNUSED = 0xff,
} TyonTalkfxState;

gboolean tyon_talk_easyshift(RoccatDevice *device, guint8 state, GError **error);
gboolean tyon_talk_easyshift_lock(RoccatDevice *device, guint8 state, GError **error);
gboolean tyon_talk_easyaim(RoccatDevice *device, guint8 state, GError **error);
gboolean tyon_talkfx(RoccatDevice *tyon, guint32 effect, guint32 ambient_color, guint32 event_color, GError **error);
gboolean tyon_talkfx_off(RoccatDevice *tyon, GError **error);

G_END_DECLS

#endif
