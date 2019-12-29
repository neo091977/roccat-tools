#ifndef __ROCCAT_RYOS_KEY_COMBO_BOX_H__
#define __ROCCAT_RYOS_KEY_COMBO_BOX_H__

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

#include "ryos.h"
#include "ryos_macro.h"
#include "ryos_led_macro.h"
#include "ryos_rkp.h"
#include "roccat_key_combo_box.h"

G_BEGIN_DECLS

#define RYOS_KEY_COMBO_BOX_TYPE (ryos_key_combo_box_get_type())
#define RYOS_KEY_COMBO_BOX(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), RYOS_KEY_COMBO_BOX_TYPE, RyosKeyComboBox))
#define IS_RYOS_KEY_COMBO_BOX(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), RYOS_KEY_COMBO_BOX_TYPE))

typedef struct _RyosKeyComboBox RyosKeyComboBox;
typedef struct _RyosKeyComboBoxData RyosKeyComboBoxData;

/* primary is extracted from key */
struct _RyosKeyComboBoxData {
	RoccatButton key;
	RyosMacro macro;
	RyosRkpTimer timer;
	RyosRkpQuicklaunch quicklaunch;
	RyosRkpTalk talk;
	RyosLedMacro led_macro;
};

typedef enum {
	RYOS_KEY_COMBO_BOX_GROUP_EXTENDED = ROCCAT_KEY_COMBO_BOX_GROUP_LAST << 0, // needs key and macro/timer/quicklaunch
	RYOS_KEY_COMBO_BOX_GROUP_KEY = ROCCAT_KEY_COMBO_BOX_GROUP_LAST << 1, // needs key
	RYOS_KEY_COMBO_BOX_GROUP_PRIMARY = ROCCAT_KEY_COMBO_BOX_GROUP_LAST << 2, // only primary key
	RYOS_KEY_COMBO_BOX_GROUP_TALK = ROCCAT_KEY_COMBO_BOX_GROUP_LAST << 3, // needs key and talk
} RyosKeyComboBoxGroup;

typedef enum {
	RYOS_KEY_COMBO_BOX_TYPE_STANDARD = 0xe000,
	RYOS_KEY_COMBO_BOX_TYPE_REMAP,
} RyosKeyComboBoxType;

GType ryos_key_combo_box_get_type(void);
GtkWidget *ryos_key_combo_box_new(guint mask, guint standard);

void ryos_key_combo_box_set_data_pointer(RyosKeyComboBox *key_combo_box, RyosKeyComboBoxData *data);
RyosKeyComboBoxData *ryos_key_combo_box_get_data_pointer(RyosKeyComboBox *key_combo_box);
void ryos_key_combo_box_update(RyosKeyComboBox *key_combo_box);

void ryos_key_combo_box_set_standard(RyosKeyComboBox *key_combo_box, guint standard);
gchar *ryos_key_combo_box_get_text_for_data(RyosKeyComboBox *key_combo_box, RyosKeyComboBoxData const *data);

G_END_DECLS

#endif
