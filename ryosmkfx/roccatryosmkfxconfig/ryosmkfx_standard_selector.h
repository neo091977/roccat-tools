#ifndef __ROCCAT_RYOSMKFX_STANDARD_SELECTOR_H__
#define __ROCCAT_RYOSMKFX_STANDARD_SELECTOR_H__

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

#include "ryosmkfx_key_combo_box.h"

G_BEGIN_DECLS

#define RYOSMKFX_STANDARD_SELECTOR_TYPE (ryosmkfx_standard_selector_get_type())
#define RYOSMKFX_STANDARD_SELECTOR(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), RYOSMKFX_STANDARD_SELECTOR_TYPE, RyosmkfxStandardSelector))
#define IS_RYOSMKFX_STANDARD_SELECTOR(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), RYOSMKFX_STANDARD_SELECTOR_TYPE))

typedef struct _RyosmkfxStandardSelector RyosmkfxStandardSelector;

GType ryosmkfx_standard_selector_get_type(void);
GtkWidget *ryosmkfx_standard_selector_new(RyosmkfxKeyComboBoxData *standard_basepointer, RyosmkfxKeyComboBoxData *secondary_basepointer);

void ryosmkfx_standard_selector_set_index(RyosmkfxStandardSelector *selector, gint index, guint standard_mask, guint standard_standard, guint secondary_mask, guint secondary_standard);
gint ryosmkfx_standard_selector_get_index(RyosmkfxStandardSelector *selector);

void ryosmkfx_standard_selector_set_secondary_text(RyosmkfxStandardSelector *selector, gchar const *text);

void ryosmkfx_standard_selector_update(RyosmkfxStandardSelector *selector);

gchar *ryosmkfx_standard_selector_get_text_for_data(RyosmkfxStandardSelector *selector, RyosmkfxKeyComboBoxData const *data);

G_END_DECLS

#endif
