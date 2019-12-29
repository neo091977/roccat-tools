#ifndef __ROCCAT_RYOS_EFFECT_CLIENT_H__
#define __ROCCAT_RYOS_EFFECT_CLIENT_H__

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

#include <glib-object.h>

G_BEGIN_DECLS

#define RYOS_EFFECT_CLIENT_TYPE (ryos_effect_client_get_type())
#define RYOS_EFFECT_CLIENT(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), RYOS_EFFECT_CLIENT_TYPE, RyosEffectClient))
#define IS_RYOS_EFFECT_CLIENT(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), RYOS_EFFECT_CLIENT_TYPE))
#define RYOS_EFFECT_CLIENT_GET_INTERFACE(inst) (G_TYPE_INSTANCE_GET_INTERFACE((inst), RYOS_EFFECT_CLIENT_TYPE, RyosEffectClientInterface))

typedef struct _RyosEffectClient RyosEffectClient;
typedef struct _RyosEffectClientInterface RyosEffectClientInterface;

struct _RyosEffectClientInterface {
	GTypeInterface parent;

	gboolean (*init)(RyosEffectClient *self, gpointer client_data, GError **error);
	gboolean (*deinit)(RyosEffectClient *self, GError **error);

	gboolean (*activate)(RyosEffectClient *self, GError **error);
	gboolean (*deactivate)(RyosEffectClient *self, GError **error);

	void (*set_key_state)(RyosEffectClient *self, guint index, gboolean on);
	void (*set_state)(RyosEffectClient *self, gboolean on);
	gboolean (*get_key_state)(RyosEffectClient *self, guint index);
	
	/* these can be NULL for devices that don't support it */
	void (*set_key_color)(RyosEffectClient *self, guint index, guint red, guint green, guint blue);
	void (*set_color)(RyosEffectClient *self, guint red, guint green, guint blue);
	
	/* these have to be implemented and should return a default value if colors
	 * are not supported.
	 */
	guint (*get_key_red)(RyosEffectClient *self, guint index);
	guint (*get_key_green)(RyosEffectClient *self, guint index);
	guint (*get_key_blue)(RyosEffectClient *self, guint index);

	gboolean (*get_all_off)(RyosEffectClient *self);
	
	gboolean (*send)(RyosEffectClient *self, GError **error);

	gboolean is_emulator;
};

GType ryos_effect_client_get_type(void);

gboolean ryos_effect_client_init(RyosEffectClient *self, gpointer client_data, GError **error);
gboolean ryos_effect_client_deinit(RyosEffectClient *self, GError **error);

gboolean ryos_effect_client_activate(RyosEffectClient *self, GError **error);
gboolean ryos_effect_client_deactivate(RyosEffectClient *self, GError **error);

gboolean ryos_effect_client_send(RyosEffectClient *self, GError **error);

void ryos_effect_client_set_key_state(RyosEffectClient *self, guint index, gboolean on);
void ryos_effect_client_set_state(RyosEffectClient *self, gboolean on);

void ryos_effect_client_set_key_color(RyosEffectClient *self, guint index, guint red, guint green, guint blue);
void ryos_effect_client_set_color(RyosEffectClient *self, guint red, guint green, guint blue);

gboolean ryos_effect_client_get_key_state(RyosEffectClient *self, guint index);

guint ryos_effect_client_get_key_red(RyosEffectClient *self, guint index);
guint ryos_effect_client_get_key_green(RyosEffectClient *self, guint index);
guint ryos_effect_client_get_key_blue(RyosEffectClient *self, guint index);

gboolean ryos_effect_client_get_all_off(RyosEffectClient *self);

gboolean ryos_effect_client_supports_color(RyosEffectClient *self);
gboolean ryos_effect_client_is_emulator(RyosEffectClient *self);

G_END_DECLS

#endif
