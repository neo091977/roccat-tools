#ifndef __ROCCAT_SUORA_RKP_KEYS_H__
#define __ROCCAT_SUORA_RKP_KEYS_H__

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

#include "suora_keys.h"
#include "roccat_button.h"

G_BEGIN_DECLS

typedef struct _SuoraRkpKeys SuoraRkpKeys;

struct _SuoraRkpKeys {
	RoccatButton keys[SUORA_KEYS_NUM];
} __attribute__ ((packed));

typedef enum {
	SUORA_RKP_KEY_TYPE_DISABLED = 0x00,

	SUORA_RKP_KEY_TYPE_SYSTEM_SHUTDOWN = 0x81,
	SUORA_RKP_KEY_TYPE_SYSTEM_SLEEP = 0x82,
	SUORA_RKP_KEY_TYPE_SYSTEM_WAKE = 0x83,

	SUORA_RKP_KEY_TYPE_SHORTCUT = 0xb8,
	
	SUORA_RKP_KEY_TYPE_MACRO = 0xbe,
	
	SUORA_RKP_KEY_TYPE_APP_EMAIL = 0xc0,
	SUORA_RKP_KEY_TYPE_APP_CALCULATOR = 0xc1,
	SUORA_RKP_KEY_TYPE_APP_MY_COMPUTER = 0xc2,

	SUORA_RKP_KEY_TYPE_WWW_SEARCH = 0xc4,
	SUORA_RKP_KEY_TYPE_WWW_HOME = 0xc5,
	SUORA_RKP_KEY_TYPE_WWW_BACK = 0xc6,
	SUORA_RKP_KEY_TYPE_WWW_FORWARD = 0xc7,
	SUORA_RKP_KEY_TYPE_WWW_STOP = 0xc8,
	SUORA_RKP_KEY_TYPE_WWW_REFRESH = 0xc9,

	SUORA_RKP_KEY_TYPE_APP_MEDIA_PLAYER = 0xcb, /* called "media select" */
	SUORA_RKP_KEY_TYPE_MULTIMEDIA_PLAY_PAUSE = 0xcc,
	SUORA_RKP_KEY_TYPE_MULTIMEDIA_NEXT_TRACK = 0xcd,
	SUORA_RKP_KEY_TYPE_MULTIMEDIA_PREV_TRACK = 0xce,
	SUORA_RKP_KEY_TYPE_MULTIMEDIA_STOP = 0xcf,
	SUORA_RKP_KEY_TYPE_MULTIMEDIA_VOLUME_UP = 0xd0,
	SUORA_RKP_KEY_TYPE_MULTIMEDIA_VOLUME_DOWN = 0xd1,
	SUORA_RKP_KEY_TYPE_MULTIMEDIA_MUTE = 0xd2, /* called "mute master volume" */
} SuoraRkpKeyType;

SuoraRkpKeys const *suora_rkp_keys_default(void);
void suora_rkp_keys_copy(SuoraRkpKeys *destination, SuoraRkpKeys const *source);
SuoraRkpKeys *suora_rkp_keys_dup(SuoraRkpKeys const *old);
gboolean suora_rkp_keys_equal(SuoraRkpKeys const *left, SuoraRkpKeys const *right);

void suora_keys_to_rkp_keys(SuoraRkpKeys *rkp_keys, SuoraKeys const *keys);
void suora_rkp_keys_to_keys(SuoraKeys *keys, SuoraRkpKeys const *rkp_keys);

G_END_DECLS

#endif
