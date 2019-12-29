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

#include "ryosmkfx_key_relations.h"

typedef struct _RyosmkfxKeyRelation RyosmkfxKeyRelation;

enum {
	RYOSMKFX_KEY_RELATIONS_NUM = 9,
};

struct _RyosmkfxKeyRelation {
	guint8 primary;
	guint8 talk_index;
};

/* order is ryos_talk_index */
static RyosmkfxKeyRelation const ryosmkfx_key_relations[RYOSMKFX_KEY_RELATIONS_NUM] = {
	{0xaa, 0},
	{0xab, 1},
	{0xac, 2},
	{0xad, 3},
	{0xae, 4},
	{0xfc, 10},
	{0xfd, 11},
	{0xfe, 12},
	{0x39, 0x39},
};

gint ryosmkfx_talk_index_from_hid(guint hid) {
	guint i;

	for (i = 0; i < RYOSMKFX_KEY_RELATIONS_NUM; ++i) {
		if (ryosmkfx_key_relations[i].primary == hid)
			return ryosmkfx_key_relations[i].talk_index;
	}

	return -1;
}

guint ryosmkfx_talk_index_from_ryos_talk_index(guint index) {
	return ryosmkfx_key_relations[index].talk_index;
}

static guint8 const effect_index_to_sdk_index[256] = {
/*    0    1    2    3    4    5    6    7    8    9 */
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, /* 0 */
	255, 255, 255, 255, 255, 255,  16,   0,   1,   3, /* 1 */
	  5,   7,   9,  10,  11,  13,  34,  36,  17,  18, /* 2 */
	  2,   4,   6,   8,  27,  29,  12,  14,  35,  37, /* 3 */
	 38,  19,  20,  21,  23,  25,  28,  50, 255,  15, /* 4 */
	 56,  58,  39,  40,  42,  22,  24,  26,  48,  51, /* 5 */
	 30,  31,  57,  59,  60,  41,  43,  44,  45,  47, /* 6 */
	 49,  72, 255,  32,  74,  76,  61,  62,  64,  66, /* 7 */
	 46,  69,  70,  73,  52,  33,  75, 255,  77,  63, /* 8 */
	 65,  67,  68,  86,  71,  89,  90,  53,  92,  94, /* 9 */
	 78,  79,  81,  83,  85,  87,  88, 255, 104,  54, /* 10*/
	 93,  95,  96,  80,  82,  84, 255, 255, 255, 103, /* 11 */
	105,  55, 108, 109,  97,  98,  99, 255, 100, 255, /* 12 */
	101, 102, 106,  91, 107, 255, 255, 255, 255, 255, /* 13 */
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, /* 14 */
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, /* 15 */
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, /* 16 */
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, /* 17 */
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, /* 18 */
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, /* 19 */
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, /* 20 */
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, /* 21 */
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, /* 22 */
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, /* 23 */
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, /* 24 */
	255, 255, 255, 255, 255, 255,                     /* 25 */
};

guint8 ryosmkfx_effect_index_to_sdk_index(guint8 effect_index) {
	/* Value range of effect index is 16-134
	 * also effect index is light/layer index - 16
	 */
	return effect_index_to_sdk_index[effect_index];
}
