#ifndef __ROCCAT_NYTH_BUTTON_COMBO_BOX_H__
#define __ROCCAT_NYTH_BUTTON_COMBO_BOX_H__

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

#include "nyth_profile_button.h"
#include "nyth_profile_data_eventhandler.h"
#include "nyth_macro.h"
#include "roccat_timer.h"
#include "roccat_key_combo_box.h"

G_BEGIN_DECLS

#define NYTH_BUTTON_COMBO_BOX_TYPE (nyth_button_combo_box_get_type())
#define NYTH_BUTTON_COMBO_BOX(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), NYTH_BUTTON_COMBO_BOX_TYPE, NythButtonComboBox))
#define IS_NYTH_BUTTON_COMBO_BOX(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), NYTH_BUTTON_COMBO_BOX_TYPE))

typedef struct _NythButtonComboBox NythButtonComboBox;
typedef struct _NythButtonComboBoxData NythButtonComboBoxData;

struct _NythButtonComboBoxData {
	RoccatButton button;
	NythMacro macro;
	RoccatTimer timer;
	RoccatSwarmOpener opener;
	guint16 talk_target;
};

typedef enum {
	NYTH_BUTTON_COMBO_BOX_GROUP_EXTENDED = ROCCAT_KEY_COMBO_BOX_GROUP_LAST << 0, // needs key and macro/timer/opener
} NythButtonComboBoxGroup;

GType nyth_button_combo_box_get_type(void);
GtkWidget *nyth_button_combo_box_new(guint mask);

void nyth_button_combo_box_set_data_pointer(NythButtonComboBox *button_combo_box, NythButtonComboBoxData *data);
NythButtonComboBoxData *nyth_button_combo_box_get_data_pointer(NythButtonComboBox *button_combo_box);
void nyth_button_combo_box_update(NythButtonComboBox *button_combo_box);

G_END_DECLS

#endif
