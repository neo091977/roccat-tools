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

#include "ryosconfig_blink.h"
#include "g_roccat_helper.h"
#include <math.h>

#define RYOSCONFIG_BLINK_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), RYOSCONFIG_BLINK_TYPE, RyosconfigBlinkPrivate))

struct _RyosconfigBlinkPrivate {
	guint timeout;
	gboolean blink_state;
};

G_DEFINE_TYPE(RyosconfigBlink, ryosconfig_blink, G_TYPE_OBJECT);

enum {
	TIMEOUT,
	LAST_SIGNAL,
};

static guint signals[LAST_SIGNAL] = { 0 };

static gboolean timeout_cb(gpointer user_data) {
	RyosconfigBlink *blink = RYOSCONFIG_BLINK(user_data);
	RyosconfigBlinkPrivate *priv = blink->priv;

	priv->blink_state = !priv->blink_state;

	g_signal_emit((gpointer)blink, signals[TIMEOUT], 0, priv->blink_state);

	return TRUE;
}

void ryosconfig_blink_start(RyosconfigBlink *blink) {
	RyosconfigBlinkPrivate *priv = blink->priv;

	if (priv->timeout)
		return;
	priv->timeout = g_timeout_add(0.5 * G_ROCCAT_MSEC_PER_SEC, timeout_cb, blink); // FIXME check
}

void ryosconfig_blink_stop(RyosconfigBlink *blink) {
	RyosconfigBlinkPrivate *priv = blink->priv;

	if (!priv->timeout)
		return;
	g_source_remove(priv->timeout);
	priv->timeout = 0;
}

RyosconfigBlink *ryosconfig_blink_new(void) {
	return RYOSCONFIG_BLINK(g_object_new(RYOSCONFIG_BLINK_TYPE, NULL));
}

static void ryosconfig_blink_init(RyosconfigBlink *gaminggear_dev) {
	RyosconfigBlinkPrivate *priv = RYOSCONFIG_BLINK_GET_PRIVATE(gaminggear_dev);
	gaminggear_dev->priv = priv;
	priv->timeout = 0;
	priv->blink_state = FALSE;
}

static void ryosconfig_blink_finalize(GObject *object) {
	ryosconfig_blink_stop(RYOSCONFIG_BLINK(object));
	G_OBJECT_CLASS(ryosconfig_blink_parent_class)->finalize(object);
}

static void ryosconfig_blink_class_init(RyosconfigBlinkClass *klass) {
	GObjectClass *gobject_class = (GObjectClass*)klass;

	gobject_class->finalize = ryosconfig_blink_finalize;

	g_type_class_add_private(klass, sizeof(RyosconfigBlinkPrivate));

	signals[TIMEOUT] = g_signal_new("timeout",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__BOOLEAN, G_TYPE_NONE,
			1, G_TYPE_BOOLEAN);
}
