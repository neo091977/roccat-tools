#ifndef __ROCCAT_SAVUCONFIG_KEY_COMBO_BOX_H__
#define __ROCCAT_SAVUCONFIG_KEY_COMBO_BOX_H__

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

#include "savu.h"
#include "roccat_key_combo_box.h"

G_BEGIN_DECLS

#define SAVUCONFIG_KEY_COMBO_BOX_TYPE (savuconfig_key_combo_box_get_type())
#define SAVUCONFIG_KEY_COMBO_BOX(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), SAVUCONFIG_KEY_COMBO_BOX_TYPE, SavuconfigKeyComboBox))
#define IS_SAVUCONFIG_KEY_COMBO_BOX(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), SAVUCONFIG_KEY_COMBO_BOX_TYPE))

typedef struct _SavuconfigKeyComboBox SavuconfigKeyComboBox;

GType savuconfig_key_combo_box_get_type(void);
GtkWidget *savuconfig_key_combo_box_new(guint mask);
void savuconfig_key_combo_box_set_from_rmp(SavuconfigKeyComboBox *key_combo_box, SavuRmp *rmp, guint index);
void savuconfig_key_combo_box_update_rmp(SavuconfigKeyComboBox *key_combo_box, SavuRmp *rmp, guint index);

G_END_DECLS

#endif
