#ifndef __ROCCAT_LUA_H__
#define __ROCCAT_LUA_H__

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

#include "roccat_key_file.h"
#include "roccat_device_scanner.h"
#include "roccat_device.h"
#include <glib.h>
#include <stdio.h>

G_BEGIN_DECLS

#define USB_DEVICE_ID_ROCCAT_LUA 0x2c2e

typedef struct _LuaControl LuaControl;
typedef struct _LuaReport LuaReport;
typedef struct _LuaRmp LuaRmp;

enum {
	LUA_REPORT_ID_CONTROL = 3,
	LUA_BUTTON_NUM = 6,
	LUA_CPI_NUM = 7,
	LUA_POLLING_RATE_NUM = 4,
};

struct _LuaControl {
	guint8 report_id; /* LUA_REPORT_ID_CONTROL */
	guint8 command;
	guint8 data[5];
	guint8 checksum;
} __attribute__ ((packed));

typedef enum {
	LUA_CONTROL_COMMAND_CPI = 0xa0,
	LUA_CONTROL_COMMAND_KEYS = 0xb0,
	LUA_CONTROL_COMMAND_POLLING = 0xc0,
	LUA_CONTROL_COMMAND_LIGHT = 0xd0,
	LUA_CONTROL_COMMAND_REQUEST = 0xdd,
	LUA_CONTROL_COMMAND_END = 0xee,
} LuaControlCommand;

typedef enum {
	LUA_CPI_250 = 1,
	LUA_CPI_500 = 2,
	LUA_CPI_1000 = 3,
	LUA_CPI_1250 = 4,
	LUA_CPI_1500 = 5,
	LUA_CPI_1750 = 6,
	LUA_CPI_2000 = 7,
} LuaCpi;

typedef enum {
	LUA_RMP_HANDED_MODE_LEFT = 1,
	LUA_RMP_HANDED_MODE_RIGHT = 2,
} LuaRmpHandedMode;

typedef enum {
	LUA_LIGHT_FULLY_LIGHTED = 0x11,
	LUA_LIGHT_BREATHING = 0x20,
	LUA_LIGHT_OFF = 0x10,
} LuaLight;

typedef enum {
	LUA_RMP_LIGHT_FULLY_LIGHTED = 1,
	LUA_RMP_LIGHT_BREATHING = 2,
	LUA_RMP_LIGHT_OFF = 3,
} LuaRmpLight;

typedef enum {
	LUA_POLLING_RATE_1000 = 0,
	LUA_POLLING_RATE_500 = 1,
	LUA_POLLING_RATE_250 = 2,
	LUA_POLLING_RATE_125 = 3,
} LuaPollingRate;

typedef enum {
	LUA_KEY_TYPE_CLICK = 0x1,
	LUA_KEY_TYPE_MENU = 0x2,
	LUA_KEY_TYPE_UNIVERSAL_SCROLL = 0x4,
	LUA_KEY_TYPE_BROWSER_FORWARD = 0x8,
	LUA_KEY_TYPE_BROWSER_BACKWARD = 0x10,
	LUA_KEY_TYPE_CPI_CYCLE = 0x81,
	LUA_KEY_TYPE_SCROLL_UP = 0x82,
	LUA_KEY_TYPE_SCROLL_DOWN = 0x83,
	LUA_KEY_TYPE_DISABLED = 0x85,
} LuaKeyType;

typedef enum {
	LUA_KEY_INDEX_LEFT = 0,
	LUA_KEY_INDEX_RIGHT = 1,
	LUA_KEY_INDEX_MIDDLE = 2,
	LUA_KEY_INDEX_WHEEL_UP = 3,
	LUA_KEY_INDEX_WHEEL_DOWN = 4,
	LUA_KEY_INDEX_BUTTON = 5,
} LuaKeyIndex;

struct _LuaReport {
	guint8 report_id;
	guint8 cpi;
	guint8 maybe_squal;
	guint8 maybe_pix_accum;
	guint8 maybe_sensor_product_id;
	guint8 unknown3;
	guint8 unknown4;
	guint8 unknown5;
} __attribute__ ((packed));

typedef enum {
	LUA_INTERFACE_MOUSE = 0,
} LuaInterface;

void lua_rmp_free(LuaRmp *rmp);
LuaRmp *lua_rmp_dup(LuaRmp const *src);
LuaRmp *lua_rmp_read_with_path(gchar const *path, GError **error);
gboolean lua_rmp_write_with_path(gchar const *path, LuaRmp *rmp, GError **error);
LuaRmp const *lua_default_rmp(void);

void lua_rmp_set_handed_mode(LuaRmp *rmp, guint value);
guint lua_rmp_get_handed_mode(LuaRmp *rmp);

void lua_rmp_set_polling_rate(LuaRmp *rmp, guint value);
guint lua_rmp_get_polling_rate(LuaRmp *rmp);

void lua_rmp_set_light(LuaRmp *rmp, guint value);
guint lua_rmp_get_light(LuaRmp *rmp);

void lua_rmp_set_current_cpi(LuaRmp *rmp, guint value);
guint lua_rmp_get_current_cpi(LuaRmp *rmp);

void lua_rmp_set_cpi(LuaRmp *rmp, guint index, gboolean value);
gboolean lua_rmp_get_cpi(LuaRmp *rmp, guint index);

void lua_rmp_set_key(LuaRmp *rmp, guint index, guint value);
guint lua_rmp_get_key(LuaRmp *rmp, guint index);

gboolean lua_rmp_get_modified(LuaRmp const *rmp);
void lua_rmp_set_modified(LuaRmp *rmp);
void lua_rmp_set_unmodified(LuaRmp *rmp);
LuaRmp *lua_rmp_load(void);
gboolean lua_rmp_save(RoccatDevice *lua, LuaRmp *rmp, GError **error);

LuaRmp *lua_rmp_load_actual(void);
gboolean lua_rmp_save_actual(LuaRmp *rmp, GError **error);

guint lua_cpi_to_value(guint cpi);
guint lua_value_to_cpi(guint cpi);
guint lua_polling_rate_to_value(guint polling_rate);
guint lua_value_to_polling_rate(guint polling_rate);

RoccatKeyFile *lua_configuration_load(void);
void lua_configuration_free(RoccatKeyFile *config);
gboolean lua_configuration_save(RoccatKeyFile *config, GError **error);

gchar *lua_configuration_get_rmp_path(RoccatKeyFile *config);
void lua_configuration_set_rmp_path(RoccatKeyFile *config, gchar const *path);

gboolean lua_write_cpi(RoccatDevice *lua, LuaRmp *rmp, GError **error);
gboolean lua_write_keys(RoccatDevice *lua, LuaRmp *rmp, GError **error);
gboolean lua_write_polling(RoccatDevice *lua, LuaRmp *rmp, GError **error);
gboolean lua_write_light(RoccatDevice *lua, LuaRmp *rmp, GError **error);
gboolean lua_write_end(RoccatDevice *lua, GError **error);
gboolean lua_write_request(RoccatDevice *lua, GError **error);
LuaReport *lua_report_read(RoccatDevice *lua, GError **error);

typedef gboolean (*lua_device_scanner_callback)(RoccatDevice *device, gpointer user_data);

RoccatDevice *lua_device_first(void);
RoccatDeviceScanner *lua_device_scanner_new(void);

#define LUA_DEVICE_NAME "Lua"

#define LUA_DBUS_SERVER_PATH "/org/roccat/Lua"
#define LUA_DBUS_SERVER_INTERFACE "org.roccat.Lua"

G_END_DECLS

#endif
