#ifndef __ROCCAT_RYOS_RKP_TALK_H__
#define __ROCCAT_RYOS_RKP_TALK_H__

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

G_BEGIN_DECLS

typedef struct _RyosRkpTalk RyosRkpTalk;

struct _RyosRkpTalk {
	guint16 vendor_id;
	guint16 product_id;
	guint8 type;
	guint16 name[RYOS_RKP_TALK_NAME_LENGTH];
} __attribute__ ((packed));

typedef enum {
	RYOS_RKP_TALK_INDEX_M1 = 0,
	RYOS_RKP_TALK_INDEX_M2 = 1,
	RYOS_RKP_TALK_INDEX_M3 = 2,
	RYOS_RKP_TALK_INDEX_M4 = 3,
	RYOS_RKP_TALK_INDEX_M5 = 4,
	RYOS_RKP_TALK_INDEX_T1 = 5,
	RYOS_RKP_TALK_INDEX_T2 = 6,
	RYOS_RKP_TALK_INDEX_T3 = 7,
	RYOS_RKP_TALK_INDEX_CAPSLOCK = 8,

} RyosRkpTalkIndex;

static inline guint16 ryos_rkp_talk_get_vendor_id(RyosRkpTalk const *talk) {
	return GUINT16_FROM_LE(talk->vendor_id);
}

static inline void ryos_rkp_talk_set_vendor_id(RyosRkpTalk *talk, guint16 new_value) {
	talk->vendor_id = GUINT16_TO_LE(new_value);
}

static inline guint16 ryos_rkp_talk_get_product_id(RyosRkpTalk const *talk) {
	return GUINT16_FROM_LE(talk->product_id);
}

static inline void ryos_rkp_talk_set_product_id(RyosRkpTalk *talk, guint16 new_value) {
	talk->product_id = GUINT16_TO_LE(new_value);
}

gchar *ryos_rkp_talk_get_name(RyosRkpTalk const *talk);
void ryos_rkp_talk_set(RyosRkpTalk *talk, guint target, guint type, gchar const *name);
gboolean ryos_rkp_talk_equal(RyosRkpTalk const *left, RyosRkpTalk const *right);
void ryos_rkp_talk_copy(RyosRkpTalk *destination, RyosRkpTalk const *source);
RyosRkpTalk *ryos_rkp_talk_dup(RyosRkpTalk const *source);
gint ryos_hid_to_rkp_talk_index(guint hid);

G_END_DECLS

#endif
