#ifndef __ROCCAT_BUTTON_H__
#define __ROCCAT_BUTTON_H__

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

#include <glib.h>

G_BEGIN_DECLS

typedef struct _RoccatButton RoccatButton;

struct _RoccatButton {
	guint8 type;
	/* Shortcut uses modifier and key.
	   Custom CPI stores cpi in modifier.
	 */
	guint8 modifier;
	guint8 key;
} __attribute__ ((packed));

typedef enum {
	ROCCAT_BUTTON_MODIFIER_BIT_NONE = 0,
	ROCCAT_BUTTON_MODIFIER_BIT_SHIFT = 1,
	ROCCAT_BUTTON_MODIFIER_BIT_CTRL = 2,
	ROCCAT_BUTTON_MODIFIER_BIT_ALT = 3,
	ROCCAT_BUTTON_MODIFIER_BIT_WIN = 4,
} RoccatButtonModifierBit;

void roccat_button_set_to_normal(RoccatButton *button, guint type);
void roccat_button_set_to_shortcut(RoccatButton *button, guint type, guint key, guint modifier);
gboolean roccat_button_equal(RoccatButton const *left, RoccatButton const *right);
void roccat_button_copy(RoccatButton *destination, RoccatButton const *source);

guint8 roccat_button_build_shortcut_modifier(gboolean ctrl, gboolean shift, gboolean alt, gboolean win);

G_END_DECLS

#endif
