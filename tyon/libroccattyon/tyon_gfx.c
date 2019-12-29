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

#include "tyon_gfx.h"
#include "tyon_device.h"
#include "talkfx.h"
#include <gaminggear/gfx.h>
#include <string.h>

#define TYON_GFX_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), TYON_GFX_TYPE, TyonGfxPrivate))

typedef struct _TyonGfxColor TyonGfxColor;
typedef struct _TyonGfxData TyonGfxData;

struct _TyonGfxColor {
	guint8 red;
	guint8 green;
	guint8 blue;
} __attribute__ ((packed));

struct _TyonGfxData {
	guint8 report_id; /* TYON_REPORT_ID_TALK */
	guint8 size; /* always 16 */
	guint8 mode;
	guint8 effect;
	TyonGfxColor colors[TYON_LIGHTS_NUM];
	guint8 unused[6];
} __attribute__ ((packed));

typedef enum {
	TYON_GFX_MODE_GFX = 0x02,
} TyonGfxMode;

typedef enum {
	TYON_GFX_EFFECT_OFF = 0x00,
	TYON_GFX_EFFECT_ON = 0x01,
	TYON_GFX_EFFECT_BLINK = 0x02,
} TyonGfxEffect;

struct _TyonGfxPrivate {
	RoccatDevice *device;
	TyonGfxData *data;
};

enum {
	PROP_0,
	PROP_DEVICE,
};

G_DEFINE_TYPE(TyonGfx, tyon_gfx, G_TYPE_OBJECT);

TyonGfx *tyon_gfx_new(RoccatDevice * const device) {
	return TYON_GFX(g_object_new(TYON_GFX_TYPE,
			"device", device,
			NULL));
}

static void tyon_gfx_init(TyonGfx *gfx) {
	gfx->priv = TYON_GFX_GET_PRIVATE(gfx);
}

static void tyon_gfx_set_property(GObject *object, guint prop_id, GValue const *value, GParamSpec *pspec) {
	TyonGfxPrivate *priv = TYON_GFX(object)->priv;
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

static GObject *tyon_gfx_constructor(GType gtype, guint n_properties, GObjectConstructParam *properties) {
	TyonGfxPrivate *priv;
	TyonGfx *gfx;
	GObject *obj;

	obj = G_OBJECT_CLASS(tyon_gfx_parent_class)->constructor(gtype, n_properties, properties);
	gfx = TYON_GFX(obj);
	priv = gfx->priv;

	priv->data = (TyonGfxData *)tyon_device_read(priv->device, TYON_REPORT_ID_TALK, sizeof(TyonGfxData), NULL);
	if (!priv->data)
		priv->data = (TyonGfxData *)g_malloc0(sizeof(TyonGfxData));

	priv->data->report_id = TYON_REPORT_ID_TALK;
	priv->data->size = sizeof(TyonGfxData);
	priv->data->mode = TYON_GFX_MODE_GFX;
	priv->data->effect = TYON_GFX_EFFECT_ON;

	return obj;
}

static void tyon_gfx_finalize(GObject *object) {
	TyonGfxPrivate *priv = TYON_GFX(object)->priv;
	g_object_unref(G_OBJECT(priv->device));
	g_free(priv->data);
	G_OBJECT_CLASS(tyon_gfx_parent_class)->finalize(object);
}

static void tyon_gfx_class_init(TyonGfxClass *gfx_class) {
	GObjectClass *gobject_class = (GObjectClass*)gfx_class;
	gobject_class->constructor = tyon_gfx_constructor;
	gobject_class->finalize = tyon_gfx_finalize;
	gobject_class->set_property = tyon_gfx_set_property;

	g_type_class_add_private(gfx_class, sizeof(TyonGfxPrivate));

	g_object_class_install_property(gobject_class, PROP_DEVICE,
			g_param_spec_object("device",
					"device",
					"Device",
					ROCCAT_DEVICE_TYPE,
					G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));
}

gboolean tyon_gfx_update(TyonGfx *gfx, GError **error) {
	TyonGfxPrivate *priv = gfx->priv;
	return tyon_device_write(priv->device, (gchar const *)priv->data, sizeof(TyonGfxData), error);
}

guint32 tyon_gfx_get_color(TyonGfx *gfx, guint index) {
	TyonGfxData *data = gfx->priv->data;
	guint32 color = 0;
	gfx_color_set_brightness(&color, 0xff);
	gfx_color_set_red(&color, data->colors[index].red);
	gfx_color_set_green(&color, data->colors[index].green);
	gfx_color_set_blue(&color, data->colors[index].blue);
	return color;
}

void tyon_gfx_set_color(TyonGfx *gfx, guint index, guint32 color) {
	TyonGfxData *data = gfx->priv->data;
	data->colors[index].red = gfx_color_get_red(color);
	data->colors[index].green = gfx_color_get_green(color);
	data->colors[index].blue = gfx_color_get_blue(color);
}
