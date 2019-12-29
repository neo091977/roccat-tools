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

#include "kiro_gfx.h"
#include "kiro_device.h"
#include "talkfx.h"
#include <gaminggear/gfx.h>
#include <string.h>

#define KIRO_GFX_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), KIRO_GFX_TYPE, KiroGfxPrivate))

typedef struct _KiroGfxColor KiroGfxColor;
typedef struct _KiroGfxData KiroGfxData;

struct _KiroGfxColor {
	guint8 red;
	guint8 green;
	guint8 blue;
} __attribute__ ((packed));

struct _KiroGfxData {
	guint8 report_id; /* KIRO_REPORT_ID_FX */
	guint8 size; /* always 12! should be 13 */
	guint8 unused1[3]; /* 0 */
	guint8 mode; /* KiroGfxMode */
	guint8 unknown1; /* 0 */
	guint8 effect; /* KiroGfxEffect */
	guint8 unknown2; /* 0 */
	KiroGfxColor colors[KIRO_LIGHTS_NUM];
	guint8 unused2; /* 0 */
} __attribute__ ((packed));

typedef enum {
	KIRO_GFX_MODE_GFX = 0x01,
} KiroGfxMode;

typedef enum {
	KIRO_GFX_EFFECT_OFF = 0x00,
	KIRO_GFX_EFFECT_ON = 0x01,
	KIRO_GFX_EFFECT_BLINK = 0x02,
	KIRO_GFX_EFFECT_BREATHE = 0x03,
} KiroGfxEffect;

struct _KiroGfxPrivate {
	RoccatDevice *device;
	KiroGfxData *data;
};

enum {
	PROP_0,
	PROP_DEVICE,
};

G_DEFINE_TYPE(KiroGfx, kiro_gfx, G_TYPE_OBJECT);

KiroGfx *kiro_gfx_new(RoccatDevice * const device) {
	return KIRO_GFX(g_object_new(KIRO_GFX_TYPE,
			"device", device,
			NULL));
}

static void kiro_gfx_init(KiroGfx *gfx) {
	gfx->priv = KIRO_GFX_GET_PRIVATE(gfx);
}

static void kiro_gfx_set_property(GObject *object, guint prop_id, GValue const *value, GParamSpec *pspec) {
	KiroGfxPrivate *priv = KIRO_GFX(object)->priv;
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

static GObject *kiro_gfx_constructor(GType gtype, guint n_properties, GObjectConstructParam *properties) {
	KiroGfxPrivate *priv;
	KiroGfx *gfx;
	GObject *obj;

	obj = G_OBJECT_CLASS(kiro_gfx_parent_class)->constructor(gtype, n_properties, properties);
	gfx = KIRO_GFX(obj);
	priv = gfx->priv;

	priv->data = (KiroGfxData *)g_malloc0(sizeof(KiroGfxData));

	priv->data->report_id = KIRO_REPORT_ID_FX;
	priv->data->size = 0x0c; // TODO wrong sizeof(KiroGfxData);
	priv->data->mode = KIRO_GFX_MODE_GFX;
	priv->data->effect = KIRO_GFX_EFFECT_ON;

	return obj;
}

static void kiro_gfx_finalize(GObject *object) {
	KiroGfxPrivate *priv = KIRO_GFX(object)->priv;
	g_object_unref(G_OBJECT(priv->device));
	g_free(priv->data);
	G_OBJECT_CLASS(kiro_gfx_parent_class)->finalize(object);
}

static void kiro_gfx_class_init(KiroGfxClass *gfx_class) {
	GObjectClass *gobject_class = (GObjectClass*)gfx_class;
	gobject_class->constructor = kiro_gfx_constructor;
	gobject_class->finalize = kiro_gfx_finalize;
	gobject_class->set_property = kiro_gfx_set_property;

	g_type_class_add_private(gfx_class, sizeof(KiroGfxPrivate));

	g_object_class_install_property(gobject_class, PROP_DEVICE,
			g_param_spec_object("device",
					"device",
					"Device",
					ROCCAT_DEVICE_TYPE,
					G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));
}

gboolean kiro_gfx_update(KiroGfx *gfx, GError **error) {
	KiroGfxPrivate *priv = gfx->priv;
	return kiro_device_write(priv->device, (gchar const *)priv->data, sizeof(KiroGfxData), error);
}

guint32 kiro_gfx_get_color(KiroGfx *gfx, guint index) {
	KiroGfxData *data = gfx->priv->data;
	guint32 color = 0;
	gfx_color_set_brightness(&color, 0xff);
	gfx_color_set_red(&color, data->colors[index].red);
	gfx_color_set_green(&color, data->colors[index].green);
	gfx_color_set_blue(&color, data->colors[index].blue);
	return color;
}

void kiro_gfx_set_color(KiroGfx *gfx, guint index, guint32 color) {
	KiroGfxData *data = gfx->priv->data;
	data->colors[index].red = gfx_color_get_red(color);
	data->colors[index].green = gfx_color_get_green(color);
	data->colors[index].blue = gfx_color_get_blue(color);
}
