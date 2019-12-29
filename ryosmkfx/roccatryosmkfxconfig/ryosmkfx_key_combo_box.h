#ifndef __ROCCAT_RYOSMKFX_KEY_COMBO_BOX_H__
#define __ROCCAT_RYOSMKFX_KEY_COMBO_BOX_H__

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

#include "ryosmkfx_macro.h"
#include "ryos_led_macro.h"
#include "roccat_button.h"
#include "roccat_swarm_rmp.h"
#include "roccat_timer.h"
#include "roccat_key_combo_box.h"

G_BEGIN_DECLS

#define RYOSMKFX_KEY_COMBO_BOX_TYPE (ryosmkfx_key_combo_box_get_type())
#define RYOSMKFX_KEY_COMBO_BOX(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), RYOSMKFX_KEY_COMBO_BOX_TYPE, RyosmkfxKeyComboBox))
#define IS_RYOSMKFX_KEY_COMBO_BOX(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), RYOSMKFX_KEY_COMBO_BOX_TYPE))

typedef struct _RyosmkfxKeyComboBox RyosmkfxKeyComboBox;
typedef struct _RyosmkfxKeyComboBoxData RyosmkfxKeyComboBoxData;

struct _RyosmkfxKeyComboBoxData {
	RoccatButton key;
	RyosmkfxMacro macro;
	RoccatTimer timer;
	RoccatSwarmOpener opener;
	guint16 talk_target;
	RyosLedMacro led_macro;
};

GType ryosmkfx_key_combo_box_get_type(void);
GtkWidget *ryosmkfx_key_combo_box_new(guint mask, guint standard);

void ryosmkfx_key_combo_box_set_data_pointer(RyosmkfxKeyComboBox *key_combo_box, RyosmkfxKeyComboBoxData *data);
RyosmkfxKeyComboBoxData *ryosmkfx_key_combo_box_get_data_pointer(RyosmkfxKeyComboBox *key_combo_box);
void ryosmkfx_key_combo_box_update(RyosmkfxKeyComboBox *key_combo_box);

void ryosmkfx_key_combo_box_set_standard(RyosmkfxKeyComboBox *key_combo_box, guint standard);
gchar *ryosmkfx_key_combo_box_get_text_for_data(RyosmkfxKeyComboBox *key_combo_box, RyosmkfxKeyComboBoxData const *data);

G_END_DECLS

#endif
