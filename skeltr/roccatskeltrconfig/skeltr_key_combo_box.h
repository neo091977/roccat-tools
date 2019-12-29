#ifndef __ROCCAT_SKELTR_KEY_COMBO_BOX_H__
#define __ROCCAT_SKELTR_KEY_COMBO_BOX_H__

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

#include "skeltr_macro.h"
#include "roccat_button.h"
#include "roccat_swarm_rmp.h"
#include "roccat_timer.h"
#include "roccat_key_combo_box.h"

G_BEGIN_DECLS

#define SKELTR_KEY_COMBO_BOX_TYPE (skeltr_key_combo_box_get_type())
#define SKELTR_KEY_COMBO_BOX(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), SKELTR_KEY_COMBO_BOX_TYPE, SkeltrKeyComboBox))
#define IS_SKELTR_KEY_COMBO_BOX(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), SKELTR_KEY_COMBO_BOX_TYPE))

typedef struct _SkeltrKeyComboBox SkeltrKeyComboBox;
typedef struct _SkeltrKeyComboBoxData SkeltrKeyComboBoxData;

struct _SkeltrKeyComboBoxData {
	RoccatButton key;
	SkeltrMacro macro;
	RoccatTimer timer;
	RoccatSwarmOpener opener;
	guint16 talk_target;
};

typedef enum {
	SKELTR_KEY_COMBO_BOX_GROUP_EXTENDED = ROCCAT_KEY_COMBO_BOX_GROUP_LAST << 0, // needs key and macro/timer/opener
	SKELTR_KEY_COMBO_BOX_GROUP_KEY = ROCCAT_KEY_COMBO_BOX_GROUP_LAST << 1, // needs key
	SKELTR_KEY_COMBO_BOX_GROUP_PRIMARY = ROCCAT_KEY_COMBO_BOX_GROUP_LAST << 2, // only primary key
	SKELTR_KEY_COMBO_BOX_GROUP_TALK = ROCCAT_KEY_COMBO_BOX_GROUP_LAST << 3, // needs key and talk_target
} SkeltrKeyComboBoxGroup;

typedef enum {
	SKELTR_KEY_COMBO_BOX_TYPE_STANDARD = 0xe000,
	SKELTR_KEY_COMBO_BOX_TYPE_REMAP,
} SkeltrKeyComboBoxType;

GType skeltr_key_combo_box_get_type(void);
GtkWidget *skeltr_key_combo_box_new(guint mask, guint standard);

void skeltr_key_combo_box_set_data_pointer(SkeltrKeyComboBox *key_combo_box, SkeltrKeyComboBoxData *data);
SkeltrKeyComboBoxData *skeltr_key_combo_box_get_data_pointer(SkeltrKeyComboBox *key_combo_box);
void skeltr_key_combo_box_update(SkeltrKeyComboBox *key_combo_box);

void skeltr_key_combo_box_set_standard(SkeltrKeyComboBox *key_combo_box, guint standard);
gchar *skeltr_key_combo_box_get_text_for_data(SkeltrKeyComboBox *key_combo_box, SkeltrKeyComboBoxData const *data);

G_END_DECLS

#endif
