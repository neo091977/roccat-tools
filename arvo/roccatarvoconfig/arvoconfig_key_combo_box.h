#ifndef __ROCCAT_ARVOCONFIG_KEY_COMBO_BOX_H__
#define __ROCCAT_ARVOCONFIG_KEY_COMBO_BOX_H__

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

#include "arvo.h"
#include "roccat_key_combo_box.h"

G_BEGIN_DECLS

#define ARVOCONFIG_KEY_COMBO_BOX_TYPE (arvoconfig_key_combo_box_get_type())
#define ARVOCONFIG_KEY_COMBO_BOX(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), ARVOCONFIG_KEY_COMBO_BOX_TYPE, ArvoconfigKeyComboBox))
#define IS_ARVOCONFIG_KEY_COMBO_BOX(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), ARVOCONFIG_KEY_COMBO_BOX_TYPE))

typedef struct _ArvoconfigKeyComboBox ArvoconfigKeyComboBox;

GType arvoconfig_key_combo_box_get_type(void);
GtkWidget *arvoconfig_key_combo_box_new(void);
void arvoconfig_key_combo_box_set_value_blocked(ArvoconfigKeyComboBox *key_combo_box, ArvoRkpButtonInfo const *key_info);
ArvoRkpButtonInfo *arvoconfig_key_combo_box_get_value(ArvoconfigKeyComboBox *key_combo_box);

G_END_DECLS

#endif
