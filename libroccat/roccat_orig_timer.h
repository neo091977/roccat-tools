#ifndef __ROCCAT_ORIG_TIMER_H__
#define __ROCCAT_ORIG_TIMER_H__

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

#include "roccat_timers.h"

G_BEGIN_DECLS

typedef struct _RoccatOrigTimer RoccatOrigTimer;

enum {
	ROCCAT_ORIG_TIMER_NAME_LENGTH = 100,
};

struct _RoccatOrigTimer {
	guint32 seconds;
	guint8 name[ROCCAT_ORIG_TIMER_NAME_LENGTH];
	guint8 checksum;
} __attribute__ ((packed));

static inline guint32 roccat_orig_timer_get_seconds(RoccatOrigTimer const *timer) {
	return GUINT32_FROM_LE(timer->seconds);
}

static inline void roccat_orig_timer_set_seconds(RoccatOrigTimer *timer, guint32 new_value) {
	timer->seconds = GUINT32_TO_LE(new_value);
}

RoccatTimers *roccat_original_timers_import(gchar const *filename, GError **error);
gboolean roccat_original_timers_export(gchar const *filename, RoccatTimers *timers, GError **error);

G_END_DECLS

#endif
