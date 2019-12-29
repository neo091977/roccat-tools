#ifndef __ROCCAT_KOVA2016_TALK_H__
#define __ROCCAT_KOVA2016_TALK_H__

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

#include "kova2016_device.h"

G_BEGIN_DECLS

typedef struct _Kova2016Talk Kova2016Talk;

struct _Kova2016Talk {
	guint8 report_id; /* KOVA2016_REPORT_ID_TALK */
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

/* also valid as Kova2016TalkEasyshiftLock */
typedef enum {
	KOVA2016_TALK_EASYSHIFT_OFF = 0,
	KOVA2016_TALK_EASYSHIFT_ON = 1,
	KOVA2016_TALK_EASYSHIFT_UNUSED = 0xff,
} Kova2016TalkEasyshift;

typedef enum {
	KOVA2016_TALK_EASYAIM_OFF = 0,
	KOVA2016_TALK_EASYAIM_1 = 1,
	KOVA2016_TALK_EASYAIM_2 = 2,
	KOVA2016_TALK_EASYAIM_3 = 3,
	KOVA2016_TALK_EASYAIM_4 = 4,
	KOVA2016_TALK_EASYAIM_5 = 5,
	KOVA2016_TALK_EASYAIM_UNUSED = 0xff,
} Kova2016TalkEasyaim;

typedef enum {
	KOVA2016_TALKFX_ZONE_EVENT = 4,
	KOVA2016_TALKFX_ZONE_AMBIENT = 3,
} Kova2016TalkfxZone;

typedef enum {
	KOVA2016_TALKFX_STATE_UNUSED = 0xff,
} Kova2016TalkfxState;

gboolean kova2016_talk_easyshift(RoccatDevice *device, guint8 state, GError **error);
gboolean kova2016_talk_easyshift_lock(RoccatDevice *device, guint8 state, GError **error);
gboolean kova2016_talk_easyaim(RoccatDevice *device, guint8 state, GError **error);
gboolean kova2016_talkfx(RoccatDevice *kova2016, guint32 effect, guint32 ambient_color, guint32 event_color, GError **error);
gboolean kova2016_talkfx_off(RoccatDevice *kova2016, GError **error);

G_END_DECLS

#endif
