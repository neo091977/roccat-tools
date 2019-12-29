#ifndef __ROCCAT_SUORA_GUI_KEY_RELATIONS_H__
#define __ROCCAT_SUORA_GUI_KEY_RELATIONS_H__

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

#include <glib.h>

G_BEGIN_DECLS

enum {
	SUORA_GUI_KEY_POSITION_INDEX_INVALID = -1,
	SUORA_GUI_KEY_POSITION_INDEX_END = -2,
};

typedef struct _SuoraGuiKeyPosition SuoraGuiKeyPosition;

struct _SuoraGuiKeyPosition {
	gint keys_index;
	guint row;
	guint height;
	guint column;
	guint width;
};

extern SuoraGuiKeyPosition const suora_gui_key_positions_general[];
extern SuoraGuiKeyPosition const suora_gui_key_positions_de[];

gchar *suora_hid_to_special_keyname(guint8 hid) ;

G_END_DECLS

#endif
