#ifndef __ROCCAT_RYOS_GUI_KEY_RELATIONS_H__
#define __ROCCAT_RYOS_GUI_KEY_RELATIONS_H__

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

#include "ryos_key_relations.h"
 
G_BEGIN_DECLS

typedef struct _RyosGuiKeyRelation RyosGuiKeyRelation;
typedef struct _RyosGuiKeyPosition RyosGuiKeyPosition;

enum {
	RYOS_GUI_KEY_POSITIONS_END = 0xff,
};

struct _RyosGuiKeyRelation {
	guint mask;
	guint mask_easyshift;
	guint8 default_normal;
	guint8 default_easyshift;
};

extern RyosGuiKeyRelation const ryos_gui_key_relations[RYOS_KEY_RELATIONS_NUM];

struct _RyosGuiKeyPosition {
	guint8 relation_index;
	guint8 row;
	guint8 height;
	guint8 column;
	guint8 width;
};

extern RyosGuiKeyPosition const ryos_gui_key_positions_macro[];
extern RyosGuiKeyPosition const ryos_gui_key_positions_thumbster[];
extern RyosGuiKeyPosition const ryos_gui_key_positions_function[];
extern RyosGuiKeyPosition const ryos_gui_key_positions_escape[];
extern RyosGuiKeyPosition const ryos_gui_key_positions_numpad[];
extern RyosGuiKeyPosition const ryos_gui_key_positions_arrows[];
extern RyosGuiKeyPosition const ryos_gui_key_positions_block_above_arrows[];

RyosGuiKeyPosition const *ryos_gui_key_positions_get_main(gchar const *layout);

gchar *ryos_hid_to_special_keyname(guint8 hid);

G_END_DECLS

#endif
