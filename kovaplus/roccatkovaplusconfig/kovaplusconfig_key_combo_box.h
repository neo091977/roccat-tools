#ifndef __ROCCAT_KOVAPLUSCONFIG_KEY_COMBO_BOX_H__
#define __ROCCAT_KOVAPLUSCONFIG_KEY_COMBO_BOX_H__

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

#include "kovaplus.h"
#include "roccat_key_combo_box.h"

G_BEGIN_DECLS

#define KOVAPLUSCONFIG_KEY_COMBO_BOX_TYPE (kovaplusconfig_key_combo_box_get_type())
#define KOVAPLUSCONFIG_KEY_COMBO_BOX(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), KOVAPLUSCONFIG_KEY_COMBO_BOX_TYPE, KovaplusconfigKeyComboBox))
#define IS_KOVAPLUSCONFIG_KEY_COMBO_BOX(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), KOVAPLUSCONFIG_KEY_COMBO_BOX_TYPE))

typedef struct _KovaplusconfigKeyComboBox KovaplusconfigKeyComboBox;

GType kovaplusconfig_key_combo_box_get_type(void);
GtkWidget *kovaplusconfig_key_combo_box_new(guint mask);
void kovaplusconfig_key_combo_box_set_value_blocked(KovaplusconfigKeyComboBox *key_combo_box, KovaplusRmpMacroKeyInfo const *key_info);
KovaplusRmpMacroKeyInfo *kovaplusconfig_key_combo_box_get_value(KovaplusconfigKeyComboBox *key_combo_box);

G_END_DECLS

#endif
