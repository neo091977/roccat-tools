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

#include "ryostkl_led_macro_thread.h"
#include "ryos_custom_lights.h"
#include "ryos_light_control.h"
#include "ryos_key_relations.h"
#include "g_roccat_helper.h"
#include "roccat_helper.h"
#include "i18n-lib.h"
#include <gaminggear/threads.h>

#define RYOSTKL_LED_MACRO_THREAD_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), RYOSTKL_LED_MACRO_THREAD_TYPE, RyostklLedMacroThreadClass))
#define IS_RYOSTKL_LED_MACRO_THREAD_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), RYOSTKL_LED_MACRO_THREAD_TYPE))
#define RYOSTKL_LED_MACRO_THREAD_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), RYOSTKL_LED_MACRO_THREAD_TYPE, RyostklLedMacroThreadPrivate))

typedef struct _RyostklLedMacroThreadClass RyostklLedMacroThreadClass;

struct _RyostklLedMacroThreadClass {
	GObjectClass parent_class;
};

struct _RyostklLedMacroThreadPrivate {
	GThread *thread;

	GaminggearMutex mutex;
	GaminggearCond condition;
	gboolean cancelled;

	gint running;

	RoccatDevice *device;
	RyostklLightLayer *light_layer;
	RyosLedMacro *led_macro;
};

G_DEFINE_TYPE(RyostklLedMacroThread, ryostkl_led_macro_thread, G_TYPE_OBJECT);

static void ryostkl_led_macro_thread_set_running(RyostklLedMacroThread *ryostkl_led_macro_thread, gboolean value) {
	g_atomic_int_set(&ryostkl_led_macro_thread->priv->running, value);
}

gboolean ryostkl_led_macro_thread_get_running(RyostklLedMacroThread *ryostkl_led_macro_thread) {
	return g_atomic_int_get(&ryostkl_led_macro_thread->priv->running);
}

void ryostkl_led_macro_thread_set_cancelled(RyostklLedMacroThread *ryostkl_led_macro_thread) {
	RyostklLedMacroThreadPrivate *priv = ryostkl_led_macro_thread->priv;
	gaminggear_mutex_lock(&priv->mutex);
	priv->cancelled = TRUE;
	gaminggear_cond_signal(&priv->condition);
	gaminggear_mutex_unlock(&priv->mutex);
}

gboolean ryostkl_led_macro_thread_get_cancelled(RyostklLedMacroThread *ryostkl_led_macro_thread) {
	RyostklLedMacroThreadPrivate *priv = ryostkl_led_macro_thread->priv;
	gboolean retval;
	gaminggear_mutex_lock(&priv->mutex);
	retval = priv->cancelled;
	gaminggear_mutex_unlock(&priv->mutex);
	return retval;
}

static gpointer thread(gpointer user_data) {
	RyostklLedMacroThread *macro_thread = RYOSTKL_LED_MACRO_THREAD(user_data);
	RyostklLedMacroThreadPrivate *priv = macro_thread->priv;
	GError *local_error = NULL;
	guint key_count, key_index;
	guint loop_count, loop;
	guint8 light_layer_index;
	RyosCustomLights custom_lights;
	RoccatKeystroke const *keystroke;
	gboolean state;

	g_debug(_("Playing LED macro %s/%s"), priv->led_macro->macroset_name, priv->led_macro->macro_name);

	if (!ryos_light_control_custom(priv->device, TRUE, &local_error)) {
		g_warning(_("Could not init custom mode: %s"), local_error->message);
		g_clear_error(&local_error);
		goto exit;
	}

	ryostkl_light_layer_to_custom_lights(priv->light_layer, &custom_lights);
	if (!ryos_custom_lights_write(priv->device, &custom_lights, &local_error)) {
		g_warning(_("Could not write custom lights: %s"), local_error->message);
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

			state = ryostkl_light_layer_key_get_state(&priv->light_layer->keys[light_layer_index]);
			ryostkl_light_layer_key_set_state(&priv->light_layer->keys[light_layer_index], !state);

			ryostkl_light_layer_to_custom_lights(priv->light_layer, &custom_lights);
			if (!ryos_custom_lights_write(priv->device, &custom_lights, &local_error)) {
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
	ryostkl_led_macro_thread_set_running(macro_thread, FALSE);
	return NULL;
}

RyostklLedMacroThread *ryostkl_led_macro_thread_new(RoccatDevice *device, RyostklLightLayer const *light_layer, RyosLedMacro const *led_macro) {
	RyostklLedMacroThread *macro_thread;
	RyostklLedMacroThreadPrivate *priv;
	GError *local_error = NULL;

	macro_thread = RYOSTKL_LED_MACRO_THREAD(g_object_new(RYOSTKL_LED_MACRO_THREAD_TYPE, NULL));
	priv = macro_thread->priv;

	priv->device = device;
	priv->light_layer = ryostkl_light_layer_dup(light_layer);
	priv->led_macro = ryos_led_macro_dup(led_macro);
	priv->cancelled = FALSE;
	ryostkl_led_macro_thread_set_running(macro_thread, TRUE);

	priv->thread = gaminggear_thread_try_new(thread, macro_thread, &local_error);

	if (local_error) {
		g_warning(_("Could not create LED macro thread: %s"), local_error->message);
		g_clear_error(&local_error);
	}

	return macro_thread;
}

static void ryostkl_led_macro_thread_init(RyostklLedMacroThread *macro_thread_data) {
	RyostklLedMacroThreadPrivate *priv = RYOSTKL_LED_MACRO_THREAD_GET_PRIVATE(macro_thread_data);
	macro_thread_data->priv = priv;

	gaminggear_mutex_init(&priv->mutex);
	gaminggear_cond_init(&priv->condition);
}

static void ryostkl_led_macro_thread_finalize(GObject *object) {
	RyostklLedMacroThreadPrivate *priv = RYOSTKL_LED_MACRO_THREAD(object)->priv;

	if (priv->thread)
		g_thread_join(priv->thread);

	g_free(priv->light_layer);
	g_free(priv->led_macro);

	gaminggear_mutex_clear(&priv->mutex);
	gaminggear_cond_clear(&priv->condition);

	G_OBJECT_CLASS(ryostkl_led_macro_thread_parent_class)->finalize(object);
}

static void ryostkl_led_macro_thread_class_init(RyostklLedMacroThreadClass *klass) {
	GObjectClass *gobject_class;

	gobject_class = (GObjectClass*)klass;

	gobject_class->finalize = ryostkl_led_macro_thread_finalize;

	g_type_class_add_private(klass, sizeof(RyostklLedMacroThreadPrivate));
}
