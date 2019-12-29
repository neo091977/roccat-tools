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

#include "roccat_button.h"
#include "roccat_helper.h"
#include <string.h>

void roccat_button_set_to_normal(RoccatButton *button, guint type) {
	button->type = type;
	button->modifier = 0;
	button->key = 0;
}

void roccat_button_set_to_shortcut(RoccatButton *button, guint type, guint key, guint modifier) {
	button->type = type;
	button->modifier = modifier;
	button->key = key;
}

gboolean roccat_button_equal(RoccatButton const *left, RoccatButton const *right) {
	return memcmp(left, right, sizeof(RoccatButton)) ? FALSE : TRUE;
}

void roccat_button_copy(RoccatButton *destination, RoccatButton const *source) {
	memcpy(destination, source, sizeof(RoccatButton));
}

guint8 roccat_button_build_shortcut_modifier(gboolean ctrl, gboolean shift, gboolean alt, gboolean win) {
	guint8 result = 0;

	roccat_set_bit8(&result, ROCCAT_BUTTON_MODIFIER_BIT_CTRL, ctrl);
	roccat_set_bit8(&result, ROCCAT_BUTTON_MODIFIER_BIT_SHIFT, shift);
	roccat_set_bit8(&result, ROCCAT_BUTTON_MODIFIER_BIT_ALT, alt);
	roccat_set_bit8(&result, ROCCAT_BUTTON_MODIFIER_BIT_WIN, win);

	return result;
}
