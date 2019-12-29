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

#include "luaconfig_light_frame.h"
#include "gtk_roccat_helper.h"
#include "i18n.h"

#define LUACONFIG_LIGHT_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), LUACONFIG_LIGHT_FRAME_TYPE, LuaconfigLightFrameClass))
#define IS_LUACONFIG_LIGHT_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), LUACONFIG_LIGHT_FRAME_TYPE))
#define LUACONFIG_LIGHT_FRAME_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), LUACONFIG_LIGHT_FRAME_TYPE, LuaconfigLightFramePrivate))

typedef struct _LuaconfigLightFrameClass LuaconfigLightFrameClass;
typedef struct _LuaconfigLightFramePrivate LuaconfigLightFramePrivate;

struct _LuaconfigLightFrame {
	GtkFrame parent;
	LuaconfigLightFramePrivate *priv;
};

struct _LuaconfigLightFrameClass {
	GtkFrameClass parent_class;
};

struct _LuaconfigLightFramePrivate {
	GSList *radios;
};

G_DEFINE_TYPE(LuaconfigLightFrame, luaconfig_light_frame, GTK_TYPE_FRAME);

GtkWidget *luaconfig_light_frame_new(void) {
	LuaconfigLightFrame *frame;

	frame = LUACONFIG_LIGHT_FRAME(g_object_new(LUACONFIG_LIGHT_FRAME_TYPE,
			"label", _("Light"),
			NULL));

	return GTK_WIDGET(frame);
}

static gchar const * const light_key = "Light";

static void pack_radio_button(gpointer button, gpointer parent) {
	gtk_box_pack_start(parent, button, TRUE, TRUE, 0);
}

static void luaconfig_light_frame_init(LuaconfigLightFrame *frame) {
	LuaconfigLightFramePrivate *priv = LUACONFIG_LIGHT_FRAME_GET_PRIVATE(frame);
	GtkWidget *vbox;
	GtkWidget *radio;

	frame->priv = priv;

	vbox = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(frame), vbox);

	radio = gtk_radio_button_new_with_label_from_widget(NULL, _("Off"));
	g_object_set_data(G_OBJECT(radio), light_key, GUINT_TO_POINTER(LUA_RMP_LIGHT_OFF));

	radio = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio), _("Breathing"));
	g_object_set_data(G_OBJECT(radio), light_key, GUINT_TO_POINTER(LUA_RMP_LIGHT_BREATHING));

	radio = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio), _("Fully lighted"));
	g_object_set_data(G_OBJECT(radio), light_key, GUINT_TO_POINTER(LUA_RMP_LIGHT_FULLY_LIGHTED));

	priv->radios = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radio));
	g_slist_foreach(priv->radios, pack_radio_button, vbox);
}

static void luaconfig_light_frame_class_init(LuaconfigLightFrameClass *klass) {
	g_type_class_add_private(klass, sizeof(LuaconfigLightFramePrivate));
}

void luaconfig_light_frame_set_from_rmp(LuaconfigLightFrame *light_frame, LuaRmp *rmp) {
	guint light_type_index;

	light_type_index = lua_rmp_get_light(rmp) - 1;

	gtk_toggle_button_set_active(
		GTK_TOGGLE_BUTTON(g_slist_nth_data(light_frame->priv->radios, light_type_index)),
		TRUE
	);
}

void luaconfig_light_frame_update_rmp(LuaconfigLightFrame *light_frame, LuaRmp *rmp) {
	GtkWidget *active_light;
	guint new_value;

	active_light = gtk_roccat_radio_button_group_get_active(light_frame->priv->radios);
	new_value = GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(active_light), light_key));
	lua_rmp_set_light(rmp, new_value);
}
