#ifndef __ROCCAT_KIRO_TALKFX_H__
#define __ROCCAT_KIRO_TALKFX_H__

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

#include "kiro_device.h"

G_BEGIN_DECLS

typedef struct _KiroTalkfx KiroTalkfx;

struct _KiroTalkfx {
	guint8 report_id; /* KIRO_REPORT_ID_FX */
	guint8 size; /* always 13 */
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

typedef enum {
	KIRO_TALKFX_ZONE_EVENT = 4,
	KIRO_TALKFX_ZONE_AMBIENT = 3,
} KiroTalkfxZone;

typedef enum {
	KIRO_TALKFX_STATE_UNUSED = 0xff,
} KiroTalkfxState;

gboolean kiro_talkfx(RoccatDevice *kiro, guint32 effect, guint32 ambient_color, guint32 event_color, GError **error);
gboolean kiro_talkfx_off(RoccatDevice *kiro, GError **error);

G_END_DECLS

#endif
