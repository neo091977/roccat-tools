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

#include "ryos_rkp_accessors.h"
#include <string.h>

gchar *ryos_rkp_get_profile_name(RyosRkp const *rkp) {
	return g_utf16_to_utf8((gunichar2 const *)(rkp->profile_name), RYOS_RKP_PROFILE_NAME_LENGTH, NULL, NULL, NULL);
}

void ryos_rkp_set_profile_name(RyosRkp *rkp, gchar const *name) {
	gunichar2 *string;
	glong items;

	string = g_utf8_to_utf16(name, -1L, NULL, &items, NULL);
	if (string == NULL)
		return;

	/* make sure target has trailing 0 */
	items = MIN(RYOS_RKP_PROFILE_NAME_LENGTH - 1, items);
	string[items] = 0;

	if (memcmp(&rkp->profile_name, string, (items + 1) * 2)) {
		memset(&rkp->profile_name, 0, RYOS_RKP_PROFILE_NAME_LENGTH * 2);
		memcpy(&rkp->profile_name, string, items * 2);
	}
	g_free(string);
}

gchar *ryos_rkp_get_gamefile_name(RyosRkp const *rkp, guint index) {
	return g_utf16_to_utf8((gunichar2 const *)(rkp->gamefile_names[index]), RYOS_RKP_PROFILE_GAMEFILE_LENGTH, NULL, NULL, NULL);
}

void ryos_rkp_set_gamefile_name(RyosRkp *rkp, guint index, gchar const *name) {
	gunichar2 *string;
	glong items;

	string = g_utf8_to_utf16(name, -1L, NULL, &items, NULL);
	if (string == NULL)
		return;

	items = MIN(RYOS_RKP_PROFILE_GAMEFILE_LENGTH - 1, items);
	string[items] = 0;

	if (memcmp(&rkp->gamefile_names[index], string, (items + 1) * 2)) {
		memset(&rkp->gamefile_names[index], 0, RYOS_RKP_PROFILE_GAMEFILE_LENGTH * 2);
		memcpy(&rkp->gamefile_names[index], string, items * 2);
	}
	g_free(string);
}
