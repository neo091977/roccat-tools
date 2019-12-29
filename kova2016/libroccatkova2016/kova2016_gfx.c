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

#include "kova2016_gfx.h"
#include "kova2016_device.h"
#include "talkfx.h"
#include <gaminggear/gfx.h>
#include <string.h>

#define KOVA2016_GFX_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), KOVA2016_GFX_TYPE, Kova2016GfxPrivate))

typedef struct _Kova2016GfxColor Kova2016GfxColor;
typedef struct _Kova2016GfxData Kova2016GfxData;

struct _Kova2016GfxColor {
	guint8 red;
	guint8 green;
	guint8 blue;
} __attribute__ ((packed));

struct _Kova2016GfxData {
	guint8 report_id; /* KOVA2016_REPORT_ID_TALK */
	guint8 size; /* always 16 */
	guint8 unused[3]; /* 0 */
	guint8 mode; /* Kova2016GfxMode */
	guint8 unknown1; /* 3 */
	guint8 unknown2; /* 1 */
	guint8 effect; /* Kova2016GfxEffect */
	guint8 unknown3; /* 2 */
	Kova2016GfxColor colors[KOVA2016_LIGHTS_NUM]; /* Only color 1 is used */
} __attribute__ ((packed));

typedef enum {
	KOVA2016_GFX_MODE_GFX = 0x01,
} Kova2016GfxMode;

typedef enum {
	KOVA2016_GFX_EFFECT_OFF = 0x00,
	KOVA2016_GFX_EFFECT_ON = 0x01,
	KOVA2016_GFX_EFFECT_BLINK = 0x02,
	KOVA2016_GFX_EFFECT_BREATHE = 0x03,
} Kova2016GfxEffect;

struct _Kova2016GfxPrivate {
	RoccatDevice *device;
	Kova2016GfxData *data;
};

enum {
	PROP_0,
	PROP_DEVICE,
};

G_DEFINE_TYPE(Kova2016Gfx, kova2016_gfx, G_TYPE_OBJECT);

Kova2016Gfx *kova2016_gfx_new(RoccatDevice * const device) {
	return KOVA2016_GFX(g_object_new(KOVA2016_GFX_TYPE,
			"device", device,
			NULL));
}

static void kova2016_gfx_init(Kova2016Gfx *gfx) {
	gfx->priv = KOVA2016_GFX_GET_PRIVATE(gfx);
}

static void kova2016_gfx_set_property(GObject *object, guint prop_id, GValue const *value, GParamSpec *pspec) {
	Kova2016GfxPrivate *priv = KOVA2016_GFX(object)->priv;
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

static GObject *kova2016_gfx_constructor(GType gtype, guint n_properties, GObjectConstructParam *properties) {
	Kova2016GfxPrivate *priv;
	Kova2016Gfx *gfx;
	GObject *obj;

	obj = G_OBJECT_CLASS(kova2016_gfx_parent_class)->constructor(gtype, n_properties, properties);
	gfx = KOVA2016_GFX(obj);
	priv = gfx->priv;

	priv->data = (Kova2016GfxData *)g_malloc0(sizeof(Kova2016GfxData));

	priv->data->report_id = KOVA2016_REPORT_ID_TALK;
	priv->data->size = sizeof(Kova2016GfxData);
	priv->data->mode = KOVA2016_GFX_MODE_GFX;
	priv->data->unknown1 = 3;
	priv->data->unknown2 = 1;
	priv->data->effect = KOVA2016_GFX_EFFECT_ON;
	priv->data->unknown3 = 2;

	return obj;
}

static void kova2016_gfx_finalize(GObject *object) {
	Kova2016GfxPrivate *priv = KOVA2016_GFX(object)->priv;
	g_object_unref(G_OBJECT(priv->device));
	g_free(priv->data);
	G_OBJECT_CLASS(kova2016_gfx_parent_class)->finalize(object);
}

static void kova2016_gfx_class_init(Kova2016GfxClass *gfx_class) {
	GObjectClass *gobject_class = (GObjectClass*)gfx_class;
	gobject_class->constructor = kova2016_gfx_constructor;
	gobject_class->finalize = kova2016_gfx_finalize;
	gobject_class->set_property = kova2016_gfx_set_property;

	g_type_class_add_private(gfx_class, sizeof(Kova2016GfxPrivate));

	g_object_class_install_property(gobject_class, PROP_DEVICE,
			g_param_spec_object("device",
					"device",
					"Device",
					ROCCAT_DEVICE_TYPE,
					G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));
}

gboolean kova2016_gfx_update(Kova2016Gfx *gfx, GError **error) {
	Kova2016GfxPrivate *priv = gfx->priv;
	return kova2016_device_write(priv->device, (gchar const *)priv->data, sizeof(Kova2016GfxData), error);
}

guint32 kova2016_gfx_get_color(Kova2016Gfx *gfx, guint index) {
	Kova2016GfxData *data = gfx->priv->data;
	guint32 color = 0;
	gfx_color_set_brightness(&color, 0xff);
	gfx_color_set_red(&color, data->colors[index].red);
	gfx_color_set_green(&color, data->colors[index].green);
	gfx_color_set_blue(&color, data->colors[index].blue);
	return color;
}

void kova2016_gfx_set_color(Kova2016Gfx *gfx, guint index, guint32 color) {
	Kova2016GfxData *data = gfx->priv->data;
	data->colors[index].red = gfx_color_get_red(color);
	data->colors[index].green = gfx_color_get_green(color);
	data->colors[index].blue = gfx_color_get_blue(color);
}
