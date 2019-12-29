#ifndef __ROCCAT_SOVA_KEYBOARD_SELECTOR_H__
#define __ROCCAT_SOVA_KEYBOARD_SELECTOR_H__

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

#include "sova_profile_data.h"
#include "sova_macro.h"
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define SOVA_KEYBOARD_SELECTOR_TYPE (sova_keyboard_selector_get_type())
#define SOVA_KEYBOARD_SELECTOR(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), SOVA_KEYBOARD_SELECTOR_TYPE, SovaKeyboardSelector))
#define IS_SOVA_KEYBOARD_SELECTOR(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), SOVA_KEYBOARD_SELECTOR_TYPE))

typedef struct _SovaKeyboardSelector SovaKeyboardSelector;

GType sova_keyboard_selector_get_type(void);
GtkWidget *sova_keyboard_selector_new(void);

void sova_keyboard_selector_set_from_profile_data(SovaKeyboardSelector *selector, SovaProfileData const *profile_data);
void sova_keyboard_selector_update_profile_data(SovaKeyboardSelector *selector, SovaProfileData *profile_data);
void sova_keyboard_selector_set_macro(SovaKeyboardSelector *selector, guint macro_index, SovaMacro *macro);

void sova_keyboard_selector_set_layout(SovaKeyboardSelector *keyboard_selector, gchar const *layout);

G_END_DECLS

#endif
