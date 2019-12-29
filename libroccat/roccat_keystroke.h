#ifndef __ROCCAT_KEYSTROKE_H__
#define __ROCCAT_KEYSTROKE_H__

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

#include <gaminggear/macro.h>

G_BEGIN_DECLS

typedef struct _RoccatKeystroke RoccatKeystroke;

struct _RoccatKeystroke {
	guint8 key;
	guint8 action;
	guint16 period; /*!< in milliseconds */
} __attribute__ ((packed));

static inline guint16 roccat_keystroke_get_period(RoccatKeystroke const *keystroke) {
	return GUINT16_FROM_LE(keystroke->period);
}

static inline void roccat_keystroke_set_period(RoccatKeystroke *keystroke, guint16 new_value) {
	keystroke->period = GUINT16_TO_LE(new_value);
}

/* The following definitions/functions are not valid for Arvo and Kone */

typedef enum {
	ROCCAT_KEYSTROKE_ACTION_PRESS = 1,
	ROCCAT_KEYSTROKE_ACTION_RELEASE = 2,
} RoccatKeystrokeAction;

void gaminggear_macro_keystroke_to_roccat_keystroke(GaminggearMacroKeystroke const *from, RoccatKeystroke *to);
void roccat_keystroke_to_gaminggear_macro_keystroke(RoccatKeystroke const *from, GaminggearMacroKeystroke *to);

G_END_DECLS

#endif
