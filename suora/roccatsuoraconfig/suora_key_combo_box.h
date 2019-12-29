#ifndef __ROCCAT_SUORA_KEY_COMBO_BOX_H__
#define __ROCCAT_SUORA_KEY_COMBO_BOX_H__

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

#include "suora_rkp_macro.h"
#include "roccat_button.h"
#include "roccat_key_combo_box.h"

G_BEGIN_DECLS

#define SUORA_KEY_COMBO_BOX_TYPE (suora_key_combo_box_get_type())
#define SUORA_KEY_COMBO_BOX(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), SUORA_KEY_COMBO_BOX_TYPE, SuoraKeyComboBox))
#define IS_SUORA_KEY_COMBO_BOX(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), SUORA_KEY_COMBO_BOX_TYPE))

typedef struct _SuoraKeyComboBox SuoraKeyComboBox;
typedef struct _SuoraKeyComboBoxData SuoraKeyComboBoxData;

struct _SuoraKeyComboBoxData {
	RoccatButton key;
	SuoraRkpMacro macro;
};

typedef enum {
	SUORA_KEY_COMBO_BOX_TYPE_STANDARD = 0xe000,
	SUORA_KEY_COMBO_BOX_TYPE_REMAP,
} SuoraKeyComboBoxType;

GType suora_key_combo_box_get_type(void);
GtkWidget *suora_key_combo_box_new(guint mask, guint standard);

void suora_key_combo_box_set_data_pointer(SuoraKeyComboBox *key_combo_box, SuoraKeyComboBoxData *data);
SuoraKeyComboBoxData *suora_key_combo_box_get_data_pointer(SuoraKeyComboBox *key_combo_box);
void suora_key_combo_box_update(SuoraKeyComboBox *key_combo_box);

void suora_key_combo_box_set_standard(SuoraKeyComboBox *key_combo_box, guint standard);
gchar *suora_key_combo_box_get_text_for_data(SuoraKeyComboBox *key_combo_box, SuoraKeyComboBoxData const *data);

G_END_DECLS

#endif
