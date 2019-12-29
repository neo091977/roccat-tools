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

#include "ryos_rkp_quicklaunch.h"
#include <string.h>

gchar *ryos_rkp_quicklaunch_get_path(RyosRkpQuicklaunch const *quicklaunch) {
	return g_utf16_to_utf8((gunichar2 const *)(quicklaunch->path), RYOS_RKP_QUICKLAUNCH_PATH_LENGTH, NULL, NULL, NULL);
}

static void ryos_rkp_quicklaunch_set_path(RyosRkpQuicklaunch *quicklaunch, gchar const *path) {
	gunichar2 *string;
	glong items;

	string = g_utf8_to_utf16(path, -1L, NULL, &items, NULL);

	items = MIN(RYOS_RKP_QUICKLAUNCH_PATH_LENGTH - 1, items);

	if (string) {
		memset(quicklaunch->path, 0, RYOS_RKP_QUICKLAUNCH_PATH_LENGTH * 2);
		memcpy(quicklaunch->path, string, items * 2);
		g_free(string);
	}
}

gchar *ryos_rkp_quicklaunch_get_name(RyosRkpQuicklaunch const *quicklaunch) {
	return g_utf16_to_utf8((gunichar2 const *)(quicklaunch->name), RYOS_RKP_QUICKLAUNCH_NAME_LENGTH, NULL, NULL, NULL);
}

static void ryos_rkp_quicklaunch_set_name(RyosRkpQuicklaunch *quicklaunch, gchar const *name) {
	gunichar2 *string;
	glong items;

	string = g_utf8_to_utf16(name, -1L, NULL, &items, NULL);

	items = MIN(RYOS_RKP_QUICKLAUNCH_NAME_LENGTH - 1, items);

	if (string) {
		memset(quicklaunch->name, 0, RYOS_RKP_QUICKLAUNCH_NAME_LENGTH * 2);
		memcpy(quicklaunch->name, string, items * 2);
		g_free(string);
	}
}

void ryos_rkp_quicklaunch_set(RyosRkpQuicklaunch *quicklaunch, gchar const *path) {
	gchar *name;

	ryos_rkp_quicklaunch_set_path(quicklaunch, path);
	name = g_path_get_basename(path);
	ryos_rkp_quicklaunch_set_name(quicklaunch, name);
	g_free(name);
}

gboolean ryos_rkp_quicklaunch_equal(RyosRkpQuicklaunch const *left, RyosRkpQuicklaunch const *right) {
	return memcmp(left, right, sizeof(RyosRkpQuicklaunch)) ? FALSE : TRUE;
}

void ryos_rkp_quicklaunch_copy(RyosRkpQuicklaunch *destination, RyosRkpQuicklaunch const *source) {
	memcpy(destination, source, sizeof(RyosRkpQuicklaunch));
}

RyosRkpQuicklaunch *ryos_rkp_quicklaunch_dup(RyosRkpQuicklaunch const *source) {
	return (RyosRkpQuicklaunch *)g_memdup(source, sizeof(RyosRkpQuicklaunch));
}
