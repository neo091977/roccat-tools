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

#include "ryostklconfig_blink.h"
#include "g_roccat_helper.h"
#include <math.h>

#define RYOSTKLCONFIG_BLINK_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), RYOSTKLCONFIG_BLINK_TYPE, RyostklconfigBlinkPrivate))

struct _RyostklconfigBlinkPrivate {
	guint timeout;
	guint count;
};

G_DEFINE_TYPE(RyostklconfigBlink, ryostklconfig_blink, G_TYPE_OBJECT);

enum {
	TIMEOUT,
	LAST_SIGNAL,
};

static guint signals[LAST_SIGNAL] = { 0 };

static gboolean timeout_cb(gpointer user_data) {
	RyostklconfigBlink *blink = RYOSTKLCONFIG_BLINK(user_data);
	RyostklconfigBlinkPrivate *priv = blink->priv;

	++priv->count;
	if (priv->count == 1 * 2 * 3 * 4 * 5 * 16)
		priv->count = 0;

	g_signal_emit((gpointer)blink, signals[TIMEOUT], 0);

	return TRUE;
}

void ryostklconfig_blink_start(RyostklconfigBlink *blink) {
	RyostklconfigBlinkPrivate *priv = blink->priv;

	if (priv->timeout)
		return;
	priv->timeout = g_timeout_add(0.1 * G_ROCCAT_MSEC_PER_SEC, timeout_cb, blink);
}

void ryostklconfig_blink_stop(RyostklconfigBlink *blink) {
	RyostklconfigBlinkPrivate *priv = blink->priv;

	if (!priv->timeout)
		return;
	g_source_remove(priv->timeout);
	priv->timeout = 0;
}

gboolean ryostklconfig_blink_get_light_layer_key_state(RyostklconfigBlink const *blink, guint16 const *key) {
	return ryostkl_light_layer_key_get_state(key);
}

gfloat ryostklconfig_blink_get_light_layer_key_intensity(RyostklconfigBlink const *blink, guint16 const *key) {
	gboolean on;
	gboolean effect;
	guint speed;

	on = ryostkl_light_layer_key_get_state(key);
	if (on) {
		effect = ryostkl_light_layer_key_get_effect(key);
		if (effect == RYOSTKL_LIGHT_LAYER_KEY_EFFECT_BLINK) {
			speed = ryostkl_light_layer_key_get_blink_speed(key);
			if (speed)
				return ((blink->priv->count / speed) % 2) ? 1.0 : 0.0;
			else
				return 1.0;
		} else if (effect == RYOSTKL_LIGHT_LAYER_KEY_EFFECT_BREATH) {
			speed = ryostkl_light_layer_key_get_breath_speed(key);
			if (speed)
				return fabs((gfloat)((blink->priv->count / speed) % 16) / 7.5 - 1.0);
			else
				return 1.0;
		} else
			return 1.0;
	} else
		return 0.0;
}

void ryostklconfig_blink_light_layer_to_custom_lights(RyostklconfigBlink const *blink, RyostklLightLayer const *light_layer, RyosCustomLights *custom_lights) {
	guint i;
	gfloat intensity;

	for (i = 0; i < RYOS_KEYS_PRIMARY_NUM; ++i) {
		intensity = ryostklconfig_blink_get_light_layer_key_intensity(blink, &light_layer->keys[i]);
		roccat_bitfield_set_bit(&custom_lights->data[0], i, intensity > 0.5);
	}
}

RyostklconfigBlink *ryostklconfig_blink_new(void) {
	return RYOSTKLCONFIG_BLINK(g_object_new(RYOSTKLCONFIG_BLINK_TYPE, NULL));
}

static void ryostklconfig_blink_init(RyostklconfigBlink *gaminggear_dev) {
	RyostklconfigBlinkPrivate *priv = RYOSTKLCONFIG_BLINK_GET_PRIVATE(gaminggear_dev);
	gaminggear_dev->priv = priv;
	priv->timeout = 0;
	priv->count = 0;
}

static void ryostklconfig_blink_finalize(GObject *object) {
	ryostklconfig_blink_stop(RYOSTKLCONFIG_BLINK(object));
	G_OBJECT_CLASS(ryostklconfig_blink_parent_class)->finalize(object);
}

static void ryostklconfig_blink_class_init(RyostklconfigBlinkClass *klass) {
	GObjectClass *gobject_class = (GObjectClass*)klass;

	gobject_class->finalize = ryostklconfig_blink_finalize;

	g_type_class_add_private(klass, sizeof(RyostklconfigBlinkPrivate));

	signals[TIMEOUT] = g_signal_new("timeout",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);
}
