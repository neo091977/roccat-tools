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

#include "ryosmkfx_gfx.h"
#include "ryosmkfx_device.h"
#include "talkfx.h"
#include <gaminggear/gfx.h>
#include <string.h>

#define RYOSMKFX_GFX_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), RYOSMKFX_GFX_TYPE, RyosmkfxGfxPrivate))

typedef struct _RyosmkfxGfxColor RyosmkfxGfxColor;
typedef struct _RyosmkfxGfxData RyosmkfxGfxData;

struct _RyosmkfxGfxData {
	guint8 report_id; /* RYOS_REPORT_ID_TALK */
	guint8 size; /* always 10 */
	guint8 easyshift;
	guint8 easyshift_lock;
	guint8 red_low;
	guint8 green_low;
	guint8 blue_low;
	guint8 red_high;
	guint8 green_high;
	guint8 blue_high;
	guint8 unknown1; /* 0x01 */
	guint8 unknown2; /* 0x00 */
	guint8 unknown3; /* 0x01 */
	guint8 unknown4; /* 0x02 */
	guint8 unknown5; /* 0x01 */
	guint8 unknown6; /* 0x00 */
} __attribute__ ((packed));

struct _RyosmkfxGfxPrivate {
	RoccatDevice *device;
	RyosmkfxGfxData *data;
};

enum {
	PROP_0,
	PROP_DEVICE,
};

G_DEFINE_TYPE(RyosmkfxGfx, ryosmkfx_gfx, G_TYPE_OBJECT);

RyosmkfxGfx *ryosmkfx_gfx_new(RoccatDevice * const device) {
	return RYOSMKFX_GFX(g_object_new(RYOSMKFX_GFX_TYPE,
			"device", device,
			NULL));
}

static void ryosmkfx_gfx_init(RyosmkfxGfx *gfx) {
	gfx->priv = RYOSMKFX_GFX_GET_PRIVATE(gfx);
}

static void ryosmkfx_gfx_set_property(GObject *object, guint prop_id, GValue const *value, GParamSpec *pspec) {
	RyosmkfxGfxPrivate *priv = RYOSMKFX_GFX(object)->priv;
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

static GObject *ryosmkfx_gfx_constructor(GType gtype, guint n_properties, GObjectConstructParam *properties) {
	RyosmkfxGfxPrivate *priv;
	RyosmkfxGfx *gfx;
	GObject *obj;

	obj = G_OBJECT_CLASS(ryosmkfx_gfx_parent_class)->constructor(gtype, n_properties, properties);
	gfx = RYOSMKFX_GFX(obj);
	priv = gfx->priv;

	priv->data = (RyosmkfxGfxData *)g_malloc0(sizeof(RyosmkfxGfxData));

	priv->data->report_id = RYOS_REPORT_ID_TALK;
	priv->data->size = sizeof(RyosmkfxGfxData);
	priv->data->unknown1 = 1;
	priv->data->unknown2 = 0;
	priv->data->unknown3 = 1;
	priv->data->unknown4 = 2;
	priv->data->unknown5 = 1;
	priv->data->unknown6 = 0;

	return obj;
}

static void ryosmkfx_gfx_finalize(GObject *object) {
	RyosmkfxGfxPrivate *priv = RYOSMKFX_GFX(object)->priv;
	g_object_unref(G_OBJECT(priv->device));
	g_free(priv->data);
	G_OBJECT_CLASS(ryosmkfx_gfx_parent_class)->finalize(object);
}

static void ryosmkfx_gfx_class_init(RyosmkfxGfxClass *gfx_class) {
	GObjectClass *gobject_class = (GObjectClass*)gfx_class;
	gobject_class->constructor = ryosmkfx_gfx_constructor;
	gobject_class->finalize = ryosmkfx_gfx_finalize;
	gobject_class->set_property = ryosmkfx_gfx_set_property;

	g_type_class_add_private(gfx_class, sizeof(RyosmkfxGfxPrivate));

	g_object_class_install_property(gobject_class, PROP_DEVICE,
			g_param_spec_object("device",
					"device",
					"Device",
					ROCCAT_DEVICE_TYPE,
					G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));
}

gboolean ryosmkfx_gfx_update(RyosmkfxGfx *gfx, GError **error) {
	RyosmkfxGfxPrivate *priv = gfx->priv;
	return ryos_device_write(priv->device, (gchar const *)priv->data, sizeof(RyosmkfxGfxData), error);
}

guint32 ryosmkfx_gfx_get_color(RyosmkfxGfx *gfx, guint index) {
	RyosmkfxGfxData *data = gfx->priv->data;
	guint32 color = 0;
	guint8 red, green, blue;

	red = ryosmkfx_hardware_to_color((guint16)data->red_high << 8 | (guint16)data->red_low);
	green = ryosmkfx_hardware_to_color((guint16)data->green_high << 8 | (guint16)data->green_low);
	blue = ryosmkfx_hardware_to_color((guint16)data->blue_high << 8 | (guint16)data->blue_low);

	gfx_color_set_brightness(&color, 0xff);
	gfx_color_set_red(&color, red);
	gfx_color_set_green(&color, green);
	gfx_color_set_blue(&color, blue);
	return color;
}

void ryosmkfx_gfx_set_color(RyosmkfxGfx *gfx, guint index, guint32 color) {
	RyosmkfxGfxData *data = gfx->priv->data;
	guint16 red, green, blue;

	red = ryosmkfx_color_to_hardware(gfx_color_get_red(color));
	green = ryosmkfx_color_to_hardware(gfx_color_get_green(color));
	blue = ryosmkfx_color_to_hardware(gfx_color_get_blue(color));

	data->red_low = red & 0xff;
	data->green_low = green & 0xff;
	data->blue_low = blue & 0xff;

	data->red_high = red >> 8;
	data->green_high = green >> 8;
	data->blue_high = blue >> 8;
}
