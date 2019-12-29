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

#include "ryosmkfx_light_effects_emulator.h"
#include "ryosmkfx_effect_gui.h"
#include "ryosmkfx_light.h"
#include "ryos_effect_lua.h"
#include "g_roccat_helper.h"
#include "config.h"
#include <string.h>

enum {
	PRIORITY_IDLE = G_PRIORITY_DEFAULT_IDLE + 10,
	BLINK_DATA_LENGTH = 2,
	BREATHE_DATA_LENGTH = 10,
	HEARTBEAT_DATA_LENGTH = 20,
};

static gfloat blink_data[BLINK_DATA_LENGTH] = {
		0.000, 1.000,
};

static gfloat breathe_data[BREATHE_DATA_LENGTH] = {
		0.000, 0.095, 0.345, 0.655, 0.905, 1.000, 0.905, 0.655, 0.345, 0.095,
};

static gfloat heartbeat_data[HEARTBEAT_DATA_LENGTH] = {
		0.000, 0.095, 0.345, 0.655, 0.905, 1.000, 0.905, 0.655, 0.345, 0.491,
		0.678, 0.750, 0.678, 0.491, 0.259, 0.072, 0.000, 0.000, 0.000, 0.000,
};

#define RYOSMKFX_LIGHT_EFFECTS_EMULATOR_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), RYOSMKFX_LIGHT_EFFECTS_EMULATOR_TYPE, RyosmkfxLightEffectsEmulatorPrivate))

typedef struct _RyosmkfxLightEffectsEmulatorPrivateData RyosmkfxLightEffectsEmulatorPrivateData;

struct _RyosmkfxLightEffectsEmulatorPrivateData {
	RyosmkfxLightLayerData data_to_send;
};

struct _RyosmkfxLightEffectsEmulatorPrivate {
	RyosmkfxKeyboardSelector *keyboard_selector;

	gboolean running;
	guint effect;
	guint speed;

	RyosmkfxLightEffectsEmulatorPrivateData *data;

	RyosEffectLua *script_effect;
	guint timeout;
};

G_DEFINE_TYPE(RyosmkfxLightEffectsEmulator, ryosmkfx_light_effects_emulator, G_TYPE_OBJECT);

enum {
	TIMEOUT,
	LAST_SIGNAL,
};

static guint signals[LAST_SIGNAL] = { 0 };

RyosmkfxLightEffectsEmulator *ryosmkfx_light_effects_emulator_new(RyosmkfxKeyboardSelector *keyboard_selector) {
	RyosmkfxLightEffectsEmulator *emulator;
	RyosmkfxLightEffectsEmulatorPrivate *priv;

	emulator = RYOSMKFX_LIGHT_EFFECTS_EMULATOR(g_object_new(RYOSMKFX_LIGHT_EFFECTS_EMULATOR_TYPE, NULL));
	priv = emulator->priv;

	priv->keyboard_selector = keyboard_selector;
	g_object_ref(priv->keyboard_selector);

	return emulator;
}

static void ryosmkfx_light_effects_emulator_init(RyosmkfxLightEffectsEmulator *gaminggear_dev) {
	RyosmkfxLightEffectsEmulatorPrivate *priv = RYOSMKFX_LIGHT_EFFECTS_EMULATOR_GET_PRIVATE(gaminggear_dev);
	gaminggear_dev->priv = priv;
	priv->running = FALSE;
	priv->effect = G_MAXUINT;
	priv->speed = G_MAXUINT;
	priv->timeout = 0;
	priv->data = (RyosmkfxLightEffectsEmulatorPrivateData *)g_malloc(sizeof(RyosmkfxLightEffectsEmulatorPrivateData));

#ifdef LUA_FOUND
	priv->script_effect = RYOS_EFFECT_LUA(ryos_effect_lua_new(RYOS_EFFECT_CLIENT(ryosmkfx_effect_gui_new())));
#else
	priv->script_effect = NULL;
#endif
}

static void finalize(GObject *object) {
	RyosmkfxLightEffectsEmulatorPrivate *priv = RYOSMKFX_LIGHT_EFFECTS_EMULATOR(object)->priv;

	ryos_effect_lua_stop(priv->script_effect);
	g_clear_object(&priv->script_effect);

	g_clear_object(&priv->keyboard_selector);

	g_clear_pointer(&priv->data, g_free);

	G_OBJECT_CLASS(ryosmkfx_light_effects_emulator_parent_class)->finalize(object);
}

static void ryosmkfx_light_effects_emulator_class_init(RyosmkfxLightEffectsEmulatorClass *klass) {
	GObjectClass *gobject_class = (GObjectClass*)klass;

	gobject_class->finalize = finalize;

	g_type_class_add_private(klass, sizeof(RyosmkfxLightEffectsEmulatorPrivate));

	signals[TIMEOUT] = g_signal_new("timeout",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__FLOAT, G_TYPE_NONE,
			1, G_TYPE_FLOAT);
}

static gboolean temp_layer_data_idle_cb(gpointer data) {
	RyosmkfxLightEffectsEmulatorPrivate *priv = RYOSMKFX_LIGHT_EFFECTS_EMULATOR(data)->priv;
	/* gdk_thread lock is held here */

	ryosmkfx_keyboard_selector_effect_show_temp_layer_data(priv->keyboard_selector, &priv->data->data_to_send);

	return FALSE; /* G_SOURCE_REMOVE */
}

static void send_temp_layer_data(RyosmkfxLightEffectsEmulator *emulator) {
	gdk_threads_add_idle_full(PRIORITY_IDLE, temp_layer_data_idle_cb, emulator, NULL);
}

static gboolean stored_layer_data_idle_cb(gpointer data) {
	RyosmkfxLightEffectsEmulatorPrivate *priv = RYOSMKFX_LIGHT_EFFECTS_EMULATOR(data)->priv;
	/* gdk_thread lock is held here */

	ryosmkfx_keyboard_selector_effect_show_stored_layer_data(priv->keyboard_selector);

	return FALSE; /* G_SOURCE_REMOVE */
}

static void send_stored_layer_data(RyosmkfxLightEffectsEmulator *emulator) {
	gdk_threads_add_idle_full(PRIORITY_IDLE, stored_layer_data_idle_cb, emulator, NULL);
}

static void set_all_off(RyosmkfxLightEffectsEmulator *emulator) {
	RyosmkfxLightEffectsEmulatorPrivateData *data = emulator->priv->data;

	guint key_index;

	for (key_index = 0; key_index < RYOSMKFX_LIGHT_LAYER_KEYS_NUM; ++key_index)
		data->data_to_send.keys[key_index].state = FALSE;

	send_temp_layer_data(emulator);
}

static void set_stored(RyosmkfxLightEffectsEmulator *emulator) {
	send_stored_layer_data(emulator);
}

static void set_intensity(RyosmkfxLightEffectsEmulator *emulator, gfloat intensity) {
	g_signal_emit((gpointer)emulator, signals[TIMEOUT], 0, intensity);
}

static gboolean blink_timeout_cb(gpointer user_data) {
	static guint index = 0;

	set_intensity(RYOSMKFX_LIGHT_EFFECTS_EMULATOR(user_data), blink_data[index]);

	++index;
	if (index == BLINK_DATA_LENGTH)
		index = 0;

	return TRUE; /* G_SOURCE_CONTINUE */
}

static gboolean breathe_timeout_cb(gpointer user_data) {
	static guint index = 0;

	set_intensity(RYOSMKFX_LIGHT_EFFECTS_EMULATOR(user_data), breathe_data[index]);

	++index;
	if (index == BREATHE_DATA_LENGTH)
		index = 0;

	return TRUE; /* G_SOURCE_CONTINUE */
}

static gboolean heartbeat_timeout_cb(gpointer user_data) {
	static guint index = 0;

	set_intensity(RYOSMKFX_LIGHT_EFFECTS_EMULATOR(user_data), heartbeat_data[index]);

	++index;
	if (index == HEARTBEAT_DATA_LENGTH)
		index = 0;

	return TRUE; /* G_SOURCE_CONTINUE */
}

static void start(RyosmkfxLightEffectsEmulator *emulator) {
	RyosmkfxLightEffectsEmulatorPrivate *priv = emulator->priv;
	float period;

	/* only starting when allowed */
	if (!priv->running)
		return;

	/* script and timeout are not running */

	switch (priv->effect) {
	case RYOSMKFX_LIGHT_EFFECT_OFF:
		set_all_off(emulator);
		break;
	case RYOSMKFX_LIGHT_EFFECT_FULLY_LIT:
		set_stored(emulator);
		set_intensity(emulator, 1.0);
		break;
	case RYOSMKFX_LIGHT_EFFECT_BLINKING:
		set_stored(emulator);
		if (priv->speed != G_MAXUINT) {
			period = 0.6 * (RYOSMKFX_LIGHT_EFFECT_SPEED_MAX + 1 - priv->speed);
			priv->timeout = g_timeout_add(period / BLINK_DATA_LENGTH * G_ROCCAT_MSEC_PER_SEC, blink_timeout_cb, emulator);
		}
		break;
	case RYOSMKFX_LIGHT_EFFECT_BREATHING:
		set_stored(emulator);
		if (priv->speed != G_MAXUINT) {
			period = 1.5 + 1.3 * (RYOSMKFX_LIGHT_EFFECT_SPEED_MAX - priv->speed);
			priv->timeout = g_timeout_add(period / BREATHE_DATA_LENGTH * G_ROCCAT_MSEC_PER_SEC, breathe_timeout_cb, emulator);
		}
		break;
	case RYOSMKFX_LIGHT_EFFECT_HEARTBEAT:
		set_stored(emulator);
		if (priv->speed != G_MAXUINT) {
			period = 2 + 0.6 * (RYOSMKFX_LIGHT_EFFECT_SPEED_MAX - priv->speed);
			priv->timeout = g_timeout_add(period / HEARTBEAT_DATA_LENGTH * G_ROCCAT_MSEC_PER_SEC, heartbeat_timeout_cb, emulator);
		}
		break;
	case RYOSMKFX_LIGHT_EFFECT_RIPPLE_FX:
		set_all_off(emulator);
		set_intensity(emulator, 1.0);

		if (priv->script_effect) {
			ryos_effect_lua_start(priv->script_effect, emulator);
			ryos_effect_lua_allow(priv->script_effect);
		}
		break;
	default:
		break;
	}
}

static void stop(RyosmkfxLightEffectsEmulator *emulator) {
	RyosmkfxLightEffectsEmulatorPrivate *priv = emulator->priv;

	/* always stopping */

	if (priv->timeout) {
		g_source_remove(priv->timeout);
		priv->timeout = 0;
	}

	if (priv->script_effect) {
		ryos_effect_lua_disallow(priv->script_effect);
		ryos_effect_lua_stop(priv->script_effect);
	}
}

void ryosmkfx_light_effects_emulator_set_script(RyosmkfxLightEffectsEmulator *emulator, gchar const *script) {
	RyosmkfxLightEffectsEmulatorPrivate *priv = emulator->priv;

	if (priv->effect == RYOSMKFX_LIGHT_EFFECT_RIPPLE_FX)
		set_all_off(emulator);

	if (priv->script_effect)
		ryos_effect_lua_set_module(priv->script_effect, script);
}

void ryosmkfx_light_effects_emulator_set_state(RyosmkfxLightEffectsEmulator *emulator, gboolean run) {
	RyosmkfxLightEffectsEmulatorPrivate *priv = emulator->priv;

	if (run != priv->running) {
		priv->running = run;
		if (priv->running) {
			start(emulator);
		} else {
			stop(emulator);
			set_stored(emulator);
			set_intensity(emulator, 1.0);
		}
	}
}

void ryosmkfx_light_effects_emulator_set_effect(RyosmkfxLightEffectsEmulator *emulator, guint effect) {
	RyosmkfxLightEffectsEmulatorPrivate *priv = emulator->priv;

	if (effect != priv->effect) {
		priv->effect = effect;
		stop(emulator);
		start(emulator);
	}
}

void ryosmkfx_light_effects_emulator_set_speed(RyosmkfxLightEffectsEmulator *emulator, guint speed) {
	RyosmkfxLightEffectsEmulatorPrivate *priv = emulator->priv;

	if (speed != priv->speed){
		priv->speed = speed;
		stop(emulator);
		start(emulator);
	}
}

void ryosmkfx_light_effects_emulator_key_event(RyosmkfxLightEffectsEmulator *emulator, guint sdk_index, gboolean action) {
	RyosmkfxLightEffectsEmulatorPrivate *priv = emulator->priv;

	if (priv->script_effect)
		ryos_effect_lua_key_event(priv->script_effect, sdk_index, action);
}

void ryosmkfx_light_effects_emulator_set_layer_data_temp(RyosmkfxLightEffectsEmulator *emulator, RyosmkfxLightLayerData const *data) {
	RyosmkfxLightEffectsEmulatorPrivate *priv = emulator->priv;

	if (priv->running) {
		memcpy(&priv->data->data_to_send, data, sizeof(RyosmkfxLightLayerData));
		send_temp_layer_data(emulator);
	}
}

gboolean ryosmkfx_light_effects_emulator_uses_temp_layer_data(RyosmkfxLightEffectsEmulator *emulator) {
	RyosmkfxLightEffectsEmulatorPrivate *priv = emulator->priv;
	return (priv->running && (priv->effect == RYOSMKFX_LIGHT_EFFECT_RIPPLE_FX || priv->effect == RYOSMKFX_LIGHT_EFFECT_OFF));
}
