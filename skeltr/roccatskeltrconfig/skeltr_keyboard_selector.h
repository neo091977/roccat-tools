#ifndef __ROCCAT_SKELTR_KEYBOARD_SELECTOR_H__
#define __ROCCAT_SKELTR_KEYBOARD_SELECTOR_H__

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

#include "skeltr_profile_data.h"
#include "skeltr_macro.h"
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define SKELTR_KEYBOARD_SELECTOR_TYPE (skeltr_keyboard_selector_get_type())
#define SKELTR_KEYBOARD_SELECTOR(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), SKELTR_KEYBOARD_SELECTOR_TYPE, SkeltrKeyboardSelector))
#define IS_SKELTR_KEYBOARD_SELECTOR(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), SKELTR_KEYBOARD_SELECTOR_TYPE))

typedef struct _SkeltrKeyboardSelector SkeltrKeyboardSelector;

GType skeltr_keyboard_selector_get_type(void);
GtkWidget *skeltr_keyboard_selector_new(void);

void skeltr_keyboard_selector_set_from_profile_data(SkeltrKeyboardSelector *selector, SkeltrProfileData const *profile_data);
void skeltr_keyboard_selector_update_profile_data(SkeltrKeyboardSelector *selector, SkeltrProfileData *profile_data);
void skeltr_keyboard_selector_set_macro(SkeltrKeyboardSelector *selector, guint macro_index, SkeltrMacro *macro);

void skeltr_keyboard_selector_set_layout(SkeltrKeyboardSelector *keyboard_selector, gchar const *layout);

G_END_DECLS

#endif
