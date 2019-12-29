#ifndef __ROCCAT_RYOS_EFFECT_LUA_H__
#define __ROCCAT_RYOS_EFFECT_LUA_H__

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

#include "ryos_effect_client.h"

G_BEGIN_DECLS

#define RYOS_EFFECT_LUA_TYPE (ryos_effect_lua_get_type())
#define RYOS_EFFECT_LUA(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), RYOS_EFFECT_LUA_TYPE, RyosEffectLua))
#define IS_RYOS_EFFECT_LUA(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), RYOS_EFFECT_LUA_TYPE))

typedef struct _RyosEffectLua RyosEffectLua;
typedef struct _RyosEffectLuaPrivate RyosEffectLuaPrivate;

struct _RyosEffectLua {
	GObject parent;
	RyosEffectLuaPrivate *priv;
};

GType ryos_effect_lua_get_type(void);
RyosEffectLua *ryos_effect_lua_new(RyosEffectClient *effect_client);

void ryos_effect_lua_set_module(RyosEffectLua *self, gchar const *name);

void ryos_effect_lua_start(RyosEffectLua *self, gpointer client_data);
void ryos_effect_lua_stop(RyosEffectLua *self);

void ryos_effect_lua_allow(RyosEffectLua *self);
void ryos_effect_lua_disallow(RyosEffectLua *self);

void ryos_effect_lua_key_event(RyosEffectLua *self, guint sdk_index, gboolean action);

G_END_DECLS

#endif
