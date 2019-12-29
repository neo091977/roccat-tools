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

#include "luaconfig_buttons_frame.h"
#include "luaconfig_key_combo_box.h"
#include "i18n.h"

#define LUACONFIG_BUTTONS_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), LUACONFIG_BUTTONS_FRAME_TYPE, LuaconfigButtonsFrameClass))
#define IS_LUACONFIG_BUTTONS_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), LUACONFIG_BUTTONS_FRAME_TYPE))
#define LUACONFIG_BUTTONS_FRAME_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), LUACONFIG_BUTTONS_FRAME_TYPE, LuaconfigButtonsFramePrivate))

typedef struct _LuaconfigButtonsFrameClass LuaconfigButtonsFrameClass;
typedef struct _LuaconfigButtonsFramePrivate LuaconfigButtonsFramePrivate;

struct _LuaconfigButtonsFrame {
	GtkFrame parent;
	LuaconfigButtonsFramePrivate *priv;
};

struct _LuaconfigButtonsFrameClass {
	GtkFrameClass parent_class;
};

struct _LuaconfigButtonsFramePrivate {
	LuaconfigKeyComboBox *buttons[LUA_BUTTON_NUM];
};

G_DEFINE_TYPE(LuaconfigButtonsFrame, luaconfig_buttons_frame, GTK_TYPE_FRAME);

GtkWidget *luaconfig_buttons_frame_new(void) {
	LuaconfigButtonsFrame *frame;

	frame = LUACONFIG_BUTTONS_FRAME(g_object_new(LUACONFIG_BUTTONS_FRAME_TYPE,
			"label", _("Buttons"),
			NULL));

	return GTK_WIDGET(frame);
}

static gchar const * const button_names[LUA_BUTTON_NUM] = {
	N_("Left"), N_("Right"), N_("Middle"), N_("Wheel up"), N_("Wheel down"), N_("Button"),
};

static void luaconfig_buttons_frame_init(LuaconfigButtonsFrame *frame) {
	LuaconfigButtonsFramePrivate *priv = LUACONFIG_BUTTONS_FRAME_GET_PRIVATE(frame);
	GtkWidget *table;
	guint i;

	frame->priv = priv;

	table = gtk_table_new(LUA_BUTTON_NUM, 2, FALSE);
	gtk_container_add(GTK_CONTAINER(frame), table);

	for (i = 0; i < LUA_BUTTON_NUM; ++i) {
		priv->buttons[i] = LUACONFIG_KEY_COMBO_BOX(luaconfig_key_combo_box_new());
		gtk_table_attach(GTK_TABLE(table), gtk_label_new(_N(button_names[i])), 0, 1, i, i + 1, GTK_EXPAND, GTK_EXPAND, 0, 0);
		gtk_table_attach(GTK_TABLE(table), GTK_WIDGET(priv->buttons[i]), 1, 2, i, i + 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 0, 0);
	}
}

static void luaconfig_buttons_frame_class_init(LuaconfigButtonsFrameClass *klass) {
	g_type_class_add_private(klass, sizeof(LuaconfigButtonsFramePrivate));
}

void luaconfig_buttons_frame_set_from_rmp(LuaconfigButtonsFrame *buttons_frame, LuaRmp *rmp) {
	LuaconfigButtonsFramePrivate *priv = buttons_frame->priv;
	guint value;
	guint i;

	for (i = 0; i < LUA_BUTTON_NUM; ++i) {
		value = lua_rmp_get_key(rmp, i);
		luaconfig_key_combo_box_set_value(priv->buttons[i], value);
	}
}

void luaconfig_buttons_frame_update_rmp(LuaconfigButtonsFrame *buttons_frame, LuaRmp *rmp) {
	LuaconfigButtonsFramePrivate *priv = buttons_frame->priv;
	guint i;
	gint value;

	for (i = 0; i < LUA_BUTTON_NUM; ++i) {
		value = luaconfig_key_combo_box_get_value(priv->buttons[i]);
		if (value != 0)
			lua_rmp_set_key(rmp, i, value);
	}
}
