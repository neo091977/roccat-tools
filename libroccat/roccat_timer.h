#ifndef __ROCCAT_TIMER_H__
#define __ROCCAT_TIMER_H__

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

#include <glib.h>

G_BEGIN_DECLS

typedef struct _RoccatTimer RoccatTimer;

enum {
	ROCCAT_TIMER_NAME_LENGTH = 24,
};

struct _RoccatTimer {
	/* name needs to be null terminated */
	guint8 name[ROCCAT_TIMER_NAME_LENGTH];
	guint32 seconds;
};

RoccatTimer *roccat_timer_new(void);
void roccat_timer_free(RoccatTimer *timer);
void roccat_timer_set_name(RoccatTimer *timer, gchar const *new_name);
gboolean roccat_timer_equal(RoccatTimer const *left, RoccatTimer const *right);
void roccat_timer_copy(RoccatTimer *destination, RoccatTimer const *source);
RoccatTimer *roccat_timer_dup(RoccatTimer const *source);

static inline guint32 roccat_timer_get_seconds(RoccatTimer const *timer) {
	return GUINT32_FROM_LE(timer->seconds);
}

static inline void roccat_timer_set_seconds(RoccatTimer *timer, guint32 new_value) {
	timer->seconds = GUINT32_TO_LE(new_value);
}

G_END_DECLS

#endif
