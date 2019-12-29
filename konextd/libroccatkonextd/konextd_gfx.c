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

#include "konextd_gfx.h"
#include "talkfx.h"
#include <gaminggear/gfx.h>
#include <string.h>

#define KONEXTD_GFX_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), KONEXTD_GFX_TYPE, KonextdGfxPrivate))

typedef struct _KonextdGfxColor KonextdGfxColor;
typedef struct _KonextdGfxData KonextdGfxData;

struct _KonextdGfxColor {
	guint8 red;
	guint8 green;
	guint8 blue;
} __attribute__ ((packed));

struct _KonextdGfxData {
	guint8 report_id; /* KONEPLUS_REPORT_ID_TALK */
	guint8 size; /* always 16 */
	guint8 mode;
	guint8 effect;
	KonextdGfxColor colors[KONEPLUS_PROFILE_SETTING_LIGHTS_NUM];
} __attribute__ ((packed));

typedef enum {
	KONEXTD_GFX_MODE_GFX = 0x02,
} KonextdGfxMode;

typedef enum {
	KONEXTD_GFX_EFFECT_OFF = 0x00,
	KONEXTD_GFX_EFFECT_ON = 0x01,
	KONEXTD_GFX_EFFECT_BLINK = 0x02,
} KonextdGfxEffect;

struct _KonextdGfxPrivate {
	RoccatDevice *device;
	KonextdGfxData *data;
};

enum {
	PROP_0,
	PROP_DEVICE,
};

G_DEFINE_TYPE(KonextdGfx, konextd_gfx, G_TYPE_OBJECT);

KonextdGfx *konextd_gfx_new(RoccatDevice * const device) {
	return KONEXTD_GFX(g_object_new(KONEXTD_GFX_TYPE,
			"device", device,
			NULL));
}

static void konextd_gfx_init(KonextdGfx *gfx) {
	gfx->priv = KONEXTD_GFX_GET_PRIVATE(gfx);
}

static void konextd_gfx_set_property(GObject *object, guint prop_id, GValue const *value, GParamSpec *pspec) {
	KonextdGfxPrivate *priv = KONEXTD_GFX(object)->priv;
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

static GObject *konextd_gfx_constructor(GType gtype, guint n_properties, GObjectConstructParam *properties) {
	KonextdGfxPrivate *priv;
	KonextdGfx *gfx;
	GObject *obj;

	obj = G_OBJECT_CLASS(konextd_gfx_parent_class)->constructor(gtype, n_properties, properties);
	gfx = KONEXTD_GFX(obj);
	priv = gfx->priv;

	priv->data = (KonextdGfxData *)koneplus_device_read(priv->device, KONEPLUS_REPORT_ID_TALK, sizeof(KonextdGfxData), NULL);
	if (!priv->data)
		priv->data = (KonextdGfxData *)g_malloc0(sizeof(KonextdGfxData));

	priv->data->report_id = KONEPLUS_REPORT_ID_TALK;
	priv->data->size = sizeof(KonextdGfxData);
	priv->data->mode = KONEXTD_GFX_MODE_GFX;
	priv->data->effect = KONEXTD_GFX_EFFECT_ON;

	return obj;
}

static void konextd_gfx_finalize(GObject *object) {
	KonextdGfxPrivate *priv = KONEXTD_GFX(object)->priv;
	g_object_unref(G_OBJECT(priv->device));
	g_free(priv->data);
	G_OBJECT_CLASS(konextd_gfx_parent_class)->finalize(object);
}

static void konextd_gfx_class_init(KonextdGfxClass *gfx_class) {
	GObjectClass *gobject_class = (GObjectClass*)gfx_class;
	gobject_class->constructor = konextd_gfx_constructor;
	gobject_class->finalize = konextd_gfx_finalize;
	gobject_class->set_property = konextd_gfx_set_property;

	g_type_class_add_private(gfx_class, sizeof(KonextdGfxPrivate));

	g_object_class_install_property(gobject_class, PROP_DEVICE,
			g_param_spec_object("device",
					"device",
					"Device",
					ROCCAT_DEVICE_TYPE,
					G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));
}

gboolean konextd_gfx_update(KonextdGfx *gfx, GError **error) {
	KonextdGfxPrivate *priv = gfx->priv;
	return koneplus_device_write(priv->device, (gchar const *)priv->data, sizeof(KonextdGfxData), error);
}

guint32 konextd_gfx_get_color(KonextdGfx *gfx, guint index) {
	KonextdGfxData *data = gfx->priv->data;
	guint32 color = 0;
	gfx_color_set_brightness(&color, 0xff);
	gfx_color_set_red(&color, data->colors[index].red);
	gfx_color_set_green(&color, data->colors[index].green);
	gfx_color_set_blue(&color, data->colors[index].blue);
	return color;
}

void konextd_gfx_set_color(KonextdGfx *gfx, guint index, guint32 color) {
	KonextdGfxData *data = gfx->priv->data;
	data->colors[index].red = gfx_color_get_red(color);
	data->colors[index].green = gfx_color_get_green(color);
	data->colors[index].blue = gfx_color_get_blue(color);
}
