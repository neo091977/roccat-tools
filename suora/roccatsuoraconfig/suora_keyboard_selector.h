#ifndef __ROCCAT_SUORA_KEYBOARD_SELECTOR_H__
#define __ROCCAT_SUORA_KEYBOARD_SELECTOR_H__

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

#include "suora_profile_data.h"
#include "suora_macro.h"
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define SUORA_KEYBOARD_SELECTOR_TYPE (suora_keyboard_selector_get_type())
#define SUORA_KEYBOARD_SELECTOR(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), SUORA_KEYBOARD_SELECTOR_TYPE, SuoraKeyboardSelector))
#define IS_SUORA_KEYBOARD_SELECTOR(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), SUORA_KEYBOARD_SELECTOR_TYPE))

typedef struct _SuoraKeyboardSelector SuoraKeyboardSelector;

GType suora_keyboard_selector_get_type(void);
GtkWidget *suora_keyboard_selector_new(void);

void suora_keyboard_selector_set_layout(SuoraKeyboardSelector *keyboard_selector, gchar const *layout);

void suora_keyboard_selector_set_from_profile_data(SuoraKeyboardSelector *selector, SuoraProfileData const *profile_data);
void suora_keyboard_selector_update_profile_data(SuoraKeyboardSelector *selector, SuoraProfileData *profile_data);

G_END_DECLS

#endif
