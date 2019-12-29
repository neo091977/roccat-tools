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

#include "ryos_rkp_talk.h"
#include "roccat.h"
#include <string.h>

gchar *ryos_rkp_talk_get_name(RyosRkpTalk const *talk) {
	return g_utf16_to_utf8((gunichar2 const *)(talk->name), RYOS_RKP_TALK_NAME_LENGTH, NULL, NULL, NULL);
}

static void ryos_rkp_talk_set_name(RyosRkpTalk *talk, gchar const *name) {
	gunichar2 *string;
	glong items;

	string = g_utf8_to_utf16(name, -1L, NULL, &items, NULL);

	items = MIN(RYOS_RKP_TALK_NAME_LENGTH - 1, items);

	if (string) {
		memset(talk->name, 0, RYOS_RKP_TALK_NAME_LENGTH * 2);
		memcpy(talk->name, string, items * 2);
		g_free(string);
	}
}

void ryos_rkp_talk_set(RyosRkpTalk *talk, guint target, guint type, gchar const *name) {
	talk->vendor_id = USB_VENDOR_ID_ROCCAT;
	talk->product_id = target;
	talk->type = type;
	ryos_rkp_talk_set_name(talk, name);
}

gboolean ryos_rkp_talk_equal(RyosRkpTalk const *left, RyosRkpTalk const *right) {
	return memcmp(left, right, sizeof(RyosRkpTalk)) ? FALSE : TRUE;
}

void ryos_rkp_talk_copy(RyosRkpTalk *destination, RyosRkpTalk const *source) {
	memcpy(destination, source, sizeof(RyosRkpTalk));
}

RyosRkpTalk *ryos_rkp_talk_dup(RyosRkpTalk const *source) {
	return (RyosRkpTalk *)g_memdup(source, sizeof(RyosRkpTalk));
}

gint ryos_hid_to_rkp_talk_index(guint hid) {
	switch (hid) {
	case 0x39:
		return RYOS_RKP_TALK_INDEX_CAPSLOCK;
		break;
	case 0xaa:
	case 0xab:
	case 0xac:
	case 0xad:
	case 0xae:
		return hid - 0xaa + RYOS_RKP_TALK_INDEX_M1;
		break;
	case 0xfc:
	case 0xfd:
	case 0xfe:
		return hid - 0xfc + RYOS_RKP_TALK_INDEX_T1;
		break;
	default:
		return -1;
		break;
	}
}
