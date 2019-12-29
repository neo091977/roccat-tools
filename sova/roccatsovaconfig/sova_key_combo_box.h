#ifndef __ROCCAT_SOVA_KEY_COMBO_BOX_H__
#define __ROCCAT_SOVA_KEY_COMBO_BOX_H__

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

#include "sova_macro.h"
#include "roccat_button.h"
#include "roccat_swarm_rmp.h"
#include "roccat_timer.h"
#include "roccat_key_combo_box.h"

G_BEGIN_DECLS

#define SOVA_KEY_COMBO_BOX_TYPE (sova_key_combo_box_get_type())
#define SOVA_KEY_COMBO_BOX(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), SOVA_KEY_COMBO_BOX_TYPE, SovaKeyComboBox))
#define IS_SOVA_KEY_COMBO_BOX(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), SOVA_KEY_COMBO_BOX_TYPE))

typedef struct _SovaKeyComboBox SovaKeyComboBox;
typedef struct _SovaKeyComboBoxData SovaKeyComboBoxData;

struct _SovaKeyComboBoxData {
	RoccatButton key;
	SovaMacro macro;
	RoccatTimer timer;
	RoccatSwarmOpener opener;
	guint16 talk_target;
};

typedef enum {
	SOVA_KEY_COMBO_BOX_GROUP_EXTENDED = ROCCAT_KEY_COMBO_BOX_GROUP_LAST << 0, // needs key and macro/timer/opener
	SOVA_KEY_COMBO_BOX_GROUP_KEY = ROCCAT_KEY_COMBO_BOX_GROUP_LAST << 1, // needs key
	SOVA_KEY_COMBO_BOX_GROUP_PRIMARY = ROCCAT_KEY_COMBO_BOX_GROUP_LAST << 2, // only primary key
	SOVA_KEY_COMBO_BOX_GROUP_TALK = ROCCAT_KEY_COMBO_BOX_GROUP_LAST << 3, // needs key and talk_target
} SovaKeyComboBoxGroup;

typedef enum {
	SOVA_KEY_COMBO_BOX_TYPE_STANDARD = 0xe000,
	SOVA_KEY_COMBO_BOX_TYPE_REMAP,
} SovaKeyComboBoxType;

GType sova_key_combo_box_get_type(void);
GtkWidget *sova_key_combo_box_new(guint mask, guint standard);

void sova_key_combo_box_set_data_pointer(SovaKeyComboBox *key_combo_box, SovaKeyComboBoxData *data);
SovaKeyComboBoxData *sova_key_combo_box_get_data_pointer(SovaKeyComboBox *key_combo_box);
void sova_key_combo_box_update(SovaKeyComboBox *key_combo_box);

void sova_key_combo_box_set_standard(SovaKeyComboBox *key_combo_box, guint standard);
gchar *sova_key_combo_box_get_text_for_data(SovaKeyComboBox *key_combo_box, SovaKeyComboBoxData const *data);

G_END_DECLS

#endif
