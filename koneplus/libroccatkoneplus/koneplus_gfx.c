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

#include "koneplus_gfx.h"
#include "talkfx.h"
#include <gaminggear/gfx.h>
#include <string.h>

#define KONEPLUS_GFX_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), KONEPLUS_GFX_TYPE, KoneplusGfxPrivate))

struct _KoneplusGfxPrivate {
	RoccatDevice *device;
	KoneplusTalk *data;
};

enum {
	PROP_0,
	PROP_DEVICE,
};

G_DEFINE_TYPE(KoneplusGfx, koneplus_gfx, G_TYPE_OBJECT);

KoneplusGfx *koneplus_gfx_new(RoccatDevice * const device) {
	return KONEPLUS_GFX(g_object_new(KONEPLUS_GFX_TYPE,
			"device", device,
			NULL));
}

static void koneplus_gfx_init(KoneplusGfx *gfx) {
	gfx->priv = KONEPLUS_GFX_GET_PRIVATE(gfx);
}

static void koneplus_gfx_set_property(GObject *object, guint prop_id, GValue const *value, GParamSpec *pspec) {
	KoneplusGfxPrivate *priv = KONEPLUS_GFX(object)->priv;
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

static GObject *koneplus_gfx_constructor(GType gtype, guint n_properties, GObjectConstructParam *properties) {
	KoneplusGfxPrivate *priv;
	KoneplusGfx *gfx;
	GObject *obj;

	obj = G_OBJECT_CLASS(koneplus_gfx_parent_class)->constructor(gtype, n_properties, properties);
	gfx = KONEPLUS_GFX(obj);
	priv = gfx->priv;

	priv->data = koneplus_talkfx_read(priv->device, NULL);
	if (!priv->data)
		priv->data = (KoneplusTalk *)g_malloc0(sizeof(KoneplusTalk));

	priv->data->report_id = KONEPLUS_REPORT_ID_TALK;
	priv->data->size = sizeof(KoneplusTalk);
	priv->data->fx_status = ROCCAT_TALKFX_STATE_ON;
	priv->data->zone = ROCCAT_TALKFX_ZONE_EVENT;
	priv->data->unused = 0;
	priv->data->effect = ROCCAT_TALKFX_EFFECT_ON;
	priv->data->speed = ROCCAT_TALKFX_SPEED_SLOW;

	return obj;
}

static void koneplus_gfx_finalize(GObject *object) {
	KoneplusGfxPrivate *priv = KONEPLUS_GFX(object)->priv;
	g_object_unref(G_OBJECT(priv->device));
	g_free(priv->data);
	G_OBJECT_CLASS(koneplus_gfx_parent_class)->finalize(object);
}

static void koneplus_gfx_class_init(KoneplusGfxClass *gfx_class) {
	GObjectClass *gobject_class = (GObjectClass*)gfx_class;
	gobject_class->constructor = koneplus_gfx_constructor;
	gobject_class->finalize = koneplus_gfx_finalize;
	gobject_class->set_property = koneplus_gfx_set_property;

	g_type_class_add_private(gfx_class, sizeof(KoneplusGfxPrivate));

	g_object_class_install_property(gobject_class, PROP_DEVICE,
			g_param_spec_object("device",
					"device",
					"Device",
					ROCCAT_DEVICE_TYPE,
					G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));
}

gboolean koneplus_gfx_update(KoneplusGfx *gfx, GError **error) {
	KoneplusGfxPrivate *priv = gfx->priv;
	return koneplus_talkfx_write(priv->device, priv->data, error);
}

guint32 koneplus_gfx_get_color(KoneplusGfx *gfx, guint index) {
	KoneplusTalk *data = gfx->priv->data;
	guint32 color = 0;
	gfx_color_set_brightness(&color, 0xff);
	gfx_color_set_red(&color, data->event_red);
	gfx_color_set_green(&color, data->event_green);
	gfx_color_set_blue(&color, data->event_blue);
	return color;
}

void koneplus_gfx_set_color(KoneplusGfx *gfx, guint index, guint32 color) {
	KoneplusTalk *data = gfx->priv->data;
	data->event_red = gfx_color_get_red(color);
	data->event_green = gfx_color_get_green(color);
	data->event_blue = gfx_color_get_blue(color);
	data->ambient_red = data->event_red;
	data->ambient_green = data->event_green;
	data->ambient_blue = data->event_blue;
}
