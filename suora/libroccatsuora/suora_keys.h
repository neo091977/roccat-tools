#ifndef __ROCCAT_SUORA_KEYS_H__
#define __ROCCAT_SUORA_KEYS_H__

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
#include "roccat_device.h"
#include "roccat_button.h"

G_BEGIN_DECLS

typedef struct _SuoraKey SuoraKey;
typedef struct _SuoraKeys SuoraKeys;

struct _SuoraKey {
	guint8 type;
	guint8 modifier1;
	guint8 key; /* hid usage id or macro index */
	guint8 modifier2;
} __attribute__ ((packed));

typedef enum {
	SUORA_KEY_TYPE_NORMAL = 0x00,
	SUORA_KEY_TYPE_SYSTEM = 0x02,
	SUORA_KEY_TYPE_CONSUMER = 0x03,
	SUORA_KEY_TYPE_5 = 0x05,
	SUORA_KEY_TYPE_MACRO = 0x0d,
	SUORA_KEY_TYPE_10 = 0x10, /* modifier1 = 0x01 */
} SuoraKeyType;

typedef enum {
	SUORA_KEY_TYPE_SYSTEM_SHUTDOWN = 1,
	SUORA_KEY_TYPE_SYSTEM_SLEEP = 2,
	SUORA_KEY_TYPE_SYSTEM_WAKE = 4,
} SuoraKeyTypeSystem;

typedef enum {
	SUORA_KEY_TYPE_10_F1 = 0x00,
	SUORA_KEY_TYPE_10_F2 = 0x01,
	SUORA_KEY_TYPE_10_F3 = 0x02,
	SUORA_KEY_TYPE_10_F4 = 0x03,
	SUORA_KEY_TYPE_10_F5 = 0x04,
	SUORA_KEY_TYPE_10_F6 = 0x05,
	SUORA_KEY_TYPE_10_F7 = 0x06,
	SUORA_KEY_TYPE_10_F8 = 0x07,
	SUORA_KEY_TYPE_10_F9 = 0x08,
	SUORA_KEY_TYPE_10_F10 = 0x09,
	SUORA_KEY_TYPE_10_F11 = 0x0a,
	SUORA_KEY_TYPE_10_F12 = 0x0b,
	SUORA_KEY_TYPE_10_PRINT_SCREEN = 0x0c,
	SUORA_KEY_TYPE_10_SCROLL_LOCK = 0x0d,
	SUORA_KEY_TYPE_10_PAUSE = 0x0f,
	SUORA_KEY_TYPE_10_DELETE = 0x10,
	SUORA_KEY_TYPE_10_HOME = 0x11,
	SUORA_KEY_TYPE_10_END = 0x12,
	SUORA_KEY_TYPE_10_PAGE_UP = 0x13,
	SUORA_KEY_TYPE_10_PAGE_DOWN = 0x14,
	SUORA_KEY_TYPE_10_RIGHT_ARROW = 0x15,
	SUORA_KEY_TYPE_10_LEFT_ARROW = 0x16,
	SUORA_KEY_TYPE_10_RIGHT_ALT = 0x17,
	SUORA_KEY_TYPE_10_RIGHT_CONTROL = 0x18,
} SuoraKeyType10;

void suora_key_set(SuoraKey *key, guint keys_index, guint type, guint modifier_mask, guint hid);
void roccat_button_to_suora_key(SuoraKey *key, RoccatButton const *button, guint keys_index);
void suora_key_to_roccat_button(RoccatButton *rkp_key, SuoraKey const *key, guint keys_index);

struct _SuoraKeys {
	SuoraKey keys[SUORA_KEYS_NUM];
} __attribute__ ((packed));

SuoraKeys const *suora_keys_default(void);
void suora_keys_copy(SuoraKeys *destination, SuoraKeys const *source);
gboolean suora_keys_equal(SuoraKeys const *left, SuoraKeys const *right);

SuoraKeys *suora_keys_read(RoccatDevice *device, GError **error);
gboolean suora_keys_write(RoccatDevice *device, SuoraKeys const *keys, GError **error);

G_END_DECLS

#endif
