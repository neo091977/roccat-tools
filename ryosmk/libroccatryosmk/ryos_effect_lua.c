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
#include "lauxlib.h"
#include "lualib.h"
#include "ryos_effect_lua.h"
#include "ryos.h"
#include "g_roccat_helper.h"
#include "roccat_helper.h"
#include "config.h"
#include "i18n-lib.h"
#include <gaminggear/threads.h>

#define RYOS_EFFECT_LUA_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), RYOS_EFFECT_LUA_TYPE, RyosEffectLuaClass))
#define IS_RYOS_EFFECT_LUA_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), RYOS_EFFECT_LUA_TYPE))
#define RYOS_EFFECT_LUA_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), RYOS_EFFECT_LUA_TYPE, RyosEffectLuaPrivate))

typedef struct _RyosEffectLuaClass RyosEffectLuaClass;

struct _RyosEffectLuaClass {
	GObjectClass parent_class;
};

enum {
	PROP_0,
	PROP_MODULE,
};

struct _RyosEffectLuaPrivate {

	/* Not protected by mutex, since it's written only at creation time */
	RyosEffectClient *effect_client;

	/* Not protected by mutex, since it's only written when no thread is running */
	gchar *module_name;

	/* Not protected by mutex */
	GThread *thread;

	/* Not protected by mutex, since it's only used inside the thread */
	lua_State *state;

	/* Members downwards are associated with mutex */
	GaminggearMutex mutex;

	gboolean active; /* set by script */

	/* Signaled when empty queue got first element, stop or allowed where activated */
	GaminggearCond condition;

	GQueue *queue;

	/* Set TRUE if the script should finish it's work and exit */
	gboolean stop;

	/* ONLY set from outside like eventhandler */
	gboolean allowed;
};

enum {
	RYOS_EFFECT_LUA_QUEUE_MAX_LENGTH = 50,
};

/* Not using event type 0, simply to make value stored in queue non-null
 * which would lead to wrong assumption in Lua script
 */
typedef enum {
	RYOS_EFFECT_LUA_EVENT_TYPE_KEY = 1,
} RyosEffectLuaEventType;

static gchar const * const registry_entry = "_SELF";
static gchar const * const red_key = "red";
static gchar const * const green_key = "green";
static gchar const * const blue_key = "blue";

static void activate_deactivate(RyosEffectLua *self) {
	RyosEffectLuaPrivate *priv = self->priv;
	GError *local_error = NULL;
	gboolean result;

	if (priv->active && priv->allowed)
		result = ryos_effect_client_activate(priv->effect_client, &local_error);
	else
		result = ryos_effect_client_deactivate(priv->effect_client, &local_error);

	if (!result) {
		g_warning(_("Could not activate/deactivate effect: %s"), local_error->message);
		g_clear_error(&local_error);
	}
}

static void set_self(lua_State *L, RyosEffectLua *effect_lua) {
	lua_pushlightuserdata(L, effect_lua);
	lua_setfield(L, LUA_REGISTRYINDEX, registry_entry);
}

static RyosEffectLua *get_self(lua_State *L) {
	RyosEffectLua *effect_lua;

	lua_getfield(L, LUA_REGISTRYINDEX, registry_entry);
	effect_lua = lua_touserdata(L, -1);
	lua_pop(L, 1);

	return effect_lua;
}

static guint8 queue_value_get_action(gpointer value) {
	return GPOINTER_TO_UINT(value) & 0xff;
}

static guint8 queue_value_get_index(gpointer value) {
	return (GPOINTER_TO_UINT(value) >> 8) & 0xff;
}

static guint8 queue_value_get_event_type(gpointer value) {
	return (GPOINTER_TO_UINT(value) >> 16) & 0xff;
}

static gpointer queue_value(guint8 type, guint8 index, guint8 action) {
	return GUINT_TO_POINTER(type << 16 | index << 8 | action);
}

/* stack:
 * -1 table
 */
static int get_color_table_entry(lua_State *L, int table_index, gchar const *key) {
	lua_Integer result;
	if (!lua_istable(L, table_index)) {
		lua_pushfstring(L, _("Expected table, got %s"), luaL_typename(L, table_index));
		return luaL_argerror(L, table_index, lua_tostring(L, table_index));
	}

	lua_getfield(L, table_index, key);
	result = luaL_checkinteger(L, -1);

	/* removing value */
	lua_pop(L, 1);
	
	if (result < 0 || result > 0xff)
		luaL_error(L, _("Color value %d is out of range [0-255]"), result);
	
	return result;
}

/* stack:
 * -2 integer/array index
 * -1 table color
 */
static int ryos_lua_set_key_color(lua_State *L) {
	switch(lua_type(L, -2)) {
	case LUA_TTABLE:
		lua_pushnil(L);
		/* stack:
		 * -3 table index
		 * -2 table color
		 * -1 nil
		 */

		while (lua_next(L, -3) != 0) {
			/* stack:
			 * -4 table index
			 * -3 table color
			 * -2 key
			 * -1 value
			 */
			ryos_effect_client_set_key_color(get_self(L)->priv->effect_client,
					luaL_checkinteger(L, -1),
					get_color_table_entry(L, -3, red_key),
					get_color_table_entry(L, -3, green_key),
					get_color_table_entry(L, -3, blue_key));

			/* remove value */
			lua_pop(L, 1);
		}
		return 0;
		break;
	case LUA_TNUMBER:
		ryos_effect_client_set_key_color(get_self(L)->priv->effect_client,
				luaL_checkinteger(L, -2),
				get_color_table_entry(L, -1, red_key),
				get_color_table_entry(L, -1, green_key),
				get_color_table_entry(L, -1, blue_key));
		return 0;
		break;
	default:
		lua_pushfstring(L, _("Table or integer expected, got %s"), luaL_typename(L, -2));
		return luaL_argerror(L, -2, lua_tostring(L, -1));
		break;
	}
}

/* stack:
 * -2 integer/array index
 * -1 boolean state
 */
static int ryos_lua_set_key_state(lua_State *L) {
	switch(lua_type(L, -2)) {
	case LUA_TTABLE:
		lua_pushnil(L);
		/* stack:
		 * -3 table index
		 * -2 boolean state
		 * -1 nil
		 */

		while (lua_next(L, -3) != 0) {
			/* stack:
			 * -4 table index
			 * -3 boolean state
			 * -2 key
			 * -1 value
			 */
			ryos_effect_client_set_key_state(get_self(L)->priv->effect_client,
					luaL_checkinteger(L, -1),
					lua_toboolean(L, -3));

			/* remove value */
			lua_pop(L, 1);
		}
		return 0;
		break;
	case LUA_TNUMBER:
		ryos_effect_client_set_key_state(get_self(L)->priv->effect_client,
				luaL_checkinteger(L, -2),
				lua_toboolean(L, -1));
		return 0;
		break;
	default:
		lua_pushfstring(L, _("Table or integer expected, got %s"), luaL_typename(L, -2));
		return luaL_argerror(L, -2, lua_tostring(L, -1));
		break;
	}
}

/* stack:
 * -1 table
 */
static int ryos_lua_set_key_states(lua_State *L) {
	if (!lua_istable(L, -1)) {
		lua_pushfstring(L, _("Expected table, got %s"), luaL_typename(L, -1));
		return luaL_argerror(L, -1, lua_tostring(L, -1));
	}
	
	lua_pushnil(L);
	while (lua_next(L, -2) != 0) {
		/* stack:
		 * -3 table
		 * -2 key
		 * -1 value
		 */
		ryos_lua_set_key_state(L);
		/* removes value, keeps key for next iteration */
		lua_pop(L, 1);
	}
	/* cleanup key */
	lua_pop(L, 1);
	return 0;
}

/* stack:
 * -1 table
 */
static int ryos_lua_set_key_colors(lua_State *L) {
	if (!lua_istable(L, -1)) {
		lua_pushfstring(L, _("Expected table, got %s"), luaL_typename(L, -1));
		return luaL_argerror(L, -1, lua_tostring(L, -1));
	}
	
	lua_pushnil(L);
	while (lua_next(L, -2) != 0) {
		/* stack:
		 * -3 table
		 * -2 key
		 * -1 value
		 */
		ryos_lua_set_key_color(L);
		/* removes value, keeps key for next iteration */
		lua_pop(L, 1);
	}
	/* cleanup key */
	lua_pop(L, 1);
	return 0;
}

/* stack:
 * -1 boolean state
 */
static int ryos_lua_set_all_states(lua_State *L) {
	ryos_effect_client_set_state(get_self(L)->priv->effect_client,
			lua_toboolean(L, -1));
	return 0;
}

/* stack:
 * -1 table color
 */
static int ryos_lua_set_all_colors(lua_State *L) {
	ryos_effect_client_set_color(get_self(L)->priv->effect_client,
			get_color_table_entry(L, -1, red_key),
			get_color_table_entry(L, -1, green_key),
			get_color_table_entry(L, -1, blue_key));
	return 0;
}

/* stack:
 * -1 integer index
 */
static int ryos_lua_get_key(lua_State *L) {
	RyosEffectLuaPrivate *priv = get_self(L)->priv;
	lua_Integer index = luaL_checkinteger(L, -1);

	lua_pushboolean(L, ryos_effect_client_get_key_state(priv->effect_client, index));

	lua_newtable(L);
	lua_pushinteger(L, ryos_effect_client_get_key_red(priv->effect_client, index));
	lua_setfield(L, -2, red_key);
	lua_pushinteger(L, ryos_effect_client_get_key_green(priv->effect_client, index));
	lua_setfield(L, -2, green_key);
	lua_pushinteger(L, ryos_effect_client_get_key_blue(priv->effect_client, index));
	lua_setfield(L, -2, blue_key);

	return 2;
}

static int ryos_lua_are_all_off(lua_State *L) {
	RyosEffectLuaPrivate *priv = get_self(L)->priv;
	lua_pushboolean(L, ryos_effect_client_get_all_off(priv->effect_client));
	return 1;
}

static int ryos_lua_supports_color(lua_State *L) {
	RyosEffectLuaPrivate *priv = get_self(L)->priv;
	lua_pushboolean(L, ryos_effect_client_supports_color(priv->effect_client));
	return 1;
}

static int ryos_lua_emulated(lua_State *L) {
	RyosEffectLuaPrivate *priv = get_self(L)->priv;
	lua_pushboolean(L, ryos_effect_client_is_emulator(priv->effect_client));
	return 1;
}

static int ryos_lua_should_stop(lua_State *L) {
	RyosEffectLuaPrivate *priv = get_self(L)->priv;
	gboolean stop;

	gaminggear_mutex_lock(&priv->mutex);
	stop = priv->stop;
	gaminggear_mutex_unlock(&priv->mutex);

	lua_pushboolean(L, stop);
	return 1;
}

/* Called only with locked mutex. Returns TRUE if queue is not empty or should stop. */
static gboolean queue_or_stop(gpointer user_data) {
	RyosEffectLuaPrivate *priv = RYOS_EFFECT_LUA(user_data)->priv;
	return !g_queue_is_empty(priv->queue) || priv->stop;
}

/* Called only with locked mutex. Returns TRUE if should stop. */
static gboolean stop(gpointer user_data) {
	RyosEffectLuaPrivate *priv = RYOS_EFFECT_LUA(user_data)->priv;
	return priv->stop;
}

/* Called only with locked mutex. */
static gboolean allowed_or_stop(gpointer user_data) {
	RyosEffectLuaPrivate *priv = RYOS_EFFECT_LUA(user_data)->priv;
	return priv->allowed || priv->stop;
}

static int ryos_lua_wait_for_allowance(lua_State *L) {
	RyosEffectLua *effect_lua = get_self(L);
	RyosEffectLuaPrivate *priv = effect_lua->priv;
	gboolean is_stop;

	gaminggear_mutex_lock(&priv->mutex);
	gaminggear_cond_wait_for(&priv->condition, &priv->mutex, allowed_or_stop, effect_lua);
	is_stop = priv->stop;
	gaminggear_mutex_unlock(&priv->mutex);

	lua_pushboolean(L, is_stop);
	return 1;
}

static int get_event(lua_State *L, gboolean timed) {
	RyosEffectLua *effect_lua = get_self(L);
	RyosEffectLuaPrivate *priv = effect_lua->priv;
	gpointer value;
	lua_Integer microseconds;
	
	if (timed)
		microseconds = luaL_checkinteger(L, -1);
	
	// FIXME mutex waiting time is not considered
	gaminggear_mutex_lock(&priv->mutex);

	if (timed)
		gaminggear_cond_wait_for_timed(&priv->condition, &priv->mutex, microseconds, queue_or_stop, effect_lua);
	else
		gaminggear_cond_wait_for(&priv->condition, &priv->mutex, queue_or_stop, effect_lua);

	value = g_queue_pop_head(priv->queue);
	gaminggear_mutex_unlock(&priv->mutex);

	/* nil is possible on timeout or stop condition */
	if (!value) {
		lua_pushnil(L);
		return 1;
	} else {
		lua_pushinteger(L, queue_value_get_event_type(value));
		lua_pushinteger(L, queue_value_get_index(value));
		lua_pushboolean(L, queue_value_get_action(value));
		return 3;
	}
}

static int ryos_lua_get_event(lua_State *L) {
	return get_event(L, FALSE);
}

static int ryos_lua_get_event_timed(lua_State *L) {
	return get_event(L, TRUE);
}

static int ryos_lua_allowed(lua_State *L) {
	RyosEffectLuaPrivate *priv = get_self(L)->priv;
	gboolean allowed;

	gaminggear_mutex_lock(&priv->mutex);
	allowed = priv->allowed;
	gaminggear_mutex_unlock(&priv->mutex);

	lua_pushboolean(L, allowed);
	return 1;
}

static int ryos_lua_activate(lua_State *L) {
	RyosEffectLua *self = get_self(L);
	RyosEffectLuaPrivate *priv = self->priv;

	gaminggear_mutex_lock(&priv->mutex);
	if (!priv->active) {
		priv->active = TRUE;
		activate_deactivate(self);
	}
	gaminggear_mutex_unlock(&priv->mutex);

	return 0;
}

static int ryos_lua_deactivate(lua_State *L) {
	RyosEffectLua *self = get_self(L);
	RyosEffectLuaPrivate *priv = self->priv;

	gaminggear_mutex_lock(&priv->mutex);
	if (priv->active) {
		priv->active = FALSE;
		activate_deactivate(self);
	}
	gaminggear_mutex_unlock(&priv->mutex);

	return 0;
}

static int ryos_lua_sleep(lua_State *L) {
	RyosEffectLua *effect_lua = get_self(L);
	RyosEffectLuaPrivate *priv = effect_lua->priv;
	lua_Integer microseconds;
	gboolean condition;
	
	microseconds = luaL_checkinteger(L, -1);
	gaminggear_mutex_lock(&priv->mutex);
	condition = gaminggear_cond_wait_for_timed(&priv->condition, &priv->mutex, microseconds, stop, effect_lua);
	gaminggear_mutex_unlock(&priv->mutex);
	
	lua_pushboolean(L, condition);
	return 1;
}

static int ryos_lua_send(lua_State *L) {
	RyosEffectLuaPrivate *priv = get_self(L)->priv;
	GError *local_error = NULL;
	
	if (!priv->active || !priv->allowed)
		return 0;
	
	if (!ryos_effect_client_send(priv->effect_client, &local_error)) {
		lua_pushfstring(L, _("Could not write custom lights: %s"), local_error->message);
		g_clear_error(&local_error);
		return lua_error(L);
	}

	return 0;
}

static lua_State *create_lua_state(RyosEffectLua *effect_lua) {
	lua_State *L;

	L = luaL_newstate();
	if (!L)
		return NULL;

	luaL_openlibs(L);

	set_self(L, effect_lua);

	lua_newtable(L);
	lua_pushinteger(L, RYOS_EFFECT_LUA_EVENT_TYPE_KEY);
	lua_setfield(L, -2, "EVENT_KEY");
	lua_pushcfunction(L, ryos_lua_supports_color);
	lua_setfield(L, -2, "supports_color");
	lua_pushcfunction(L, ryos_lua_emulated);
	lua_setfield(L, -2, "emulated");
	lua_pushcfunction(L, ryos_lua_get_event);
	lua_setfield(L, -2, "get_event");
	lua_pushcfunction(L, ryos_lua_get_event_timed);
	lua_setfield(L, -2, "get_event_timed");
	lua_pushcfunction(L, ryos_lua_set_key_state);
	lua_setfield(L, -2, "set_key_state");
	lua_pushcfunction(L, ryos_lua_set_key_states);
	lua_setfield(L, -2, "set_key_states");
	lua_pushcfunction(L, ryos_lua_set_key_color);
	lua_setfield(L, -2, "set_key_color");
	lua_pushcfunction(L, ryos_lua_set_key_colors);
	lua_setfield(L, -2, "set_key_colors");
	lua_pushcfunction(L, ryos_lua_set_all_states);
	lua_setfield(L, -2, "set_all_states");
	lua_pushcfunction(L, ryos_lua_set_all_colors);
	lua_setfield(L, -2, "set_all_colors");
	lua_pushcfunction(L, ryos_lua_get_key);
	lua_setfield(L, -2, "get_key");
	lua_pushcfunction(L, ryos_lua_are_all_off);
	lua_setfield(L, -2, "are_all_off");
	lua_pushcfunction(L, ryos_lua_send);
	lua_setfield(L, -2, "send");
	lua_pushcfunction(L, ryos_lua_allowed);
	lua_setfield(L, -2, "allowed");
	lua_pushcfunction(L, ryos_lua_wait_for_allowance);
	lua_setfield(L, -2, "wait_for_allowance");
	lua_pushcfunction(L, ryos_lua_activate);
	lua_setfield(L, -2, "activate");
	lua_pushcfunction(L, ryos_lua_deactivate);
	lua_setfield(L, -2, "deactivate");
	lua_pushcfunction(L, ryos_lua_sleep);
	lua_setfield(L, -2, "sleep");
	lua_pushcfunction(L, ryos_lua_should_stop);
	lua_setfield(L, -2, "should_stop");

	lua_setglobal(L, "ryos");

	return L;
}

static gpointer worker(gpointer user_data) {
	RyosEffectLua *effect_lua = RYOS_EFFECT_LUA(user_data);
	RyosEffectLuaPrivate *priv = effect_lua->priv;
	int retval;

	if (!priv->module_name || !g_strcmp0(priv->module_name, ""))
		return NULL;

	priv->state = create_lua_state(effect_lua);
	if (!priv->state) {
		g_warning(_("Could not create Lua interpreter"));
		return NULL;
	}

	retval = luaL_dofile(priv->state, priv->module_name);
	if (retval) {
		g_warning(_("Could not run Lua script %s: %s"), priv->module_name, lua_tostring(priv->state, -1));
		lua_pop(priv->state, 1);
		ryos_effect_lua_disallow(effect_lua);
	}

	if (priv->state)
		g_clear_pointer(&priv->state, lua_close);

	return NULL;
}

static void start_worker(RyosEffectLua *self) {
	RyosEffectLuaPrivate *priv = self->priv;
	GError *local_error = NULL;

	g_assert(priv->thread == NULL);

	priv->stop = FALSE;

	priv->thread = gaminggear_thread_try_new(worker, self, &local_error);
	if (local_error) {
		g_warning(_("Could not create effect thread: %s"), local_error->message);
		g_clear_error(&local_error);
	}
}

/* Returns TRUE if a thread was running */
static gboolean stop_worker(RyosEffectLua *self) {
	RyosEffectLuaPrivate *priv = self->priv;
	gboolean retval;

	gaminggear_mutex_lock(&priv->mutex);
	priv->stop = TRUE;
	gaminggear_cond_signal(&priv->condition);
	gaminggear_mutex_unlock(&priv->mutex);

	if (priv->thread) {
		(void)g_thread_join(priv->thread);
		priv->thread = NULL;
		retval = TRUE;
	} else
		retval = FALSE;

	g_queue_clear(priv->queue);

	return retval;
}

void ryos_effect_lua_stop(RyosEffectLua *self) {
	RyosEffectLuaPrivate *priv = self->priv;
	GError *local_error = NULL;

	stop_worker(self);

	if (!ryos_effect_client_deinit(priv->effect_client, &local_error)) {
		g_warning(_("Could not deinit effect: %s"), local_error->message);
		g_clear_error(&local_error);
	}
}

void ryos_effect_lua_start(RyosEffectLua *self, gpointer client_data) {
	RyosEffectLuaPrivate *priv = self->priv;
	GError *local_error = NULL;

	if (priv->thread)
		return;

	priv->active = FALSE;

	if (!ryos_effect_client_init(priv->effect_client, client_data, &local_error)) {
		g_warning(_("Could not init effect: %s"), local_error->message);
		g_clear_error(&local_error);
	}
	
	start_worker(self);
}

void ryos_effect_lua_allow(RyosEffectLua *self) {
	RyosEffectLuaPrivate *priv = self->priv;
	
	gaminggear_mutex_lock(&priv->mutex);

	if (!priv->allowed) {
		priv->allowed = TRUE;
		activate_deactivate(self);
		gaminggear_cond_signal(&priv->condition);
	}
	gaminggear_mutex_unlock(&priv->mutex);
}

void ryos_effect_lua_disallow(RyosEffectLua *self) {
	RyosEffectLuaPrivate *priv = self->priv;

	gaminggear_mutex_lock(&priv->mutex);

	if (priv->allowed) {
		priv->allowed = FALSE;
		activate_deactivate(self);
	}

	gaminggear_mutex_unlock(&priv->mutex);
}

static void ryos_effect_lua_add_event(RyosEffectLua *self, guint event_type, guint value, gboolean action) {
	RyosEffectLuaPrivate *priv = self->priv;
	guint old_length;

	gaminggear_mutex_lock(&priv->mutex);

	old_length = g_queue_get_length(priv->queue);
	
	/* Limit size of queue like ringbuffer. */
	if (old_length == RYOS_EFFECT_LUA_QUEUE_MAX_LENGTH)
		(void)g_queue_pop_head(priv->queue);
	
	g_queue_push_tail(priv->queue, queue_value(event_type, value, action));
	
	if (old_length == 0)
		gaminggear_cond_signal(&priv->condition);
	gaminggear_mutex_unlock(&priv->mutex);
}

void ryos_effect_lua_key_event(RyosEffectLua *self, guint sdk_index, gboolean action) {
	ryos_effect_lua_add_event(self, RYOS_EFFECT_LUA_EVENT_TYPE_KEY, sdk_index, action);
}

G_DEFINE_TYPE(RyosEffectLua, ryos_effect_lua, G_TYPE_OBJECT);

RyosEffectLua *ryos_effect_lua_new(RyosEffectClient *effect_client) {
	RyosEffectLua *self = RYOS_EFFECT_LUA(g_object_new(RYOS_EFFECT_LUA_TYPE, NULL));
	self->priv->effect_client = effect_client; // TODO maybe property
	return self;
}

static void ryos_effect_lua_init(RyosEffectLua *effect_lua) {
	RyosEffectLuaPrivate *priv = RYOS_EFFECT_LUA_GET_PRIVATE(effect_lua);

	effect_lua->priv = priv;

	priv->module_name = NULL;
	priv->queue = g_queue_new();
	priv->stop = TRUE;
	priv->active = FALSE;
	priv->allowed = FALSE;
	priv->thread = NULL;

	gaminggear_mutex_init(&priv->mutex);
	gaminggear_cond_init(&priv->condition);
}

static void ryos_effect_lua_finalize(GObject *object) {
	RyosEffectLua *self = RYOS_EFFECT_LUA(object);
	RyosEffectLuaPrivate *priv = self->priv;

	ryos_effect_lua_stop(self);

	gaminggear_cond_clear(&priv->condition);
	gaminggear_mutex_clear(&priv->mutex);
	g_clear_pointer(&priv->queue, g_queue_free);
	g_free(priv->module_name);

	G_OBJECT_CLASS(ryos_effect_lua_parent_class)->finalize(object);
}

void ryos_effect_lua_set_module(RyosEffectLua *self, gchar const *name) {
	RyosEffectLuaPrivate *priv = self->priv;
	gboolean restart;

	restart = stop_worker(self);

	g_free(priv->module_name);
	priv->module_name = g_strdup(name);

	if (restart)
		start_worker(self);
}

static void ryos_effect_lua_class_init(RyosEffectLuaClass *klass) {
	GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

	gobject_class->finalize = ryos_effect_lua_finalize;

	g_type_class_add_private(klass, sizeof(RyosEffectLuaPrivate));
}

