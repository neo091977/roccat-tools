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

#include "lua_rmp.h"
#include "g_roccat_helper.h"
#include "i18n-lib.h"

static gchar const * const lua_rmp_group_name = "ConfigData";

static gchar const * const lua_rmp_handed_mode_name = "HandedMode";
static gchar const * const lua_rmp_polling_rate_name = "PollingRate";
static gchar const * const lua_rmp_current_cpi_name = "CurDPI";
static gchar const * const lua_rmp_light_name = "AdjustEffect";

static gchar *lua_rmp_create_key_name(guint index) {
	g_assert(index < LUA_BUTTON_NUM);
	return g_strdup_printf("Key%i", index + 1);
}

static gchar *lua_rmp_create_cpi_name(guint index) {
	g_assert(index < LUA_CPI_NUM);
	return g_strdup_printf("DPISwitch%i", lua_cpi_to_value(index + 1));
}

/*
 * May terminate application if error occurs
 */
static gint lua_rmp_get_default_integer(gchar const *key) {
	LuaRmp const *default_rmp;
	GError *error = NULL;
	gint result;

	default_rmp = lua_default_rmp();
	result = g_key_file_get_integer(default_rmp->key_file, lua_rmp_group_name, key, &error);
	if (error)
		g_error(_("Could not get default value for key '%s': %s"), key, error->message);
	return result;
}

static gint lua_rmp_get_integer(LuaRmp *rmp, gchar const *key) {
	GError *error = NULL;
	gint result = g_key_file_get_integer(rmp->key_file, lua_rmp_group_name, key, &error);
	if (error) {
		g_clear_error(&error);
		result = lua_rmp_get_default_integer(key);
	}
	return result;
}

static gboolean lua_rmp_set_integer(LuaRmp *rmp, gchar const *key, gint value) {
	if (lua_rmp_get_integer(rmp, key) != value) {
		g_key_file_set_integer(rmp->key_file, lua_rmp_group_name, key, value);
		return TRUE;
	}
	return FALSE;
}

void lua_rmp_set_handed_mode(LuaRmp *rmp, guint value) {
	g_assert(value == LUA_RMP_HANDED_MODE_LEFT || value == LUA_RMP_HANDED_MODE_RIGHT);
 	if (lua_rmp_set_integer(rmp, lua_rmp_handed_mode_name, value))
 		rmp->modified_keys = TRUE;
}

guint lua_rmp_get_handed_mode(LuaRmp *rmp) {
	return lua_rmp_get_integer(rmp, lua_rmp_handed_mode_name);
}

void lua_rmp_set_polling_rate(LuaRmp *rmp, guint value) {
	// FIXME assert
 	if (lua_rmp_set_integer(rmp, lua_rmp_polling_rate_name, value))
 		rmp->modified_polling = TRUE;
}

guint lua_rmp_get_polling_rate(LuaRmp *rmp) {
	return lua_rmp_get_integer(rmp, lua_rmp_polling_rate_name);
}

void lua_rmp_set_light(LuaRmp *rmp, guint value) {
	g_assert(value >= LUA_RMP_LIGHT_FULLY_LIGHTED && value <= LUA_RMP_LIGHT_OFF);
 	if (lua_rmp_set_integer(rmp, lua_rmp_light_name, value))
 		rmp->modified_light = TRUE;
}

guint lua_rmp_get_light(LuaRmp *rmp) {
	return lua_rmp_get_integer(rmp, lua_rmp_light_name);
}

void lua_rmp_set_current_cpi(LuaRmp *rmp, guint value) {
	// FIXME assert
 	if (lua_rmp_set_integer(rmp, lua_rmp_current_cpi_name, value))
 		rmp->modified_cpi = TRUE;
}

guint lua_rmp_get_current_cpi(LuaRmp *rmp) {
	return lua_rmp_get_integer(rmp, lua_rmp_current_cpi_name);
}

void lua_rmp_set_cpi(LuaRmp *rmp, guint index, gboolean value) {
	g_assert(value == 0 || value == 1);
	gchar *name = lua_rmp_create_cpi_name(index);
	if (lua_rmp_set_integer(rmp, name, value))
		rmp->modified_cpi = TRUE;
	g_free(name);
}

gboolean lua_rmp_get_cpi(LuaRmp *rmp, guint index) {
	gchar *name = lua_rmp_create_cpi_name(index);
	gboolean cpi = lua_rmp_get_integer(rmp, name) != 0 ? TRUE : FALSE;
	g_free(name);
	return cpi;
}

void lua_rmp_set_key(LuaRmp *rmp, guint index, guint value) {
	gchar *name = lua_rmp_create_key_name(index);
	if (lua_rmp_set_integer(rmp, name, value))
		rmp->modified_keys = TRUE;
	g_free(name);
}

guint lua_rmp_get_key(LuaRmp *rmp, guint index) {
	gchar *name = lua_rmp_create_key_name(index);
	guint key = lua_rmp_get_integer(rmp, name);
	g_free(name);
	return key;
}
