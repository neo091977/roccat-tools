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

#include "lua.h"

static guint const polling_rate_values[LUA_POLLING_RATE_NUM] = { 1000, 500, 250, 125 };

guint lua_polling_rate_to_value(guint polling_rate) {
	g_assert(polling_rate < LUA_POLLING_RATE_NUM);
	return polling_rate_values[polling_rate];
}

/* returns next lower polling_rate if value doesn't match */
guint lua_value_to_polling_rate(guint value) {
	guint i;

	for (i = 0; i < LUA_POLLING_RATE_NUM; ++i) {
		if (value >= polling_rate_values[i])
			return i;
	}

	return LUA_POLLING_RATE_125;
}
