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

#include "ryosmkfx_led_macro_thread.h"
#include "ryosmkfx_custom_lights.h"
#include "ryosmkfx_stored_lights.h"
#include "ryos_light_control.h"
#include "ryos_key_relations.h"
#include "g_roccat_helper.h"
#include "roccat_helper.h"
#include "i18n-lib.h"
#include <gaminggear/threads.h>

#define RYOSMKFX_LED_MACRO_THREAD_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), RYOSMKFX_LED_MACRO_THREAD_TYPE, RyosmkfxLedMacroThreadClass))
#define IS_RYOSMKFX_LED_MACRO_THREAD_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), RYOSMKFX_LED_MACRO_THREAD_TYPE))
#define RYOSMKFX_LED_MACRO_THREAD_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), RYOSMKFX_LED_MACRO_THREAD_TYPE, RyosmkfxLedMacroThreadPrivate))

typedef struct _RyosmkfxLedMacroThreadClass RyosmkfxLedMacroThreadClass;

struct _RyosmkfxLedMacroThreadClass {
	GObjectClass parent_class;
};

struct _RyosmkfxLedMacroThreadPrivate {
	GThread *thread;

	GaminggearMutex mutex;
	GaminggearCond condition;
	gboolean cancelled;

	gint running;

	RoccatDevice *device;
	RyosLedMacro *led_macro;
};

G_DEFINE_TYPE(RyosmkfxLedMacroThread, ryosmkfx_led_macro_thread, G_TYPE_OBJECT);

static void ryosmkfx_led_macro_thread_set_running(RyosmkfxLedMacroThread *ryosmkfx_led_macro_thread, gboolean value) {
	g_atomic_int_set(&ryosmkfx_led_macro_thread->priv->running, value);
}

gboolean ryosmkfx_led_macro_thread_get_running(RyosmkfxLedMacroThread *ryosmkfx_led_macro_thread) {
	return g_atomic_int_get(&ryosmkfx_led_macro_thread->priv->running);
}

void ryosmkfx_led_macro_thread_set_cancelled(RyosmkfxLedMacroThread *ryosmkfx_led_macro_thread) {
	RyosmkfxLedMacroThreadPrivate *priv = ryosmkfx_led_macro_thread->priv;
	gaminggear_mutex_lock(&priv->mutex);
	priv->cancelled = TRUE;
	gaminggear_cond_signal(&priv->condition);
	gaminggear_mutex_unlock(&priv->mutex);
}

gboolean ryosmkfx_led_macro_thread_get_cancelled(RyosmkfxLedMacroThread *ryosmkfx_led_macro_thread) {
	RyosmkfxLedMacroThreadPrivate *priv = ryosmkfx_led_macro_thread->priv;
	gboolean retval;
	gaminggear_mutex_lock(&priv->mutex);
	retval = priv->cancelled;
	gaminggear_mutex_unlock(&priv->mutex);
	return retval;
}

static gpointer thread(gpointer user_data) {
	RyosmkfxLedMacroThread *macro_thread = RYOSMKFX_LED_MACRO_THREAD(user_data);
	RyosmkfxLedMacroThreadPrivate *priv = macro_thread->priv;
	GError *local_error = NULL;
	guint key_count, key_index;
	guint loop_count, loop;
	guint8 light_layer_index;
	RyosmkfxCustomLights custom_lights;
	RyosmkfxLightLayer *light_layer;
	RoccatKeystroke const *keystroke;

	g_debug(_("Playing LED macro %s/%s"), priv->led_macro->macroset_name, priv->led_macro->macro_name);

	light_layer = ryosmkfx_get_active_light_layer(priv->device, &local_error);
	if (light_layer == NULL) {
		g_warning(_("Could not get active light layer: %s"), local_error->message);
		g_clear_error(&local_error);
		goto exit;
	}

	custom_lights.light_layer = *light_layer;
	g_free(light_layer);

	if (!ryosmkfx_custom_lights_write(priv->device, &custom_lights, &local_error)) {
		g_warning(_("Could not write custom lights: %s"), local_error->message);
		g_clear_error(&local_error);
		goto exit;
	}

	if (!ryos_light_control_custom(priv->device, TRUE, &local_error)) {
		g_warning(_("Could not init custom mode: %s"), local_error->message);
		g_clear_error(&local_error);
		goto exit;
	}

	key_count = ryos_led_macro_get_count(priv->led_macro);
	loop_count = priv->led_macro->loop;

	for (loop = 0; loop < loop_count; ++loop) {
		for (key_index = 0; key_index < key_count; ++key_index) {

			gaminggear_mutex_lock(&priv->mutex);
			if (priv->cancelled) {
				g_debug(_("Cancelling LED macro %s/%s"), priv->led_macro->macroset_name, priv->led_macro->macro_name);
				gaminggear_mutex_unlock(&priv->mutex);
				goto exit;
			}
			gaminggear_mutex_unlock(&priv->mutex);

			keystroke = &priv->led_macro->keystrokes[key_index];
			light_layer_index = ryos_hid_to_layer_index[keystroke->key];

			roccat_bitfield_toggle_bit(custom_lights.light_layer.states, light_layer_index);

			if (!ryosmkfx_custom_lights_write(priv->device, &custom_lights, &local_error)) {
				g_warning(_("Could not write custom lights: %s"), local_error->message);
				g_clear_error(&local_error);
				goto exit;
			}

			/* Skip wait of last key in last loop */
			if (key_index != key_count - 1 || loop != loop_count - 1)
				g_usleep(roccat_keystroke_get_period(keystroke) * G_ROCCAT_USEC_PER_MSEC);
		}
	}

	if (!ryos_light_control_custom(priv->device, FALSE, &local_error)) {
		g_warning(_("Could not deinit custom mode: %s"), local_error->message);
		g_clear_error(&local_error);
	}

	g_debug(_("Finished LED macro %s/%s"), priv->led_macro->macroset_name, priv->led_macro->macro_name);
exit:
	ryosmkfx_led_macro_thread_set_running(macro_thread, FALSE);
	return NULL;
}

RyosmkfxLedMacroThread *ryosmkfx_led_macro_thread_new(RoccatDevice *device, RyosLedMacro const *led_macro) {
	RyosmkfxLedMacroThread *macro_thread;
	RyosmkfxLedMacroThreadPrivate *priv;
	GError *local_error = NULL;

	macro_thread = RYOSMKFX_LED_MACRO_THREAD(g_object_new(RYOSMKFX_LED_MACRO_THREAD_TYPE, NULL));
	priv = macro_thread->priv;

	priv->device = device;
	priv->led_macro = ryos_led_macro_dup(led_macro);
	priv->cancelled = FALSE;
	ryosmkfx_led_macro_thread_set_running(macro_thread, TRUE);

	priv->thread = gaminggear_thread_try_new(thread, macro_thread, &local_error);

	if (local_error) {
		g_warning(_("Could not create LED macro thread: %s"), local_error->message);
		g_clear_error(&local_error);
	}

	return macro_thread;
}

static void ryosmkfx_led_macro_thread_init(RyosmkfxLedMacroThread *macro_thread_data) {
	RyosmkfxLedMacroThreadPrivate *priv = RYOSMKFX_LED_MACRO_THREAD_GET_PRIVATE(macro_thread_data);
	macro_thread_data->priv = priv;

	gaminggear_mutex_init(&priv->mutex);
	gaminggear_cond_init(&priv->condition);
}

static void ryosmkfx_led_macro_thread_finalize(GObject *object) {
	RyosmkfxLedMacroThreadPrivate *priv = RYOSMKFX_LED_MACRO_THREAD(object)->priv;

	if (priv->thread)
		g_thread_join(priv->thread);

	g_free(priv->led_macro);

	gaminggear_mutex_clear(&priv->mutex);
	gaminggear_cond_clear(&priv->condition);

	G_OBJECT_CLASS(ryosmkfx_led_macro_thread_parent_class)->finalize(object);
}

static void ryosmkfx_led_macro_thread_class_init(RyosmkfxLedMacroThreadClass *klass) {
	GObjectClass *gobject_class;

	gobject_class = (GObjectClass*)klass;

	gobject_class->finalize = ryosmkfx_led_macro_thread_finalize;

	g_type_class_add_private(klass, sizeof(RyosmkfxLedMacroThreadPrivate));
}
