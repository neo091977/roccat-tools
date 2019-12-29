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

#include "roccat_timer.h"
#include <string.h>

void roccat_timer_set_name(RoccatTimer *timer, gchar const *new_name) {
	g_strlcpy((gchar *)timer->name, new_name, ROCCAT_TIMER_NAME_LENGTH);
}

RoccatTimer *roccat_timer_new(void) {
	RoccatTimer *timer;
	timer = g_malloc0(sizeof(RoccatTimer));
	return timer;
}

void roccat_timer_free(RoccatTimer *timer) {
	g_free(timer);
}

gboolean roccat_timer_equal(RoccatTimer const *left, RoccatTimer const *right) {
	return memcmp(left, right, sizeof(RoccatTimer)) ? FALSE : TRUE;
}

void roccat_timer_copy(RoccatTimer *destination, RoccatTimer const *source) {
	memcpy(destination, source, sizeof(RoccatTimer));
}

RoccatTimer *roccat_timer_dup(RoccatTimer const *source) {
	return (RoccatTimer *)g_memdup(source, sizeof(RoccatTimer));
}
