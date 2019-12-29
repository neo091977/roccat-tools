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

#include "ryosmkfx_effect_gui.h"
#include "ryosmkfx_light_layer.h"
#include "ryosmkfx_light_effects_emulator.h"
#include "ryos_key_relations.h"
#include "ryos_device.h"
#include "g_roccat_helper.h"
#include <string.h>

#define RYOSMKFX_EFFECT_GUI_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), RYOSMKFX_EFFECT_GUI_TYPE, RyosmkfxEffectGuiClass))
#define IS_RYOSMKFX_EFFECT_GUI_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), RYOSMKFX_EFFECT_GUI_TYPE))
#define RYOSMKFX_EFFECT_GUI_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), RYOSMKFX_EFFECT_GUI_TYPE, RyosmkfxEffectGuiClass))
#define RYOSMKFX_EFFECT_GUI_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), RYOSMKFX_EFFECT_GUI_TYPE, RyosmkfxEffectGuiPrivate))

typedef struct _RyosmkfxEffectGuiClass RyosmkfxEffectGuiClass;
typedef struct _RyosmkfxEffectGuiPrivate RyosmkfxEffectGuiPrivate;

struct _RyosmkfxEffectGui {
	GObject parent;
	RyosmkfxEffectGuiPrivate *priv;
};

struct _RyosmkfxEffectGuiClass {
	GObjectClass parent_class;
};

struct _RyosmkfxEffectGuiPrivate {
	RyosmkfxLightEffectsEmulator *emulator;
	RyosmkfxLightLayerData data;
};

static gboolean ryosmkfx_effect_client_iface_init(RyosEffectClient *self, gpointer client_data, GError **error) {
	RyosmkfxEffectGuiPrivate *priv = RYOSMKFX_EFFECT_GUI(self)->priv;

	priv->emulator = RYOSMKFX_LIGHT_EFFECTS_EMULATOR(client_data);
	g_object_ref(G_OBJECT(priv->emulator));

	return TRUE;
}

static gboolean ryosmkfx_effect_client_iface_deinit(RyosEffectClient *self, GError **error) {
	RyosmkfxEffectGuiPrivate *priv = RYOSMKFX_EFFECT_GUI(self)->priv;

	g_clear_object(&priv->emulator);

	return TRUE;
}

static gboolean ryosmkfx_effect_client_iface_activate(RyosEffectClient *self, GError **error) {
	return TRUE;
}

static gboolean ryosmkfx_effect_client_iface_deactivate(RyosEffectClient *self, GError **error) {
	return TRUE;
}

static gboolean ryosmkfx_effect_client_iface_send(RyosEffectClient *self, GError **error) {
	RyosmkfxEffectGuiPrivate *priv = RYOSMKFX_EFFECT_GUI(self)->priv;
	ryosmkfx_light_effects_emulator_set_layer_data_temp(priv->emulator, &priv->data);
	/* keeping speed in sync with hardware timing */
	g_usleep(RYOS_WRITE_CHECK_WAIT_LIGHT_CONTROL * G_ROCCAT_USEC_PER_MSEC);
	return TRUE;
}

static void ryosmkfx_effect_client_iface_set_key_state(RyosEffectClient *self, guint index, gboolean on) {
	RyosmkfxEffectGuiPrivate *priv = RYOSMKFX_EFFECT_GUI(self)->priv;
	RyosmkfxLightLayerKey *key = &priv->data.keys[ryos_sdk_index_to_light_index(index)];
	key->state = on;
}

static void ryosmkfx_effect_client_iface_set_state(RyosEffectClient *self, gboolean on) {
	RyosmkfxEffectGuiPrivate *priv = RYOSMKFX_EFFECT_GUI(self)->priv;
	RyosmkfxLightLayerKey *key;
	guint key_index;

	for (key_index = 0; key_index < RYOSMKFX_LIGHT_LAYER_KEYS_NUM; ++key_index) {
		key = &priv->data.keys[key_index];
		key->state = on;
	}
}

static void ryosmkfx_effect_client_iface_set_key_color(RyosEffectClient *self, guint index, guint red, guint green, guint blue) {
	RyosmkfxEffectGuiPrivate *priv = RYOSMKFX_EFFECT_GUI(self)->priv;
	RyosmkfxLightLayerKey *key = &priv->data.keys[ryos_sdk_index_to_light_index(index)];
	key->red = red;
	key->green = green;
	key->blue = blue;
}

static void ryosmkfx_effect_client_iface_set_color(RyosEffectClient *self, guint red, guint green, guint blue) {
	RyosmkfxEffectGuiPrivate *priv = RYOSMKFX_EFFECT_GUI(self)->priv;
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
	RyosmkfxEffectGuiPrivate *priv = RYOSMKFX_EFFECT_GUI(self)->priv;
	RyosmkfxLightLayerKey *key = &priv->data.keys[ryos_sdk_index_to_light_index(index)];
	return key->state;
}

static guint ryosmkfx_effect_client_iface_get_key_red(RyosEffectClient *self, guint index) {
	RyosmkfxEffectGuiPrivate *priv = RYOSMKFX_EFFECT_GUI(self)->priv;
	RyosmkfxLightLayerKey *key = &priv->data.keys[ryos_sdk_index_to_light_index(index)];
	return key->red;
}

static guint ryosmkfx_effect_client_iface_get_key_green(RyosEffectClient *self, guint index) {
	RyosmkfxEffectGuiPrivate *priv = RYOSMKFX_EFFECT_GUI(self)->priv;
	RyosmkfxLightLayerKey *key = &priv->data.keys[ryos_sdk_index_to_light_index(index)];
	return key->green;
}

static guint ryosmkfx_effect_client_iface_get_key_blue(RyosEffectClient *self, guint index) {
	RyosmkfxEffectGuiPrivate *priv = RYOSMKFX_EFFECT_GUI(self)->priv;
	RyosmkfxLightLayerKey *key = &priv->data.keys[ryos_sdk_index_to_light_index(index)];
	return key->blue;
}

static gboolean ryosmkfx_effect_client_iface_get_all_off(RyosEffectClient *self) {
	RyosmkfxEffectGuiPrivate *priv = RYOSMKFX_EFFECT_GUI(self)->priv;
	return ryosmkfx_light_layer_data_are_all_off(&priv->data);
}

static void ryosmkfx_effect_gui_init_client_iface(RyosEffectClientInterface *iface) {
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
	iface->is_emulator = TRUE;
}

G_DEFINE_TYPE_WITH_CODE(RyosmkfxEffectGui, ryosmkfx_effect_gui, G_TYPE_OBJECT,
		G_IMPLEMENT_INTERFACE(RYOS_EFFECT_CLIENT_TYPE, ryosmkfx_effect_gui_init_client_iface));

RyosmkfxEffectGui *ryosmkfx_effect_gui_new(void) {
	return RYOSMKFX_EFFECT_GUI(g_object_new(RYOSMKFX_EFFECT_GUI_TYPE, NULL));
}

static void ryosmkfx_effect_gui_init(RyosmkfxEffectGui *self) {
	RyosmkfxEffectGuiPrivate *priv = RYOSMKFX_EFFECT_GUI_GET_PRIVATE(self);
	self->priv = priv;
}

static void ryosmkfx_effect_gui_class_init(RyosmkfxEffectGuiClass *klass) {
	g_type_class_add_private(klass, sizeof(RyosmkfxEffectGuiPrivate));
}
