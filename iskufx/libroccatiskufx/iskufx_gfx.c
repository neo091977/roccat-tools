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

#include "iskufx_gfx.h"
#include "talkfx.h"
#include <gaminggear/gfx.h>
#include <string.h>

#define ISKUFX_GFX_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), ISKUFX_GFX_TYPE, IskufxGfxPrivate))

struct _IskufxGfxPrivate {
	RoccatDevice *device;
	IskufxTalkfx *data;
};

enum {
	PROP_0,
	PROP_DEVICE,
};

G_DEFINE_TYPE(IskufxGfx, iskufx_gfx, G_TYPE_OBJECT);

IskufxGfx *iskufx_gfx_new(RoccatDevice * const device) {
	return ISKUFX_GFX(g_object_new(ISKUFX_GFX_TYPE,
			"device", device,
			NULL));
}

static void iskufx_gfx_init(IskufxGfx *gfx) {
	gfx->priv = ISKUFX_GFX_GET_PRIVATE(gfx);
}

static void iskufx_gfx_set_property(GObject *object, guint prop_id, GValue const *value, GParamSpec *pspec) {
	IskufxGfxPrivate *priv = ISKUFX_GFX(object)->priv;
	switch(prop_id) {
	case PROP_DEVICE:
		priv->device = ROCCAT_DEVICE(g_value_get_object(value));
		g_object_ref(G_OBJECT(priv->device));
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
		break;
	}
}

static GObject *iskufx_gfx_constructor(GType gtype, guint n_properties, GObjectConstructParam *properties) {
	IskufxGfxPrivate *priv;
	IskufxGfx *gfx;
	GObject *obj;

	obj = G_OBJECT_CLASS(iskufx_gfx_parent_class)->constructor(gtype, n_properties, properties);
	gfx = ISKUFX_GFX(obj);
	priv = gfx->priv;

	priv->data = iskufx_talkfx_read(priv->device, NULL);
	if (!priv->data)
		priv->data = (IskufxTalkfx *)g_malloc0(sizeof(IskufxTalkfx));

	priv->data->report_id = ISKU_REPORT_ID_TALK;
	priv->data->size = sizeof(IskufxTalkfx);
	priv->data->state = ROCCAT_TALKFX_STATE_ON;
	priv->data->zone = ROCCAT_TALKFX_ZONE_EVENT;
	priv->data->effect = ROCCAT_TALKFX_EFFECT_ON;
	priv->data->speed = ROCCAT_TALKFX_SPEED_SLOW;
	priv->data->unused[0] = 0;
	priv->data->unused[1] = 0;
	priv->data->unused[2] = 0;
	priv->data->unused[3] = 0;

	return obj;
}

static void iskufx_gfx_finalize(GObject *object) {
	IskufxGfxPrivate *priv = ISKUFX_GFX(object)->priv;
	g_object_unref(G_OBJECT(priv->device));
	g_free(priv->data);
	G_OBJECT_CLASS(iskufx_gfx_parent_class)->finalize(object);
}

static void iskufx_gfx_class_init(IskufxGfxClass *gfx_class) {
	GObjectClass *gobject_class = (GObjectClass*)gfx_class;
	gobject_class->constructor = iskufx_gfx_constructor;
	gobject_class->finalize = iskufx_gfx_finalize;
	gobject_class->set_property = iskufx_gfx_set_property;

	g_type_class_add_private(gfx_class, sizeof(IskufxGfxPrivate));

	g_object_class_install_property(gobject_class, PROP_DEVICE,
			g_param_spec_object("device",
					"device",
					"Device",
					ROCCAT_DEVICE_TYPE,
					G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));
}

gboolean iskufx_gfx_update(IskufxGfx *gfx, GError **error) {
	IskufxGfxPrivate *priv = gfx->priv;
	return iskufx_talkfx_write(priv->device, priv->data, error);
}

guint32 iskufx_gfx_get_color(IskufxGfx *gfx, guint index) {
	IskufxTalkfx *data = gfx->priv->data;
	guint32 color = 0;
	gfx_color_set_brightness(&color, 0xff);
	gfx_color_set_red(&color, data->event_red);
	gfx_color_set_green(&color, data->event_green);
	gfx_color_set_blue(&color, data->event_blue);
	return color;
}

void iskufx_gfx_set_color(IskufxGfx *gfx, guint index, guint32 color) {
	IskufxTalkfx *data = gfx->priv->data;
	data->event_red = gfx_color_get_red(color);
	data->event_green = gfx_color_get_green(color);
	data->event_blue = gfx_color_get_blue(color);
	data->ambient_red = data->event_red;
	data->ambient_green = data->event_green;
	data->ambient_blue = data->event_blue;
}
