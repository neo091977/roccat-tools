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

static guint const cpi_values[LUA_CPI_NUM] = { 250, 500, 1000, 1250, 1500, 1750, 2000 };

guint lua_cpi_to_value(guint cpi) {
	g_assert(cpi >= 1 && cpi <= LUA_CPI_NUM);
	return cpi_values[cpi - 1];
}

/* returns next higher cpi if value doesn't match */
guint lua_value_to_cpi(guint value) {
	guint i;

	for (i = 0; i < LUA_CPI_NUM; ++i) {
		if (value <= cpi_values[i])
			return i + 1;
	}

	return LUA_CPI_2000;
}
