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

#include "luaconfig_handedness_selector.h"
#include "lua.h"

#define LUACONFIG_HANDEDNESS_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), LUACONFIG_HANDEDNESS_SELECTOR_TYPE, LuaconfigHandednessSelectorClass))
#define IS_LUACONFIG_HANDEDNESS_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), LUACONFIG_HANDEDNESS_SELECTOR_TYPE))
#define LUACONFIG_HANDEDNESS_SELECTOR_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), LUACONFIG_HANDEDNESS_SELECTOR_TYPE, LuaconfigHandednessSelectorPrivate))

typedef struct _LuaconfigHandednessSelectorClass LuaconfigHandednessSelectorClass;

struct _LuaconfigHandednessSelector {
	RoccatHandednessSelector parent;
};

struct _LuaconfigHandednessSelectorClass {
	RoccatHandednessSelectorClass parent_class;
};

G_DEFINE_TYPE(LuaconfigHandednessSelector, luaconfig_handedness_selector, ROCCAT_HANDEDNESS_SELECTOR_TYPE);

GtkWidget *luaconfig_handedness_selector_new(void) {
	LuaconfigHandednessSelector *handedness_selector;

	handedness_selector = LUACONFIG_HANDEDNESS_SELECTOR(g_object_new(LUACONFIG_HANDEDNESS_SELECTOR_TYPE,
			NULL));

	return GTK_WIDGET(handedness_selector);
}

static void luaconfig_handedness_selector_init(LuaconfigHandednessSelector *handedness_selector) {}

static void luaconfig_handedness_selector_class_init(LuaconfigHandednessSelectorClass *klass) {}

guint luaconfig_handedness_selector_get_value(LuaconfigHandednessSelector *selector) {
	switch (roccat_handedness_selector_get_value(ROCCAT_HANDEDNESS_SELECTOR(selector))) {
	case ROCCAT_HANDEDNESS_SELECTOR_LEFT:
		return LUA_RMP_HANDED_MODE_LEFT;
		break;
	case ROCCAT_HANDEDNESS_SELECTOR_RIGHT:
	default:
		return LUA_RMP_HANDED_MODE_RIGHT;
		break;
	}
}

void luaconfig_handedness_selector_set_value(LuaconfigHandednessSelector *selector, guint rmp_value) {
	switch (rmp_value) {
	case LUA_RMP_HANDED_MODE_LEFT:
		roccat_handedness_selector_set_value(ROCCAT_HANDEDNESS_SELECTOR(selector), ROCCAT_HANDEDNESS_SELECTOR_LEFT);
		break;
	case LUA_RMP_HANDED_MODE_RIGHT:
	default:
		roccat_handedness_selector_set_value(ROCCAT_HANDEDNESS_SELECTOR(selector), ROCCAT_HANDEDNESS_SELECTOR_RIGHT);
		break;
	}
}
