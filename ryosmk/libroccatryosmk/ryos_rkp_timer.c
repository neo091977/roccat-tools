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

#include "ryos_rkp_timer.h"
#include <string.h>

void ryos_rkp_timer_set_name(RyosRkpTimer *timer, gchar const *new_name) {
	g_strlcpy((gchar *)timer->name, new_name, RYOS_RKP_TIMER_NAME_LENGTH);
}

void roccat_timer_to_ryos_rkp_timer(RoccatTimer const *roccat_timer, RyosRkpTimer *ryos_timer) {
	memset(ryos_timer, 0, sizeof(RyosRkpTimer));
	ryos_rkp_timer_set_name(ryos_timer, (gchar const *)roccat_timer->name);
	ryos_timer->duration = roccat_timer_get_seconds(roccat_timer);
	// TODO error handling;
}

gboolean ryos_rkp_timer_equal(RyosRkpTimer const *left, RyosRkpTimer const *right) {
	return memcmp(left, right, sizeof(RyosRkpTimer)) ? FALSE : TRUE;
}

void ryos_rkp_timer_copy(RyosRkpTimer *destination, RyosRkpTimer const *source) {
	memcpy(destination, source, sizeof(RyosRkpTimer));
}

RyosRkpTimer *ryos_rkp_timer_dup(RyosRkpTimer const *source) {
	return (RyosRkpTimer *)g_memdup(source, sizeof(RyosRkpTimer));
}
