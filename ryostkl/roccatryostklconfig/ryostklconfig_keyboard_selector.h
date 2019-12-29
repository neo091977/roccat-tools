#ifndef __ROCCAT_RYOSTKLCONFIG_KEYBOARD_SELECTOR_H__
#define __ROCCAT_RYOSTKLCONFIG_KEYBOARD_SELECTOR_H__

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

#include "ryostkl_profile_data.h"
#include "ryostklconfig_blink.h"
#include "ryos_macro.h"
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define RYOSTKLCONFIG_KEYBOARD_SELECTOR_TYPE (ryostklconfig_keyboard_selector_get_type())
#define RYOSTKLCONFIG_KEYBOARD_SELECTOR(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), RYOSTKLCONFIG_KEYBOARD_SELECTOR_TYPE, RyostklconfigKeyboardSelector))
#define IS_RYOSTKLCONFIG_KEYBOARD_SELECTOR(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), RYOSTKLCONFIG_KEYBOARD_SELECTOR_TYPE))

typedef struct _RyostklconfigKeyboardSelector RyostklconfigKeyboardSelector;

GType ryostklconfig_keyboard_selector_get_type(void);
GtkWidget *ryostklconfig_keyboard_selector_new(void);

void ryostklconfig_keyboard_selector_set_device_type(RyostklconfigKeyboardSelector *selector, RoccatDevice const *device);
void ryostklconfig_keyboard_selector_set_layout(RyostklconfigKeyboardSelector *keyboard_selector, gchar const *layout);

void ryostklconfig_keyboard_selector_set_from_profile_data(RyostklconfigKeyboardSelector *selector, RyostklProfileData const *profile_data);
void ryostklconfig_keyboard_selector_update_profile_data(RyostklconfigKeyboardSelector *selector, RyostklProfileData *profile_data);
void ryostklconfig_keyboard_selector_set_macro(RyostklconfigKeyboardSelector *selector, guint macro_index, RyosMacro *macro);

void ryostklconfig_keyboard_selector_update_keyboard_illumination(RyostklconfigKeyboardSelector *keyboard_selector, RyostklconfigBlink const *blink);

G_END_DECLS

#endif
