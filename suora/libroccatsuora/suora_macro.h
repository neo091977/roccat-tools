#ifndef __ROCCAT_SUORA_MACRO_H__
#define __ROCCAT_SUORA_MACRO_H__

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
#include <gaminggear/macro.h>

G_BEGIN_DECLS

typedef struct _SuoraMacro SuoraMacro;
typedef struct _SuoraMacroAction SuoraMacroAction;

enum {
	SUORA_MACRO_NUM = 6,
	SUORA_MACRO_ACTION_NUM = 62,
	SUORA_MACRO_ACTION_PERIOD_FACTOR = 10,
	SUORA_MACRO_ACTION_PROPERTIES_ACTION_MASK = 0x80,
	SUORA_MACRO_ACTION_PROPERTIES_PERIOD_MASK = 0x7f,
	SUORA_MACRO_ACTION_PROPERTIES_ACTION_PRESS = 0x00,
	SUORA_MACRO_ACTION_PROPERTIES_ACTION_RELEASE = 0x80,
};

struct _SuoraMacroAction {
	guint8 properties; /*!< holds action and period in 10 milliseconds */
	guint8 key;
} __attribute__((packed));

struct _SuoraMacro {
	guint16 loop;
	SuoraMacroAction actions[SUORA_MACRO_ACTION_NUM];
	guint16 unused;
} __attribute__ ((packed));

static inline guint16 suora_macro_get_loop(SuoraMacro const *macro) {
	return GUINT16_FROM_BE(macro->loop);
}

static inline void suora_macro_set_loop(SuoraMacro *macro, guint16 new_value) {
	macro->loop = GUINT16_TO_BE(new_value);
}

SuoraMacro *suora_macro_read(RoccatDevice *device, guint index, GError **error);
gboolean suora_macro_write(RoccatDevice *device, guint index, SuoraMacro const *macro, GError **error);

G_END_DECLS

#endif
