#ifndef __ROCCAT_KONECONFIG_KEY_COMBO_BOX_H__
#define __ROCCAT_KONECONFIG_KEY_COMBO_BOX_H__

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

#include "kone.h"
#include "roccat_key_combo_box.h"

G_BEGIN_DECLS

#define KONECONFIG_KEY_COMBO_BOX_TYPE (koneconfig_key_combo_box_get_type())
#define KONECONFIG_KEY_COMBO_BOX(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), KONECONFIG_KEY_COMBO_BOX_TYPE, KoneconfigKeyComboBox))
#define IS_KONECONFIG_KEY_COMBO_BOX(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), KONECONFIG_KEY_COMBO_BOX_TYPE))

typedef struct _KoneconfigKeyComboBox KoneconfigKeyComboBox;

GType koneconfig_key_combo_box_get_type(void);
GtkWidget *koneconfig_key_combo_box_new(void);
void koneconfig_key_combo_box_set_value_blocked(KoneconfigKeyComboBox *key_combo_box, KoneRMPButtonInfo const *key_info);
KoneRMPButtonInfo *koneconfig_key_combo_box_get_value(KoneconfigKeyComboBox *key_combo_box);

G_END_DECLS

#endif
