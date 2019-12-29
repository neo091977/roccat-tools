#ifndef __ROCCAT_SUORA_KEY_RELATIONS_H__
#define __ROCCAT_SUORA_KEY_RELATIONS_H__

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

#include "suora.h"

G_BEGIN_DECLS

enum {
	SUORA_KEY_RELATION_INVALID = -1,
};

typedef struct _SuoraKeyRelation SuoraKeyRelation;

typedef enum {
	SUORA_KEY_RELATION_TYPE_NONE = 0,
	SUORA_KEY_RELATION_TYPE_NORMAL,
	SUORA_KEY_RELATION_TYPE_MACRO,
} SuoraKeyRelationType;

typedef enum {
	SUORA_KEY_RELATION_VALUE_GAME_MODE = 0xaa,
	SUORA_KEY_RELATION_VALUE_FN,
	SUORA_KEY_RELATION_VALUE_MUTE,
	SUORA_KEY_RELATION_VALUE_VOLUMEDOWN,
	SUORA_KEY_RELATION_VALUE_VOLUMEUP,
} SuoraKeyRelationValue;

struct _SuoraKeyRelation {
	SuoraKeyRelationType type;
	gint value; /* HID usage id or SuoraKeyRelationValue */
	gint macro_index; /* SUORA_MACRO_NUM */
};

extern SuoraKeyRelation const suora_key_relations[SUORA_KEYS_NUM];

gint suora_key_relation_find_by_macro_index(guint macro_index);

G_END_DECLS

#endif
