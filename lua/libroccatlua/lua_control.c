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
#include "g_roccat_helper.h"
#include "roccat_helper.h"
#include "roccat_device_hidraw.h"

static guint8 lua_control_calc_checksum(LuaControl const *control) {
	return ROCCAT_BYTESUM_PARTIALLY(control, LuaControl, command, checksum);
}

static void lua_control_finalize(LuaControl *control) {
	control->report_id = LUA_REPORT_ID_CONTROL;
	control->checksum = lua_control_calc_checksum(control);
}

static gboolean lua_control_write(RoccatDevice *lua, LuaControl *control, GError **error) {
	lua_control_finalize(control);
	return roccat_device_hidraw_write(lua, LUA_INTERFACE_MOUSE, (gchar const *)control, sizeof(LuaControl), error);
}

static gboolean lua_write_control(RoccatDevice *lua, guint8 command,
		guint8 data0, guint8 data1, guint8 data2, guint8 data3, guint8 data4, GError **error) {
	LuaControl control;

	control.command = command;
	control.data[0] = data0;
	control.data[1] = data1;
	control.data[2] = data2;
	control.data[3] = data3;
	control.data[4] = data4;

	return lua_control_write(lua, &control, error);
}

gboolean lua_write_cpi(RoccatDevice *lua, LuaRmp *rmp, GError **error) {
	gboolean retval;
	guint8 array[7] = { 0, 0, 0, 0, 0, 0, 0 };
	guint index;
	guint list_length;

	list_length = 0;
	for (index = 0; index < LUA_CPI_NUM; ++index) {
		if (lua_rmp_get_cpi(rmp, index)) {
			array[list_length] = index + 1;
			++list_length;
		}
	}

	retval = lua_write_control(lua, LUA_CONTROL_COMMAND_CPI, array[0], array[1], array[2], array[3], array[4], error);
	if (!retval)
		return retval;

	g_usleep(100 * G_ROCCAT_USEC_PER_MSEC);

	retval = lua_write_control(lua, LUA_CONTROL_COMMAND_CPI | 1, array[5], array[6], list_length, 0, 0, error);
	if (!retval)
		return retval;

	g_usleep(100 * G_ROCCAT_USEC_PER_MSEC);

	return lua_write_control(lua, LUA_CONTROL_COMMAND_CPI | 2, lua_value_to_cpi(lua_rmp_get_current_cpi(rmp)), 0, 0, 0, 0, error);
}

gboolean lua_write_keys(RoccatDevice *lua, LuaRmp *rmp, GError **error) {
	gboolean retval;
	guint8 left;
	guint8 right;

	if (lua_rmp_get_handed_mode(rmp) == LUA_RMP_HANDED_MODE_RIGHT) {
		left = lua_rmp_get_key(rmp, LUA_KEY_INDEX_LEFT);
		right = lua_rmp_get_key(rmp, LUA_KEY_INDEX_RIGHT);
	} else {
		left = lua_rmp_get_key(rmp, LUA_KEY_INDEX_RIGHT);
		right = lua_rmp_get_key(rmp, LUA_KEY_INDEX_LEFT);
	}

	retval = lua_write_control(lua, LUA_CONTROL_COMMAND_KEYS,
			left, right,
			lua_rmp_get_key(rmp, LUA_KEY_INDEX_MIDDLE),
			lua_rmp_get_key(rmp, LUA_KEY_INDEX_BUTTON),
			lua_rmp_get_key(rmp, LUA_KEY_INDEX_WHEEL_UP),
			error);
	if (!retval)
		return retval;

	g_usleep(100 * G_ROCCAT_USEC_PER_MSEC);

	return lua_write_control(lua, LUA_CONTROL_COMMAND_KEYS | 1, lua_rmp_get_key(rmp, LUA_KEY_INDEX_WHEEL_DOWN), 0, 0, 0, 0, error);
}

gboolean lua_write_polling(RoccatDevice *lua, LuaRmp *rmp, GError **error) {
	return lua_write_control(lua, LUA_CONTROL_COMMAND_POLLING, lua_value_to_polling_rate(lua_rmp_get_polling_rate(rmp)), 0, 0, 0, 0, error);
}

gboolean lua_write_light(RoccatDevice *lua, LuaRmp *rmp, GError **error) {
	guint8 value;

	switch (lua_rmp_get_light(rmp)) {
	case LUA_RMP_LIGHT_FULLY_LIGHTED:
		value = LUA_LIGHT_FULLY_LIGHTED;
		break;
	case LUA_RMP_LIGHT_BREATHING:
		value = LUA_LIGHT_BREATHING;
		break;
	case LUA_RMP_LIGHT_OFF:
		value = LUA_LIGHT_OFF;
		break;
	default:
		value = LUA_LIGHT_FULLY_LIGHTED;
	}

	return lua_write_control(lua, LUA_CONTROL_COMMAND_LIGHT, value, 0, 0, 0, 0, error);
}

gboolean lua_write_end(RoccatDevice *lua, GError **error) {
	return lua_write_control(lua, LUA_CONTROL_COMMAND_END, 0, 0, 0, 0, 0, error);
}

gboolean lua_write_request(RoccatDevice *lua, GError **error) {
	return lua_write_control(lua, LUA_CONTROL_COMMAND_REQUEST, 0, 0, 0, 0, 0, error);
}

LuaReport *lua_report_read(RoccatDevice *lua, GError **error) {
	LuaReport *report;
	report = (LuaReport *)roccat_device_hidraw_read(lua, LUA_INTERFACE_MOUSE, LUA_REPORT_ID_CONTROL, sizeof(LuaReport), error);
	return report;
}
