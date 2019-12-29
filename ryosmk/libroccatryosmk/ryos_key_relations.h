#ifndef __ROCCAT_RYOS_KEY_RELATIONS_H__
#define __ROCCAT_RYOS_KEY_RELATIONS_H__

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

typedef struct _RyosKeyRelation RyosKeyRelation;

enum {
	RYOS_KEY_RELATIONS_NUM = 119,
	RYOS_KEY_RELATIONS_INDEX_INVALID = 0xff,
	RYOS_SDK_KEY_NUM = 110,
};

struct _RyosKeyRelation {
	guint8 primary;
	guint8 talk_index;
	guint8 macro_index; /* values range from 0x00-0x6f */
	guint8 macro_index_easyshift;
	guint8 primary_index; /* extended with thumbster keys */
	guint8 keys_type;
	guint8 keys_index;
	guint8 keys_index_easyshift;
};

typedef enum {
	RYOS_KEY_RELATION_KEYS_TYPE_NOTHING,
	RYOS_KEY_RELATION_KEYS_TYPE_FN,
	RYOS_KEY_RELATION_KEYS_TYPE_CAPSLOCK,
	RYOS_KEY_RELATION_KEYS_TYPE_MACRO,
	RYOS_KEY_RELATION_KEYS_TYPE_THUMBSTER,
	RYOS_KEY_RELATION_KEYS_TYPE_FUNCTION,
	RYOS_KEY_RELATION_KEYS_TYPE_EASYZONE,
} RyosKeyRelationKeysType;

extern RyosKeyRelation const ryos_key_relations[RYOS_KEY_RELATIONS_NUM];

extern guint8 const ryos_layer_index_preset_all_keys[];
extern guint8 const ryos_layer_index_preset_main_keys[];
extern guint8 const ryos_layer_index_preset_numpad[];
extern guint8 const ryos_layer_index_preset_f_keys[];
extern guint8 const ryos_layer_index_preset_m_keys[];
extern guint8 const ryos_layer_index_preset_t_keys[];
extern guint8 const ryos_layer_index_preset_arrows[];
extern guint8 const ryos_layer_index_preset_wasd[];
extern guint8 const ryos_layer_index_preset_qwer[];

extern guint8 const ryos_hid_to_layer_index[];

guint8 ryos_hid_to_sdk_index(guint8 hid);
guint8 ryos_sdk_index_to_light_index(guint8 sdk_index);

G_END_DECLS

#endif
