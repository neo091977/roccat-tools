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
#include "roccat_helper.h"
#include "g_roccat_helper.h"
#include "i18n-lib.h"

static guint8 const default_rmp_data[] = {
	"[ConfigData]\n"
	"CurDPI=1000\n"
	"ScrollSpeed=3\n"
	"MouseWheel=1\n"
	"1Page=0\n"
	"PollingRate=1000\n"
	"DPISwitch250=1\n"
	"DPISwitch500=1\n"
	"DPISwitch1000=1\n"
	"DPISwitch1250=1\n"
	"DPISwitch1500=1\n"
	"DPISwitch1750=1\n"
	"DPISwitch2000=1\n"
	"PointerSpeed=6\n"
	"MouseAcceleration=0\n"
	"DisplayPointerTrails=0\n"
	"DClickSpeed=7\n"
	"HandedMode=2\n"
	"AdjustEffect=2\n"
	"Key1=1\n"
	"Key2=2\n"
	"Key3=4\n"
	"Key4=130\n"
	"Key5=131\n"
	"Key6=129\n"
	"Language=64\n"
};

static LuaRmp *lua_rmp_new(void) {
	LuaRmp *rmp = (LuaRmp *)g_malloc0(sizeof(LuaRmp));
	rmp->key_file = g_key_file_new();
	return rmp;
}

void lua_rmp_free(LuaRmp *rmp) {
	if (rmp) {
		g_key_file_free(rmp->key_file);
		g_free(rmp);
	}
}

LuaRmp *lua_rmp_dup(LuaRmp const *src) {
	LuaRmp *dest = (LuaRmp *)g_malloc0(sizeof(LuaRmp));

	dest->key_file = roccat_key_file_dup(src->key_file);
	dest->modified_rmp = src->modified_rmp;
	dest->modified_cpi = src->modified_cpi;
	dest->modified_keys = src->modified_keys;
	dest->modified_polling = src->modified_polling;
	dest->modified_light = src->modified_light;

	return dest;
}

LuaRmp *lua_rmp_read_with_path(gchar const *path, GError **error) {
	LuaRmp *rmp = lua_rmp_new();

	if (!g_key_file_load_from_file(rmp->key_file, path, G_KEY_FILE_NONE, error)) {
		lua_rmp_free(rmp);
		return NULL;
	}

	return rmp;
}

gboolean lua_rmp_write_with_path(gchar const *path, LuaRmp *rmp, GError **error) {
	gchar *data;
	gsize size;
	gboolean ret;

	data = g_key_file_to_data(rmp->key_file, &size, error);
	if (!data)
		goto error;

	ret = roccat_profile_write_with_path(path, data, size, error);
	g_free(data);
	if (!ret)
		goto error;

	return TRUE;
error:
	g_warning(_("Could not write rmp %s: %s"), path, (*error)->message);
	return FALSE;
}

LuaRmp const *lua_default_rmp(void) {
	static LuaRmp *rmp = NULL;
	GError *error = NULL;

	if (rmp == NULL) {
		rmp = lua_rmp_new();

		if (!g_key_file_load_from_data(rmp->key_file, (gchar const *)default_rmp_data, sizeof(default_rmp_data), 0, &error)) {
			g_clear_pointer(&rmp, lua_rmp_free);
			g_critical(_("Could not create default rmp: %s"), error->message);
			g_error_free(error);
		}
	}

	return rmp;
}
