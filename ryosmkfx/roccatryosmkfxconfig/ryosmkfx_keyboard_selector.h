#ifndef __ROCCAT_RYOSMKFX_KEYBOARD_SELECTOR_H__
#define __ROCCAT_RYOSMKFX_KEYBOARD_SELECTOR_H__

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

#include "ryosmkfx_profile_data.h"
#include "ryosmkfx_light_layer.h"
#include "ryosmkfx_macro.h"
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define RYOSMKFX_KEYBOARD_SELECTOR_TYPE (ryosmkfx_keyboard_selector_get_type())
#define RYOSMKFX_KEYBOARD_SELECTOR(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), RYOSMKFX_KEYBOARD_SELECTOR_TYPE, RyosmkfxKeyboardSelector))
#define IS_RYOSMKFX_KEYBOARD_SELECTOR(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), RYOSMKFX_KEYBOARD_SELECTOR_TYPE))

typedef struct _RyosmkfxKeyboardSelector RyosmkfxKeyboardSelector;

GType ryosmkfx_keyboard_selector_get_type(void);
GtkWidget *ryosmkfx_keyboard_selector_new(void);

void ryosmkfx_keyboard_selector_set_layout(RyosmkfxKeyboardSelector *keyboard_selector, gchar const *layout);

void ryosmkfx_keyboard_selector_set_from_profile_data(RyosmkfxKeyboardSelector *selector, RyosmkfxProfileData const *profile_data);
void ryosmkfx_keyboard_selector_update_profile_data(RyosmkfxKeyboardSelector *selector, RyosmkfxProfileData *profile_data);
void ryosmkfx_keyboard_selector_set_macro(RyosmkfxKeyboardSelector *selector, guint macro_index, RyosmkfxMacro *macro);

void ryosmkfx_keyboard_selector_effect_set_script(RyosmkfxKeyboardSelector *selector, gchar const *script);
void ryosmkfx_keyboard_selector_effect_set_effect(RyosmkfxKeyboardSelector *selector, guint effect);
void ryosmkfx_keyboard_selector_effect_set_speed(RyosmkfxKeyboardSelector *selector, guint speed);

void ryosmkfx_keyboard_selector_effect_show_temp_layer_data(RyosmkfxKeyboardSelector *selector, RyosmkfxLightLayerData const *data);
void ryosmkfx_keyboard_selector_effect_show_stored_layer_data(RyosmkfxKeyboardSelector *selector);

G_END_DECLS

#endif
