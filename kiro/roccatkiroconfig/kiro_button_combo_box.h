#ifndef __ROCCAT_KIRO_BUTTON_COMBO_BOX_H__
#define __ROCCAT_KIRO_BUTTON_COMBO_BOX_H__

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

#include "kiro_profile_data_eventhandler.h"
#include "kiro_profile.h"
#include "roccat_key_combo_box.h"
#include "roccat_button.h"

G_BEGIN_DECLS

#define KIRO_BUTTON_COMBO_BOX_TYPE (kiro_button_combo_box_get_type())
#define KIRO_BUTTON_COMBO_BOX(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), KIRO_BUTTON_COMBO_BOX_TYPE, KiroButtonComboBox))
#define IS_KIRO_BUTTON_COMBO_BOX(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), KIRO_BUTTON_COMBO_BOX_TYPE))

typedef struct _KiroButtonComboBox KiroButtonComboBox;
typedef struct _KiroButtonComboBoxData KiroButtonComboBoxData;

struct _KiroButtonComboBoxData {
	RoccatButton button;
	RoccatSwarmOpener opener;
};

typedef enum {
	KIRO_BUTTON_COMBO_BOX_GROUP_EXTENDED = ROCCAT_KEY_COMBO_BOX_GROUP_LAST << 0, // needs key and opener
} KiroButtonComboBoxGroup;

GType kiro_button_combo_box_get_type(void);
GtkWidget *kiro_button_combo_box_new(guint mask);

void kiro_button_combo_box_set_data_pointer(KiroButtonComboBox *button_combo_box, KiroButtonComboBoxData *data);
KiroButtonComboBoxData *kiro_button_combo_box_get_data_pointer(KiroButtonComboBox *button_combo_box);
void kiro_button_combo_box_update(KiroButtonComboBox *button_combo_box);

void kiro_button_combo_box_data_copy(KiroButtonComboBoxData *destination, KiroButtonComboBoxData const *source);
void kiro_button_combo_box_data_swap(KiroButtonComboBoxData *one, KiroButtonComboBoxData *other);

G_END_DECLS

#endif
