#ifndef __ROCCAT_RYOSCONFIG_KEYBOARD_SELECTOR_H__
#define __ROCCAT_RYOSCONFIG_KEYBOARD_SELECTOR_H__

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

#include "ryos_profile_data.h"
#include "ryos_macro.h"
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define RYOSCONFIG_KEYBOARD_SELECTOR_TYPE (ryosconfig_keyboard_selector_get_type())
#define RYOSCONFIG_KEYBOARD_SELECTOR(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), RYOSCONFIG_KEYBOARD_SELECTOR_TYPE, RyosconfigKeyboardSelector))
#define IS_RYOSCONFIG_KEYBOARD_SELECTOR(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), RYOSCONFIG_KEYBOARD_SELECTOR_TYPE))

typedef struct _RyosconfigKeyboardSelector RyosconfigKeyboardSelector;

GType ryosconfig_keyboard_selector_get_type(void);
GtkWidget *ryosconfig_keyboard_selector_new(void);

void ryosconfig_keyboard_selector_set_device_type(RyosconfigKeyboardSelector *selector, RoccatDevice const *device);
void ryosconfig_keyboard_selector_set_layout(RyosconfigKeyboardSelector *keyboard_selector, gchar const *layout);

void ryosconfig_keyboard_selector_set_from_profile_data(RyosconfigKeyboardSelector *selector, RyosProfileData const *profile_data);
void ryosconfig_keyboard_selector_update_profile_data(RyosconfigKeyboardSelector *selector, RyosProfileData *profile_data);
void ryosconfig_keyboard_selector_set_macro(RyosconfigKeyboardSelector *selector, guint macro_index, RyosMacro *macro);

void ryosconfig_keyboard_selector_update_keyboard_illumination(RyosconfigKeyboardSelector *keyboard_selector, gboolean blink_state);

G_END_DECLS

#endif
