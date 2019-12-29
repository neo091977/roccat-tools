#ifndef __ROCCAT_KOVA2016_BUTTON_COMBO_BOX_H__
#define __ROCCAT_KOVA2016_BUTTON_COMBO_BOX_H__

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

#include "kova2016_profile_button.h"
#include "kova2016_profile_data_eventhandler.h"
#include "kova2016_macro.h"
#include "roccat_timer.h"
#include "roccat_key_combo_box.h"

G_BEGIN_DECLS

#define KOVA2016_BUTTON_COMBO_BOX_TYPE (kova2016_button_combo_box_get_type())
#define KOVA2016_BUTTON_COMBO_BOX(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), KOVA2016_BUTTON_COMBO_BOX_TYPE, Kova2016ButtonComboBox))
#define IS_KOVA2016_BUTTON_COMBO_BOX(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), KOVA2016_BUTTON_COMBO_BOX_TYPE))

typedef struct _Kova2016ButtonComboBox Kova2016ButtonComboBox;
typedef struct _Kova2016ButtonComboBoxData Kova2016ButtonComboBoxData;

struct _Kova2016ButtonComboBoxData {
	RoccatButton button;
	Kova2016Macro macro;
	RoccatTimer timer;
	RoccatSwarmOpener opener;
	guint16 talk_target;
};

typedef enum {
	KOVA2016_BUTTON_COMBO_BOX_GROUP_EXTENDED = ROCCAT_KEY_COMBO_BOX_GROUP_LAST << 0, // needs key and macro/timer/opener
} Kova2016ButtonComboBoxGroup;

GType kova2016_button_combo_box_get_type(void);
GtkWidget *kova2016_button_combo_box_new(guint mask);

void kova2016_button_combo_box_set_data_pointer(Kova2016ButtonComboBox *button_combo_box, Kova2016ButtonComboBoxData *data);
Kova2016ButtonComboBoxData *kova2016_button_combo_box_get_data_pointer(Kova2016ButtonComboBox *button_combo_box);
void kova2016_button_combo_box_update(Kova2016ButtonComboBox *button_combo_box);

void kova2016_button_combo_box_data_copy(Kova2016ButtonComboBoxData *destination, Kova2016ButtonComboBoxData const *source);
void kova2016_button_combo_box_data_swap(Kova2016ButtonComboBoxData *one, Kova2016ButtonComboBoxData *other);

G_END_DECLS

#endif
