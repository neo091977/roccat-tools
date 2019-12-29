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

#include "ryos_effect_device.h"
#include "ryos_custom_lights.h"
#include "ryos_key_relations.h"
#include "ryos_light_control.h"
#include "roccat.h"
#include "roccat_helper.h"
#include "g_roccat_helper.h"
#include "config.h"
#include <string.h>

#define RYOS_EFFECT_DEVICE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), RYOS_EFFECT_DEVICE_TYPE, RyosEffectDeviceClass))
#define IS_RYOS_EFFECT_DEVICE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), RYOS_EFFECT_DEVICE_TYPE))
#define RYOS_EFFECT_DEVICE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), RYOS_EFFECT_DEVICE_TYPE, RyosEffectDeviceClass))
#define RYOS_EFFECT_DEVICE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), RYOS_EFFECT_DEVICE_TYPE, RyosEffectDevicePrivate))

typedef struct _RyosEffectDeviceClass RyosEffectDeviceClass;
typedef struct _RyosEffectDevicePrivate RyosEffectDevicePrivate;

struct _RyosEffectDevice {
	GObject parent;
	RyosEffectDevicePrivate *priv;
};

struct _RyosEffectDeviceClass {
	GObjectClass parent_class;
};

struct _RyosEffectDevicePrivate {
	RoccatDevice *device;
	RyosCustomLights custom_lights;
};

static gboolean ryos_effect_client_iface_init(RyosEffectClient *self, gpointer client_data, GError **error) {
	RyosEffectDevicePrivate *priv = RYOS_EFFECT_DEVICE(self)->priv;

	priv->device = ROCCAT_DEVICE(client_data);
	g_object_ref(G_OBJECT(priv->device));

	return TRUE;
}

static gboolean ryos_effect_client_iface_deinit(RyosEffectClient *self, GError **error) {
	RyosEffectDevicePrivate *priv = RYOS_EFFECT_DEVICE(self)->priv;
	
	g_clear_object(&priv->device);
	
	return TRUE;
}

static gboolean ryos_effect_client_iface_activate(RyosEffectClient *self, GError **error) {
	RyosEffectDevicePrivate *priv = RYOS_EFFECT_DEVICE(self)->priv;
	return ryos_light_control_custom(priv->device, TRUE, error);
}

static gboolean ryos_effect_client_iface_deactivate(RyosEffectClient *self, GError **error) {
	RyosEffectDevicePrivate *priv = RYOS_EFFECT_DEVICE(self)->priv;
	return ryos_light_control_custom(priv->device, FALSE, error);
}

static gboolean ryos_effect_client_iface_send(RyosEffectClient *self, GError **error) {
	RyosEffectDevicePrivate *priv = RYOS_EFFECT_DEVICE(self)->priv;
	return ryos_custom_lights_write(priv->device, &priv->custom_lights, error);
}

static void ryos_effect_client_iface_set_key_state(RyosEffectClient *self, guint index, gboolean on) {
	RyosEffectDevicePrivate *priv = RYOS_EFFECT_DEVICE(self)->priv;
	roccat_bitfield_set_bit(priv->custom_lights.data, ryos_sdk_index_to_light_index(index), on);
}

static void ryos_effect_client_iface_set_state(RyosEffectClient *self, gboolean on) {
	RyosEffectDevicePrivate *priv = RYOS_EFFECT_DEVICE(self)->priv;
	memset(&priv->custom_lights.data, on ? 0xff : 0x00, RYOS_CUSTOM_LIGHTS_DATA_LENGTH);
}

static gboolean ryos_effect_client_iface_get_key_state(RyosEffectClient *self, guint index) {
	RyosEffectDevicePrivate *priv = RYOS_EFFECT_DEVICE(self)->priv;
	return roccat_bitfield_get_bit(priv->custom_lights.data, ryos_sdk_index_to_light_index(index));
}

static guint ryos_effect_client_iface_get_key_red(RyosEffectClient *self, guint index) {
	return ROCCAT_BLUE_RED;
}

static guint ryos_effect_client_iface_get_key_green(RyosEffectClient *self, guint index) {
	return ROCCAT_BLUE_GREEN;
}

static guint ryos_effect_client_iface_get_key_blue(RyosEffectClient *self, guint index) {
	return ROCCAT_BLUE_BLUE;
}

static gboolean ryos_effect_client_iface_get_all_off(RyosEffectClient *self) {
	RyosEffectDevicePrivate *priv = RYOS_EFFECT_DEVICE(self)->priv;
	return ryos_custom_lights_empty(&priv->custom_lights);
}

static void ryos_effect_device_init_client_iface(RyosEffectClientInterface *iface) {
	iface->init = ryos_effect_client_iface_init;
	iface->deinit = ryos_effect_client_iface_deinit;
	iface->activate = ryos_effect_client_iface_activate;
	iface->deactivate = ryos_effect_client_iface_deactivate;
	iface->set_key_state = ryos_effect_client_iface_set_key_state;
	iface->set_state = ryos_effect_client_iface_set_state;
	iface->get_key_state = ryos_effect_client_iface_get_key_state;
	iface->set_key_color = NULL;
	iface->set_color = NULL;
	iface->get_key_red = ryos_effect_client_iface_get_key_red;
	iface->get_key_green = ryos_effect_client_iface_get_key_green;
	iface->get_key_blue = ryos_effect_client_iface_get_key_blue;
	iface->get_all_off = ryos_effect_client_iface_get_all_off;
	iface->send = ryos_effect_client_iface_send;
	iface->is_emulator = FALSE;
}

G_DEFINE_TYPE_WITH_CODE(RyosEffectDevice, ryos_effect_device, G_TYPE_OBJECT,
		G_IMPLEMENT_INTERFACE(RYOS_EFFECT_CLIENT_TYPE, ryos_effect_device_init_client_iface));

RyosEffectDevice *ryos_effect_device_new(void) {
	return RYOS_EFFECT_DEVICE(g_object_new(RYOS_EFFECT_DEVICE_TYPE, NULL));
}

static void ryos_effect_device_init(RyosEffectDevice *self) {
	RyosEffectDevicePrivate *priv = RYOS_EFFECT_DEVICE_GET_PRIVATE(self);
	self->priv = priv;
}

static void ryos_effect_device_class_init(RyosEffectDeviceClass *klass) {
	g_type_class_add_private(klass, sizeof(RyosEffectDevicePrivate));
}
