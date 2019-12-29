#ifndef __ROCCAT_KONEPLUS_KEY_COMBO_BOX_H__
#define __ROCCAT_KONEPLUS_KEY_COMBO_BOX_H__

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

#include "koneplus.h"
#include "roccat_key_combo_box.h"

G_BEGIN_DECLS

#define KONEPLUS_KEY_COMBO_BOX_TYPE (koneplus_key_combo_box_get_type())
#define KONEPLUS_KEY_COMBO_BOX(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), KONEPLUS_KEY_COMBO_BOX_TYPE, KoneplusKeyComboBox))
#define IS_KONEPLUS_KEY_COMBO_BOX(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), KONEPLUS_KEY_COMBO_BOX_TYPE))

typedef struct _KoneplusKeyComboBox KoneplusKeyComboBox;

GType koneplus_key_combo_box_get_type(void);
GtkWidget *koneplus_key_combo_box_new(guint mask);
void koneplus_key_combo_box_set_value_blocked(KoneplusKeyComboBox *key_combo_box, KoneplusRmpMacroKeyInfo const *key_info);
KoneplusRmpMacroKeyInfo *koneplus_key_combo_box_get_value(KoneplusKeyComboBox *key_combo_box);

G_END_DECLS

#endif
