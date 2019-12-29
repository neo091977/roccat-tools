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

#include "luaconfig_polling_rate_frame.h"
#include "roccat_polling_rate_selector.h"
#include "lua.h"
#include "i18n.h"

#define LUACONFIG_POLLING_RATE_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), LUACONFIG_POLLING_RATE_FRAME_TYPE, LuaconfigPollingRateFrameClass))
#define IS_LUACONFIG_POLLING_RATE_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), LUACONFIG_POLLING_RATE_FRAME_TYPE))
#define LUACONFIG_POLLING_RATE_FRAME_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), LUACONFIG_POLLING_RATE_FRAME_TYPE, LuaconfigPollingRateFramePrivate))

typedef struct _LuaconfigPollingRateFrameClass LuaconfigPollingRateFrameClass;
typedef struct _LuaconfigPollingRateFramePrivate LuaconfigPollingRateFramePrivate;

struct _LuaconfigPollingRateFrame {
	GtkFrame parent;
	LuaconfigPollingRateFramePrivate *priv;
};

struct _LuaconfigPollingRateFrameClass {
	GtkFrameClass parent_class;
};

struct _LuaconfigPollingRateFramePrivate {
	RoccatPollingRateSelector *selector;
};

G_DEFINE_TYPE(LuaconfigPollingRateFrame, luaconfig_polling_rate_frame, GTK_TYPE_FRAME);

GtkWidget *luaconfig_polling_rate_frame_new(void) {
	return GTK_WIDGET(g_object_new(LUACONFIG_POLLING_RATE_FRAME_TYPE, NULL));
}

static void luaconfig_polling_rate_frame_init(LuaconfigPollingRateFrame *frame) {
	frame->priv = LUACONFIG_POLLING_RATE_FRAME_GET_PRIVATE(frame);
	frame->priv->selector = ROCCAT_POLLING_RATE_SELECTOR(roccat_polling_rate_selector_new(ROCCAT_POLLING_RATE_SELECTOR_ALL));
	gtk_container_add(GTK_CONTAINER(frame), GTK_WIDGET(frame->priv->selector));
	gtk_frame_set_label(GTK_FRAME(frame), _("Polling rate"));
}

static void luaconfig_polling_rate_frame_class_init(LuaconfigPollingRateFrameClass *klass) {
	g_type_class_add_private(klass, sizeof(LuaconfigPollingRateFramePrivate));
}

void luaconfig_polling_rate_frame_set_value(LuaconfigPollingRateFrame *frame, guint rmp_value) {
	guint lua_value;
	guint roccat_value;

	lua_value = lua_value_to_polling_rate(rmp_value);

	switch (lua_value) {
	case LUA_POLLING_RATE_125:
		roccat_value = ROCCAT_POLLING_RATE_SELECTOR_125;
		break;
	case LUA_POLLING_RATE_250:
		roccat_value = ROCCAT_POLLING_RATE_SELECTOR_250;
		break;
	case LUA_POLLING_RATE_500:
		roccat_value = ROCCAT_POLLING_RATE_SELECTOR_500;
		break;
	case LUA_POLLING_RATE_1000:
		roccat_value = ROCCAT_POLLING_RATE_SELECTOR_1000;
		break;
	default:
		g_warn_if_reached();
		roccat_value = ROCCAT_POLLING_RATE_SELECTOR_NONE;
		break;
	}

	roccat_polling_rate_selector_set_value(frame->priv->selector, roccat_value);
}

guint luaconfig_polling_rate_frame_get_value(LuaconfigPollingRateFrame *frame) {
	guint roccat_value;
	guint lua_value;

	roccat_value = roccat_polling_rate_selector_get_value(frame->priv->selector);

	switch (roccat_value) {
	case ROCCAT_POLLING_RATE_SELECTOR_125:
		lua_value = LUA_POLLING_RATE_125;
		break;
	case ROCCAT_POLLING_RATE_SELECTOR_250:
		lua_value = LUA_POLLING_RATE_250;
		break;
	case ROCCAT_POLLING_RATE_SELECTOR_500:
		lua_value = LUA_POLLING_RATE_500;
		break;
	case ROCCAT_POLLING_RATE_SELECTOR_1000:
		lua_value = LUA_POLLING_RATE_1000;
		break;
	default:
		g_warn_if_reached();
		__attribute__ ((fallthrough));
	case ROCCAT_POLLING_RATE_SELECTOR_NONE:
		lua_value = LUA_POLLING_RATE_1000;
		break;
	}

	return lua_polling_rate_to_value(lua_value);
}
