#ifndef __ROCCAT_RYOS_KEY_MASK_SELECTOR_H__
#define __ROCCAT_RYOS_KEY_MASK_SELECTOR_H__

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

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define RYOS_KEY_MASK_SELECTOR_TYPE (ryos_key_mask_selector_get_type())
#define RYOS_KEY_MASK_SELECTOR(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), RYOS_KEY_MASK_SELECTOR_TYPE, RyosKeyMaskSelector))
#define IS_RYOS_KEY_MASK_SELECTOR(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), RYOS_KEY_MASK_SELECTOR_TYPE))

typedef struct _RyosKeyMaskSelector RyosKeyMaskSelector;

GType ryos_key_mask_selector_get_type(void);
GtkWidget *ryos_key_mask_selector_new(void);

void ryos_key_mask_selector_set_mask(RyosKeyMaskSelector *selector, guint8 mask);
guint8 ryos_key_mask_selector_get_mask(RyosKeyMaskSelector *selector);

G_END_DECLS

#endif
