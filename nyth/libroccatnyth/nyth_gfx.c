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

#include "nyth_gfx.h"
#include "nyth_device.h"
#include "talkfx.h"
#include <gaminggear/gfx.h>
#include <string.h>

#define NYTH_GFX_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), NYTH_GFX_TYPE, NythGfxPrivate))

typedef struct _NythGfxColor NythGfxColor;
typedef struct _NythGfxData NythGfxData;

struct _NythGfxColor {
	guint8 red;
	guint8 green;
	guint8 blue;
} __attribute__ ((packed));

struct _NythGfxData {
	guint8 report_id; /* NYTH_REPORT_ID_TALK */
	guint8 size; /* always 16 */
	guint8 unused[3]; /* 0 */
	guint8 mode; /* NythGfxMode */
	guint8 unknown1; /* 4 */
	guint8 unknown2; /* 1 */
	guint8 effect; /* NythGfxEffect */
	guint8 unknown3; /* 2 */
	NythGfxColor colors[NYTH_LIGHTS_NUM]; /* Only color 1 is used */
} __attribute__ ((packed));

typedef enum {
	NYTH_GFX_MODE_GFX = 0x01,
} NythGfxMode;

typedef enum {
	NYTH_GFX_EFFECT_OFF = 0x00,
	NYTH_GFX_EFFECT_ON = 0x01,
	NYTH_GFX_EFFECT_BLINK = 0x02,
	NYTH_GFX_EFFECT_BREATHE = 0x03,
} NythGfxEffect;

struct _NythGfxPrivate {
	RoccatDevice *device;
	NythGfxData *data;
};

enum {
	PROP_0,
	PROP_DEVICE,
};

G_DEFINE_TYPE(NythGfx, nyth_gfx, G_TYPE_OBJECT);

NythGfx *nyth_gfx_new(RoccatDevice * const device) {
	return NYTH_GFX(g_object_new(NYTH_GFX_TYPE,
			"device", device,
			NULL));
}

static void nyth_gfx_init(NythGfx *gfx) {
	gfx->priv = NYTH_GFX_GET_PRIVATE(gfx);
}

static void nyth_gfx_set_property(GObject *object, guint prop_id, GValue const *value, GParamSpec *pspec) {
	NythGfxPrivate *priv = NYTH_GFX(object)->priv;
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

static GObject *nyth_gfx_constructor(GType gtype, guint n_properties, GObjectConstructParam *properties) {
	NythGfxPrivate *priv;
	NythGfx *gfx;
	GObject *obj;

	obj = G_OBJECT_CLASS(nyth_gfx_parent_class)->constructor(gtype, n_properties, properties);
	gfx = NYTH_GFX(obj);
	priv = gfx->priv;

	priv->data = (NythGfxData *)g_malloc0(sizeof(NythGfxData));

	priv->data->report_id = NYTH_REPORT_ID_TALK;
	priv->data->size = sizeof(NythGfxData);
	priv->data->mode = NYTH_GFX_MODE_GFX;
	priv->data->unknown1 = 4;
	priv->data->unknown2 = 1;
	priv->data->effect = NYTH_GFX_EFFECT_ON;
	priv->data->unknown3 = 2;

	return obj;
}

static void nyth_gfx_finalize(GObject *object) {
	NythGfxPrivate *priv = NYTH_GFX(object)->priv;
	g_object_unref(G_OBJECT(priv->device));
	g_free(priv->data);
	G_OBJECT_CLASS(nyth_gfx_parent_class)->finalize(object);
}

static void nyth_gfx_class_init(NythGfxClass *gfx_class) {
	GObjectClass *gobject_class = (GObjectClass*)gfx_class;
	gobject_class->constructor = nyth_gfx_constructor;
	gobject_class->finalize = nyth_gfx_finalize;
	gobject_class->set_property = nyth_gfx_set_property;

	g_type_class_add_private(gfx_class, sizeof(NythGfxPrivate));

	g_object_class_install_property(gobject_class, PROP_DEVICE,
			g_param_spec_object("device",
					"device",
					"Device",
					ROCCAT_DEVICE_TYPE,
					G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));
}

gboolean nyth_gfx_update(NythGfx *gfx, GError **error) {
	NythGfxPrivate *priv = gfx->priv;
	return nyth_device_write(priv->device, (gchar const *)priv->data, sizeof(NythGfxData), error);
}

guint32 nyth_gfx_get_color(NythGfx *gfx, guint index) {
	NythGfxData *data = gfx->priv->data;
	guint32 color = 0;
	gfx_color_set_brightness(&color, 0xff);
	gfx_color_set_red(&color, data->colors[index].red);
	gfx_color_set_green(&color, data->colors[index].green);
	gfx_color_set_blue(&color, data->colors[index].blue);
	return color;
}

void nyth_gfx_set_color(NythGfx *gfx, guint index, guint32 color) {
	NythGfxData *data = gfx->priv->data;
	data->colors[index].red = gfx_color_get_red(color);
	data->colors[index].green = gfx_color_get_green(color);
	data->colors[index].blue = gfx_color_get_blue(color);
}
