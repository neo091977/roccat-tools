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

#include "luaconfig_cpi_selector.h"
#include "roccat_cpi_fixed_selector.h"

#define LUACONFIG_CPI_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), LUACONFIG_CPI_SELECTOR_TYPE, LuaconfigCpiSelectorClass))
#define IS_LUACONFIG_CPI_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), LUACONFIG_CPI_SELECTOR_TYPE))
#define LUACONFIG_CPI_SELECTOR_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), LUACONFIG_CPI_SELECTOR_TYPE, LuaconfigCpiSelectorPrivate))

typedef struct _LuaconfigCpiSelectorClass LuaconfigCpiSelectorClass;

struct _LuaconfigCpiSelector {
	RoccatCpiFixedSelector parent;
};

struct _LuaconfigCpiSelectorClass {
	RoccatCpiFixedSelectorClass parent_class;
};

G_DEFINE_TYPE(LuaconfigCpiSelector, luaconfig_cpi_selector, ROCCAT_CPI_FIXED_SELECTOR_TYPE);

static guint const cpi_values[LUA_CPI_NUM] = { 250, 500, 1000, 1250, 1500, 1750, 2000 };

void luaconfig_cpi_selector_set_from_rmp(LuaconfigCpiSelector *selector, LuaRmp *rmp) {
	guint i;

	roccat_cpi_fixed_selector_set_selected(ROCCAT_CPI_FIXED_SELECTOR(selector), lua_value_to_cpi(lua_rmp_get_current_cpi(rmp)) - 1);
	for (i = 0; i < LUA_CPI_NUM; ++i)
		roccat_cpi_fixed_selector_set_active(ROCCAT_CPI_FIXED_SELECTOR(selector), i, lua_rmp_get_cpi(rmp, i));
}

void luaconfig_cpi_selector_update_rmp(LuaconfigCpiSelector *selector, LuaRmp *rmp) {
	guint i;

	lua_rmp_set_current_cpi(rmp, lua_cpi_to_value(roccat_cpi_fixed_selector_get_selected(ROCCAT_CPI_FIXED_SELECTOR(selector)) + 1));
	for (i = 0; i < LUA_CPI_NUM; ++i)
		lua_rmp_set_cpi(rmp, i, roccat_cpi_fixed_selector_get_active(ROCCAT_CPI_FIXED_SELECTOR(selector), i));
}

GtkWidget *luaconfig_cpi_selector_new(void) {
	LuaconfigCpiSelector *cpi_selector;

	cpi_selector = LUACONFIG_CPI_SELECTOR(g_object_new(LUACONFIG_CPI_SELECTOR_TYPE,
			"count", LUA_CPI_NUM,
			"values", cpi_values,
			"with-buttons", TRUE,
			NULL));

	return GTK_WIDGET(cpi_selector);
}

static void luaconfig_cpi_selector_init(LuaconfigCpiSelector *cpi_selector) {}

static void luaconfig_cpi_selector_class_init(LuaconfigCpiSelectorClass *klass) {}
