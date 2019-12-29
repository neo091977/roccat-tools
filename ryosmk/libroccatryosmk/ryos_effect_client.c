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

#include "ryos_effect_client.h"

gboolean ryos_effect_client_init(RyosEffectClient *self, gpointer client_data, GError **error) {
	return RYOS_EFFECT_CLIENT_GET_INTERFACE(self)->init(self, client_data, error);
}

gboolean ryos_effect_client_deinit(RyosEffectClient *self, GError **error) {
	return RYOS_EFFECT_CLIENT_GET_INTERFACE(self)->deinit(self, error);
}

gboolean ryos_effect_client_activate(RyosEffectClient *self, GError **error) {
	return RYOS_EFFECT_CLIENT_GET_INTERFACE(self)->activate(self, error);
}

gboolean ryos_effect_client_deactivate(RyosEffectClient *self, GError **error) {
	return RYOS_EFFECT_CLIENT_GET_INTERFACE(self)->deactivate(self, error);
}

gboolean ryos_effect_client_send(RyosEffectClient *self, GError **error) {
	return RYOS_EFFECT_CLIENT_GET_INTERFACE(self)->send(self, error);
}

void ryos_effect_client_set_key_state(RyosEffectClient *self, guint index, gboolean on) {
	RYOS_EFFECT_CLIENT_GET_INTERFACE(self)->set_key_state(self, index, on);
}

void ryos_effect_client_set_state(RyosEffectClient *self, gboolean on) {
	RYOS_EFFECT_CLIENT_GET_INTERFACE(self)->set_state(self, on);
}

void ryos_effect_client_set_key_color(RyosEffectClient *self, guint index, guint red, guint green, guint blue) {
	if (RYOS_EFFECT_CLIENT_GET_INTERFACE(self)->set_key_color)
		RYOS_EFFECT_CLIENT_GET_INTERFACE(self)->set_key_color(self, index, red, green, blue);
}

void ryos_effect_client_set_color(RyosEffectClient *self, guint red, guint green, guint blue) {
	if (RYOS_EFFECT_CLIENT_GET_INTERFACE(self)->set_color)
		RYOS_EFFECT_CLIENT_GET_INTERFACE(self)->set_color(self, red, green, blue);
}

gboolean ryos_effect_client_get_key_state(RyosEffectClient *self, guint index) {
	return RYOS_EFFECT_CLIENT_GET_INTERFACE(self)->get_key_state(self, index);
}

guint ryos_effect_client_get_key_red(RyosEffectClient *self, guint index) {
	return RYOS_EFFECT_CLIENT_GET_INTERFACE(self)->get_key_red(self, index);
}

guint ryos_effect_client_get_key_green(RyosEffectClient *self, guint index) {
	return RYOS_EFFECT_CLIENT_GET_INTERFACE(self)->get_key_green(self, index);
}

guint ryos_effect_client_get_key_blue(RyosEffectClient *self, guint index) {
	return RYOS_EFFECT_CLIENT_GET_INTERFACE(self)->get_key_blue(self, index);
}

gboolean ryos_effect_client_get_all_off(RyosEffectClient *self) {
	return RYOS_EFFECT_CLIENT_GET_INTERFACE(self)->get_all_off(self);
}

gboolean ryos_effect_client_supports_color(RyosEffectClient *self) {
	return RYOS_EFFECT_CLIENT_GET_INTERFACE(self)->set_key_color ? TRUE : FALSE;
}

gboolean ryos_effect_client_is_emulator(RyosEffectClient *self) {
	return RYOS_EFFECT_CLIENT_GET_INTERFACE(self)->is_emulator;
}

G_DEFINE_INTERFACE(RyosEffectClient, ryos_effect_client, G_TYPE_OBJECT);

static void ryos_effect_client_default_init(RyosEffectClientInterface *iface) {
}
