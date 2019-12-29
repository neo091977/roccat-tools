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

#include "ryosmkfx_effect_device.h"
#include "ryosmkfx_custom_lights.h"
#include "ryosmkfx_light_control.h"
#include "ryosmkfx_light_layer.h"
#include "ryos_key_relations.h"
#include "ryos_light_control.h"
#include "g_roccat_helper.h"
#include "i18n-lib.h"

#define RYOSMKFX_EFFECT_DEVICE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), RYOSMKFX_EFFECT_DEVICE_TYPE, RyosmkfxEffectDeviceClass))
#define IS_RYOSMKFX_EFFECT_DEVICE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), RYOSMKFX_EFFECT_DEVICE_TYPE))
#define RYOSMKFX_EFFECT_DEVICE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), RYOSMKFX_EFFECT_DEVICE_TYPE, RyosmkfxEffectDeviceClass))
#define RYOSMKFX_EFFECT_DEVICE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), RYOSMKFX_EFFECT_DEVICE_TYPE, RyosmkfxEffectDevicePrivate))

typedef struct _RyosmkfxEffectDeviceClass RyosmkfxEffectDeviceClass;
typedef struct _RyosmkfxEffectDevicePrivate RyosmkfxEffectDevicePrivate;

struct _RyosmkfxEffectDevice {
	GObject parent;
	RyosmkfxEffectDevicePrivate *priv;
};

struct _RyosmkfxEffectDeviceClass {
	GObjectClass parent_class;
};

struct _RyosmkfxEffectDevicePrivate {
	RoccatDevice *device;
	RyosmkfxCustomLights custom_lights;
	RyosmkfxLightLayerData data;
};

static gboolean ryosmkfx_effect_client_iface_init(RyosEffectClient *self, gpointer client_data, GError **error) {
	RyosmkfxEffectDevicePrivate *priv = RYOSMKFX_EFFECT_DEVICE(self)->priv;

	priv->device = ROCCAT_DEVICE(client_data);
	g_object_ref(G_OBJECT(priv->device));

	return TRUE;
}

static gboolean ryosmkfx_effect_client_iface_deinit(RyosEffectClient *self, GError **error) {
	RyosmkfxEffectDevicePrivate *priv = RYOSMKFX_EFFECT_DEVICE(self)->priv;
	
	g_clear_object(&priv->device);
	
	return TRUE;
}

static gboolean ryosmkfx_effect_client_iface_activate(RyosEffectClient *self, GError **error) {
	RyosmkfxEffectDevicePrivate *priv = RYOSMKFX_EFFECT_DEVICE(self)->priv;
	return ryos_light_control_custom(priv->device, TRUE, error);
}

static gboolean ryosmkfx_effect_client_iface_deactivate(RyosEffectClient *self, GError **error) {
	RyosmkfxEffectDevicePrivate *priv = RYOSMKFX_EFFECT_DEVICE(self)->priv;
	return ryos_light_control_custom(priv->device, FALSE, error);
}

static gboolean ryosmkfx_effect_client_iface_send(RyosEffectClient *self, GError **error) {
	RyosmkfxEffectDevicePrivate *priv = RYOSMKFX_EFFECT_DEVICE(self)->priv;

	ryosmkfx_light_layer_set_data(&priv->custom_lights.light_layer, &priv->data);
	return ryosmkfx_custom_lights_write(priv->device, &priv->custom_lights, error);
}

static void ryosmkfx_effect_client_iface_set_key_state(RyosEffectClient *self, guint index, gboolean on) {
	RyosmkfxEffectDevicePrivate *priv = RYOSMKFX_EFFECT_DEVICE(self)->priv;
	RyosmkfxLightLayerKey *key = &priv->data.keys[ryos_sdk_index_to_light_index(index)];
	key->state = on;
}

static void ryosmkfx_effect_client_iface_set_state(RyosEffectClient *self, gboolean on) {
	RyosmkfxEffectDevicePrivate *priv = RYOSMKFX_EFFECT_DEVICE(self)->priv;
	RyosmkfxLightLayerKey *key;
	guint key_index;

	for (key_index = 0; key_index < RYOSMKFX_LIGHT_LAYER_KEYS_NUM; ++key_index) {
		key = &priv->data.keys[key_index];
		key->state = on;
	}
}

static void ryosmkfx_effect_client_iface_set_key_color(RyosEffectClient *self, guint index, guint red, guint green, guint blue) {
	RyosmkfxEffectDevicePrivate *priv = RYOSMKFX_EFFECT_DEVICE(self)->priv;
	RyosmkfxLightLayerKey *key = &priv->data.keys[ryos_sdk_index_to_light_index(index)];
	key->red = red;
	key->green = green;
	key->blue = blue;
}

static void ryosmkfx_effect_client_iface_set_color(RyosEffectClient *self, guint red, guint green, guint blue) {
	RyosmkfxEffectDevicePrivate *priv = RYOSMKFX_EFFECT_DEVICE(self)->priv;
	RyosmkfxLightLayerKey *key;
	guint key_index;

	for (key_index = 0; key_index < RYOSMKFX_LIGHT_LAYER_KEYS_NUM; ++key_index) {
		key = &priv->data.keys[key_index];
		key->red = red;
		key->green = green;
		key->blue = blue;
	}
}

static gboolean ryosmkfx_effect_client_iface_get_key_state(RyosEffectClient *self, guint index) {
	RyosmkfxEffectDevicePrivate *priv = RYOSMKFX_EFFECT_DEVICE(self)->priv;
	RyosmkfxLightLayerKey *key = &priv->data.keys[ryos_sdk_index_to_light_index(index)];
	return key->state;
}

static guint ryosmkfx_effect_client_iface_get_key_red(RyosEffectClient *self, guint index) {
	RyosmkfxEffectDevicePrivate *priv = RYOSMKFX_EFFECT_DEVICE(self)->priv;
	RyosmkfxLightLayerKey *key = &priv->data.keys[ryos_sdk_index_to_light_index(index)];
	return key->red;
}

static guint ryosmkfx_effect_client_iface_get_key_green(RyosEffectClient *self, guint index) {
	RyosmkfxEffectDevicePrivate *priv = RYOSMKFX_EFFECT_DEVICE(self)->priv;
	RyosmkfxLightLayerKey *key = &priv->data.keys[ryos_sdk_index_to_light_index(index)];
	return key->green;
}

static guint ryosmkfx_effect_client_iface_get_key_blue(RyosEffectClient *self, guint index) {
	RyosmkfxEffectDevicePrivate *priv = RYOSMKFX_EFFECT_DEVICE(self)->priv;
	RyosmkfxLightLayerKey *key = &priv->data.keys[ryos_sdk_index_to_light_index(index)];
	return key->blue;
}

static gboolean ryosmkfx_effect_client_iface_get_all_off(RyosEffectClient *self) {
	RyosmkfxEffectDevicePrivate *priv = RYOSMKFX_EFFECT_DEVICE(self)->priv;
	return ryosmkfx_light_layer_data_are_all_off(&priv->data);
}

static void ryosmkfx_effect_device_init_client_iface(RyosEffectClientInterface *iface) {
	iface->init = ryosmkfx_effect_client_iface_init;
	iface->deinit = ryosmkfx_effect_client_iface_deinit;
	iface->activate = ryosmkfx_effect_client_iface_activate;
	iface->deactivate = ryosmkfx_effect_client_iface_deactivate;
	iface->set_key_state = ryosmkfx_effect_client_iface_set_key_state;
	iface->set_state = ryosmkfx_effect_client_iface_set_state;
	iface->get_key_state = ryosmkfx_effect_client_iface_get_key_state;
	iface->set_key_color = ryosmkfx_effect_client_iface_set_key_color;
	iface->set_color = ryosmkfx_effect_client_iface_set_color;
	iface->get_key_red = ryosmkfx_effect_client_iface_get_key_red;
	iface->get_key_green = ryosmkfx_effect_client_iface_get_key_green;
	iface->get_key_blue = ryosmkfx_effect_client_iface_get_key_blue;
	iface->get_all_off = ryosmkfx_effect_client_iface_get_all_off;
	iface->send = ryosmkfx_effect_client_iface_send;
	iface->is_emulator = FALSE;
}

G_DEFINE_TYPE_WITH_CODE(RyosmkfxEffectDevice, ryosmkfx_effect_device, G_TYPE_OBJECT,
		G_IMPLEMENT_INTERFACE(RYOS_EFFECT_CLIENT_TYPE, ryosmkfx_effect_device_init_client_iface));

RyosmkfxEffectDevice *ryosmkfx_effect_device_new(void) {
	return RYOSMKFX_EFFECT_DEVICE(g_object_new(RYOSMKFX_EFFECT_DEVICE_TYPE, NULL));
}

static void ryosmkfx_effect_device_init(RyosmkfxEffectDevice *self) {
	RyosmkfxEffectDevicePrivate *priv = RYOSMKFX_EFFECT_DEVICE_GET_PRIVATE(self);
	self->priv = priv;
}

static void ryosmkfx_effect_device_class_init(RyosmkfxEffectDeviceClass *klass) {
	g_type_class_add_private(klass, sizeof(RyosmkfxEffectDevicePrivate));
}
