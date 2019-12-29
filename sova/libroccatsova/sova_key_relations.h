#ifndef __ROCCAT_SOVA_KEY_RELATIONS_H__
#define __ROCCAT_SOVA_KEY_RELATIONS_H__

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

#include "sova.h"

G_BEGIN_DECLS

enum {
	SOVA_KEY_RELATION_INVALID = 0xff,
	SOVA_KEYS_NUM = 85,
};

typedef struct _SovaKeyRelation SovaKeyRelation;

struct _SovaKeyRelation {
	guint8 primary_default;
	guint8 fn_default;
	guint8 primary_index;
	guint8 easyzone_index;
	guint8 function_index;
	guint8 type;
};

typedef enum {
	SOVA_KEY_RELATION_TYPE_NOTHING, /* no configuration possible */
	SOVA_KEY_RELATION_TYPE_CAPSLOCK, /* normal: capslock or easyshift/talk, easyhift: none */
	SOVA_KEY_RELATION_TYPE_FUNCTION, /* normal: key, easyhift: key */
	SOVA_KEY_RELATION_TYPE_EASYZONE, /* normal: primary, easyhift: macro */
	SOVA_KEY_RELATION_TYPE_PRIMARY, /* normal: primary, easyhift: none */
} SovaKeyRelationType;

extern SovaKeyRelation const sova_key_relations[SOVA_KEYS_NUM];

guint sova_key_relation_find_by_easyzone_index(guint easyzone_index);

G_END_DECLS

#endif
