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
#include "lua_rmp.h"
#include "roccat_helper.h"
#include "g_roccat_helper.h"
#include "roccat.h"

static gchar *lua_profile_dir(void) {
	gchar *base = roccat_profile_dir();
	gchar *dir = g_build_path("/", base, "lua", NULL);
	g_free(base);
	return dir;
}

static gboolean lua_profile_dir_create_if_needed(GError **error) {
	gchar *dir;
	gboolean retval;

	dir = lua_profile_dir();
	retval = roccat_profile_dir_create_if_needed(dir, error);
	g_free(dir);
	return retval;
}

static gboolean lua_rmp_get_modified_binary(LuaRmp const *rmp) {
	return rmp->modified_cpi || rmp->modified_keys || rmp->modified_polling || rmp->modified_light;
}

gboolean lua_rmp_get_modified(LuaRmp const *rmp) {
	return rmp->modified_rmp || lua_rmp_get_modified_binary(rmp);
}

static void lua_rmp_set_modified_state(LuaRmp *rmp, gboolean state) {
	rmp->modified_rmp = state;
	rmp->modified_cpi = state;
	rmp->modified_keys = state;
	rmp->modified_polling = state;
	rmp->modified_light = state;
}

void lua_rmp_set_modified(LuaRmp *rmp) {
	lua_rmp_set_modified_state(rmp, TRUE);
}

void lua_rmp_set_unmodified(LuaRmp *rmp) {
	lua_rmp_set_modified_state(rmp, FALSE);
}

static gchar *lua_build_config_rmp_path(void) {
	gchar *dir = lua_profile_dir();
	gchar *path = g_build_path("/", dir, "actual." ROCCAT_MOUSE_PROFILE_EXTENSION, NULL);
	g_free(dir);
	return path;
}

LuaRmp *lua_rmp_load_actual(void) {
	LuaRmp *rmp;
	gchar *config_path;

	config_path = lua_build_config_rmp_path();
	rmp = lua_rmp_read_with_path(config_path, NULL);
	g_free(config_path);
	if (!rmp)
		rmp = lua_rmp_dup(lua_default_rmp());
	return rmp;
}

LuaRmp *lua_rmp_load(void) {
	return lua_rmp_load_actual();
}

gboolean lua_rmp_save_actual(LuaRmp *rmp, GError **error) {
	gchar *config_path;
	gboolean retval;

	if (!lua_rmp_get_modified(rmp))
		return TRUE;

	lua_profile_dir_create_if_needed(error);
	if (*error)
		return FALSE;

	config_path = lua_build_config_rmp_path();
	retval = lua_rmp_write_with_path(config_path, rmp, error);
	g_free(config_path);
	return retval;
}

gboolean lua_rmp_save(RoccatDevice *lua, LuaRmp *rmp, GError **error) {
	gboolean retval = FALSE;

	if (lua_rmp_get_modified(rmp) && !lua_rmp_save_actual(rmp, error)) return FALSE;

	gaminggear_device_lock(GAMINGGEAR_DEVICE(lua));

	if (rmp->modified_cpi && !lua_write_cpi(lua, rmp, error))  goto error;
	g_usleep(100 * G_ROCCAT_USEC_PER_MSEC);
	if (rmp->modified_keys && !lua_write_keys(lua, rmp, error))  goto error;
	g_usleep(100 * G_ROCCAT_USEC_PER_MSEC);
	if (rmp->modified_polling && !lua_write_polling(lua, rmp, error))  goto error;
	g_usleep(100 * G_ROCCAT_USEC_PER_MSEC);
	if (rmp->modified_light && !lua_write_light(lua, rmp, error))  goto error;
	g_usleep(500 * G_ROCCAT_USEC_PER_MSEC);
	if (lua_rmp_get_modified_binary(rmp) && !lua_write_end(lua, error)) goto error;

	lua_rmp_set_unmodified(rmp);
	retval = TRUE;
error:
	gaminggear_device_unlock(GAMINGGEAR_DEVICE(lua));
	return retval;
}
