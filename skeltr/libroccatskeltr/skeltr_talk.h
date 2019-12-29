#ifndef __ROCCAT_SKELTR_TALK_H__
#define __ROCCAT_SKELTR_TALK_H__

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

#include "roccat_device.h"

G_BEGIN_DECLS

typedef struct _SkeltrTalk SkeltrTalk;

struct _SkeltrTalk {
	guint8 report_id; /* SKELTR_REPORT_ID_TALK */
	guint8 size; /* always 0x10 */
	guint8 easyshift;
	guint8 unused[13];
} __attribute__ ((packed));

/* also valid as SkeltrTalkEasyshiftLock */
typedef enum {
	SKELTR_TALK_EASYSHIFT_OFF = 0,
	SKELTR_TALK_EASYSHIFT_ON = 1,
} SkeltrTalkEasyshift;

gboolean skeltr_talk_write_easyshift(RoccatDevice *skeltr, guint state, GError **error);

G_END_DECLS

#endif
