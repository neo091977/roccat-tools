#ifndef __ROCCAT_LUACONFIG_KEY_COMBO_BOX_H__
#define __ROCCAT_LUACONFIG_KEY_COMBO_BOX_H__

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

#include "roccat_key_combo_box.h"

G_BEGIN_DECLS

#define LUACONFIG_KEY_COMBO_BOX_TYPE (luaconfig_key_combo_box_get_type())
#define LUACONFIG_KEY_COMBO_BOX(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), LUACONFIG_KEY_COMBO_BOX_TYPE, LuaconfigKeyComboBox))
#define IS_LUACONFIG_KEY_COMBO_BOX(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), LUACONFIG_KEY_COMBO_BOX_TYPE))

typedef struct _LuaconfigKeyComboBox LuaconfigKeyComboBox;

GType luaconfig_key_combo_box_get_type(void);
GtkWidget *luaconfig_key_combo_box_new(void);
void luaconfig_key_combo_box_set_value(LuaconfigKeyComboBox *key_combo_box, guint type);
guint luaconfig_key_combo_box_get_value(LuaconfigKeyComboBox *key_combo_box);

G_END_DECLS

#endif
