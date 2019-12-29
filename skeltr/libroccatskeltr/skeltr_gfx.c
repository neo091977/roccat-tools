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

#include "skeltr_gfx.h"
#include "skeltr_light_control.h"
#include "skeltr_light_custom.h"
#include <gaminggear/gfx.h>
#include <string.h>

#define SKELTR_GFX_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), SKELTR_GFX_TYPE, SkeltrGfxPrivate))

struct _SkeltrGfxPrivate {
	RoccatDevice *device;
	SkeltrLightCustom *data;
};

enum {
	PROP_0,
	PROP_DEVICE,
};

G_DEFINE_TYPE(SkeltrGfx, skeltr_gfx, G_TYPE_OBJECT);

SkeltrGfx *skeltr_gfx_new(RoccatDevice * const device) {
	return SKELTR_GFX(g_object_new(SKELTR_GFX_TYPE,
			"device", device,
			NULL));
}

static void skeltr_gfx_init(SkeltrGfx *gfx) {
	gfx->priv = SKELTR_GFX_GET_PRIVATE(gfx);
}

static void skeltr_gfx_set_property(GObject *object, guint prop_id, GValue const *value, GParamSpec *pspec) {
	SkeltrGfxPrivate *priv = SKELTR_GFX(object)->priv;
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

static GObject *skeltr_gfx_constructor(GType gtype, guint n_properties, GObjectConstructParam *properties) {
	SkeltrGfxPrivate *priv;
	SkeltrGfx *gfx;
	GObject *obj;

	obj = G_OBJECT_CLASS(skeltr_gfx_parent_class)->constructor(gtype, n_properties, properties);
	gfx = SKELTR_GFX(obj);
	priv = gfx->priv;

	/* FIXME custom colors can't be read.
	 * Initial values would have to be taken from stored values which might not be
	 * correct
	 */
	priv->data = (SkeltrLightCustom *)g_malloc0(sizeof(SkeltrLightCustom));

	return obj;
}

static void skeltr_gfx_finalize(GObject *object) {
	SkeltrGfxPrivate *priv = SKELTR_GFX(object)->priv;
	g_object_unref(G_OBJECT(priv->device));
	g_free(priv->data);
	G_OBJECT_CLASS(skeltr_gfx_parent_class)->finalize(object);
}

static void skeltr_gfx_class_init(SkeltrGfxClass *gfx_class) {
	GObjectClass *gobject_class = (GObjectClass*)gfx_class;
	gobject_class->constructor = skeltr_gfx_constructor;
	gobject_class->finalize = skeltr_gfx_finalize;
	gobject_class->set_property = skeltr_gfx_set_property;

	g_type_class_add_private(gfx_class, sizeof(SkeltrGfxPrivate));

	g_object_class_install_property(gobject_class, PROP_DEVICE,
			g_param_spec_object("device",
					"device",
					"Device",
					ROCCAT_DEVICE_TYPE,
					G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));
}

gboolean skeltr_gfx_update(SkeltrGfx *gfx, GError **error) {
	SkeltrGfxPrivate *priv = gfx->priv;
	/* There's no way to activate custom mode on gfx start, so it's done
	 * every time custom data is sent */
	if (!skeltr_light_control_write(priv->device, SKELTR_LIGHT_CONTROL_MODE_CUSTOM, error))
		return FALSE;
	return skeltr_light_custom_write(priv->device, priv->data, error);
}

guint32 skeltr_gfx_get_color(SkeltrGfx *gfx, guint index) {
	SkeltrLightCustom *data = gfx->priv->data;
	guint32 color = 0;
	gfx_color_set_brightness(&color, 0xff);
	gfx_color_set_red(&color, data->colors[index].red);
	gfx_color_set_green(&color, data->colors[index].green);
	gfx_color_set_blue(&color, data->colors[index].blue);
	return color;
}

void skeltr_gfx_set_color(SkeltrGfx *gfx, guint index, guint32 color) {
	SkeltrLightCustom *data = gfx->priv->data;
	data->colors[index].red = gfx_color_get_red(color);
	data->colors[index].green = gfx_color_get_green(color);
	data->colors[index].blue = gfx_color_get_blue(color);
}
