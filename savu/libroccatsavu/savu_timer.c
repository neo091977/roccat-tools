/*
 * This file is part of roccat-tools.
 *
 * roccat-tools is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Macro Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * roccat-tools is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Macro Public License for more details.
 *
 * You should have received a copy of the GNU Macro Public License
 * along with roccat-tools. If not, see <http://www.gnu.org/licenses/>.
 */

#include "savu_rmp.h"

static void savu_rmp_timer_set_name(SavuRmpTimer *timer, gchar const *new_name) {
	g_strlcpy((gchar *)timer->name, new_name, SAVU_RMP_TIMER_NAME_LENGTH);
}

RoccatTimer *savu_rmp_timer_to_roccat_timer(SavuRmpTimer const *rmp_timer) {
	RoccatTimer *timer;

	timer = roccat_timer_new();
	roccat_timer_set_name(timer, (gchar const *)rmp_timer->name);
	roccat_timer_set_seconds(timer, savu_rmp_timer_get_seconds(rmp_timer));

	return timer;
}

SavuRmpTimer *savu_roccat_timer_to_rmp_timer(RoccatTimer const *timer) {
	SavuRmpTimer *rmp_timer;

	rmp_timer = (SavuRmpTimer *)g_malloc0(sizeof(SavuRmpTimer));
	savu_rmp_timer_set_name(rmp_timer, (gchar const *)timer->name);
	savu_rmp_timer_set_seconds(rmp_timer, roccat_timer_get_seconds(timer));

	return rmp_timer;
}

void savu_rmp_update_with_roccat_timer(SavuRmp *rmp, guint index, RoccatTimer const *timer) {
	SavuRmpTimer *rmp_timer;
	RoccatButton button;

	button.type = SAVU_BUTTON_TYPE_TIMER;
	button.modifier = 0;
	button.key = 0;
	savu_rmp_set_button(rmp, index, &button);

	rmp_timer = savu_roccat_timer_to_rmp_timer(timer);
	savu_rmp_set_timer(rmp, index, rmp_timer);
	g_free(rmp_timer);
}
