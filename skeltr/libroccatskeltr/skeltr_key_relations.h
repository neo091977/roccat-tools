#ifndef __ROCCAT_SKELTR_KEY_RELATIONS_H__
#define __ROCCAT_SKELTR_KEY_RELATIONS_H__

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

#include "skeltr.h"

G_BEGIN_DECLS

enum {
	SKELTR_KEY_RELATION_INVALID = 0xff,
	SKELTR_KEYS_NUM = 113,
};

typedef struct _SkeltrKeyRelation SkeltrKeyRelation;

struct _SkeltrKeyRelation {
	guint8 default_value;
	guint8 default_value_easyshift;
	guint8 keys_type;
	guint8 keys_index;
	guint8 keys_index_easyshift;
	guint8 macro_index;
	guint8 macro_index_easyshift;
	guint8 talk_index;
};

typedef enum {
	/* keys_index = SKELTR_KEY_RELATION_INVALID
	 * keys_index_easyshift = SKELTR_KEY_RELATION_INVALID
	 */
	SKELTR_KEY_RELATION_TYPE_NOTHING,
	
	/* keys_index = SkeltrKeysExtra
	 * keys_index_easyshift = SKELTR_KEY_RELATION_INVALID
	 */
	SKELTR_KEY_RELATION_TYPE_FN, /* FIXME confirm */
	
	/* keys_index = SkeltrKeysExtra
	 * keys_index_easyshift = SKELTR_KEY_RELATION_INVALID
	 */
	SKELTR_KEY_RELATION_TYPE_CAPSLOCK,
	
	/* keys_index = SkeltrKeysMacro + SkeltrMacro
	 * keys_index_easyshift = SkeltrKeysMacro + SkeltrMacro
	 */
	SKELTR_KEY_RELATION_TYPE_MACRO,

	/* keys_index = SkeltrKeysThumbster + SkeltrMacro
	 * keys_index_easyshift = SkeltrKeysThumbster + SkeltrMacro
	 */
	SKELTR_KEY_RELATION_TYPE_THUMBSTER,
	
	/* keys_index = SkeltrKeysFunction
	 * keys_index_easyshift = SkeltrKeysFunction
	 */
	SKELTR_KEY_RELATION_TYPE_FUNCTION,
	
	/* keys_index = SkeltrKeysPrimary
	 * keys_index_easyshift = SkeltrKeysEasyzone + SkeltrMacro
	 */
	SKELTR_KEY_RELATION_TYPE_EASYZONE,
	
	/* keys_index = SkeltrKeysPrimary
	 * keys_index_easyshift = SKELTR_KEY_RELATION_INVALID
	 */
	SKELTR_KEY_RELATION_TYPE_PRIMARY,
} SkeltrKeyRelationType;

extern SkeltrKeyRelation const skeltr_key_relations[SKELTR_KEYS_NUM];

gchar *skeltr_hid_to_special_keyname(guint8 hid);

G_END_DECLS

#endif
