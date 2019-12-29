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

#include "luaconfig_key_combo_box.h"
#include "lua.h"
#include "gtk_roccat_helper.h"
#include "i18n.h"

#define LUACONFIG_KEY_COMBO_BOX_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), LUACONFIG_KEY_COMBO_BOX_TYPE, LuaconfigKeyComboBoxClass))
#define IS_LUACONFIG_KEY_COMBO_BOX_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), LUACONFIG_KEY_COMBO_BOX_TYPE))
#define LUACONFIG_KEY_COMBO_BOX_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), LUACONFIG_KEY_COMBO_BOX_TYPE, LuaconfigKeyComboBoxClass))
#define LUACONFIG_KEY_COMBO_BOX_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), LUACONFIG_KEY_COMBO_BOX_TYPE, LuaconfigKeyComboBoxPrivate))

typedef struct _LuaconfigKeyComboBoxClass LuaconfigKeyComboBoxClass;
typedef struct _LuaconfigKeyComboBoxPrivate LuaconfigKeyComboBoxPrivate;

struct _LuaconfigKeyComboBox {
	RoccatKeyComboBox parent;
	LuaconfigKeyComboBoxPrivate *priv;
};

struct _LuaconfigKeyComboBoxClass {
	RoccatKeyComboBoxClass parent_class;
	GtkTreeModel *model;
};

struct _LuaconfigKeyComboBoxPrivate {
	guint type;
	gulong changed_handler_id;
};

G_DEFINE_TYPE(LuaconfigKeyComboBox, luaconfig_key_combo_box, ROCCAT_KEY_COMBO_BOX_TYPE);

static void set_text(LuaconfigKeyComboBox *key_combo_box) {
	LuaconfigKeyComboBoxPrivate *priv = key_combo_box->priv;
	gchar *text;

	text = roccat_key_combo_box_model_get_title_for_type(gtk_combo_box_get_model(GTK_COMBO_BOX(key_combo_box)), priv->type);
	g_signal_handler_block(G_OBJECT(key_combo_box), priv->changed_handler_id);
	roccat_key_combo_box_set_text(ROCCAT_KEY_COMBO_BOX(key_combo_box), text);
	g_signal_handler_unblock(G_OBJECT(key_combo_box), priv->changed_handler_id);
	g_free(text);
}

static void button_changed_cb(GtkComboBox *combo, gpointer user_data) {
	LuaconfigKeyComboBox *key_combo_box = LUACONFIG_KEY_COMBO_BOX(combo);
	LuaconfigKeyComboBoxPrivate *priv = key_combo_box->priv;
	gint active_type;

	active_type = roccat_key_combo_box_get_active_type(ROCCAT_KEY_COMBO_BOX(combo));
	if (active_type == ROCCAT_KEY_COMBO_BOX_TYPE_NONE)
		return;

	priv->type = active_type;
	set_text(key_combo_box);
}

GtkWidget *luaconfig_key_combo_box_new(void) {
	LuaconfigKeyComboBox *key_combo_box;
	LuaconfigKeyComboBoxClass *key_combo_box_class;
	LuaconfigKeyComboBoxPrivate *priv;

	key_combo_box = LUACONFIG_KEY_COMBO_BOX(g_object_new(LUACONFIG_KEY_COMBO_BOX_TYPE,
			NULL));

	priv = key_combo_box->priv;

	key_combo_box_class = LUACONFIG_KEY_COMBO_BOX_GET_CLASS(key_combo_box);
	roccat_key_combo_box_set_model(ROCCAT_KEY_COMBO_BOX(key_combo_box), key_combo_box_class->model);

	priv->changed_handler_id = g_signal_connect(G_OBJECT(key_combo_box), "changed", G_CALLBACK(button_changed_cb), NULL);

	return GTK_WIDGET(key_combo_box);
}

static void luaconfig_key_combo_box_init(LuaconfigKeyComboBox *key_combo_box) {
	LuaconfigKeyComboBoxPrivate *priv = LUACONFIG_KEY_COMBO_BOX_GET_PRIVATE(key_combo_box);
	key_combo_box->priv = priv;
}

static GtkTreeModel *button_info_model_new(void) {
	GtkTreeStore *store;

	store = roccat_key_combo_box_store_new();

	roccat_key_combo_box_store_append(store, NULL, NULL, _("Button 1 (Click)"), LUA_KEY_TYPE_CLICK, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, NULL, _("Button 2 (Universal scrolling)"), LUA_KEY_TYPE_UNIVERSAL_SCROLL, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, NULL, _("Button 3 (Menu)"), LUA_KEY_TYPE_MENU, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, NULL, _("Button 4 (Scroll up)"), LUA_KEY_TYPE_SCROLL_UP, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, NULL, _("Button 5 (Scroll down)"), LUA_KEY_TYPE_SCROLL_DOWN, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, NULL, _("Button 8 (Browser backward)"), LUA_KEY_TYPE_BROWSER_BACKWARD, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, NULL, _("Button 9 (Browser forward)"), LUA_KEY_TYPE_BROWSER_FORWARD, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, NULL, _("CPI cycle"), LUA_KEY_TYPE_CPI_CYCLE, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);
	roccat_key_combo_box_store_append(store, NULL, NULL, _("Disabled"), LUA_KEY_TYPE_DISABLED, ROCCAT_KEY_COMBO_BOX_GROUP_NONE);

	return GTK_TREE_MODEL(store);
}

static void luaconfig_key_combo_box_class_init(LuaconfigKeyComboBoxClass *klass) {
	g_type_class_add_private(klass, sizeof(LuaconfigKeyComboBoxPrivate));
	klass->model = button_info_model_new();
}

void luaconfig_key_combo_box_set_value(LuaconfigKeyComboBox *key_combo_box, guint type) {
	key_combo_box->priv->type = type;
	set_text(key_combo_box);
}

guint luaconfig_key_combo_box_get_value(LuaconfigKeyComboBox *key_combo_box) {
	return key_combo_box->priv->type;
}
