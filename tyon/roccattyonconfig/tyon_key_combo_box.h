#ifndef __ROCCAT_TYON_KEY_COMBO_BOX_H__
#define __ROCCAT_TYON_KEY_COMBO_BOX_H__

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

#include "tyon_rmp.h"
#include "roccat_key_combo_box.h"

G_BEGIN_DECLS

#define TYON_KEY_COMBO_BOX_TYPE (tyon_key_combo_box_get_type())
#define TYON_KEY_COMBO_BOX(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), TYON_KEY_COMBO_BOX_TYPE, TyonKeyComboBox))
#define IS_TYON_KEY_COMBO_BOX(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), TYON_KEY_COMBO_BOX_TYPE))

typedef struct _TyonKeyComboBox TyonKeyComboBox;

typedef enum {
	TYON_KEY_COMBO_BOX_GROUP_NOT_ANALOG = ROCCAT_KEY_COMBO_BOX_GROUP_LAST << 0, /* are not supported by analog buttons */
} TyonKeyComboBoxGroup;

GType tyon_key_combo_box_get_type(void);
GtkWidget *tyon_key_combo_box_new(guint mask);
void tyon_key_combo_box_set_value_blocked(TyonKeyComboBox *key_combo_box, TyonRmpMacroKeyInfo const *key_info);
TyonRmpMacroKeyInfo *tyon_key_combo_box_get_value(TyonKeyComboBox *key_combo_box);

G_END_DECLS

#endif
