#ifndef __ROCCAT_SUORA_GAME_MODE_H__
#define __ROCCAT_SUORA_GAME_MODE_H__

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

#include "roccat_device.h"

G_BEGIN_DECLS

typedef struct _SuoraGameMode SuoraGameMode;

struct _SuoraGameMode {
	guint8 state;
	guint8 unused[5];
} __attribute__ ((packed));

typedef enum {
	SUORA_GAME_MODE_STATE_OFF = 0,
	SUORA_GAME_MODE_STATE_ON = 1,
} SuoraGameModeState;

gint suora_game_mode_read(RoccatDevice *device, GError **error);
gboolean suora_game_mode_write(RoccatDevice *device, guint state, GError **error);

G_END_DECLS

#endif
