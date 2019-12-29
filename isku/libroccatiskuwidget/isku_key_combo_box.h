#ifndef __ROCCAT_ISKU_KEY_COMBO_BOX_H__
#define __ROCCAT_ISKU_KEY_COMBO_BOX_H__

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

#include "isku.h"
#include "roccat_key_combo_box.h"

G_BEGIN_DECLS

#define ISKU_KEY_COMBO_BOX_TYPE (isku_key_combo_box_get_type())
#define ISKU_KEY_COMBO_BOX(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), ISKU_KEY_COMBO_BOX_TYPE, IskuKeyComboBox))
#define IS_ISKU_KEY_COMBO_BOX(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), ISKU_KEY_COMBO_BOX_TYPE))

typedef struct _IskuKeyComboBox IskuKeyComboBox;

GType isku_key_combo_box_get_type(void);
GtkWidget *isku_key_combo_box_new(guint mask, gint standard);
void isku_key_combo_box_set_value_blocked(IskuKeyComboBox *key_combo_box, IskuRkpMacroKeyInfo const *key_info);
IskuRkpMacroKeyInfo *isku_key_combo_box_get_value(IskuKeyComboBox *key_combo_box);

G_END_DECLS

#endif
